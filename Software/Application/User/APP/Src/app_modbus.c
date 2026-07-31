#include "app_modbus.h"

#include <string.h>

#include "agile_modbus.h"
#include "agile_modbus_rtu.h"
#include "agile_modbus_slave_util.h"
#include "app_kv.h"
#include "bsp_uart_fifo.h"
#include "main.h"

#define MODBUS_RX_BUF_SIZE 256
#define MODBUS_TX_BUF_SIZE 256

/* Bootloader 进入命令 magic，地址与 Bootloader 侧 MAGIC_ADDR/MAGIC_UPDATE_REQ
 * 对应 */
#define BOOT_CMD_MAGIC 0x4B4F /* "OK" in ASCII */

enum {
#define X(id, member, type, count) \
  REG_ADDR_##id, REG_ADDR_##id##_END = REG_ADDR_##id + count - 1,
  REG_LIST
#undef X
      TOTAL_HOLDING_REG_COUNT
};

/* Modbus 协议栈上下文 */
static agile_modbus_rtu_t ctx_rtu;
static uint8_t ctx_tx_buf[MODBUS_TX_BUF_SIZE];
static uint8_t ctx_rx_buf[MODBUS_RX_BUF_SIZE];

/* 接收断帧参数 */
static uint16_t rx_len = 0;
static uint32_t last_rx_tick = 0;

/* 保持寄存器数组 — 测量数据 (只读) */
static uint16_t holding_registers[TOTAL_HOLDING_REG_COUNT];

/* 保持寄存器数组 — 配置参数 (可读写) */
static uint16_t config_registers[CFG_REG_COUNT];
static uint8_t config_dirty = 0; /* 有未保存的修改 */

static uint8_t g_pending_reboot = 0;
static uint32_t g_reboot_tick = 0;

/* ---------------------------------------------------------
 * 测量数据 回调
 * --------------------------------------------------------- */
/*
 * 注意 agile_modbus_slave_util 的回调约定：
 *   get(buf, bufsz)              -> bufsz 是 map_buf
 * 的容量(253)，不是寄存器区大小，必须自己裁剪 set(index, len, buf, bufsz)  ->
 * buf 是"整个 map 从 0 开始"的镜像，新数据位于 buf[index..index+len-1] bufsz
 * 同样是 map_buf 容量，绝不能当成数据长度使用 所以 set 里必须：源偏移
 * index，长度 len*2。
 */
static int get_holding_regs(void *buf, int bufsz) {
  int len = (int)sizeof(holding_registers);
  if (len > bufsz) len = bufsz;
  memcpy(buf, holding_registers, (size_t)len);
  return 0;
}

static int set_holding_regs(int index, int len, void *buf, int bufsz) {
  const uint16_t *src = (const uint16_t *)buf;

  if (index < 0 || len <= 0 || (index + len) > TOTAL_HOLDING_REG_COUNT)
    return -AGILE_MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  if ((index + len) * 2 > bufsz)
    return -AGILE_MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE;

  memcpy(&holding_registers[index], &src[index], (size_t)len * 2);
  return 0;
}

/* ---------------------------------------------------------
 * 配置参数 回调
 * --------------------------------------------------------- */

/* 从 config_registers 同步两个 float 到 g_config */
static void config_regs_to_g_config(void) {
  /* modbus_addr */
  g_config.modbus_addr = (int)config_registers[0];

  /* rshunt_ohm (reg 1=high, 2=low) */
  {
    uint32_t u = ((uint32_t)config_registers[1] << 16) | config_registers[2];
    memcpy(&g_config.rshunt_ohm, &u, 4);
  }
  /* iexp_max (reg 3=high, 4=low) */
  {
    uint32_t u = ((uint32_t)config_registers[3] << 16) | config_registers[4];
    memcpy(&g_config.iexp_max, &u, 4);
  }
  /* 阈值 (uint16) */
  g_config.shunt_over_voltage_th = config_registers[5];
  g_config.shunt_under_voltage_th = config_registers[6];
  g_config.bus_over_voltage_th = config_registers[7];
  g_config.bus_under_voltage_th = config_registers[8];
  g_config.temp_over_limit_th = config_registers[9];
  g_config.power_over_limit_th = config_registers[10];

  /* voltage_k (reg 11=high, 12=low) */
  {
    uint32_t u = ((uint32_t)config_registers[11] << 16) | config_registers[12];
    memcpy(&g_config.voltage_k, &u, 4);
  }

  /* current_k (reg 13=high, 14=low) */
  {
    uint32_t u = ((uint32_t)config_registers[13] << 16) | config_registers[14];
    memcpy(&g_config.current_k, &u, 4);
  }

  g_config.rshunt_ppm = config_registers[15];
}

/* 将 g_config 同步到 config_registers */
static void g_config_to_config_regs(void) {
  config_registers[0] = (uint16_t)g_config.modbus_addr;

  {
    uint32_t u;
    memcpy(&u, &g_config.rshunt_ohm, 4);
    config_registers[1] = (uint16_t)(u >> 16);
    config_registers[2] = (uint16_t)(u & 0xFFFF);
  }
  {
    uint32_t u;
    memcpy(&u, &g_config.iexp_max, 4);
    config_registers[3] = (uint16_t)(u >> 16);
    config_registers[4] = (uint16_t)(u & 0xFFFF);
  }

  config_registers[5] = g_config.shunt_over_voltage_th;
  config_registers[6] = g_config.shunt_under_voltage_th;
  config_registers[7] = g_config.bus_over_voltage_th;
  config_registers[8] = g_config.bus_under_voltage_th;
  config_registers[9] = g_config.temp_over_limit_th;
  config_registers[10] = g_config.power_over_limit_th;

  {
    uint32_t u;
    memcpy(&u, &g_config.voltage_k, 4);
    config_registers[11] = (uint16_t)(u >> 16);
    config_registers[12] = (uint16_t)(u & 0xFFFF);
  }

  {
    uint32_t u;
    memcpy(&u, &g_config.current_k, 4);
    config_registers[13] = (uint16_t)(u >> 16);
    config_registers[14] = (uint16_t)(u & 0xFFFF);
  }

  config_registers[15] = g_config.rshunt_ppm;
  config_registers[16] = 0; /* SAVE_CMD */
  config_registers[17] = 0; /* BOOT_CMD */
}

static int get_config_regs(void *buf, int bufsz) {
  int len = (int)sizeof(config_registers);
  if (len > bufsz) len = bufsz;
  memcpy(buf, config_registers, (size_t)len);
  return 0;
}

#define IDX_SAVE_CMD 16
#define IDX_BOOT_CMD 17

static int set_config_regs(int index, int len, void *buf, int bufsz) {
  const uint16_t *src = (const uint16_t *)buf;

  /* 越界保护：index/len 来自主机报文，必须校验 */
  if (index < 0 || len <= 0 || (index + len) > CFG_REG_COUNT)
    return -AGILE_MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS;
  if ((index + len) * 2 > bufsz)
    return -AGILE_MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE;

  /* 只拷贝本次真正写入的那几个寄存器，源同样要偏移 index */
  memcpy(&config_registers[index], &src[index], (size_t)len * 2);
  config_regs_to_g_config();

  /* 1. 处理 SAVE_CMD (0x0110 = 16) */
  if (index <= IDX_SAVE_CMD && (index + len) > IDX_SAVE_CMD) {
    uint16_t cmd = config_registers[IDX_SAVE_CMD];
    if (cmd == CFG_SAVE_MAGIC) {
      config_registers[IDX_SAVE_CMD] = 0;

      app_KV_SetInt("1", g_config.modbus_addr);
      app_KV_SetFloat("2", g_config.rshunt_ohm);
      app_KV_SetFloat("3", g_config.iexp_max);
      app_KV_SetUint16("4", g_config.shunt_over_voltage_th);
      app_KV_SetUint16("5", g_config.shunt_under_voltage_th);
      app_KV_SetUint16("6", g_config.bus_over_voltage_th);
      app_KV_SetUint16("7", g_config.bus_under_voltage_th);
      app_KV_SetUint16("8", g_config.temp_over_limit_th);
      app_KV_SetUint16("9", g_config.power_over_limit_th);
      app_KV_SetFloat("10", g_config.voltage_k);
      app_KV_SetFloat("11", g_config.current_k);
      app_KV_SetUint16("12", g_config.rshunt_ppm);

      app_KV_Save();
      config_dirty = 0;
      LOG_printf("[KV] Config saved to Flash successfully.\r\n");
    }
    return 0;  // ?? 直接返回，防止掉入下方的 config_dirty 和 警告中
  }

  /* 2. 处理 BOOT_CMD (0x0111 = 17) */
  if (index <= IDX_BOOT_CMD && (index + len) > IDX_BOOT_CMD) {
    uint16_t cmd = config_registers[IDX_BOOT_CMD];
    if (cmd == BOOT_CMD_MAGIC) {
      LOG_printf("Entering Bootloader for firmware upgrade...\r\n");

      /* 向 RAM 尾地址写入 magic，通知 Bootloader 不要跳转 */
      *(volatile uint32_t *)0x20000FFC = 0x5AA51234;

      /* 标记延迟重启，给串口发 ACK 留出 50ms 时间 */
      g_pending_reboot = 1;
      g_reboot_tick = HAL_GetTick();

      /* 清除指令，防止重复触发 */
      config_registers[IDX_BOOT_CMD] = 0;
    }
    return 0;  // ?? 直接返回！
  }

  /* 只有修改了普通的配置参数，才会标记 dirty */
  config_dirty = 1;
  LOG_printf("[Warning] Config modified in RAM only, not saved yet.\r\n");

  return 0;
}

/* ---------------------------------------------------------
 * 寄存器映射表
 * --------------------------------------------------------- */
static const agile_modbus_slave_util_map_t register_maps[] = {
    {0x0000, TOTAL_HOLDING_REG_COUNT - 1, get_holding_regs, set_holding_regs},
    {CFG_REG_ADDR_MODBUS_ADDR, CFG_REG_ADDR_BOOT_CMD, get_config_regs,
     set_config_regs},
};

static const agile_modbus_slave_util_t slave_util = {
    NULL,
    0, /* 线圈 */
    NULL,
    0, /* 离散输入 */
    register_maps,
    sizeof(register_maps) / sizeof(register_maps[0]), /* 保持寄存器 */
    NULL,
    0, /* 输入寄存器 */
    NULL,
    NULL,
    NULL};

/* ---------------------------------------------------------
 * 初始化
 * --------------------------------------------------------- */
void app_ModbusInit(uint8_t slave_address) {
  agile_modbus_rtu_init(&ctx_rtu, ctx_tx_buf, sizeof(ctx_tx_buf), ctx_rx_buf,
                        sizeof(ctx_rx_buf));
  agile_modbus_set_slave(&ctx_rtu._ctx, slave_address);
  memset(holding_registers, 0, sizeof(holding_registers));
  memset(config_registers, 0, sizeof(config_registers));
  config_dirty = 0;
  rx_len = 0;
}

/*
 * @brief  用 g_config 初始化配置寄存器
 */
void app_Modbus_ConfigInit(void) {
  g_config_to_config_regs();
  config_dirty = 0;
}

/* ---------------------------------------------------------
 * Modbus 轮询
 * --------------------------------------------------------- */
void app_Modbus_Poll(void) {
  uint8_t ch;
  while (comGetChar(COM1, &ch)) {
    if (rx_len < MODBUS_RX_BUF_SIZE) {
      ctx_rx_buf[rx_len++] = ch;
    }
    last_rx_tick = HAL_GetTick();
  }

  if (rx_len > 0 && (HAL_GetTick() - last_rx_tick >= 10)) {
    int send_len = agile_modbus_slave_handle(&ctx_rtu._ctx, rx_len, 1,
                                             agile_modbus_slave_util_callback,
                                             &slave_util, NULL);

    if (send_len > 0) {
      comSendBuf(COM1, ctx_rtu._ctx.send_buf, send_len);
    }
    rx_len = 0;
  }

  if (g_pending_reboot) {
    /* 等 50ms 让 ACK 发完再复位 (115200 下 8 字节约 0.7ms) */
    if (HAL_GetTick() - g_reboot_tick >= 50) {
      g_pending_reboot = 0;
      NVIC_SystemReset();
    }
  }
}

/* ---------------------------------------------------------
 * 测量数据写入辅助
 * --------------------------------------------------------- */
static void float_to_registers(float val, uint16_t *reg_high,
                               uint16_t *reg_low) {
  uint32_t u_val;
  memcpy(&u_val, &val, sizeof(u_val));
  *reg_high = (uint16_t)(u_val >> 16);
  *reg_low = (uint16_t)(u_val & 0xFFFF);
}

#define WRITE_REG_float(addr, val)                  \
  float_to_registers(val, &holding_registers[addr], \
                     &holding_registers[addr + 1])
#define WRITE_REG_uint16_t(addr, val) holding_registers[addr] = val
#define WRITE_REG_uint32_t(addr, val)                         \
  do {                                                        \
    holding_registers[addr] = (uint16_t)((val) >> 16);        \
    holding_registers[addr + 1] = (uint16_t)((val) & 0xFFFF); \
  } while (0)

void Modbus_Update_INA229(const ina229_data_t *data) {
  if (data == NULL) return;
#define X(id, member, type, count) \
  WRITE_REG_##type(REG_ADDR_##id, data->member);
  REG_LIST
#undef X
}