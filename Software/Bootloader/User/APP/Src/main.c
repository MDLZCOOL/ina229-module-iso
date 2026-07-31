/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main.h"

#include "bsp_led.h"
#include "bsp_uart_fifo.h"
#include "ymodem.h"

#define APPLICATION_ADDRESS 0x00002000
#define MAGIC_ADDR 0x20000FFC
#define MAGIC_UPDATE_REQ 0x5AA51234
extern uint8_t g_filename[FILE_NAME_LENGTH];

static void Int2Str(uint8_t *str, int32_t intnum) {
  uint32_t i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++) {
    str[j++] = (intnum / Div) + 48;

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j - 1] == '0') & (Status == 0)) {
      j = 0;
    } else {
      Status++;
    }
  }
}

static int Download2Flash(void) {
  uint8_t Number[10] = {0};
  int32_t Size = 0;

  LOG_printf("\r\n Waiting for the file to be sent...\r\n");
  comSendChar(COM1, 'C');

  Size = Ymodem_Receive(APPLICATION_ADDRESS);

  if (Size > 0) {
    LOG_printf("--------------------------------\r\n");
    LOG_printf(" Programming Completed Successfully!\r\n");
    LOG_printf(" Name: %s\r\n", g_filename);
    Int2Str(Number, Size);
    LOG_printf(" Size: %s Bytes\r\n", Number);
    LOG_printf("--------------------------------\r\n");
    return SUCCESS;
  } else if (Size == -1) {
    LOG_printf(
        "\r\n The image size is higher than the allowed space memory!\r\n");
  } else if (Size == -2) {
    LOG_printf("\r\n Verification failed!\r\n");
  } else if (Size == -3) {
    LOG_printf("\r\n Aborted by user.\r\n");
  } else {
    LOG_printf("\r\n Failed to receive the file!\r\n");
  }
  return ERROR;
}

void JumpToApplication(uint32_t appAddress) {
  if (((*(volatile uint32_t *)appAddress) & 0x2FFE0000) != 0x20000000) {
    return;  // 地址不合法，说明没有有效APP，直接返回
  }

  bsp_DeInitUart();

  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;

  HAL_NVIC_DisableIRQ(UART1_IRQn);

  HAL_DeInit();

  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
  __HAL_RCC_UART1_CLK_DISABLE();

  typedef void (*pFunction)(void);
  pFunction JumpAddress;
  uint32_t jumpAddress = *(volatile uint32_t *)(appAddress + 4);
  JumpAddress = (pFunction)jumpAddress;

  __set_MSP(*(volatile uint32_t *)appAddress);

  JumpAddress();

  // The code should never reach here.
  while (1)
    ;
}

// TODO remove this from here
// PB4 - KEY
void bsp_InitKey(void) {
  GPIO_InitTypeDef gpio_init;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  gpio_init.Pin = GPIO_PIN_4;
  gpio_init.Mode = GPIO_MODE_INPUT;
  gpio_init.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &gpio_init);
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

int main(void) {
  HAL_Init();

  SystemClock_Config();
  SystemCoreClockUpdate();

#if DEBUG_RTT
  SEGGER_RTT_Init();
#endif

  bsp_InitLed();

  bsp_InitKey();

  bsp_InitUart();

  LOG_printf("CS32L010 YMODEM BOOTLOADER v1.0\r\n");

  uint32_t magic = *(volatile uint32_t *)MAGIC_ADDR;

  uint8_t key_pressed = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_SET;

  if ((magic != MAGIC_UPDATE_REQ) && (!key_pressed)) {
    LOG_printf("Jump to application...\r\n");
    HAL_Delay(20);
    JumpToApplication(APPLICATION_ADDRESS);
    LOG_printf("APP not found! Fallback to YMODEM mode.\r\n");
  } else {
    if (key_pressed) {
      LOG_printf("Update requested by KEY.\r\n");
    } else {
      LOG_printf("Update requested by APP.\r\n");
    }
  }

  *(volatile uint32_t *)MAGIC_ADDR = 0;

  while (1) {
    if (Download2Flash() == SUCCESS) {
      LOG_printf("Update Success!Reboot...\r\n");
      HAL_Delay(10);
      NVIC_SystemReset();
    } else {
      LOG_printf("Update Failed!\r\n");
      while (1)
        ;
    }
  }
}

void Error_Handler(void) {}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
