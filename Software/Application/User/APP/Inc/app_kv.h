/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _APP_KV_H_
#define _APP_KV_H_

#include <stdint.h>

/*-----------------------------------------------------------------------
 * 容量与 Flash 布局限制 (针对 CS32L010F8 优化)
 *-----------------------------------------------------------------------*/
#define KV_MAX_ENTRIES 13 /* API兼容保留 */
#define KV_KEY_MAX_LEN 16 /* 键名最长16字符  */
#define KV_VAL_MAX_LEN 32 /* 值最长32字节    */

/*
 * 乒乓双区设置 (极度安全防掉电)
 * CS32L010F8 的 Flash 映射在 0x00000000!
 */
#define KV_FLASH_PAGE_SIZE 512
#define KV_ZONE_SIZE 1024 /* 1KB = 2 Pages */

/* 必须使用 0x0000XXXX 基地址! (原版错写成了0x0800) */
#define KV_ZONE_A_ADDR 0x0000F800U /* Page 124, 125 */
#define KV_ZONE_B_ADDR 0x0000FC00U /* Page 126, 127 */

/*-----------------------------------------------------------------------
 * 返回值
 *-----------------------------------------------------------------------*/
#define KV_OK 0
#define KV_ERR_EXIST 1
#define KV_ERR_FULL 2
#define KV_ERR_KEYLEN 3
#define KV_ERR_VALLEN 4
#define KV_ERR_FLASH 5

/* API 接口 */
void app_KV_Init(void);
int app_KV_Set(const char *key, const uint8_t *val, uint16_t val_len);
int app_KV_Get(const char *key, uint8_t *buf, uint16_t *p_len);
int app_KV_GetInt(const char *key, int32_t *p_val);
int app_KV_GetUint16(const char *key, uint16_t *p_val);
int app_KV_GetFloat(const char *key, float *p_val);
int app_KV_SetInt(const char *key, int32_t val);
int app_KV_SetUint16(const char *key, uint16_t val);
int app_KV_SetFloat(const char *key, float val);
int app_KV_Del(const char *key);
void app_KV_Save(void);
int app_KV_Count(void);
int app_KV_Exist(const char *key);
void app_KV_List(void);

#endif