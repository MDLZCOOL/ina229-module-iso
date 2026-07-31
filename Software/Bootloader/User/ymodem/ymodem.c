// SPDX-License-Identifier: Apache-2.0
// SPDX-SnippetCopyrightText: 2025 MDLZCOOL <mdlzcool@foxmail.com>
// Modified from STM AN395 example

// Includes
#include "ymodem.h"

#include <stdint.h>
#include <string.h>

#include "bsp_cpu_flash.h"
#include "bsp_uart_fifo.h"

// extern variables
static __attribute__((aligned(4)))
uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
uint8_t g_filename[FILE_NAME_LENGTH];

static int32_t mini_atoi(const char *str) {
  int32_t res = 0;
  while (*str >= '0' && *str <= '9') {
    res = res * 10 + (*str - '0');
    str++;
  }
  return res;
}

// check if one byte rx successfully
// key: variabl to store rx data
// return: 1=success, 0=fail
static uint32_t SerialKeyPressed(uint8_t *key) {
  if (comGetChar(COM1, key) == 1) {
    return 1;
  }
  return 0;
}

#define FLASH_BASE_ADDR ((uint32_t)0x00000000)
#define FLASH_END_ADDR ((uint32_t)0x0000EFFF)
#define FLASH_SECTOR_SIZE ((uint32_t)512)

uint32_t CS32_Flash_Erase(uint32_t start_address) {
  uint32_t addr;

  for (addr = start_address; addr <= FLASH_END_ADDR;
       addr += FLASH_SECTOR_SIZE) {
    if (bsp_EraseCpuFlash(addr) != 0) {
      return 1;  // 擦除失败
    }
  }
  return 0;
}

// This function writes a data buffer in flash (data are 32-bit aligned).
// After writing data buffer, the flash content is checked.
// FlashAddress: start address for writing data buffer
// Data: pointer on data buffer
// DataLength: length of data buffer (unit is 32-bit word)
// return:
//       0=Data successfully written to Flash memory
//       1=Error occurred while writing data in Flash memory
//       2=Written Data in flash memory is different from expected one
uint32_t CS32_FLASH_Write(uint32_t FlashAddress, uint8_t *Data,
                          uint32_t ByteLength) {
  if (bsp_WriteCpuFlash(FlashAddress, Data, ByteLength) == 0) {
    return 0;  // 成功
  }
  return 1;  // 失败
}

// Rx a byte from sender
// c: variable to store data
// timeout: read time out
// return: 0=success, -1=fail
static int32_t Receive_Byte(uint8_t *c, uint32_t timeout_ms) {
  uint32_t tickstart = HAL_GetTick();

  while ((HAL_GetTick() - tickstart) < timeout_ms) {
    if (SerialKeyPressed(c) == 1) {
      return 0;
    }
  }
  return -1;
}

// send a byte via IAP_Port
// c: byte to send
// return: 0
static uint32_t Send_Byte(uint8_t c) {
  comSendChar(COM1, c);
  return 0;
}

// Rx a packet from sender
// data: pointer to store rx data
// length: packet length
// timeout: rx time out
// return:0=normally return
//       -1=timeout or packet error
//        1=abort by user
static int32_t Receive_Packet(uint8_t *data, int32_t *length,
                              uint32_t timeout) {
  uint16_t i, packet_size;
  uint8_t c;

  *length = 0;
  if (Receive_Byte(&c, timeout) != 0) {
    return -1;
  }
  switch (c) {
    case SOH:
      packet_size = PACKET_SIZE;
      break;
    case STX:
      packet_size = PACKET_1K_SIZE;
      break;
    case EOT:
      return 0;
    case CA:
      if ((Receive_Byte(&c, timeout) == 0) && (c == CA)) {
        *length = -1;
        return 0;
      } else {
        return -1;
      }
    case ABORT1:
    case ABORT2:
      return 1;
    default:
      return -1;
  }
  *data = c;
  for (i = 1; i < (packet_size + PACKET_OVERHEAD); i++) {
    if (Receive_Byte(data + i, timeout) != 0) {
      return -1;
    }
  }
  //  if ((uint8_t)data[PACKET_SEQNO_INDEX] !=
  //  ((uint8_t)(data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
  //  {
  //    return -1;
  //  }
  *length = packet_size;
  return 0;
}

// Receive a file using the ymodem protocol
// appaddr: User Application address
// return: size of IAP file
int32_t Ymodem_Receive(uint32_t appaddr) {
  uint8_t *file_ptr, flag_EOT = 0;
  int32_t i, packet_length, session_done = 0, file_done = 0,
                            packets_received = 0;
  int32_t errors = 0, session_begin = 0, size = 0;
  char file_size[FILE_SIZE_LENGTH];
  uint32_t flashdestination = appaddr;

  for (;;) {
    for (packets_received = 0, file_done = 0;;) {
      switch (Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT)) {
        case 0:
          errors = 0;
          switch (packet_length) {
            case -1:  // Abort
              Send_Byte(ACK);
              return 0;
            case 0:  // EOT
              if (flag_EOT == 0) {
                Send_Byte(NACK);
                flag_EOT = 1;
              } else {
                Send_Byte(ACK);
                Send_Byte('C');
                file_done = 1;
              }
              break;
            default:
              if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) !=
                  (packets_received & 0xff)) {
                Send_Byte(NACK);  // 序号不对，重发
              } else {
                if (packets_received == 0) {
                  /* 第 0 包：文件名和大小 */
                  if (packet_data[PACKET_HEADER] != 0) {
                    for (i = 0, file_ptr = packet_data + PACKET_HEADER;
                         (*file_ptr != 0) && (i < FILE_NAME_LENGTH);) {
                      g_filename[i++] = *file_ptr++;
                    }
                    g_filename[i++] = '\0';

                    for (i = 0, file_ptr++;
                         (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);) {
                      file_size[i++] = *file_ptr++;
                    }
                    file_size[i++] = '\0';
                    size = mini_atoi(file_size);

                    if (size > (FLASH_END_ADDR - appaddr + 1)) {
                      Send_Byte(CA);
                      Send_Byte(CA);
                      return -1;  // 文件太大
                    }

                    /* 擦除 APP 区 */
                    CS32_Flash_Erase(appaddr);
                    Send_Byte(ACK);
                    Send_Byte(CRC16);
                  } else {
                    Send_Byte(ACK);
                    file_done = 1;
                    session_done = 1;
                    break;
                  }
                } else {
                  if (CS32_FLASH_Write(flashdestination,
                                       packet_data + PACKET_HEADER,
                                       packet_length) == 0) {
                    flashdestination += packet_length;
                    Send_Byte(ACK);
                  } else {
                    Send_Byte(CA);
                    Send_Byte(CA);
                    return -2;  // 写入失败
                  }
                }
                packets_received++;
                session_begin = 1;
              }
          }
          break;
        case 1:
          Send_Byte(CA);
          Send_Byte(CA);
          return -3;
        default:
          if (session_begin > 0) errors++;
          if (errors > MAX_ERRORS) {
            Send_Byte(CA);
            Send_Byte(CA);
            return 0;
          }
          Send_Byte(CRC16);  // 要求重传
          break;
      }
      if (file_done != 0) break;
    }
    if (session_done != 0) break;
  }
  return (int32_t)size;
}
