/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bsp_cpu_flash.h"

#include "cs32l010_hal.h"

/*
*********************************************************************************************************
* 函 数 名: bsp_GetPageAddr
* 功能说明: 根据地址计算所在页的起始地址
* 形    参: Address - Flash地址
* 返 回 值: 页起始地址
*********************************************************************************************************
*/
static uint32_t bsp_GetPageAddr(uint32_t Address) {
  return Address & ~(CPU_FLASH_PAGE_SIZE - 1); /* 向下对齐到512B */
}

/*
*********************************************************************************************************
* 函 数 名: bsp_ReadCpuFlash
* 功能说明: 读取CPU Flash的内容
* 形    参: _ucpDst : 目标缓冲区
*      _ulFlashAddr : 起始地址
*      _ulSize : 数据大小,单位是字节
* 返 回 值: 0=成功，1=失败
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst,
                         uint32_t _ulSize) {
  uint32_t i;

  if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE) {
    return 1;
  }

  /* 长度为0时不继续操作 */
  if (_ulSize == 0) {
    return 1;
  }

  for (i = 0; i < _ulSize; i++) {
    *_ucpDst++ = *(uint8_t *)_ulFlashAddr++;
  }

  return 0;
}

/*
*********************************************************************************************************
* 函 数 名: bsp_CmpCpuFlash
* 功能说明: 比较Flash指定地址的数据
* 形    参: _ulFlashAddr : Flash地址
*      _ucpBuf : 数据缓冲区
*      _ulSize : 数据大小,单位是字节
* 返 回 值:
*     FLASH_IS_EQU    0   Flash内容和待写入的数据相等，不需要擦除和写操作
*     FLASH_REQ_WRITE   1 Flash不需要擦除，直接写
*     FLASH_REQ_ERASE   2 Flash需要先擦除再写
*     FLASH_PARAM_ERR   3 函数参数错误
*********************************************************************************************************
*/
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf,
                        uint32_t _ulSize) {
  uint32_t i;
  uint8_t ucIsEqu;
  uint8_t ucByte;

  if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE) {
    return FLASH_PARAM_ERR;
  }

  if (_ulSize == 0) {
    return FLASH_IS_EQU;
  }

  ucIsEqu = 1;
  for (i = 0; i < _ulSize; i++) {
    ucByte = *(uint8_t *)_ulFlashAddr;

    if (ucByte != *_ucpBuf) {
      if (ucByte != 0xFF) {
        return FLASH_REQ_ERASE;
      } else {
        ucIsEqu = 0;
      }
    }

    _ulFlashAddr++;
    _ucpBuf++;
  }

  if (ucIsEqu == 1) {
    return FLASH_IS_EQU;
  } else {
    return FLASH_REQ_WRITE;
  }
}

/*
*********************************************************************************************************
* 函 数 名: bsp_EraseCpuFlash
* 功能说明: 擦除CPU FLASH一个页，输入地址自动对齐到页起始地址
* 形    参: _ulFlashAddr : Flash地址
* 返 回 值: 0=成功，1=失败
*********************************************************************************************************
*/
uint8_t bsp_EraseCpuFlash(uint32_t _ulFlashAddr) {
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t PageError = 0;

  /* 填充擦除结构体，每次擦除1页 */
  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = bsp_GetPageAddr(_ulFlashAddr);
  EraseInitStruct.NbPages = 1;

  if (HAL_FLASH_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
    return 1;
  }

  return 0;
}

/*
*********************************************************************************************************
* 函 数 名: bsp_WriteCpuFlash
* 功能说明: 写数据到CPU内部Flash
*           写之前建议先调用bsp_EraseCpuFlash擦除目标页
* 形    参: _ulFlashAddr : Flash地址
*      _ucpSrc : 数据缓冲区
*      _ulSize : 数据大小,单位是字节
* 返 回 值: 0=成功，1=数据长度或地址溢出，2=写Flash出错
*********************************************************************************************************
*/
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc,
                          uint32_t _ulSize) {
  uint32_t i;
  uint8_t ucRet;

  if (_ulFlashAddr + _ulSize > CPU_FLASH_BASE_ADDR + CPU_FLASH_SIZE) {
    return 1;
  }

  if (_ulSize == 0) {
    return 0;
  }

  /* 检查是否需要写，避免不必要的Flash操作 */
  ucRet = bsp_CmpCpuFlash(_ulFlashAddr, _ucpSrc, _ulSize);
  if (ucRet == FLASH_IS_EQU) {
    return 0;
  }

  __disable_irq();

  /* 逐字节写入 */
  for (i = 0; i < _ulSize; i++) {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, _ulFlashAddr, _ucpSrc[i]) !=
        HAL_OK) {
      __enable_irq();
      return 2;
    }
    _ulFlashAddr++;
  }

  __enable_irq();

  return 0;
}
