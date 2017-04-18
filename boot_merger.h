/*
 * (C) Copyright 2008-2015 Fuzhou Rockchip Electronics Co., Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef BOOT_MERGER_H
#define BOOT_MERGER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>



#define SCANF_EAT(in)   fscanf(in, "%*[ \r\n\t/]")//, gEat)
#define MAX_LINE_LEN        256

typedef char line_t[MAX_LINE_LEN];



#define DEF_CONFIG_FILE     "config.ini"

#define DEF_MAJOR           2
#define DEF_MINOR           30
#define DEF_CHIP            "RK320A"
#define DEF_CODE471_NUM     1
#define DEF_CODE472_NUM     1
#define DEF_CODE471_SLEEP   0
#define DEF_CODE472_SLEEP   0
#define DEF_CODE471_PATH    "rk3288_ddr_400MHZ_v1.01.bin"
#define DEF_CODE472_PATH    "rk3288_usbplug_v2.32.bin"
#define DEF_LOADER_NUM      2
#define DEF_LOADER0         "FlashData"
#define DEF_LOADER0_PATH    "rk3288_ddr_400MHZ_v1.01.bin"
#define DEF_LOADER1         "FlashBoot"
#define DEF_LOADER1_PATH    "u-boot.bin"
#define DEF_OUT_PATH        "rk3288_bootloader_test.bin"

#define OUT_SUBFIX          ".bin"

#define SEC_CHIP            "[CHIP_NAME]"
#define SEC_VERSION         "[VERSION]"
#define SEC_471             "[CODE471_OPTION]"
#define SEC_472             "[CODE472_OPTION]"
#define SEC_LOADER          "[LOADER_OPTION]"
#define SEC_OUT             "[OUTPUT]"

#define OPT_NAME            "NAME"
#define OPT_MAJOR           "MAJOR"
#define OPT_MINOR           "MINOR"
#define OPT_NUM             "NUM"
#define OPT_LOADER_NUM      "LOADERCOUNT"
#define OPT_PATH            "Path"
#define OPT_SLEEP           "Sleep"
#define OPT_LOADER_NAME     "LOADER"
#define OPT_OUT_PATH        "PATH"

typedef struct {
	char       name[MAX_LINE_LEN];
	char       path[MAX_LINE_LEN];
} name_entry;

typedef struct {
	int         major;
	int         minor;
	char        chip[MAX_LINE_LEN];
	int         code471Sleep;
	int         code472Sleep;
	int         code471Num;
	int         code472Num;
	line_t*     code471Path;
	line_t*     code472Path;
	int         loaderNum;
	name_entry* loader;
	char        outPath[MAX_LINE_LEN];
} options;


#define TAG						0x544F4F42
#define MERGER_VERSION          0x01030000
#define SMALL_PACKET			512

#define MAX_NAME_LEN            20
#define MAX_MERGE_SIZE          (1024 << 10)

#define SEC_CHIP_TYPES          "[CHIP_TYPES]"

#define CHIP_RK28               "RK28"
#define CHIP_RK281X             "RK281X"
#define CHIP_RKPANDA            "RKPANDA"
#define CHIP_RK27               "RK27"
#define CHIP_RKNANO             "RKNANO"
#define CHIP_RKSMART            "RKSMART"
#define CHIP_RKCROWN            "RKCROWN"
#define CHIP_RKCAYMAN           "RKCAYMAN"
#define CHIP_RK29               "RK29"
#define CHIP_RK292X             "RK292X"
#define CHIP_RK30               "RK30"
#define CHIP_RK30B              "RK30B"
#define CHIP_RK31               "RK31"
#define CHIP_RK32               "RK32"

typedef enum {
	ENTRY_471       =1,
	ENTRY_472       =2,
	ENTRY_LOADER    =4,
} rk_entry_type;

#pragma pack(1)
typedef struct {
	uint16_t  year;
	uint8_t   month;
	uint8_t   day;
	uint8_t   hour;
	uint8_t   minute;
	uint8_t   second;
} rk_time;

#define  BOOT_RESERVED_SIZE 57
typedef struct {
	uint32_t        tag;
	uint16_t        size;
	uint32_t        version;
	uint32_t        mergerVersion;
	rk_time         releaseTime;
	uint32_t        chipType;
	uint8_t         code471Num;
	uint32_t        code471Offset;
	uint8_t         code471Size;
	uint8_t         code472Num;
	uint32_t        code472Offset;
	uint8_t         code472Size;
	uint8_t         loaderNum;
	uint32_t        loaderOffset;
	uint8_t         loaderSize;
	uint8_t         signFlag;
	uint8_t         rc4Flag;
	uint8_t         reserved[BOOT_RESERVED_SIZE];
} rk_boot_header;

typedef struct {
	uint8_t         size;
	rk_entry_type   type;
	uint16_t        name[MAX_NAME_LEN];
	uint32_t        dataOffset;
	uint32_t        dataSize;
	uint32_t        dataDelay;
} rk_boot_entry;
#pragma pack()

#endif// BOOT_MERGER_H
