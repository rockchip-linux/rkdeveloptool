/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "RKDevice.h"

const char* szManufName[] =
{
	"SAMSUNG",
	"TOSHIBA",
	"HYNIX",
	"INFINEON",
	"MICRON",
	"RENESAS",
	"ST",
	"INTEL"
};

void CRKDevice::SetVendorID(USHORT value)
{
	m_vid = value;
}
void CRKDevice::SetProductID(USHORT value)
{
	m_pid = value;
}
void CRKDevice::SetDeviceType(ENUM_RKDEVICE_TYPE value)
{
	m_device = value;
}
void CRKDevice::SetOsType(ENUM_OS_TYPE value)
{
	m_os = value;
}

void CRKDevice::SetUsbType(ENUM_RKUSB_TYPE value)
{
	m_usb = value;
}
void CRKDevice::SetBcdUsb(USHORT value)
{
	m_bcdUsb = value;
}
void CRKDevice::SetLayerName(char *value)
{
	strcpy(m_layerName,value);
}
void CRKDevice::SetLocationID(DWORD value)
{
	m_locationID = value;
}

void CRKDevice::SetCallBackPointer(ProgressPromptCB value)
{
	if (value)
	{
		m_callBackProc = value;
	}
}
CRKLog* CRKDevice::GetLogObjectPointer()
{
	return m_pLog;
}

CRKComm* CRKDevice::GetCommObjectPointer()
{
	return m_pComm;
}

USHORT CRKDevice::GetVendorID()
{
	return m_vid;
}
USHORT CRKDevice::GetProductID()
{
	return m_pid;
}
ENUM_RKDEVICE_TYPE CRKDevice::GetDeviceType()
{
	return m_device;
}
ENUM_OS_TYPE CRKDevice::GetOsType()
{
	return m_os;
}

ENUM_RKUSB_TYPE CRKDevice::GetUsbType()
{
	return m_usb;
}

USHORT CRKDevice::GetBcdUsb()
{
	return m_bcdUsb;
}
DWORD CRKDevice::GetLocationID()
{
	return m_locationID;
}
char* CRKDevice::GetLayerName()
{
	return m_layerName;
}

string CRKDevice::GetLayerString(DWORD dwLocationID)
{
	char szLocation[32] = "\0";
	snprintf(szLocation, sizeof(szLocation), "%d-%d", dwLocationID >> 8, dwLocationID & 0xff);
	return szLocation;
}

CRKDevice::CRKDevice(STRUCT_RKDEVICE_DESC &device)
{
	VendorID.setContainer(this);
    	VendorID.getter(&CRKDevice::GetVendorID);
 	VendorID.setter(&CRKDevice::SetVendorID);

	ProductID.setContainer(this);
    	ProductID.getter(&CRKDevice::GetProductID);
 	ProductID.setter(&CRKDevice::SetProductID);

	DeviceType.setContainer(this);
    	DeviceType.getter(&CRKDevice::GetDeviceType);
 	DeviceType.setter(&CRKDevice::SetDeviceType);

	UsbType.setContainer(this);
    	UsbType.getter(&CRKDevice::GetUsbType);
 	UsbType.setter(&CRKDevice::SetUsbType);

	LayerName.setContainer(this);
    	LayerName.getter(&CRKDevice::GetLayerName);
 	LayerName.setter(&CRKDevice::SetLayerName);

	BcdUsb.setContainer(this);
    	BcdUsb.getter(&CRKDevice::GetBcdUsb);
 	BcdUsb.setter(&CRKDevice::SetBcdUsb);

	LocationID.setContainer(this);
    	LocationID.getter(&CRKDevice::GetLocationID);
	LocationID.setter(&CRKDevice::SetLocationID);

	OsType.setContainer(this);
    	OsType.getter(&CRKDevice::GetOsType);
 	OsType.setter(&CRKDevice::SetOsType);

	LogObjectPointer.setContainer(this);
    	LogObjectPointer.getter(&CRKDevice::GetLogObjectPointer);

	CommObjectPointer.setContainer(this);
    	CommObjectPointer.getter(&CRKDevice::GetCommObjectPointer);

	CallBackPointer.setContainer(this);
 	CallBackPointer.setter(&CRKDevice::SetCallBackPointer);

	m_vid = device.usVid;
	m_pid = device.usPid;
	m_usb = device.emUsbType;
	m_device = device.emDeviceType;
	m_bcdUsb = device.usbcdUsb;
	m_locationID = device.uiLocationID;
	strcpy(m_layerName, GetLayerString(m_locationID).c_str());

	memset(m_flashInfo.blockState, 0, IDBLOCK_TOP);
	m_flashInfo.usPhyBlokcPerIDB = 1;
	m_flashInfo.uiSecNumPerIDB = 0;
	m_callBackProc = NULL;
	m_chipData = NULL;
	m_pImage = NULL;
	m_pLog = NULL;
	m_pComm = NULL;
	m_pFlashInfoData = NULL;
	m_usFlashInfoDataLen = 0;
	m_usFlashInfoDataOffset = 0;
	m_bEmmc = false;
	m_bDirectLba = false;
	m_bFirst4mAccess = false;
}
CRKDevice::~CRKDevice()
{
	if (m_pComm) {
		delete m_pComm;
		m_pComm = NULL;
	}
	if (m_chipData) {
		delete []m_chipData;
		m_chipData = NULL;
	}

	if (m_pFlashInfoData) {
		delete []m_pFlashInfoData;
		m_pFlashInfoData = NULL;
	}
}
bool CRKDevice::SetObject(CRKImage *pImage, CRKComm *pComm, CRKLog *pLog)
{
	if (!pComm) {
		return false;
	}
	m_pImage = pImage;
	m_pComm = pComm;
	m_pLog = pLog;
	if (m_pImage) {
		m_os = m_pImage->OsType;
	} else
		m_os = RK_OS;
	return true;
}
int CRKDevice::EraseEmmcBlock(UCHAR ucFlashCS, DWORD dwPos, DWORD dwCount)
{
	int sectorOffset,nWrittenBlcok,iRet;
	BYTE emptyData[4 * (SECTOR_SIZE+SPARE_SIZE)];
	memset(emptyData, 0xff, 4 * (SECTOR_SIZE + SPARE_SIZE));
	nWrittenBlcok = 0;
	while (dwCount > 0) {
 		sectorOffset = (ucFlashCS * m_flashInfo.uiBlockNum + dwPos + nWrittenBlcok) * m_flashInfo.uiSectorPerBlock;
		iRet = m_pComm->RKU_WriteSector(sectorOffset, 4, emptyData);
		if ((iRet != ERR_SUCCESS) && (iRet != ERR_FOUND_BAD_BLOCK)) {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:EraseEmmcBlock-->RKU_WriteSector failed, RetCode(%d)", m_layerName, iRet);
			}
			return iRet;
		}
		dwCount--;
		nWrittenBlcok++;
	}
	return ERR_SUCCESS;
}
int CRKDevice::EraseEmmcByWriteLBA(DWORD dwSectorPos, DWORD dwCount)
{
	int nWritten,iRet;
	BYTE emptyData[32 * SECTOR_SIZE];
	memset(emptyData, 0xff, 32 * SECTOR_SIZE);

	while (dwCount > 0) {
		nWritten = (dwCount < 32) ? dwCount : 32;
		iRet = m_pComm->RKU_WriteLBA(dwSectorPos, nWritten, emptyData);
		if (iRet != ERR_SUCCESS) {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:EraseEmmcByWriteLBA-->RKU_WriteLBA failed, RetCode(%d)", m_layerName, iRet);
			}
			return iRet;
		}
		dwCount -= nWritten;
		dwSectorPos += nWritten;
	}
	return ERR_SUCCESS;
}
bool CRKDevice::EraseEmmc()
{
	UINT uiCount,uiEraseCount,uiSectorOffset,uiTotalCount;
	UINT uiErase=1024*32;
	int iRet=ERR_SUCCESS,iLoopTimes=0;
	uiTotalCount = uiCount = m_flashInfo.uiFlashSize*2*1024;
	uiSectorOffset = 0;
	DWORD dwLayerID;
	dwLayerID = m_locationID;
	ENUM_CALL_STEP emCallStep = CALL_FIRST;

	while (uiCount)
	{
		if (uiCount >= uiErase)
		{
			uiEraseCount = uiErase;
		}
		else
			uiEraseCount = uiCount;
		iRet = m_pComm->RKU_EraseLBA(uiSectorOffset, uiEraseCount);
			
		if (iRet != ERR_SUCCESS) {
			if (m_pLog) {
				m_pLog->Record("ERROR:EraseEmmc-->RKU_EraseLBA failed,RetCode(%d),offset=0x%x,count=0x%x",iRet, uiSectorOffset, uiEraseCount);
			}
			return false;
		}
		uiCount -= uiEraseCount;
		uiSectorOffset += uiEraseCount;
		iLoopTimes++;
		if (iLoopTimes % 8 == 0) {
			if (m_callBackProc) {
				m_callBackProc(dwLayerID, ERASEFLASH_PROGRESS, uiTotalCount, uiSectorOffset, emCallStep);
				emCallStep = CALL_MIDDLE;
			}
		}
	}
	if (m_callBackProc) {
		emCallStep = CALL_LAST;
		m_callBackProc(dwLayerID, ERASEFLASH_PROGRESS, uiTotalCount, uiTotalCount, emCallStep);
	}
	return true;
}
bool CRKDevice::GetFlashInfo()
{
	STRUCT_FLASHINFO_CMD info;
	BYTE flashID[5];
	int iRet;
	UINT uiRead;
	iRet = m_pComm->RKU_ReadFlashInfo((PBYTE)&info, &uiRead);
	if( ERR_SUCCESS == iRet ) {
		if ((info.usBlockSize == 0) || (info.bPageSize == 0)) {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:GetFlashInfo-->RKU_ReadFlashInfo failed,pagesize or blocksize is zero", m_layerName);
			}
			return false;
		}
		if (info.bManufCode <= 7) {
			strcpy(m_flashInfo.szManufacturerName, szManufName[info.bManufCode]);
		} else {
			strcpy(m_flashInfo.szManufacturerName, "UNKNOWN");
		}
		m_flashInfo.uiFlashSize = info.uiFlashSize / 2 / 1024;
		m_flashInfo.uiPageSize = info.bPageSize / 2;
		m_flashInfo.usBlockSize = info.usBlockSize / 2;
		m_flashInfo.bECCBits = info.bECCBits;
		m_flashInfo.bAccessTime = info.bAccessTime;
		m_flashInfo.uiBlockNum = m_flashInfo.uiFlashSize * 1024 / m_flashInfo.usBlockSize;
		m_flashInfo.uiSectorPerBlock = info.usBlockSize;
		m_flashInfo.bFlashCS = info.bFlashCS;
		m_flashInfo.usValidSecPerBlock = (info.usBlockSize / info.bPageSize) * 4;
		if (m_pFlashInfoData) {
			delete []m_pFlashInfoData;
			m_pFlashInfoData = NULL;
		}
		m_usFlashInfoDataLen = BYTE2SECTOR(uiRead);
		m_pFlashInfoData = new BYTE[SECTOR_SIZE * m_usFlashInfoDataLen];
		memset(m_pFlashInfoData, 0, SECTOR_SIZE * m_usFlashInfoDataLen);
		memcpy(m_pFlashInfoData, (PBYTE)&info, uiRead);
		if (m_pLog) {
			string strFlashInfo;
			m_pLog->PrintBuffer(strFlashInfo, m_pFlashInfoData, 11);
			m_pLog->Record("<LAYER %s> INFO:FlashInfo:%s", m_layerName, strFlashInfo.c_str());
		}
	} else {
		if (m_pLog) {
			m_pLog->Record("<LAYER %s> ERROR:GetFlashInfo-->RKU_ReadFlashInfo failed, RetCode(%d)", m_layerName, iRet);
		}
		return false;
	}
	iRet = m_pComm->RKU_ReadFlashID(flashID);
	if( ERR_SUCCESS == iRet ) {
		DWORD *pID = (DWORD *)flashID;
		if (*pID==0x434d4d45)/*emmc*/ {
			m_bEmmc = true;
		} else
			m_bEmmc = false;
	} else {
		if (m_pLog) {
			m_pLog->Record("<LAYER %s> ERROR:GetFlashInfo-->RKU_ReadFlashID failed, RetCode(%d)", m_layerName, iRet);
		}
		return false;
	}
	return true;
}
bool CRKDevice::TestDevice()
{
	int iResult, iTryCount;
	DWORD dwTotal, dwCurrent, dwLayerID;
	dwLayerID = m_locationID;
	ENUM_CALL_STEP emCallStep = CALL_FIRST;
	do {
		iTryCount = 3;
		while (iTryCount > 0) {
			iResult = m_pComm->RKU_TestDeviceReady(&dwTotal, &dwCurrent);
			if ((iResult == ERR_SUCCESS) || (iResult == ERR_DEVICE_UNREADY)) {
				break;
			}
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:TestDevice-->RKU_TestDeviceReady failed, RetCode(%d)", m_layerName, iResult);
			}
			iTryCount--;
			sleep(1);
		}
		if (iTryCount <= 0) {
			return false;
		}

		if (iResult == ERR_SUCCESS) {
			if (emCallStep == CALL_MIDDLE) {
				if (m_callBackProc) {
					dwCurrent = dwTotal;
					emCallStep = CALL_LAST;
					m_callBackProc(dwLayerID, TESTDEVICE_PROGRESS, dwTotal, dwCurrent, emCallStep);
				}
			}
			break;
		}
		if (dwCurrent>dwTotal) {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:TestDevice-->RKU_TestDeviceReady failed,Total=%d, Current=%d", m_layerName, dwTotal, dwCurrent);
			}
			return false;
		}
		if (UsbType == RKUSB_LOADER) {
			if (dwTotal == 0) {
				if (m_pLog)
				{
					m_pLog->Record("<LAYER %s> ERROR:TestDevice-->RKU_TestDeviceReady failed, Total is zero", m_layerName);
				}
				return false;
			}
		}
		if (m_callBackProc)
		{
			m_callBackProc(dwLayerID, TESTDEVICE_PROGRESS, dwTotal, dwCurrent, emCallStep);
			emCallStep = CALL_MIDDLE;
		}
		sleep(1);
	}while(iResult == ERR_DEVICE_UNREADY);
	return true;
}
bool CRKDevice::ResetDevice()
{
	int iRet;
	iRet = m_pComm->RKU_ResetDevice();
	if (iRet == ERR_SUCCESS) {
		return true;
	} else {
		bool bRet = false;
		if ((iRet == -2) || (iRet == -4)) {
			bRet = true;
		}
		if (m_pLog) {
			m_pLog->Record("<LAYER %s> ERROR:ResetDevice-->RKU_ResetDevice failed, RetCode(%d)", m_layerName, iRet);
		}
		return bRet;
	}
}

bool CRKDevice::PowerOffDevice()
{
	int iRet;
	iRet = m_pComm->RKU_ResetDevice(RST_POWEROFF_SUBCODE);
	if (iRet == ERR_SUCCESS) {
		return true;
	} else {
		if (m_pLog) {
			m_pLog->Record("<LAYER %s> ERROR:PowerOffDevice-->RKU_ResetDevice failed, RetCode(%d)", m_layerName, iRet);
		}
		return false;
	}
}
bool CRKDevice::CheckChip()
{
	int iRet;
	BYTE bChipInfo[CHIPINFO_LEN];
	ENUM_RKDEVICE_TYPE curDeviceType = RKNONE_DEVICE;
	memset(bChipInfo, 0, CHIPINFO_LEN);
	iRet = m_pComm->RKU_ReadChipInfo(bChipInfo);
	if (iRet == ERR_SUCCESS) {
		if (!m_chipData) {
			m_chipData = new BYTE[CHIPINFO_LEN];
		}
		memset(m_chipData, 0, CHIPINFO_LEN);
		memcpy(m_chipData, bChipInfo, CHIPINFO_LEN);
		DWORD *pValue;
		pValue = (DWORD *)(&bChipInfo[0]);

		if ((ENUM_RKDEVICE_TYPE)(*pValue) == m_device) {
			return true;
		}
		if (*pValue == 0x524B3237) {
			curDeviceType = RK27_DEVICE;
		} else if (*pValue == 0x32373341) {
			curDeviceType = RKCAYMAN_DEVICE;
		} else if (*pValue == 0x524B3238) {
			curDeviceType = RK28_DEVICE;
		} else if (*pValue == 0x32383158) {
			curDeviceType = RK281X_DEVICE;
		} else if (*pValue == 0x32383242) {
			curDeviceType = RKPANDA_DEVICE;
		} else if (*pValue == 0x32393058) {
			curDeviceType = RK29_DEVICE;
		} else if (*pValue == 0x32393258) {
			curDeviceType = RK292X_DEVICE;
		} else if (*pValue == 0x33303041) {
			curDeviceType = RK30_DEVICE;
		} else if (*pValue == 0x33313041) {
			curDeviceType = RK30B_DEVICE;
		} else if (*pValue == 0x33313042) {
			curDeviceType = RK31_DEVICE;
		} else if (*pValue == 0x33323041) {
			curDeviceType = RK32_DEVICE;
		} else if (*pValue == 0x32363243) {
			curDeviceType = RKSMART_DEVICE;
		} else if (*pValue == 0x6E616E6F) {
			curDeviceType = RKNANO_DEVICE;
		} else if (*pValue == 0x4E4F5243) {
			curDeviceType = RKCROWN_DEVICE;
		}

		if (curDeviceType == m_device){
			return true;
		} else {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:CheckChip-->Chip is not match, firmware(0x%x), device(0x%x)", m_layerName, m_device, *pValue);
			}
			return false;
		}
	}
	else {
		if (m_pLog) {
			m_pLog->Record("<LAYER %s> ERROR:CheckChip-->RKU_ReadChipInfo failed,RetCode(%d)", m_layerName, iRet);
		}
		return false;
	}
}

int CRKDevice::DownloadBoot()
{
	UCHAR i;
	DWORD dwSize, dwDelay;
	PBYTE pBuffer = NULL;
	for ( i = 0; i < m_pImage->m_bootObject->Entry471Count; i++ ) {
		if ( !m_pImage->m_bootObject->GetEntryProperty(ENTRY471, i, dwSize, dwDelay) ) {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:DownloadBoot-->GetEntry471Property failed,index(%d)", m_layerName, i);
			}
			return -2;
		}
		if (dwSize>0) {
			pBuffer = new BYTE[dwSize];
			if ( !m_pImage->m_bootObject->GetEntryData(ENTRY471, i, pBuffer) ) {
				if (m_pLog) {
					m_pLog->Record("<LAYER %s> ERROR:DownloadBoot-->GetEntry471Data failed,index(%d)", m_layerName, i);
				}
				delete []pBuffer;
				return -3;
			}
			if ( !Boot_VendorRequest(0x0471,pBuffer,dwSize) ) {
				if (m_pLog) {
					m_pLog->Record("<LAYER %s> ERROR:DownloadBoot-->Boot_VendorRequest471 failed,index(%d)", m_layerName, i);
				}
				delete []pBuffer;
				return -4;
			}
			delete []pBuffer;
			pBuffer = NULL;
			if (dwDelay>0) {
				usleep(dwDelay * 1000);
			}

		}
	}

	for ( i=0; i < m_pImage->m_bootObject->Entry472Count; i++ ) {
		if ( !m_pImage->m_bootObject->GetEntryProperty(ENTRY472, i, dwSize, dwDelay) ) {
			if (m_pLog) {
				m_pLog->Record("<LAYER %s> ERROR:DownloadBoot-->GetEntry472Property failed,index(%d)", m_layerName, i);
			}
			return -2;
		}
		if (dwSize > 0) {
			pBuffer = new BYTE[dwSize];
			if ( !m_pImage->m_bootObject->GetEntryData(ENTRY472, i, pBuffer) ) {
				if (m_pLog) {
					m_pLog->Record("<LAYER %s> ERROR:DownloadBoot-->GetEntry472Data failed,index(%d)", m_layerName, i);
				}
				delete []pBuffer;
				return -3;
			}
			if ( !Boot_VendorRequest(0x0472, pBuffer, dwSize) ) {
				if (m_pLog) {
					m_pLog->Record("<LAYER %s> ERROR:DownloadBoot-->Boot_VendorRequest472 failed,index(%d)", m_layerName, i);
				}
				delete []pBuffer;
				return -4;
			}
			delete []pBuffer;
			pBuffer = NULL;
			if (dwDelay > 0) {
				usleep(dwDelay * 1000);
			}
		}
	}
	sleep(1);
	return 0;

}
bool CRKDevice::Boot_VendorRequest( DWORD requestCode, PBYTE pBuffer, DWORD dwDataSize)
{
	int iRet;
	iRet = m_pComm->RKU_DeviceRequest(requestCode, pBuffer, dwDataSize);
	return (iRet == ERR_SUCCESS) ? true : false;
}
int CRKDevice::EraseAllBlocks(bool force_block_erase)
{
	int i;
	UINT uiBlockCount;
	int iRet = ERR_SUCCESS, iErasePos = 0, iEraseBlockNum = 0, iEraseTimes = 0, iCSIndex = 0;
	BYTE bCSCount = 0;
	for (i = 0; i < 8; i++) {
		if ( m_flashInfo.bFlashCS & (1 << i) ) {
			bCSCount++;
		}
	}
	ReadCapability();
	DWORD dwLayerID;
	dwLayerID = LocationID;
	ENUM_CALL_STEP emCallStep = CALL_FIRST;
	if (!force_block_erase) {
		if ((m_bEmmc)||(m_bDirectLba)) {
			if (!EraseEmmc()) {
				if (m_pLog) {
					m_pLog->Record("<LAYER %s> ERROR:EraseAllBlocks-->EraseEmmc failed", m_layerName);
				}
				return -1;
			}
			return 0;
		}
	}
	for (i = 0; i < 8; i++) {
		if ( m_flashInfo.bFlashCS & (1 << i) ) {
			uiBlockCount = m_flashInfo.uiBlockNum;
			iErasePos = 0;
			iEraseTimes = 0;
			while (uiBlockCount > 0) {
				iEraseBlockNum = (uiBlockCount < MAX_ERASE_BLOCKS) ? uiBlockCount : MAX_ERASE_BLOCKS;
				iRet = m_pComm->RKU_EraseBlock(i, iErasePos, iEraseBlockNum, ERASE_FORCE);
				if ((iRet != ERR_SUCCESS) && (iRet != ERR_FOUND_BAD_BLOCK)) {
					if (m_pLog) {
						m_pLog->Record("<LAYER %s> ERROR:EraseAllBlocks-->RKU_EraseBlock failed,RetCode(%d)", m_layerName, iRet);
					}
					return -1;
				}
				iErasePos += iEraseBlockNum;
				uiBlockCount -= iEraseBlockNum;
				iEraseTimes++;
				if (iEraseTimes % 8 == 0) {
					if (m_callBackProc) {
						m_callBackProc(dwLayerID, ERASEFLASH_PROGRESS, m_flashInfo.uiBlockNum * bCSCount, iCSIndex * m_flashInfo.uiBlockNum + iErasePos, emCallStep);
						emCallStep = CALL_MIDDLE;
					}
				}
			}
			iCSIndex++;
		}
	}

	if (m_callBackProc) {
		emCallStep = CALL_LAST;
		m_callBackProc(dwLayerID, ERASEFLASH_PROGRESS, m_flashInfo.uiBlockNum * bCSCount, iCSIndex * m_flashInfo.uiBlockNum, emCallStep);
	}
	return 0;
}
bool CRKDevice::ReadCapability()
{
	int ret;
	BYTE data[8];
	ret = m_pComm->RKU_ReadCapability(data);
	if (ret != ERR_SUCCESS)
	{
		if (m_pLog)
		{
			m_pLog->Record("ERROR:ReadCapability-->RKU_ReadCapability failed,err(%d)", ret);
		}
		return false;
	}
	if (data[0] & 0x1)
	{
		m_bDirectLba = true;
	}
	else
		m_bDirectLba = false;
	if (data[0] & 0x4)
	{
		m_bFirst4mAccess = true;
	}
	else
		m_bFirst4mAccess = false;
	return true;
}


