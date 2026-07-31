/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bsp_uart_fifo.h"

#include <string.h>

#if UART1_FIFO_EN == 1
#define UART1_CLK_ENABLE() __HAL_RCC_UART1_CLK_ENABLE()
#define UART1_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART1_TX_GPIO_PORT GPIOD
#define UART1_TX_PIN GPIO_PIN_5
#define UART1_TX_AF GPIO_AF5_UART1_TXD
#define UART1_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART1_RX_GPIO_PORT GPIOD
#define UART1_RX_PIN GPIO_PIN_6
#define UART1_RX_AF GPIO_AF5_UART1_RXD
#endif

#if UART1_FIFO_EN == 1
static UART_T g_tUart1;
static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];
static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];
#endif

static void UartVarInit(void);
static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);

void bsp_InitUart(void) {
  UartVarInit();
  InitHardUart();
}

void bsp_DeInitUart(void) { HAL_UART_DeInit(&(g_tUart1.huart)); }

UART_T *ComToUart(COM_PORT_E _ucPort) {
  if (_ucPort == COM1) {
#if UART1_FIFO_EN == 1
    return &g_tUart1;
#else
    return 0;
#endif
  }
  return 0;
}

void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen) {
  UART_T *pUart = ComToUart(_ucPort);
  if (pUart == 0) return;

  if (pUart->SendBefor != 0) {
    pUart->SendBefor();
  }
  UartSend(pUart, _ucaBuf, _usLen);
}

void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte) {
  comSendBuf(_ucPort, &_ucByte, 1);
}

uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte) {
  UART_T *pUart = ComToUart(_ucPort);
  if (pUart == 0) return 0;
  return UartGetChar(pUart, _pByte);
}

void comClearTxFifo(COM_PORT_E _ucPort) {
  UART_T *pUart = ComToUart(_ucPort);
  if (pUart == 0) return;
  pUart->usTxWrite = 0;
  pUart->usTxRead = 0;
  pUart->usTxCount = 0;
}

void comClearRxFifo(COM_PORT_E _ucPort) {
  UART_T *pUart = ComToUart(_ucPort);
  if (pUart == 0) return;
  pUart->usRxWrite = 0;
  pUart->usRxRead = 0;
  pUart->usRxCount = 0;
}

static void UartVarInit(void) {
#if UART1_FIFO_EN == 1
  memset(&g_tUart1, 0, sizeof(UART_T));
  g_tUart1.huart.Instance = UART1;
  g_tUart1.pTxBuf = g_TxBuf1;
  g_tUart1.pRxBuf = g_RxBuf1;
  g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;
  g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;
  g_tUart1.usTxWrite = 0;
  g_tUart1.usTxRead = 0;
  g_tUart1.usRxWrite = 0;
  g_tUart1.usRxRead = 0;
  g_tUart1.usRxCount = 0;
  g_tUart1.usTxCount = 0;
  g_tUart1.Sending = 0;
#endif
}

void bsp_InitUartParam(COM_PORT_E _ucPort, uint32_t BaudRate, uint32_t Parity,
                       uint32_t Mode) {
  UART_T *pUart = ComToUart(_ucPort);
  if (pUart == 0) return;

  memset(&pUart->huart.Init, 0, sizeof(pUart->huart.Init));

  pUart->huart.Instance = UART1;
  pUart->huart.Init.BaudRate = BaudRate;
  pUart->huart.Init.WordLength = UART_WORDLENGTH_8B;
  pUart->huart.Init.Parity = Parity;
  pUart->huart.Init.Mode = Mode;
  pUart->huart.Init.HalfDuplexMode = UART_HALFDUPLEX_DISABLE;
  pUart->huart.Init.BaudDouble =
      UART_BAUDDOUBLE_ENABLE; /* 防止较高波特率出现异常 */

  if (HAL_UART_Init(&pUart->huart) != HAL_OK) {
    while (1)
      ; /* spin */
  }
}

static void InitHardUart(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

#if UART1_FIFO_EN == 1
  UART1_TX_GPIO_CLK_ENABLE();
  UART1_RX_GPIO_CLK_ENABLE();
  UART1_CLK_ENABLE();

  GPIO_InitStruct.Pin = UART1_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF;
  GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Alternate = UART1_TX_AF;
  HAL_GPIO_Init(UART1_TX_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = UART1_RX_PIN;
  GPIO_InitStruct.Alternate = UART1_RX_AF;
  HAL_GPIO_Init(UART1_RX_GPIO_PORT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(UART1_IRQn, 2);  // 调整中断优先级
  HAL_NVIC_EnableIRQ(UART1_IRQn);

  bsp_InitUartParam(COM1, UART1_BAUD, UART_PARITY_NONE, UART_MODE_TX_RX);

  __HAL_UART_ENABLE_IT(&g_tUart1.huart, UART_IT_RXNE);
#endif
}

static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen) {
  uint16_t i;

  for (i = 0; i < _usLen; i++) {
    /* 缓冲区满，则等待 */
    while (1) {
      __IO uint16_t usCount;
      __disable_irq();
      usCount = _pUart->usTxCount;
      __enable_irq();

      if (usCount < _pUart->usTxBufSize) {
        break;
      }
    }

    /* 压入FIFO */
    _pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

    __disable_irq();
    if (++_pUart->usTxWrite >= _pUart->usTxBufSize) {
      _pUart->usTxWrite = 0;
    }
    _pUart->usTxCount++;
    __enable_irq();
  }

  __disable_irq();
  if (_pUart->Sending == 0) {
    _pUart->Sending = 1;
    _pUart->huart.Instance->SBUF = _pUart->pTxBuf[_pUart->usTxRead];

    if (++_pUart->usTxRead >= _pUart->usTxBufSize) {
      _pUart->usTxRead = 0;
    }
    _pUart->usTxCount--;

    __HAL_UART_ENABLE_IT(&_pUart->huart, UART_IT_TC);
  }
  __enable_irq();
}

static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte) {
  uint16_t usCount;

  __disable_irq();
  usCount = _pUart->usRxCount;
  __enable_irq();

  if (usCount == 0) {
    return 0;
  } else {
    *_pByte = _pUart->pRxBuf[_pUart->usRxRead];
    __disable_irq();
    if (++_pUart->usRxRead >= _pUart->usRxBufSize) {
      _pUart->usRxRead = 0;
    }
    _pUart->usRxCount--;
    __enable_irq();
    return 1;
  }
}

void UartIRQ(UART_T *_pUart) {
  uint32_t isrflags = READ_REG(_pUart->huart.Instance->INTSR);
  uint32_t sconits = READ_REG(_pUart->huart.Instance->SCON);

  /* 处理接收中断 */
  if (((isrflags & UART_INTSR_RI) != RESET) &&
      ((sconits & UART_SCON_RIEN) != RESET)) {
    /* 清除接收中断标志 */
    __HAL_UART_CLEAR_FLAG(&_pUart->huart, UART_FLAG_RXNE);

    /* 读取数据 */
    uint8_t ch = (uint8_t)(_pUart->huart.Instance->SBUF & 0xFF);

    _pUart->pRxBuf[_pUart->usRxWrite] = ch;
    if (++_pUart->usRxWrite >= _pUart->usRxBufSize) {
      _pUart->usRxWrite = 0;
    }
    if (_pUart->usRxCount < _pUart->usRxBufSize) {
      _pUart->usRxCount++;
    }

    if (_pUart->ReciveNew) {
      _pUart->ReciveNew(ch);
    }
  }

  /* 处理帧错误中断等 */
  if ((isrflags & UART_INTSR_FE) != RESET) {
    __HAL_UART_CLEAR_FLAG(&_pUart->huart, UART_FLAG_FE);
  }

  /* 处理发送完成中断 */
  if (((isrflags & UART_INTSR_TI) != RESET) &&
      ((sconits & UART_SCON_TIEN) != RESET)) {
    /* 清除发送中断标志 */
    __HAL_UART_CLEAR_FLAG(&_pUart->huart, UART_FLAG_TC);

    if (_pUart->usTxCount == 0) {
      /* 发送完毕，关闭发送中断 */
      __HAL_UART_DISABLE_IT(&_pUart->huart, UART_IT_TC);
      _pUart->Sending = 0;

      if (_pUart->SendOver) {
        _pUart->SendOver();
      }
    } else {
      /* 继续发送FIFO里的下一个字节 */
      _pUart->huart.Instance->SBUF = _pUart->pTxBuf[_pUart->usTxRead];

      if (++_pUart->usTxRead >= _pUart->usTxBufSize) {
        _pUart->usTxRead = 0;
      }
      _pUart->usTxCount--;
    }
  }
}

#if UART1_FIFO_EN == 1
void UART1_IRQHandler(void) { UartIRQ(&g_tUart1); }
#endif