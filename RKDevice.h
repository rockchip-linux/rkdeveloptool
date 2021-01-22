#ifndef RKDEVICE_HEADER
#define RKDEVICE_HEADER
#include "RKImage.h"
#include "RKComm.h"
#include "RKLog.h"
#include "DefineHeader.h"

#define SECTOR_SIZE 512
#define PAGE_SIZE 2048
#define SPARE_SIZE 16
#define CHIPINFO_LEN 16
#define IDBLOCK_TOP 50

#define CALC_UNIT(a, b)		((a > 0) ? ((a - 1) / b + 1) : (a))
#define BYTE2SECTOR(x)		(CALC_UNIT(x, SECTOR_SIZE))
#define PAGEALIGN(x)		(CALC_UNIT(x, 4))

#pragma pack(1)
typedef struct _STRUCT_FLASH_INFO {
	char	szManufacturerName[16];
	UINT	uiFlashSize;
	USHORT	usBlockSize;
	UINT	uiPageSize;
	UINT	uiSectorPerBlock;
	BYTE	blockState[IDBLOCK_TOP];
	UINT	uiBlockNum;
	BYTE	bECCBits;
	BYTE	bAccessTime;
	BYTE	bFlashCS;
	USHORT  usValidSecPerBlock;
	USHORT  usPhyBlokcPerIDB;
	UINT    uiSecNumPerIDB;
} STRUCT_FLASH_INFO, *PSTRUCT_FLASH_INFO;
typedef struct _STRUCT_FLASHINFO_CMD {
	UINT	uiFlashSize;
	USHORT	usBlockSize;
	BYTE	bPageSize;
	BYTE	bECCBits;
	BYTE	bAccessTime;
	BYTE	bManufCode;
	BYTE	bFlashCS;
	BYTE	reserved[501];
} STRUCT_FLASHINFO_CMD, *PSTRUCT_FLASHINFO_CMD;
#pragma pack()

class CRKDevice
{
public:
	USHORT GetVendorID();
	void SetVendorID(USHORT value);
 	property<CRKDevice, USHORT, READ_WRITE> VendorID;

	USHORT GetProductID();
	void SetProductID(USHORT value);
 	property<CRKDevice, USHORT, READ_WRITE> ProductID;

	ENUM_RKDEVICE_TYPE GetDeviceType();
	void SetDeviceType(ENUM_RKDEVICE_TYPE value);
 	property<CRKDevice, ENUM_RKDEVICE_TYPE, READ_WRITE> DeviceType;

	ENUM_RKUSB_TYPE GetUsbType();
	void SetUsbType(ENUM_RKUSB_TYPE value);
 	property<CRKDevice, ENUM_RKUSB_TYPE, READ_WRITE> UsbType;

	char *GetLayerName();
	void SetLayerName(char *value);
 	property<CRKDevice, char *, READ_WRITE> LayerName;

	DWORD GetLocationID();
	void SetLocationID(DWORD value);
 	property<CRKDevice, DWORD, READ_WRITE> LocationID;

	USHORT GetBcdUsb();
	void SetBcdUsb(USHORT value);
 	property<CRKDevice, USHORT, READ_WRITE> BcdUsb;

	ENUM_OS_TYPE GetOsType();
	void SetOsType(ENUM_OS_TYPE value);
 	property<CRKDevice, ENUM_OS_TYPE, READ_WRITE> OsType;

	CRKLog *GetLogObjectPointer();
 	property<CRKDevice, CRKLog *, READ_ONLY> LogObjectPointer;

	CRKComm *GetCommObjectPointer();
 	property<CRKDevice, CRKComm *, READ_ONLY> CommObjectPointer;

	void SetCallBackPointer(ProgressPromptCB value);
 	property<CRKDevice, ProgressPromptCB, WRITE_ONLY> CallBackPointer;

	int DownloadBoot();
	bool TestDevice();
	bool ResetDevice();
	bool PowerOffDevice();
	bool CheckChip();
	bool GetFlashInfo();
	int EraseAllBlocks(bool force_block_erase=false);
	bool SetObject(CRKImage *pImage, CRKComm *pComm, CRKLog *pLog);
	string GetLayerString(DWORD dwLocationID);
	CRKDevice(STRUCT_RKDEVICE_DESC &device);
	~CRKDevice();
protected:
	STRUCT_FLASH_INFO m_flashInfo;
	PBYTE	m_pFlashInfoData;
	USHORT	m_usFlashInfoDataOffset;
	USHORT  m_usFlashInfoDataLen;
	PBYTE  m_chipData;
	CRKImage *m_pImage;
	CRKComm  *m_pComm;
	CRKLog   *m_pLog;
	ProgressPromptCB m_callBackProc;
	bool m_bEmmc;
	bool m_bDirectLba;
	bool m_bFirst4mAccess;
	int EraseEmmcBlock(UCHAR ucFlashCS, DWORD dwPos, DWORD dwCount);
	int EraseEmmcByWriteLBA(DWORD dwSectorPos, DWORD dwCount);
	bool EraseEmmc();
	bool Boot_VendorRequest(DWORD requestCode, PBYTE pBuffer, DWORD dwDataSize);
	bool ReadCapability();
private:
	USHORT m_vid;
	USHORT m_pid;
	ENUM_RKDEVICE_TYPE m_device;
	ENUM_OS_TYPE m_os;
	ENUM_RKUSB_TYPE m_usb;
	UINT m_locationID;
	USHORT m_bcdUsb;
protected:
	char  m_layerName[32];
};

#endif