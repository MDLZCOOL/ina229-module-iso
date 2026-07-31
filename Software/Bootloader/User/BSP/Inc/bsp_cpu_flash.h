/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BSP_CPU_FLASH_H_
#define _BSP_CPU_FLASH_H_

#include "stdint.h"

#define CPU_FLASH_BASE_ADDR (uint32_t)(FLASH_MEMORY_BASE) /* 0x00000000 */
#define CPU_FLASH_END_ADDR (uint32_t)(0x0000FFFF)

#define CPU_FLASH_SIZE (64 * 1024) /* FLASH总容量 64KB */
#define CPU_FLASH_PAGE_SIZE (512)  /* 页大小 512 字节 */

/* Flash操作返回值 */
#define FLASH_IS_EQU 0    /* Flash内容和待写入的数据相等，不需要擦除和写操作 */
#define FLASH_REQ_WRITE 1 /* Flash不需要擦除，直接写 */
#define FLASH_REQ_ERASE 2 /* Flash需要先擦除再写 */
#define FLASH_PARAM_ERR 3 /* 函数参数错误 */

uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst,
                         uint32_t _ulSize);
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc,
                          uint32_t _ulSize);
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf,
                        uint32_t _ulSize);
uint8_t bsp_EraseCpuFlash(uint32_t _ulFlashAddr);

#endif
