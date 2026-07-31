/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main.h"

#include "app_kv.h"
#include "app_modbus.h"
#include "bsp_ina229.h"
#include "bsp_led.h"
#include "bsp_uart_fifo.h"
#include "perf_counter.h"
#include "soft_timer.h"

ina229_data_t g_ina229_data_inst;

CONFIG_T g_config;
int g_config_flash_valid = 0;

/* 加载默认值 */
static void config_load_defaults(void) {
  g_config.modbus_addr = CFG_DEFAULT_MODBUS_ADDR;
  g_config.rshunt_ohm = CFG_DEFAULT_RSHUNT_OHM;
  g_config.iexp_max = CFG_DEFAULT_IEXP_MAX;
  g_config.shunt_over_voltage_th = CFG_DEFAULT_SHUNT_OVER_VOLTAGE_TH;
  g_config.shunt_under_voltage_th = CFG_DEFAULT_SHUNT_UNDER_VOLTAGE_TH;
  g_config.bus_over_voltage_th = CFG_DEFAULT_BUS_OVER_VOLTAGE_TH;
  g_config.bus_under_voltage_th = CFG_DEFAULT_BUS_UNDER_VOLTAGE_TH;
  g_config.temp_over_limit_th = CFG_DEFAULT_TEMP_OVER_LIMIT_TH;
  g_config.power_over_limit_th = CFG_DEFAULT_POWER_OVER_LIMIT_TH;
  g_config.voltage_k = CFG_DEFAULT_VOLTAGE_K;
  g_config.current_k = CFG_DEFAULT_CURRENT_K;
  g_config.rshunt_ppm = CFG_DEFAULT_RSHUNT_PPM;
}

static void config_load_from_flash(void) {
  int ok = 0;
  ok += (app_KV_GetInt("1", &g_config.modbus_addr) == KV_OK);
  ok += (app_KV_GetFloat("2", &g_config.rshunt_ohm) == KV_OK);
  ok += (app_KV_GetFloat("3", &g_config.iexp_max) == KV_OK);
  ok += (app_KV_GetUint16("4", &g_config.shunt_over_voltage_th) == KV_OK);
  ok += (app_KV_GetUint16("5", &g_config.shunt_under_voltage_th) == KV_OK);
  ok += (app_KV_GetUint16("6", &g_config.bus_over_voltage_th) == KV_OK);
  ok += (app_KV_GetUint16("7", &g_config.bus_under_voltage_th) == KV_OK);
  ok += (app_KV_GetUint16("8", &g_config.temp_over_limit_th) == KV_OK);
  ok += (app_KV_GetUint16("9", &g_config.power_over_limit_th) == KV_OK);
  ok += (app_KV_GetFloat("10", &g_config.voltage_k) == KV_OK);
  ok += (app_KV_GetFloat("11", &g_config.current_k) == KV_OK);
  ok += (app_KV_GetUint16("12", &g_config.rshunt_ppm) == KV_OK);

  /* 参数全部读到才算有效 */
  if (ok == 12) {
    g_config_flash_valid = 1;
    LOG_printf("Loaded from Flash.\r\n");
  } else {
    LOG_printf("Flash incomplete, using defaults.\r\n");
  }
}

/* 首次烧录写入默认值 */
static void config_save_defaults_to_flash(void) {
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
  LOG_printf("Defaults saved to Flash.\r\n");
}

// 24M clock
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HIRC;
  RCC_OscInitStruct.HIRCState = RCC_HIRC_ON;
  RCC_OscInitStruct.HIRCCalibrationValue = RCC_HIRCCALIBRATION_24M;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType =
      RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HIRC;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APBCLKDivider = RCC_PCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK) {
    Error_Handler();
  }
}

#define APP_ADDR 0x00002000

int main(void) {
  /* 硬件复位后 PRIMASK=0；若从 Bootloader 跳转来，确保中断开启 */
  __enable_irq();

  HAL_Init();

  SystemClock_Config();

#if DEBUG_RTT
  SEGGER_RTT_Init();
#endif

  app_KV_Init();
  app_KV_List();

  config_load_defaults();

  config_load_from_flash();

  if (!g_config_flash_valid) {
    config_save_defaults_to_flash();
  }

  LOG_printf("modbus addr=%d\r\n", g_config.modbus_addr);
  LOG_printf("rshunt %d.%03d Ohm, Imax=%d.%03d A\r\n",
             FLOAT_INT(g_config.rshunt_ohm), FLOAT_FRAC3(g_config.rshunt_ohm),
             FLOAT_INT(g_config.iexp_max), FLOAT_FRAC3(g_config.iexp_max));

  bsp_InitLed();

  bsp_InitIna229(g_config.rshunt_ohm, g_config.iexp_max);

  INA229_SetShuntTemperatureCoefficient(g_config.rshunt_ppm);
  INA229_SetTemperatureCompensation(1);

  INA229_SetAverage(INA229_ADC_CONFIG_AVG_64);
  INA229_SetShuntOvervoltageTH(g_config.shunt_over_voltage_th);
  INA229_SetShuntUndervoltageTH(g_config.shunt_under_voltage_th);
  INA229_SetBusOvervoltageTH(g_config.bus_over_voltage_th);
  INA229_SetBusUndervoltageTH(g_config.bus_under_voltage_th);
  INA229_SetTemperatureOverLimitTH(g_config.temp_over_limit_th);
  INA229_SetPowerOverLimitTH(g_config.power_over_limit_th);

  bsp_InitUart();

  app_ModbusInit(g_config.modbus_addr);
  app_Modbus_ConfigInit();

  InitSoftTimer();
  StartSoftAutoTimer(0, 1);
  StartSoftAutoTimer(1, 10);
  StartSoftAutoTimer(2, 500);

  while (1) {
    if (CheckSoftTimer(0) == 1) {
      app_Modbus_Poll();
    }
    if (CheckSoftTimer(1) == 1) {
      int32_t cycle_used = 0;
      double t = INA229_GetShuntVoltage();
      // LOG_printf("%d.%06d V\r\n", FLOAT_INT(t), FLOAT_FRAC6(t));
      __cycleof__("", { cycle_used = _; }) {
        g_ina229_data_inst.voltage =
            INA229_GetBusVoltage() * g_config.voltage_k;
        g_ina229_data_inst.current = INA229_GetCurrent() * g_config.current_k;
        g_ina229_data_inst.temp = INA229_GetTemperature();
        g_ina229_data_inst.charge = INA229_GetCharge();
        g_ina229_data_inst.power =
            INA229_GetPower() * g_config.current_k * g_config.voltage_k;
        g_ina229_data_inst.energy = INA229_GetEnergy();
        g_ina229_data_inst.err_code = INA229_Get_ErrorCode();
        Modbus_Update_INA229(&g_ina229_data_inst);
      }

      // LOG_printf("Task: %d cycles, approx %d us\r\n", cycle_used, cycle_used
      // / 24);
    }
    if (CheckSoftTimer(2) == 1) {
      bsp_LedToggle(1);
    }
  }
}

void Error_Handler(void) {}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
