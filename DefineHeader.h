#ifndef DEFINE_HEADER
#define DEFINE_HEADER
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <iconv.h>
#include <wchar.h>
#include <errno.h>
#include <pthread.h>
#include <libusb.h>

#include "Property.hpp"
#include <list>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;
typedef unsigned char BYTE, *PBYTE;
typedef unsigned char UCHAR;
typedef unsigned short WCHAR;
typedef unsigned short USHORT;
typedef unsigned int	UINT;
typedef unsigned int	DWORD;

typedef enum{
	RKNONE_DEVICE = 0,
	RK27_DEVICE = 0x10,
	RKCAYMAN_DEVICE,
	RK28_DEVICE = 0x20,
	RK281X_DEVICE,
	RKPANDA_DEVICE,
	RKNANO_DEVICE = 0x30,
	RKSMART_DEVICE,
	RKCROWN_DEVICE = 0x40,
	RK29_DEVICE = 0x50,
	RK292X_DEVICE,
	RK30_DEVICE = 0x60,
	RK30B_DEVICE,
	RK31_DEVICE = 0x70,
	RK32_DEVICE = 0x80
} ENUM_RKDEVICE_TYPE;
typedef enum{
	RK_OS = 0,
	ANDROID_OS = 0x1
} ENUM_OS_TYPE;

typedef enum{
	RKUSB_NONE = 0x0,
	RKUSB_MASKROM = 0x01,
	RKUSB_LOADER = 0x02,
	RKUSB_MSC = 0x04
} ENUM_RKUSB_TYPE;
typedef enum{
	ENTRY471 = 1,
	ENTRY472 = 2,
	ENTRYLOADER = 4
} ENUM_RKBOOTENTRY;

#pragma pack(1)
typedef struct{
	USHORT	usYear;
	BYTE	ucMonth;
	BYTE  	ucDay;
	BYTE  	ucHour;
	BYTE  	ucMinute;
	BYTE  	ucSecond;
} STRUCT_RKTIME, *PSTRUCT_RKTIME;

typedef struct{
	char szItemName[20];
	char szItemValue[256];
} STRUCT_CONFIG_ITEM, *PSTRUCT_CONFIG_ITEM;
typedef struct _STRUCT_RKDEVICE_DESC{
	USHORT usVid;
	USHORT usPid;
	USHORT usbcdUsb;
	UINT     uiLocationID;
	ENUM_RKUSB_TYPE emUsbType;
	ENUM_RKDEVICE_TYPE emDeviceType;
	void   *pUsbHandle;
} STRUCT_RKDEVICE_DESC, *PSTRUCT_RKDEVICE_DESC;
#pragma pack()
typedef list<STRUCT_RKDEVICE_DESC> RKDEVICE_DESC_SET;
typedef RKDEVICE_DESC_SET::iterator device_list_iter;
typedef vector<string> STRING_VECTOR;
typedef vector<UINT> UINT_VECTOR;
typedef vector<STRUCT_CONFIG_ITEM> CONFIG_ITEM_VECTOR;
typedef enum{
	TESTDEVICE_PROGRESS,
	DOWNLOADIMAGE_PROGRESS,
	CHECKIMAGE_PROGRESS,
	TAGBADBLOCK_PROGRESS,
	TESTBLOCK_PROGRESS,
	ERASEFLASH_PROGRESS,
	ERASESYSTEM_PROGRESS,
	LOWERFORMAT_PROGRESS,
	ERASEUSERDATA_PROGRESS
} ENUM_PROGRESS_PROMPT;

typedef enum{
	CALL_FIRST,
	CALL_MIDDLE,
	CALL_LAST
} ENUM_CALL_STEP;

typedef void (*ProgressPromptCB)(DWORD deviceLayer, ENUM_PROGRESS_PROMPT promptID, long long totalValue, long long currentValue, ENUM_CALL_STEP emCall);

bool WideStringToString(wchar_t *pszSrc, char *&pszDest);
bool StringToWideString(char *pszSrc, wchar_t *&pszDest);
#endif