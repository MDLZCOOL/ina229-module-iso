#ifndef _APP_MODBUS_H_
#define _APP_MODBUS_H_

#include <stdint.h>

#include "cs32l010_hal.h"

/* ---------------------------------------------------------
 * 保持寄存器数据表 (只读, 地址 0x0000 - 0x000A)
 * 格式：X(唯一标识ID, 结构体成员名, 数据类型, 占用Modbus寄存器个数)
 * --------------------------------------------------------- */
#define REG_LIST                \
  X(VOLTAGE, voltage, float, 2) \
  X(CURRENT, current, float, 2) \
  X(POWER, power, float, 2)     \
  X(CHARGE, charge, float, 2)   \
  X(TEMP, temp, float, 2)       \
  X(ENERGY, energy, float, 2)   \
  X(ERR_CODE, err_code, uint16_t, 1)

/* ---------------------------------------------------------
 * 配置参数寄存器 (可读写, 地址 0x0100 - 0x010B)
 * --------------------------------------------------------- */
#define CFG_REG_ADDR_MODBUS_ADDR 0x0100 /* int, 1 regs */
#define CFG_REG_ADDR_RSHUNT_OHM 0x0101  /* float, 2 regs */
#define CFG_REG_ADDR_IEXP_MAX 0x0103    /* float, 2 regs */
#define CFG_REG_ADDR_SHUNT_OV_TH 0x0105 /* uint16_t, 1 regs */
#define CFG_REG_ADDR_SHUNT_UV_TH 0x0106 /* uint16_t, 1 regs */
#define CFG_REG_ADDR_BUS_OV_TH 0x0107   /* uint16_t, 1 regs */
#define CFG_REG_ADDR_BUS_UV_TH 0x0108   /* uint16_t, 1 regs */
#define CFG_REG_ADDR_TEMP_OL_TH 0x0109  /* uint16_t, 1 regs */
#define CFG_REG_ADDR_POWER_OL_TH 0x010A /* uint16_t, 1 regs */
#define CFG_REG_ADDR_VOLTAGE_K 0x010B   /* float, 2 regs */
#define CFG_REG_ADDR_CURRENT_K 0x010D   /* float, 2 regs */
#define CFG_REG_ADDR_RSHUNT_PPM 0x010F  /* uint16_t, 1 regs */
#define CFG_REG_ADDR_SAVE_CMD 0x0110    /* 写magic触发保存 */
#define CFG_REG_ADDR_BOOT_CMD 0x0111    /* 写magic触发进入Bootloader */

#define CFG_REG_COUNT 18

/* ---------------------------------------------------------
 * 数据结构体
 * --------------------------------------------------------- */
typedef struct {
#define X(id, member, type, count) type member;
  REG_LIST
#undef X
} ina229_data_t;

/* Modbus 初始化 */
void app_ModbusInit(uint8_t slave_address);

/* 用 g_config 初始化配置寄存器 (启动时调用) */
void app_Modbus_ConfigInit(void);

/* Modbus 轮询处理 */
void app_Modbus_Poll(void);

/*
 * 更新数据到 Modbus 寄存器中
 * 参数 data: 传入的 ina229_data_t 结构体指针
 */
void Modbus_Update_INA229(const ina229_data_t *data);

#endif /* _APP_MODBUS_H_ */