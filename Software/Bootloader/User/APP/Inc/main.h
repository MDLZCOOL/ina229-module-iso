/**
 ******************************************************************************
 * @file    main.h
 * @author  Application Team
 * @Version V1.0.0
 * @Date    1-April-2019
 * @brief   Header for main.c file.
 *          This file contains the common defines of the application.
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cs32l010_hal.h"

/* Private includes ----------------------------------------------------------*/
#include <stdbool.h>
/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define DEBUG_RTT 0
#if DEBUG_RTT
#include "SEGGER_RTT.h"
#define LOG_printf(...)                \
  do {                                 \
    SEGGER_RTT_printf(0, __VA_ARGS__); \
  } while (0)
#else
#define LOG_printf(...) \
  do {                  \
  } while (0)
#endif

#define BSP_SET_GPIO_1(gpio, pin) (gpio->ODSET = (pin))
#define BSP_SET_GPIO_0(gpio, pin) (gpio->ODCLR = (pin))

#define FLOAT_INT(x) ((int)(x))
#define FLOAT_FRAC3(x) ((int)(((x) > 0.0f ? (x) : -(x)) * 1000.0f) % 1000)
#define FLOAT_FRAC6(x) ((int)(((x) > 0.0f ? (x) : -(x)) * 1000000.0f) % 1000000)

#define CFG_DEFAULT_MODBUS_ADDR 1U
#define CFG_DEFAULT_RSHUNT_OHM 0.01f
#define CFG_DEFAULT_IEXP_MAX 15.0f
#define CFG_DEFAULT_SHUNT_OVER_VOLTAGE_TH 0x7FFF  /* 正满量程 */
#define CFG_DEFAULT_SHUNT_UNDER_VOLTAGE_TH 0x8000 /* 负满量程 */
#define CFG_DEFAULT_BUS_OVER_VOLTAGE_TH 0x7FFF
#define CFG_DEFAULT_BUS_UNDER_VOLTAGE_TH 0x8000
#define CFG_DEFAULT_TEMP_OVER_LIMIT_TH 0x7FFF
#define CFG_DEFAULT_POWER_OVER_LIMIT_TH 0x7FFF
#define CFG_DEFAULT_VOLTAGE_K 1.0f
#define CFG_DEFAULT_CURRENT_K 1.0f
#define CFG_DEFAULT_RSHUNT_PPM 50U

/* Modbus 保存命令magic */
#define CFG_SAVE_MAGIC 0x4B56

typedef struct {
  int modbus_addr;
  float rshunt_ohm;
  float iexp_max;
  uint16_t shunt_over_voltage_th;
  uint16_t shunt_under_voltage_th;
  uint16_t bus_over_voltage_th;
  uint16_t bus_under_voltage_th;
  uint16_t temp_over_limit_th;
  uint16_t power_over_limit_th;
  float voltage_k;
  float current_k;
  uint16_t rshunt_ppm;
} CONFIG_T;

/* 全局配置实例 */
extern CONFIG_T g_config;
extern int g_config_flash_valid; /* 1 = Flash 中有有效配置 */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
