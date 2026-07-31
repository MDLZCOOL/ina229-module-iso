/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __BSP_INA229_H
#define __BSP_INA229_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "main.h"

#define INA229_CURRENT_LSB_DENOM (524288.0f)

/* INA229 register map (Table 7-3) */
#define INA229_REG_CONFIG 0x00U
#define INA229_REG_ADC_CONFIG 0x01U
#define INA229_REG_SHUNT_CAL 0x02U
#define INA229_REG_SHUNT_TEMPCO 0x03U
#define INA229_REG_VSHUNT 0x04U
#define INA229_REG_VBUS 0x05U
#define INA229_REG_DIETEMP 0x06U
#define INA229_REG_CURRENT 0x07U
#define INA229_REG_POWER 0x08U
#define INA229_REG_ENERGY 0x09U
#define INA229_REG_CHARGE 0x0AU
#define INA229_REG_DIAG_ALRT 0x0BU
#define INA229_REG_SOVL 0x0CU
#define INA229_REG_SUVL 0x0DU
#define INA229_REG_BOVL 0x0EU
#define INA229_REG_BUVL 0x0FU
#define INA229_REG_TEMP_LIMIT 0x10U
#define INA229_REG_PWR_LIMIT 0x11U
#define INA229_REG_MANUFACTURER_ID 0x3EU
#define INA229_REG_DEVICE_ID 0x3FU

/* CONFIG register fields */
#define INA229_CONFIG_RST_Pos 15U
#define INA229_CONFIG_RST_Msk (0x1U << INA229_CONFIG_RST_Pos)
#define INA229_CONFIG_RST INA229_CONFIG_RST_Msk
#define INA229_CONFIG_RSTACC_Pos 14U
#define INA229_CONFIG_RSTACC_Msk (0x1U << INA229_CONFIG_RSTACC_Pos)
#define INA229_CONFIG_RSTACC INA229_CONFIG_RSTACC_Msk
#define INA229_CONFIG_CONVDLY_Pos 6U
#define INA229_CONFIG_CONVDLY_Msk (0xFFU << INA229_CONFIG_CONVDLY_Pos)
#define INA229_CONFIG_TEMPCOMP_Pos 5U
#define INA229_CONFIG_TEMPCOMP_Msk (0x1U << INA229_CONFIG_TEMPCOMP_Pos)
#define INA229_CONFIG_TEMPCOMP INA229_CONFIG_TEMPCOMP_Msk
#define INA229_CONFIG_ADCRANGE_Pos 4U
#define INA229_CONFIG_ADCRANGE_Msk (0x1U << INA229_CONFIG_ADCRANGE_Pos)
#define INA229_CONFIG_ADCRANGE_163mV (0x0U << INA229_CONFIG_ADCRANGE_Pos)
#define INA229_CONFIG_ADCRANGE_40mV (0x1U << INA229_CONFIG_ADCRANGE_Pos)

/* ADC_CONFIG register fields */
#define INA229_ADC_CONFIG_MODE_Pos 12U
#define INA229_ADC_CONFIG_MODE_Msk (0xFU << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_SHUTDOWN (0x0U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_BUS (0x1U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_SHUNT (0x2U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_SHUNT_BUS \
  (0x3U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_TEMP (0x4U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_TEMP_BUS \
  (0x5U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_TEMP_SHUNT \
  (0x6U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_TRIG_BUS_SHUNT_TEMP \
  (0x7U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_SHUTDOWN_ALT (0x8U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_BUS (0x9U << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_SHUNT (0xAU << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_SHUNT_BUS \
  (0xBU << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_TEMP (0xCU << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_BUS_TEMP \
  (0xDU << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_TEMP_SHUNT \
  (0xEU << INA229_ADC_CONFIG_MODE_Pos)
#define INA229_ADC_CONFIG_MODE_CONT_BUS_SHUNT_TEMP \
  (0xFU << INA229_ADC_CONFIG_MODE_Pos)

#define INA229_ADC_CONFIG_VBUSCT_Pos 9U
#define INA229_ADC_CONFIG_VBUSCT_Msk (0x7U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_50US (0x0U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_84US (0x1U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_150US (0x2U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_280US (0x3U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_540US (0x4U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_1052US (0x5U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_2074US (0x6U << INA229_ADC_CONFIG_VBUSCT_Pos)
#define INA229_ADC_CONFIG_VBUSCT_4120US (0x7U << INA229_ADC_CONFIG_VBUSCT_Pos)

#define INA229_ADC_CONFIG_VSHCT_Pos 6U
#define INA229_ADC_CONFIG_VSHCT_Msk (0x7U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_50US (0x0U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_84US (0x1U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_150US (0x2U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_280US (0x3U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_540US (0x4U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_1052US (0x5U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_2074US (0x6U << INA229_ADC_CONFIG_VSHCT_Pos)
#define INA229_ADC_CONFIG_VSHCT_4120US (0x7U << INA229_ADC_CONFIG_VSHCT_Pos)

#define INA229_ADC_CONFIG_VTCT_Pos 3U
#define INA229_ADC_CONFIG_VTCT_Msk (0x7U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_50US (0x0U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_84US (0x1U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_150US (0x2U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_280US (0x3U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_540US (0x4U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_1052US (0x5U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_2074US (0x6U << INA229_ADC_CONFIG_VTCT_Pos)
#define INA229_ADC_CONFIG_VTCT_4120US (0x7U << INA229_ADC_CONFIG_VTCT_Pos)

#define INA229_ADC_CONFIG_AVG_Pos 0U
#define INA229_ADC_CONFIG_AVG_Msk (0x7U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_1 (0x0U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_4 (0x1U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_16 (0x2U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_64 (0x3U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_128 (0x4U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_256 (0x5U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_512 (0x6U << INA229_ADC_CONFIG_AVG_Pos)
#define INA229_ADC_CONFIG_AVG_1024 (0x7U << INA229_ADC_CONFIG_AVG_Pos)

typedef enum {
  INA229_ERR_NONE = 0x0000U,
  INA229_ERR_MEMSTAT = (1U << 0),    // Bit  0: 存储器校验
  INA229_ERR_CNVRF = (1U << 1),      // Bit  1: 转换完成标志
  INA229_ERR_POL = (1U << 2),        // Bit  2: 功率超限
  INA229_ERR_BUSUL = (1U << 3),      // Bit  3: 总线欠压
  INA229_ERR_BUSOL = (1U << 4),      // Bit  4: 总线过压
  INA229_ERR_SHNTUL = (1U << 5),     // Bit  5: 分流欠压
  INA229_ERR_SHNTOL = (1U << 6),     // Bit  6: 分流过压
  INA229_ERR_TMPOL = (1U << 7),      // Bit  7: 温度超限
  INA229_ERR_OFFLINE = (1U << 8),    // Bit  8: 芯片离线
  INA229_ERR_MATHOF = (1U << 9),     // Bit  9: 数学计算溢出
  INA229_ERR_CHARGEOF = (1U << 10),  // Bit 10: 电荷寄存器溢出
  INA229_ERR_ENERGYOF = (1U << 11),  // Bit 11: 能量寄存器溢出
} INA229_Error_t;

typedef struct {
  SPI_HandleTypeDef *hspi;
  float shunt_ohms;
  float max_current;
  float current_lsb;
} INA229_Handle;

extern SPI_HandleTypeDef hspi;
extern INA229_Handle g_ina229_dev;

void bsp_InitIna229(float shunt_ohms, float max_current);
float INA229_GetBusVoltage(void);
float INA229_GetShuntVoltage(void);
int32_t INA229_GetShuntVoltageRaw(void);
float INA229_GetCurrent(void);
float INA229_GetPower(void);
float INA229_GetTemperature(void);
double INA229_GetEnergy(void);
double INA229_GetCharge(void);
void INA229_Reset(void);
HAL_StatusTypeDef INA229_SetAccumulation(uint8_t value);
uint8_t INA229_GetAccumulation(void);
void INA229_SetConversionDelay(uint8_t steps);
uint8_t INA229_GetConversionDelay(void);
void INA229_SetTemperatureCompensation(uint8_t on);
uint8_t INA229_GetTemperatureCompensation(void);
HAL_StatusTypeDef INA229_SetADCRange(uint8_t flag);
uint8_t INA229_GetADCRange(void);
HAL_StatusTypeDef INA229_SetMode(uint8_t mode);
uint8_t INA229_GetMode(void);
HAL_StatusTypeDef INA229_SetBusVoltageConversionTime(uint8_t bvct);
uint8_t INA229_GetBusVoltageConversionTime(void);
HAL_StatusTypeDef INA229_SetShuntVoltageConversionTime(uint8_t svct);
uint8_t INA229_GetShuntVoltageConversionTime(void);
HAL_StatusTypeDef INA229_SetTemperatureConversionTime(uint8_t tct);
uint8_t INA229_GetTemperatureConversionTime(void);
HAL_StatusTypeDef INA229_SetAverage(uint8_t avg);
uint8_t INA229_GetAverage(void);
int INA229_SetMaxCurrentShunt(float max_current, float shunt_ohms);
float INA229_GetMaxCurrent(void);
float INA229_GetShunt(void);
float INA229_GetCurrentLSB(void);
HAL_StatusTypeDef INA229_SetShuntTemperatureCoefficient(uint16_t ppm);
uint16_t INA229_GetShuntTemperatureCoefficient(void);
void INA229_SetDiagnoseAlert(uint16_t flags);
uint16_t INA229_GetDiagnoseAlert(void);
void INA229_SetDiagnoseAlertBit(uint8_t bit);
void INA229_ClearDiagnoseAlertBit(uint8_t bit);
uint16_t INA229_GetDiagnoseAlertBit(uint8_t bit);
void INA229_SetShuntOvervoltageTH(uint16_t threshold);
uint16_t INA229_GetShuntOvervoltageTH(void);
void INA229_SetShuntUndervoltageTH(uint16_t threshold);
uint16_t INA229_GetShuntUndervoltageTH(void);
void INA229_SetBusOvervoltageTH(uint16_t threshold);
uint16_t INA229_GetBusOvervoltageTH(void);
void INA229_SetBusUndervoltageTH(uint16_t threshold);
uint16_t INA229_GetBusUndervoltageTH(void);
void INA229_SetTemperatureOverLimitTH(uint16_t threshold);
uint16_t INA229_GetTemperatureOverLimitTH(void);
void INA229_SetPowerOverLimitTH(uint16_t threshold);
uint16_t INA229_GetPowerOverLimitTH(void);
uint16_t INA229_GetManufacturer(void);
uint16_t INA229_GetID(void);
uint16_t INA229_GetRevision(void);
uint16_t INA229_Get_ErrorCode(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_INA229_H */