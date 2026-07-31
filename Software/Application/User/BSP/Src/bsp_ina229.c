/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bsp_ina229.h"

#include <string.h>

/*
    »´∏Ù¿ÎµÁ—πµÁ¡˜ƒ£øÈV1.0

    PC5  --->  INA229_SPI_SCK
    PD3  --->  INA229_SPI_MOSI
    PD2  --->  INA229_SPI_MISO
    PA1  --->  INA229_SPI_CS
    PA2  --->  INA229_ALERT
*/

#define ALL_INA229_GPIO_CLK_ENABLE() \
  __HAL_RCC_GPIOA_CLK_ENABLE();      \
  __HAL_RCC_GPIOC_CLK_ENABLE();      \
  __HAL_RCC_GPIOD_CLK_ENABLE();

#define INA229_CS_GPIO GPIOA
#define INA229_CS_PIN GPIO_PIN_1
#define INA229_CS_0() BSP_SET_GPIO_0(INA229_CS_GPIO, INA229_CS_PIN)
#define INA229_CS_1() BSP_SET_GPIO_1(INA229_CS_GPIO, INA229_CS_PIN)

#define SPI_SCK_GPIO GPIOC
#define SPI_SCK_PIN GPIO_PIN_5
#define SPI_SCK_AF GPIO_AF3_SPI_CLK

#define SPI_MOSI_GPIO GPIOD
#define SPI_MOSI_PIN GPIO_PIN_3
#define SPI_MOSI_AF GPIO_AF3_SPI_MOSI

#define SPI_MISO_GPIO GPIOD
#define SPI_MISO_PIN GPIO_PIN_2
#define SPI_MISO_AF GPIO_AF3_SPI_MISO

#define INA229_MAX_REG_BYTES 5U
#define INA229_SHUNT_CAL_SCALE (13107.2e6f)

SPI_HandleTypeDef hspi = {0};
INA229_Handle g_ina229_dev = {0};
static INA229_Handle *g_ina229 = &g_ina229_dev;
static uint16_t g_err_code = INA229_ERR_NONE;

static void bsp_InitSPIParam(void) {
  hspi.Instance = SPI;
  hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi.Init.NSS = SPI_NSS_MODE_HIGH;
  hspi.Init.Mode = SPI_MODE_MASTER;

  HAL_SPI_Init(&hspi);
}

static void bsp_ina229_config_gpio(void) {
  GPIO_InitTypeDef gpio_init = {0};

  ALL_INA229_GPIO_CLK_ENABLE();
  __HAL_RCC_SPI_CLK_ENABLE();

  INA229_CS_1();

  gpio_init.Pin = INA229_CS_PIN;
  gpio_init.Mode = GPIO_MODE_OUTPUT;
  gpio_init.OpenDrain = GPIO_PUSHPULL;
  gpio_init.Debounce.Enable = GPIO_DEBOUNCE_DISABLE;
  gpio_init.SlewRate = GPIO_SLEW_RATE_HIGH;
  gpio_init.DrvStrength = GPIO_DRV_STRENGTH_HIGH;
  gpio_init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INA229_CS_GPIO, &gpio_init);

  gpio_init.Pin = SPI_MOSI_PIN;
  gpio_init.Mode = GPIO_MODE_AF;
  gpio_init.Alternate = SPI_MOSI_AF;
  gpio_init.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SPI_MOSI_GPIO, &gpio_init);

  gpio_init.Pin = SPI_MISO_PIN;
  gpio_init.Alternate = SPI_MISO_AF;
  HAL_GPIO_Init(SPI_MISO_GPIO, &gpio_init);

  gpio_init.Pin = SPI_SCK_PIN;
  gpio_init.Alternate = SPI_SCK_AF;
  HAL_GPIO_Init(SPI_SCK_GPIO, &gpio_init);

  gpio_init.Pin = GPIO_PIN_2;
  gpio_init.Mode = GPIO_MODE_INPUT;
  gpio_init.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &gpio_init);

  bsp_InitSPIParam();
}

void bsp_InitIna229(float shunt_ohms, float max_current) {
  bsp_ina229_config_gpio();
  INA229_Reset();

  g_ina229->hspi = &hspi;
  g_ina229->shunt_ohms = shunt_ohms;
  g_ina229->max_current = max_current;
  g_ina229->current_lsb =
      (max_current > 0.0f) ? (max_current / INA229_CURRENT_LSB_DENOM) : 0.0f;

  uint16_t mfg_id = INA229_GetManufacturer();
  uint16_t dev_id = INA229_GetID();
  LOG_printf("Manufacturer ID : 0x%04X\r\n", mfg_id);
  LOG_printf("Device ID       : 0x%03X\r\n", dev_id);

  if (mfg_id != 0x5449 || dev_id != 0x229) {
    LOG_printf("SPI Comm Failed!\r\n");
    g_err_code = INA229_ERR_OFFLINE;  // ¿Îœﬂ
  } else {
    LOG_printf("INA229 OK!\r\n");
    g_err_code = INA229_ERR_NONE;  // ‘⁄œﬂ

    INA229_SetMaxCurrentShunt(max_current, shunt_ohms);
  }
}

static uint8_t SPI_ReadWriteByte(uint8_t tx_data) {
  uint8_t rx_data = 0;

  g_ina229->hspi->Instance->DATA = tx_data;
  while ((g_ina229->hspi->Instance->SR & SPI_FLAG_SPIF) != SPI_FLAG_SPIF)
    ;
  rx_data = g_ina229->hspi->Instance->DATA;

  return rx_data;
}

static uint8_t SPI_BuildCommand(uint8_t reg, uint8_t is_read) {
  return (uint8_t)(((reg & 0x3FU) << 2) | (is_read ? 0x01U : 0x00U));
}

// –¥ºƒ¥Ê∆˜
static HAL_StatusTypeDef SPIWriteRegister(uint8_t reg, uint8_t bytes,
                                          const uint8_t *data) {
  if ((g_ina229->hspi == NULL) || (data == NULL) || (bytes == 0U) ||
      (bytes > INA229_MAX_REG_BYTES)) {
    return HAL_ERROR;
  }

  uint8_t tx_cmd = SPI_BuildCommand(reg, 0U);  // –¥√¸¡Ó

  INA229_CS_0();

  SPI_ReadWriteByte(tx_cmd);  // ∑¢ÀÕ√¸¡Ó
  for (uint8_t i = 0; i < bytes; i++) {
    SPI_ReadWriteByte(data[i]);  // ∑¢ÀÕ ˝æ›
  }

  INA229_CS_1();
  return HAL_OK;
}

// ∂¡ºƒ¥Ê∆˜
static HAL_StatusTypeDef SPIreadregister(uint8_t reg, uint8_t bytes,
                                         uint8_t *rx_frame) {
  if ((g_ina229->hspi == NULL) || (rx_frame == NULL) || (bytes == 0U) ||
      (bytes > INA229_MAX_REG_BYTES)) {
    return HAL_ERROR;
  }

  uint8_t tx_cmd = SPI_BuildCommand(reg, 1U);  // ∂¡√¸¡Ó

  INA229_CS_0();

  rx_frame[0] = SPI_ReadWriteByte(tx_cmd);  // ∑¢ÀÕ√¸¡Ó
  for (uint8_t i = 0; i < bytes; i++) {
    rx_frame[1 + i] = SPI_ReadWriteByte(0x00);  // dummy
  }

  INA229_CS_1();
  return HAL_OK;
}

static int32_t INA229_SignExtend20(uint32_t value) {
  if (value & 0x00080000U) {
    value |= 0xFFF00000U;
  }
  return (int32_t)value;
}

static int64_t INA229_SignExtend40(uint64_t value) {
  if (value & (1ULL << 39)) {
    value |= 0xFFFFFF0000000000ULL;
  }
  return (int64_t)value;
}

static HAL_StatusTypeDef INA229_ReadBytes(uint8_t reg, uint8_t bytes,
                                          uint8_t *data) {
  uint8_t rx[1U + INA229_MAX_REG_BYTES] = {0};
  uint8_t i;
  if ((bytes == 0U) || (bytes > INA229_MAX_REG_BYTES)) return HAL_ERROR;
  if (SPIreadregister(reg, bytes, rx) != HAL_OK) return HAL_ERROR;
  for (i = 0U; i < bytes; i++) data[i] = rx[1U + i];
  return HAL_OK;
}

static HAL_StatusTypeDef INA229_WriteU16(uint8_t reg, uint16_t value) {
  uint8_t data[2];
  data[0] = (uint8_t)(value >> 8);
  data[1] = (uint8_t)(value & 0xFFU);
  return SPIWriteRegister(reg, 2U, data);
}

static HAL_StatusTypeDef INA229_ReadU16(uint8_t reg, uint16_t *value) {
  uint8_t data[2];
  if (INA229_ReadBytes(reg, 2U, data) != HAL_OK) return HAL_ERROR;
  *value = (uint16_t)((uint16_t)data[0] << 8) | data[1];
  return HAL_OK;
}

static HAL_StatusTypeDef INA229_ReadU24(uint8_t reg, uint32_t *value) {
  uint8_t data[3];
  if (INA229_ReadBytes(reg, 3U, data) != HAL_OK) return HAL_ERROR;
  *value = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];
  return HAL_OK;
}

static HAL_StatusTypeDef INA229_ReadU40(uint8_t reg, uint64_t *value) {
  uint8_t data[5];
  uint8_t i;
  if (INA229_ReadBytes(reg, 5U, data) != HAL_OK) return HAL_ERROR;
  *value = 0U;
  for (i = 0U; i < 5U; i++) *value = (*value << 8) | data[i];
  return HAL_OK;
}

void INA229_Reset(void) {
  uint16_t value = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &value) != HAL_OK) return;
  value |= INA229_CONFIG_RST;
  (void)INA229_WriteU16(INA229_REG_CONFIG, value);
}

HAL_StatusTypeDef INA229_SetAccumulation(uint8_t value) {
  uint16_t reg = 0U;
  if (value > 1U) return HAL_ERROR;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  if (value == 1U)
    reg |= INA229_CONFIG_RSTACC;
  else
    reg &= (uint16_t)~INA229_CONFIG_RSTACC;
  return INA229_WriteU16(INA229_REG_CONFIG, reg);
}

uint8_t INA229_GetAccumulation(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_CONFIG_RSTACC) != 0U);
}

void INA229_SetConversionDelay(uint8_t steps) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return;
  reg &= (uint16_t)~INA229_CONFIG_CONVDLY_Msk;
  reg |= (uint16_t)((uint16_t)steps << INA229_CONFIG_CONVDLY_Pos);
  (void)INA229_WriteU16(INA229_REG_CONFIG, reg);
}

uint8_t INA229_GetConversionDelay(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_CONFIG_CONVDLY_Msk) >>
                   INA229_CONFIG_CONVDLY_Pos);
}

void INA229_SetTemperatureCompensation(uint8_t on) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return;
  if (on != 0U)
    reg |= INA229_CONFIG_TEMPCOMP;
  else
    reg &= (uint16_t)~INA229_CONFIG_TEMPCOMP;
  (void)INA229_WriteU16(INA229_REG_CONFIG, reg);
}

uint8_t INA229_GetTemperatureCompensation(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_CONFIG_TEMPCOMP) != 0U);
}

HAL_StatusTypeDef INA229_SetADCRange(uint8_t flag) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  if (flag != 0U)
    reg |= INA229_CONFIG_ADCRANGE_40mV;
  else
    reg &= (uint16_t)~INA229_CONFIG_ADCRANGE_Msk;
  return INA229_WriteU16(INA229_REG_CONFIG, reg);
}

uint8_t INA229_GetADCRange(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_CONFIG_ADCRANGE_Msk) != 0U);
}

HAL_StatusTypeDef INA229_SetMode(uint8_t mode) {
  uint16_t reg = 0U;
  if (mode > 0x0FU) return HAL_ERROR;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  reg &= (uint16_t)~INA229_ADC_CONFIG_MODE_Msk;
  reg |= (uint16_t)((uint16_t)mode << INA229_ADC_CONFIG_MODE_Pos);
  return INA229_WriteU16(INA229_REG_ADC_CONFIG, reg);
}

uint8_t INA229_GetMode(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_ADC_CONFIG_MODE_Msk) >>
                   INA229_ADC_CONFIG_MODE_Pos);
}

HAL_StatusTypeDef INA229_SetBusVoltageConversionTime(uint8_t bvct) {
  uint16_t reg = 0U;
  if (bvct > 7U) return HAL_ERROR;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  reg &= (uint16_t)~INA229_ADC_CONFIG_VBUSCT_Msk;
  reg |= (uint16_t)((uint16_t)bvct << INA229_ADC_CONFIG_VBUSCT_Pos);
  return INA229_WriteU16(INA229_REG_ADC_CONFIG, reg);
}

uint8_t INA229_GetBusVoltageConversionTime(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_ADC_CONFIG_VBUSCT_Msk) >>
                   INA229_ADC_CONFIG_VBUSCT_Pos);
}

HAL_StatusTypeDef INA229_SetShuntVoltageConversionTime(uint8_t svct) {
  uint16_t reg = 0U;
  if (svct > 7U) return HAL_ERROR;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  reg &= (uint16_t)~INA229_ADC_CONFIG_VSHCT_Msk;
  reg |= (uint16_t)((uint16_t)svct << INA229_ADC_CONFIG_VSHCT_Pos);
  return INA229_WriteU16(INA229_REG_ADC_CONFIG, reg);
}

uint8_t INA229_GetShuntVoltageConversionTime(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_ADC_CONFIG_VSHCT_Msk) >>
                   INA229_ADC_CONFIG_VSHCT_Pos);
}

HAL_StatusTypeDef INA229_SetTemperatureConversionTime(uint8_t tct) {
  uint16_t reg = 0U;
  if (tct > 7U) return HAL_ERROR;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  reg &= (uint16_t)~INA229_ADC_CONFIG_VTCT_Msk;
  reg |= (uint16_t)((uint16_t)tct << INA229_ADC_CONFIG_VTCT_Pos);
  return INA229_WriteU16(INA229_REG_ADC_CONFIG, reg);
}

uint8_t INA229_GetTemperatureConversionTime(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)((reg & INA229_ADC_CONFIG_VTCT_Msk) >>
                   INA229_ADC_CONFIG_VTCT_Pos);
}

HAL_StatusTypeDef INA229_SetAverage(uint8_t avg) {
  uint16_t reg = 0U;
  if (avg > 7U) return HAL_ERROR;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return HAL_ERROR;
  reg &= (uint16_t)~INA229_ADC_CONFIG_AVG_Msk;
  reg |= (uint16_t)avg;
  return INA229_WriteU16(INA229_REG_ADC_CONFIG, reg);
}

uint8_t INA229_GetAverage(void) {
  uint16_t reg = 0U;
  if (INA229_ReadU16(INA229_REG_ADC_CONFIG, &reg) != HAL_OK) return 0U;
  return (uint8_t)(reg & INA229_ADC_CONFIG_AVG_Msk);
}

int INA229_SetMaxCurrentShunt(float max_current, float shunt_ohms) {
  float shunt_cal;
  uint16_t reg;
  if (shunt_ohms < 0.0001f) return -2;
  if (g_ina229->hspi == NULL) return -1;

  g_ina229->max_current = max_current;
  g_ina229->shunt_ohms = shunt_ohms;
  g_ina229->current_lsb =
      (max_current > 0.0f) ? (max_current / INA229_CURRENT_LSB_DENOM) : 0.0f;
  shunt_cal =
      INA229_SHUNT_CAL_SCALE * g_ina229->current_lsb * g_ina229->shunt_ohms;
  if (INA229_GetADCRange() != 0U) shunt_cal *= 4.0f;

  reg = (uint16_t)shunt_cal;
  if (INA229_WriteU16(INA229_REG_SHUNT_CAL, reg) != HAL_OK) return -3;
  return 0;
}

float INA229_GetMaxCurrent(void) {
  return (g_ina229->hspi != NULL) ? g_ina229->max_current : 0.0f;
}
float INA229_GetShunt(void) {
  return (g_ina229->hspi != NULL) ? g_ina229->shunt_ohms : 0.0f;
}
float INA229_GetCurrentLSB(void) {
  return (g_ina229->hspi != NULL) ? g_ina229->current_lsb : 0.0f;
}

HAL_StatusTypeDef INA229_SetShuntTemperatureCoefficient(uint16_t ppm) {
  if (ppm > 16383U) return HAL_ERROR;
  return INA229_WriteU16(INA229_REG_SHUNT_TEMPCO, ppm);
}

uint16_t INA229_GetShuntTemperatureCoefficient(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_SHUNT_TEMPCO, &value);
  return value;
}

float INA229_GetShuntVoltage(void) {
  uint32_t raw = 0U;
  float shunt_lsb = 312.5e-9f;
  if (INA229_ReadU24(INA229_REG_VSHUNT, &raw) != HAL_OK) return 0.0f;
  if (INA229_GetADCRange() != 0U) shunt_lsb = 78.125e-9f;
  raw >>= 4;
  return (float)INA229_SignExtend20(raw) * shunt_lsb;
}

int32_t INA229_GetShuntVoltageRaw(void) {
  uint32_t raw = 0U;
  if (INA229_ReadU24(INA229_REG_VSHUNT, &raw) != HAL_OK) return 0;
  raw >>= 4;
  return INA229_SignExtend20(raw);
}

float INA229_GetBusVoltage(void) {
  uint32_t value = 0U;
  float bus_lsb = 195.3125e-6f;
  if (INA229_ReadU24(INA229_REG_VBUS, &value) != HAL_OK) return 0.0f;
  value >>= 4;
  return (float)value * bus_lsb;
}

float INA229_GetTemperature(void) {
  uint16_t raw = 0U;
  float lsb = 7.8125e-3f;
  if (INA229_ReadU16(INA229_REG_DIETEMP, &raw) != HAL_OK) return 0.0f;
  return (float)raw * lsb;
}

float INA229_GetCurrent(void) {
  uint32_t raw = 0U;
  if (INA229_ReadU24(INA229_REG_CURRENT, &raw) != HAL_OK) return 0.0f;
  raw >>= 4;
  return (float)INA229_SignExtend20(raw) * g_ina229->current_lsb;
}

float INA229_GetPower(void) {
  uint32_t raw = 0U;
  float lsb = (3.2f * g_ina229->current_lsb);
  if (INA229_ReadU24(INA229_REG_POWER, &raw) != HAL_OK) return 0.0f;
  return (float)raw * lsb;
}

double INA229_GetEnergy(void) {
  uint64_t raw = 0U;
  double lsb = (16.0 * 3.2 * (double)g_ina229->current_lsb);
  if (INA229_ReadU40(INA229_REG_ENERGY, &raw) != HAL_OK) return 0.0;
  return (double)raw * lsb;
}

double INA229_GetCharge(void) {
  uint64_t raw = 0U;
  double lsb = ((double)g_ina229->current_lsb);
  if (INA229_ReadU40(INA229_REG_CHARGE, &raw) != HAL_OK) return 0.0;
  return (double)INA229_SignExtend40(raw) * lsb;
}

void INA229_SetDiagnoseAlert(uint16_t flags) {
  (void)INA229_WriteU16(INA229_REG_DIAG_ALRT, flags);
}
uint16_t INA229_GetDiagnoseAlert(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_DIAG_ALRT, &value);
  return value;
}

void INA229_SetDiagnoseAlertBit(uint8_t bit) {
  uint16_t value = 0U, mask = (uint16_t)(1U << bit);
  if (INA229_ReadU16(INA229_REG_DIAG_ALRT, &value) != HAL_OK) return;
  if ((value & mask) == 0U) {
    value |= mask;
    (void)INA229_WriteU16(INA229_REG_DIAG_ALRT, value);
  }
}

void INA229_ClearDiagnoseAlertBit(uint8_t bit) {
  uint16_t value = 0U, mask = (uint16_t)(1U << bit);
  if (INA229_ReadU16(INA229_REG_DIAG_ALRT, &value) != HAL_OK) return;
  if ((value & mask) != 0U) {
    value &= (uint16_t)~mask;
    (void)INA229_WriteU16(INA229_REG_DIAG_ALRT, value);
  }
}

uint16_t INA229_GetDiagnoseAlertBit(uint8_t bit) {
  uint16_t value = 0U;
  if (INA229_ReadU16(INA229_REG_DIAG_ALRT, &value) != HAL_OK) return 0U;
  return (uint16_t)((value >> bit) & 0x01U);
}

void INA229_SetShuntOvervoltageTH(uint16_t threshold) {
  (void)INA229_WriteU16(INA229_REG_SOVL, threshold);
}
uint16_t INA229_GetShuntOvervoltageTH(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_SOVL, &value);
  return value;
}

void INA229_SetShuntUndervoltageTH(uint16_t threshold) {
  (void)INA229_WriteU16(INA229_REG_SUVL, threshold);
}
uint16_t INA229_GetShuntUndervoltageTH(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_SUVL, &value);
  return value;
}

void INA229_SetBusOvervoltageTH(uint16_t threshold) {
  if (threshold > 0x7FFFU) return;
  (void)INA229_WriteU16(INA229_REG_BOVL, threshold);
}
uint16_t INA229_GetBusOvervoltageTH(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_BOVL, &value);
  return value;
}

void INA229_SetBusUndervoltageTH(uint16_t threshold) {
  if (threshold > 0x7FFFU) return;
  (void)INA229_WriteU16(INA229_REG_BUVL, threshold);
}
uint16_t INA229_GetBusUndervoltageTH(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_BUVL, &value);
  return value;
}

void INA229_SetTemperatureOverLimitTH(uint16_t threshold) {
  (void)INA229_WriteU16(INA229_REG_TEMP_LIMIT, threshold);
}
uint16_t INA229_GetTemperatureOverLimitTH(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_TEMP_LIMIT, &value);
  return value;
}

void INA229_SetPowerOverLimitTH(uint16_t threshold) {
  (void)INA229_WriteU16(INA229_REG_PWR_LIMIT, threshold);
}
uint16_t INA229_GetPowerOverLimitTH(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_PWR_LIMIT, &value);
  return value;
}

uint16_t INA229_GetManufacturer(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_MANUFACTURER_ID, &value);
  return value;
}

uint16_t INA229_GetID(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_DEVICE_ID, &value);
  return (uint16_t)((value >> 4) & 0x0FFFU);
}

uint16_t INA229_GetRevision(void) {
  uint16_t value = 0U;
  (void)INA229_ReadU16(INA229_REG_DEVICE_ID, &value);
  return (uint16_t)(value & 0x000FU);
}

uint16_t INA229_Get_ErrorCode(void) {
  if (g_err_code & INA229_ERR_OFFLINE) {
    return g_err_code;
  }

  uint16_t diag_alrt = INA229_GetDiagnoseAlert();

  uint16_t valid_mask = INA229_ERR_ENERGYOF | INA229_ERR_CHARGEOF |
                        INA229_ERR_MATHOF | INA229_ERR_TMPOL |
                        INA229_ERR_SHNTOL | INA229_ERR_SHNTUL |
                        INA229_ERR_BUSOL | INA229_ERR_BUSUL | INA229_ERR_POL;

  g_err_code = diag_alrt & valid_mask;

  if ((diag_alrt & INA229_ERR_MEMSTAT) == 0U) {
    g_err_code |= INA229_ERR_MEMSTAT;
  } else {
    g_err_code &= ~INA229_ERR_MEMSTAT;
  }

  return g_err_code;
}
