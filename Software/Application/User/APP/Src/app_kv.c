/*
 * SPDX-FileCopyrightText: 2026 MDLZCOOL <mdlzcool@foxmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_kv.h"

#include <stdbool.h>
#include <string.h>

#include "bsp_cpu_flash.h"
#include "main.h"

#define KV_MAGIC 0x4B564150UL /* "KVAP" */
#define ENTRY_STAT_VALID 0xAA
#define ENTRY_STAT_DELETED 0x00

static uint32_t g_active_zone = 0;
static uint32_t g_active_serial = 0;
static uint32_t g_write_offset = 0;

static uint8_t kv_crc8(const uint8_t *data, uint32_t len) {
  uint8_t crc = 0xFF;
  for (uint32_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x80)
        crc = (crc << 1) ^ 0x31;
      else
        crc <<= 1;
    }
  }
  return crc;
}

static int EraseZone(uint32_t zone_addr) {
  for (uint32_t offset = 0; offset < KV_ZONE_SIZE;
       offset += KV_FLASH_PAGE_SIZE) {
    if (bsp_EraseCpuFlash(zone_addr + offset) != 0) return -1;
  }
  return 0;
}

static bool IsLatest(const char *key, uint8_t kl, uint32_t start_scan) {
  while (start_scan < g_write_offset) {
    uint8_t l_kl = *(volatile uint8_t *)start_scan;
    if (l_kl == 0xFF) break;

    uint8_t l_vl = *(volatile uint8_t *)(start_scan + 1);
    uint32_t l_len = (4 + l_kl + l_vl + 3) & ~3;

    if (l_kl == kl) {
      if (memcmp(key, (const char *)(start_scan + 4), kl) == 0) {
        return false;
      }
    }
    start_scan += l_len;
  }
  return true;
}

void app_KV_Init(void) {
  uint32_t hdrA[2] = {*(volatile uint32_t *)KV_ZONE_A_ADDR,
                      *(volatile uint32_t *)(KV_ZONE_A_ADDR + 4)};
  uint32_t hdrB[2] = {*(volatile uint32_t *)KV_ZONE_B_ADDR,
                      *(volatile uint32_t *)(KV_ZONE_B_ADDR + 4)};

  bool a_valid = (hdrA[0] == KV_MAGIC);
  bool b_valid = (hdrB[0] == KV_MAGIC);

  if (a_valid && b_valid) {
    g_active_zone = (hdrA[1] > hdrB[1]) ? KV_ZONE_A_ADDR : KV_ZONE_B_ADDR;
    g_active_serial = (hdrA[1] > hdrB[1]) ? hdrA[1] : hdrB[1];
  } else if (a_valid) {
    g_active_zone = KV_ZONE_A_ADDR;
    g_active_serial = hdrA[1];
  } else if (b_valid) {
    g_active_zone = KV_ZONE_B_ADDR;
    g_active_serial = hdrB[1];
  } else {
    EraseZone(KV_ZONE_A_ADDR);
    uint32_t init_hdr[2] = {KV_MAGIC, 1};
    bsp_WriteCpuFlash(KV_ZONE_A_ADDR, (uint8_t *)init_hdr, 8);
    g_active_zone = KV_ZONE_A_ADDR;
    g_active_serial = 1;
  }

  g_write_offset = g_active_zone + 8;
  while (g_write_offset < g_active_zone + KV_ZONE_SIZE) {
    uint8_t kl = *(volatile uint8_t *)g_write_offset;
    if (kl == 0xFF) break;

    uint8_t vl = *(volatile uint8_t *)(g_write_offset + 1);
    g_write_offset += (4 + kl + vl + 3) & ~3;
  }
}

void app_KV_Save(void) {
  uint32_t target_zone =
      (g_active_zone == KV_ZONE_A_ADDR) ? KV_ZONE_B_ADDR : KV_ZONE_A_ADDR;
  uint32_t target_offset = target_zone + 8;
  uint32_t new_serial = g_active_serial + 1;

  if (EraseZone(target_zone) != 0) return;

  uint32_t hdr[2] = {KV_MAGIC, new_serial};
  bsp_WriteCpuFlash(target_zone, (uint8_t *)hdr, 8);

  uint32_t scan = g_active_zone + 8;
  while (scan < g_write_offset) {
    uint8_t kl = *(volatile uint8_t *)scan;
    uint8_t vl = *(volatile uint8_t *)(scan + 1);
    uint8_t status = *(volatile uint8_t *)(scan + 2);
    uint32_t entry_len = (4 + kl + vl + 3) & ~3;

    if (status == ENTRY_STAT_VALID) {
      const char *key = (const char *)(scan + 4);
      if (IsLatest(key, kl, scan + entry_len)) {
        bsp_WriteCpuFlash(target_offset, (uint8_t *)scan, entry_len);
        target_offset += entry_len;
      }
    }
    scan += entry_len;
  }

  g_active_zone = target_zone;
  g_active_serial = new_serial;
  g_write_offset = target_offset;
}

static int Internal_Append(const char *key, const uint8_t *val,
                           uint16_t val_len, uint8_t status) {
  uint8_t kl = (uint8_t)strlen(key);
  uint32_t entry_len = (4 + kl + val_len + 3) & ~3;

  if (g_write_offset + entry_len > g_active_zone + KV_ZONE_SIZE) {
    app_KV_Save();
    if (g_write_offset + entry_len > g_active_zone + KV_ZONE_SIZE)
      return KV_ERR_FULL;
  }

  /* 【关键修复】使用 uint32_t 数组，强制在栈上实现 4 字节对齐，防止 M0 内核产生
   * HardFault */
  uint32_t buf_w[16];
  uint8_t *buf = (uint8_t *)buf_w;
  memset(buf, 0xFF, sizeof(buf_w));

  buf[0] = kl;
  buf[1] = (uint8_t)val_len;
  buf[2] = status;
  buf[3] = kv_crc8((const uint8_t *)key, kl) ^ kv_crc8(val, val_len);

  memcpy(&buf[4], key, kl);
  if (val_len > 0) memcpy(&buf[4 + kl], val, val_len);

  if (bsp_WriteCpuFlash(g_write_offset, buf, entry_len) != 0)
    return KV_ERR_FLASH;

  g_write_offset += entry_len;
  return KV_OK;
}

int app_KV_Set(const char *key, const uint8_t *val, uint16_t val_len) {
  if (!key || !val) return KV_ERR_EXIST;
  if (strlen(key) == 0 || strlen(key) > KV_KEY_MAX_LEN) return KV_ERR_KEYLEN;
  if (val_len > KV_VAL_MAX_LEN) return KV_ERR_VALLEN;
  return Internal_Append(key, val, val_len, ENTRY_STAT_VALID);
}

int app_KV_Get(const char *key, uint8_t *buf, uint16_t *p_len) {
  uint32_t scan = g_active_zone + 8;
  uint32_t latest_match = 0;
  uint8_t latest_vl = 0;
  uint8_t kl_target = (uint8_t)strlen(key);

  while (scan < g_write_offset) {
    uint8_t kl = *(volatile uint8_t *)scan;
    if (kl == 0xFF) break;

    uint8_t vl = *(volatile uint8_t *)(scan + 1);
    uint8_t status = *(volatile uint8_t *)(scan + 2);
    uint32_t entry_len = (4 + kl + vl + 3) & ~3;

    if (kl == kl_target && memcmp((const char *)(scan + 4), key, kl) == 0) {
      if (status == ENTRY_STAT_VALID) {
        latest_match = scan;
        latest_vl = vl;
      } else {
        latest_match = 0;
      }
    }
    scan += entry_len;
  }

  if (latest_match == 0) return KV_ERR_EXIST;

  uint16_t copy_len = (latest_vl > *p_len) ? *p_len : latest_vl;
  memcpy(buf, (const uint8_t *)(latest_match + 4 + kl_target), copy_len);
  *p_len = latest_vl;

  return KV_OK;
}

int app_KV_Del(const char *key) {
  if (!key || strlen(key) > KV_KEY_MAX_LEN) return KV_ERR_KEYLEN;
  return Internal_Append(key, NULL, 0, ENTRY_STAT_DELETED);
}

int app_KV_Count(void) {
  uint32_t scan = g_active_zone + 8;
  int count = 0;
  while (scan < g_write_offset) {
    uint8_t kl = *(volatile uint8_t *)scan;
    if (kl == 0xFF) break;
    uint8_t vl = *(volatile uint8_t *)(scan + 1);
    uint8_t status = *(volatile uint8_t *)(scan + 2);
    uint32_t entry_len = (4 + kl + vl + 3) & ~3;

    if (status == ENTRY_STAT_VALID) {
      if (IsLatest((const char *)(scan + 4), kl, scan + entry_len)) {
        count++;
      }
    }
    scan += entry_len;
  }
  return count;
}

int app_KV_Exist(const char *key) {
  uint8_t dummy[1];
  uint16_t len = 0;
  return (app_KV_Get(key, dummy, &len) == KV_OK) ? 1 : 0;
}

int app_KV_GetInt(const char *key, int32_t *p_val) {
  uint16_t len = 4;
  return app_KV_Get(key, (uint8_t *)p_val, &len);
}

int app_KV_GetUint16(const char *key, uint16_t *p_val) {
  uint16_t len = 2;
  return app_KV_Get(key, (uint8_t *)p_val, &len);
}

int app_KV_GetFloat(const char *key, float *p_val) {
  uint16_t len = 4;
  return app_KV_Get(key, (uint8_t *)p_val, &len);
}

int app_KV_SetInt(const char *key, int32_t val) {
  return app_KV_Set(key, (uint8_t *)&val, 4);
}

int app_KV_SetUint16(const char *key, uint16_t val) {
  return app_KV_Set(key, (uint8_t *)&val, 2);
}

int app_KV_SetFloat(const char *key, float val) {
  return app_KV_Set(key, (uint8_t *)&val, 4);
}

void app_KV_List(void) {
  LOG_printf("=== KV Flash List (Addr: 0x%X) ===\r\n", g_active_zone);
  uint32_t scan = g_active_zone + 8;
  while (scan < g_write_offset) {
    uint8_t kl = *(volatile uint8_t *)scan;
    if (kl == 0xFF) break;
    uint8_t vl = *(volatile uint8_t *)(scan + 1);
    uint8_t status = *(volatile uint8_t *)(scan + 2);
    uint32_t entry_len = (4 + kl + vl + 3) & ~3;

    if (status == ENTRY_STAT_VALID) {
      const char *key = (const char *)(scan + 4);
      if (IsLatest(key, kl, scan + entry_len)) {
        char tmp_key[20] = {0};
        memcpy(tmp_key, key, kl);
        LOG_printf("  [%s] len=%d\r\n", tmp_key, vl);
      }
    }
    scan += entry_len;
  }
}