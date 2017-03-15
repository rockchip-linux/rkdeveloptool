/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include   <unistd.h>
#include   <dirent.h>
#include "DefineHeader.h"
#include "RKLog.h"
#include "RKScan.h"
#include "RKComm.h"
#include "RKDevice.h"
#include "RKImage.h"
extern const char *szManufName[];
CRKLog *g_pLogObject=NULL;
CONFIG_ITEM_VECTOR g_ConfigItemVec;
#define DEFAULT_RW_LBA 128
#define CURSOR_MOVEUP_LINE(n) printf("%c[%dA", 0x1B, n)
#define CURSOR_DEL_LINE printf("%c[2K", 0x1B)
#define CURSOR_MOVE_HOME printf("%c[H", 0x1B)
#define CURSOR_CLEAR_SCREEN printf("%c[2J", 0x1B)
#define ERROR_COLOR_ATTR  printf("%c[30;41m", 0x1B);
#define NORMAL_COLOR_ATTR  printf("%c[37;40m", 0x1B);
void usage()
{
	printf("\r\n---------------------Tool Usage ---------------------\r\n");
	printf("Help:             -H\r\n");
	printf("Version:          -V\r\n");
	printf("DownloadBoot:	DB <Loader>\r\n");
	printf("ReadLBA:		RL  <BeginSec> <SectorLen> <File>\r\n");
	printf("WriteLBA:		WL  <BeginSec> <File>\r\n");
	printf("EraseFlash:		EF \r\n");
	printf("TestDevice:		TD\r\n");
	printf("ResetDevice:	RD [subcode]\r\n");
	printf("ReadFlashID:	RID\r\n");
	printf("ReadFlashInfo:	RFI\r\n");
	printf("ReadChipInfo:	RCI\r\n");
	printf("-------------------------------------------------------\r\n\r\n");
}
void ProgressInfoProc(DWORD deviceLayer, ENUM_PROGRESS_PROMPT promptID, long long totalValue, long long currentValue, ENUM_CALL_STEP emCall)
{
	string strInfoText="";
	char szText[256];
	switch (promptID) {
	case TESTDEVICE_PROGRESS:
		sprintf(szText, "Test Device Total(%lld),Current(%lld)", totalValue, currentValue);
		strInfoText = szText;
		break;
	case LOWERFORMAT_PROGRESS:
		sprintf(szText, "Lowerformat Device Total(%lld),Current(%lld)", totalValue, currentValue);
		strInfoText = szText;
		break;
	case DOWNLOADIMAGE_PROGRESS:
		sprintf(szText, "Download Image Total(%lldK),Current(%lldK)", totalValue/1024, currentValue/1024);
		strInfoText = szText;
		break;
	case CHECKIMAGE_PROGRESS:
		sprintf(szText, "Check Image Total(%lldK),Current(%lldK)", totalValue/1024, currentValue/1024);
		strInfoText = szText;
		break;
	case TAGBADBLOCK_PROGRESS:
		sprintf(szText, "Tag Bad Block Total(%lld),Current(%lld)", totalValue, currentValue);
		strInfoText = szText;
		break;
	case TESTBLOCK_PROGRESS:
		sprintf(szText, "Test Block Total(%lld),Current(%lld)", totalValue, currentValue);
		strInfoText = szText;
		break;
	case ERASEFLASH_PROGRESS:
		sprintf(szText, "Erase Flash Total(%lld),Current(%lld)", totalValue, currentValue);
		strInfoText = szText;
		break;
	case ERASESYSTEM_PROGRESS:
		sprintf(szText, "Erase System partition Total(%lld),Current(%lld)", totalValue, currentValue);
		strInfoText = szText;
		break;
	case ERASEUSERDATA_PROGRESS:
		sprintf(szText, "<LocationID=%x> Erase Userdata partition Total(%lld),Current(%lld)",deviceLayer,totalValue, currentValue);
		strInfoText = szText;
		break;
	}
	if (strInfoText.size() > 0){
		CURSOR_MOVEUP_LINE(1);
		CURSOR_DEL_LINE;
		printf("%s\r\n", strInfoText.c_str());
	}
	if (emCall == CALL_LAST)
		deviceLayer = 0;
}

char *strupr(char *szSrc)
{
	char *p = szSrc;
	while(*p){
		if ((*p >= 'a') && (*p <= 'z'))
			*p = *p - 'a' + 'A';
		p++;
	}
	return szSrc;
}
void PrintData(PBYTE pData, int nSize)
{
	char szPrint[17] = "\0";
	int i;
	for( i = 0; i < nSize; i++){
		if(i % 16 == 0){
			if(i / 16 > 0)
				printf("     %s\r\n", szPrint);
			printf("%08d ", i / 16);
		}
		printf("%02X ", pData[i]);
		szPrint[i%16] = isprint(pData[i]) ? pData[i] : '.';
	}
	if(i / 16 > 0)
		printf("     %s\r\n", szPrint);
}

bool StringToWideString(char *pszSrc, wchar_t *&pszDest)
{
	if (!pszSrc)
		return false;
	int nSrcLen = strlen(pszSrc);
	int nDestLen = nSrcLen * 2;

	pszDest = NULL;
	pszDest = new wchar_t[nDestLen];
	if (!pszDest)
		return false;
	nDestLen = nDestLen * sizeof(wchar_t);
	memset(pszDest, 0, nDestLen);
	int iRet;
	iconv_t cd;
	cd = iconv_open("UTF-32", "UTF-8");
	if((iconv_t)-1 == cd) {
		delete []pszDest;
		pszDest = NULL;
	      return false;
	 }
	char *pIn, *pOut;
	pIn = (char *)pszSrc;
	pOut = (char *)pszDest;

	iRet = iconv(cd, (char **)&pIn, (size_t *)&nSrcLen, (char **)&pOut, (size_t *)&nDestLen);

	if(iRet == -1) {
		delete []pszDest;
		pszDest = NULL;
		iconv_close(cd);
		return false;
	 }

	 iconv_close(cd);

	 return true;
}
bool WideStringToString(wchar_t *pszSrc, char *&pszDest)
{
	if (!pszSrc)
		return false;
	int nSrcLen = wcslen(pszSrc);
	int nDestLen = nSrcLen * 2;
	nSrcLen = nSrcLen * sizeof(wchar_t);
	pszDest = NULL;
	pszDest = new char[nDestLen];
	if (!pszDest)
		return false;
	memset(pszDest, 0, nDestLen);
	int iRet;
	iconv_t cd;
	cd = iconv_open("UTF-8", "UTF-32");

	if((iconv_t)-1 == cd) {
		delete []pszDest;
		pszDest = NULL;
	      return false;
	 }
	char *pIn, *pOut;
	pIn = (char *)pszSrc;
	pOut = (char *)pszDest;
	iRet = iconv(cd, (char **)&pIn, (size_t *)&nSrcLen, (char **)&pOut, (size_t *)&nDestLen);

	if(iRet == -1) {
		delete []pszDest;
		pszDest = NULL;
		iconv_close(cd);
		return false;
	 }

	 iconv_close(cd);

	 return true;
}
int find_config_item(const char *pszName)
{
	unsigned int i;
	for(i = 0; i < g_ConfigItemVec.size(); i++){
		if (strcasecmp(pszName, g_ConfigItemVec[i].szItemName) == 0){
			return i;
		}
	}
	return -1;
}

bool parse_config(char *pConfig, CONFIG_ITEM_VECTOR &vecItem)
{

	stringstream configStream(pConfig);
	string strLine, strItemName, strItemValue;
	string::size_type line_size,pos;
	STRUCT_CONFIG_ITEM item;
	vecItem.clear();
	while (!configStream.eof()){
		getline(configStream, strLine);
		line_size = strLine.size();
		if (line_size == 0)
			continue;
		if (strLine[line_size-1] == '\r'){
			strLine = strLine.substr(0, line_size-1);
		}
		pos = strLine.find("=");
		if (pos == string::npos){
			continue;
		}
		strItemName = strLine.substr(0, pos);
		strItemValue = strLine.substr(pos + 1);
		strItemName.erase(0, strItemName.find_first_not_of(" "));
		strItemName.erase(strItemName.find_last_not_of(" ") + 1);
		strItemValue.erase(0, strItemValue.find_first_not_of(" "));
		strItemValue.erase(strItemValue.find_last_not_of(" ") + 1);
		if ((strItemName.size() > 0) && (strItemValue.size() > 0)){
			strcpy(item.szItemName, strItemName.c_str());
			strcpy(item.szItemValue, strItemValue.c_str());
			vecItem.push_back(item);
		}
	}
	return true;

}
bool parse_config_file(const char *pConfigFile, CONFIG_ITEM_VECTOR &vecItem)
{
	FILE *file = NULL;
	file = fopen(pConfigFile, "rb");
	if( !file ){
		if (g_pLogObject)
			g_pLogObject->Record("parse_config_file failed,err=%d,can't open file: %s\r\n", errno, pConfigFile);
		return false;
	}
	int iFileSize;
	fseek(file, 0, SEEK_END);
	iFileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *pConfigBuf = NULL;
	pConfigBuf = new char[iFileSize + 1];
	if (!pConfigBuf){
		fclose(file);
		return false;
	}
	memset(pConfigBuf, 0, iFileSize + 1);
	int iRead;
	iRead = fread(pConfigBuf, 1, iFileSize, file);
	if (iRead != iFileSize){
		if (g_pLogObject)
			g_pLogObject->Record("parse_config_file failed,err=%d, read=%d, total=%d\r\n", errno, iRead, iFileSize);
		fclose(file);
		delete []pConfigBuf;
		return false;
	}
	fclose(file);
	bool bRet;
	bRet = parse_config(pConfigBuf, vecItem);
	delete []pConfigBuf;
	return bRet;
}

bool check_device_type(STRUCT_RKDEVICE_DESC &dev, UINT uiSupportType)
{
	if ((dev.emUsbType & uiSupportType) == dev.emUsbType)
		return true;
	else
	{
		ERROR_COLOR_ATTR;
		printf("The  Device did not support this operation!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return false;
	}
}

bool download_boot(STRUCT_RKDEVICE_DESC &dev, char *szLoader)
{
	if (!check_device_type(dev, RKUSB_MASKROM))
		return false;
	CRKImage *pImage = NULL;
	CRKBoot *pBoot = NULL;
	bool bRet, bSuccess = false;
	int iRet;

	pImage = new CRKImage(szLoader, bRet);
	if (!bRet){
		ERROR_COLOR_ATTR;
		printf("Open loader failed,exit download boot!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	} else {
		pBoot = (CRKBoot *)pImage->m_bootObject;
		CRKComm *pComm = NULL;
		CRKDevice *pDevice = NULL;

		dev.emDeviceType = pBoot->SupportDevice;
		pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
		if (!bRet) {
			if (pImage)
				delete pImage;
			ERROR_COLOR_ATTR;
			printf("Creating Comm Object failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			return bSuccess;
		}

		pDevice = new CRKDevice(dev);
		if (!pDevice) {
			if (pImage)
				delete pImage;
			if (pComm)
				delete pComm;
			ERROR_COLOR_ATTR;
			printf("Creating device object failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			return bSuccess;
		}

		pDevice->SetObject(pImage, pComm, g_pLogObject);
		printf("Download boot...\r\n");
		iRet = pDevice->DownloadBoot();

		CURSOR_MOVEUP_LINE(1);
		CURSOR_DEL_LINE;
		if (iRet == 0) {
			pComm->Reset_Usb_Device();
			CRKScan *pScan = NULL;
			pScan = new CRKScan();
			if (pScan) {
				pScan->SetVidPid();
				pScan->Wait(dev, RKUSB_MASKROM, dev.usVid, dev.usPid);
				delete pScan;
			}
			bSuccess = true;
			printf("Download boot ok.\r\n");
		}
		else
			printf("Download boot failed!\r\n");

		if (pImage)
			delete pImage;
		if(pDevice)
			delete pDevice;
	}
	return bSuccess;
}
bool erase_flash(STRUCT_RKDEVICE_DESC &dev)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKImage *pImage = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	CRKScan *pScan = NULL;
	pScan = new CRKScan();
	pScan->SetVidPid();

	CRKComm *pComm = NULL;
	CRKDevice *pDevice = NULL;

	pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
	if (!bRet) {
		if (pScan)
			delete pScan;
		ERROR_COLOR_ATTR;
		printf("Creating Comm Object failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}

	pDevice = new CRKDevice(dev);
	if (!pDevice) {
		if (pComm)
			delete pComm;
		if (pScan)
			delete pScan;
		ERROR_COLOR_ATTR;
		printf("Creating device object failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}

	pDevice->SetObject(pImage, pComm, g_pLogObject);
	pDevice->CallBackPointer = ProgressInfoProc;

	printf("Start to erase flash...\r\n");
	iRet = pDevice->EraseAllBlocks();
	if (pDevice)
		delete pDevice;

	if (iRet == 0) {
		if (pScan) {
			pScan->SetVidPid();
			pScan->Wait(dev, RKUSB_MASKROM, dev.usVid, dev.usPid);
			delete pScan;
		}
		CURSOR_MOVEUP_LINE(1);
		CURSOR_DEL_LINE;
		bSuccess = true;
		printf("Erase flash ok.\r\n");
	}

	return bSuccess;
}

bool test_device(STRUCT_RKDEVICE_DESC &dev)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		iRet = pComm->RKU_TestDeviceReady();
		if (iRet != ERR_SUCCESS) {
			if (g_pLogObject)
				g_pLogObject->Record("Error:RKU_TestDeviceReady failed,err=%d", iRet);
			printf("Test Device Fail!\r\n");
		} else {
			bSuccess = true;
			printf("Test Device OK.\r\n");
		}
	} else {
		printf("Test Device quit,Creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}
bool reset_device(STRUCT_RKDEVICE_DESC &dev, BYTE subCode = RST_NONE_SUBCODE)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		iRet = pComm->RKU_ResetDevice(subCode);
		if (iRet != ERR_SUCCESS) {
			if (g_pLogObject)
				g_pLogObject->Record("Error:RKU_ResetDevice failed,err=%d", iRet);
			printf("Reset Device Fail!\r\n");
		} else {
			bSuccess = true;
			printf("Reset Device OK.\r\n");
		}
	} else {
		printf("Reset Device quit,Creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}

bool read_flash_id(STRUCT_RKDEVICE_DESC &dev)
{
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return bSuccess;

	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		BYTE flashID[5];
		iRet = pComm->RKU_ReadFlashID(flashID);
		if (iRet != ERR_SUCCESS) {
			if (g_pLogObject)
				g_pLogObject->Record("Error:RKU_ReadFlashID failed,err=%d", iRet);
			printf("Read flash ID Fail!\r\n");
		} else {
			printf("Flash ID:%02X %02X %02X %02X %02X \r\n", flashID[0], flashID[1], flashID[2], flashID[3], flashID[4]);
			bSuccess = true;
		}
	} else {
		printf("Read flash ID quit,Creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}
bool read_flash_info(STRUCT_RKDEVICE_DESC &dev)
{
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return bSuccess;

	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		STRUCT_FLASHINFO_CMD info;
		UINT uiRead;
		iRet = pComm->RKU_ReadFlashInfo((BYTE *)&info, &uiRead);
		if (iRet != ERR_SUCCESS) {
			if (g_pLogObject)
				g_pLogObject->Record("Error:RKU_ReadFlashInfo failed,err=%d", iRet);
			printf("Read flash Info Fail!\r\n");
		} else {
			printf("Flash Info:\r\n");
			if (info.bManufCode <= 7) {
				printf("\tManufacturer: %s,value=%02X\r\n", szManufName[info.bManufCode], info.bManufCode);
			}
			else
				printf("\tManufacturer: %s,value=%02X\r\n", "Unknown", info.bManufCode);

			printf("\tFlash Size: %dMB\r\n", info.uiFlashSize / 2 / 1024);
			printf("\tBlock Size: %dKB\r\n", info.usBlockSize / 2);
			printf("\tPage Size: %dKB\r\n", info.bPageSize / 2);
			printf("\tECC Bits: %d\r\n", info.bECCBits);
			printf("\tAccess Time: %d\r\n", info.bAccessTime);
			printf("\tFlash CS: ");
			for(int i = 0; i < 8; i++) {
				if( info.bFlashCS & (1 << i) )
					printf("Flash<%d> ", i);
			}
			printf("\r\n");
			bSuccess = true;
		}
	}else {
		printf("Read flash Info quit,Creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}
bool read_chip_info(STRUCT_RKDEVICE_DESC &dev)
{
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return bSuccess;

	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		BYTE chipInfo[16];
		iRet = pComm->RKU_ReadChipInfo(chipInfo);
		if (iRet != ERR_SUCCESS) {
			if (g_pLogObject)
				g_pLogObject->Record("Error:RKU_ReadChipInfo failed,err=%d", iRet);
			printf("Read Chip Info Fail!\r\n");
		} else {
			string strChipInfo;
			g_pLogObject->PrintBuffer(strChipInfo, chipInfo, 16, 16);
			printf("Chip Info:%s\r\n", strChipInfo.c_str());
			bSuccess = true;
		}
	} else {
		printf("Read Chip Info quit,Creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}
bool read_lba(STRUCT_RKDEVICE_DESC &dev, UINT uiBegin, UINT uiLen, char *szFile)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKUsbComm *pComm = NULL;
	FILE *file = NULL;
	bool bRet, bFirst = true, bSuccess = false;
	int iRet;
	UINT iTotalRead = 0,iRead = 0;
	int nSectorSize = 512;
	BYTE pBuf[nSectorSize * DEFAULT_RW_LBA];
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		if(szFile) {
			file = fopen(szFile, "wb+");
			if( !file ) {
				printf("Read LBA failed,err=%d,can't open file: %s\r\n", errno, szFile);
				goto Exit_ReadLBA;
			}
		}

		while(uiLen > 0) {
			memset(pBuf, 0, nSectorSize * DEFAULT_RW_LBA);
			iRead = (uiLen >= DEFAULT_RW_LBA) ? DEFAULT_RW_LBA : uiLen;
			iRet = pComm->RKU_ReadLBA( uiBegin + iTotalRead, iRead, pBuf);
			if(ERR_SUCCESS == iRet) {
				uiLen -= iRead;
				iTotalRead += iRead;

				if(szFile) {
					fwrite(pBuf, 1, iRead * nSectorSize, file);
					if (bFirst){
						if (iTotalRead >= 1024)
							printf("Read LBA from file (%d%%)\r\n", (iTotalRead / 1024) * 100 / ((uiLen + iTotalRead) / 1024));
						else
							printf("Read LBA from file %d%%)\r\n", iTotalRead * 100 / (uiLen + iTotalRead));
						bFirst = false;
					} else {
						CURSOR_MOVEUP_LINE(1);
						CURSOR_DEL_LINE;
						if (iTotalRead >= 1024)
							printf("Read LBA from file (%d%%)\r\n", (iTotalRead / 1024) * 100 / ((uiLen + iTotalRead) / 1024));
						else
							printf("Read LBA from file %d%%)\r\n", iTotalRead * 100 / (uiLen + iTotalRead));
					}
				}
				else
					PrintData(pBuf, nSectorSize * iRead);
			} else {
				if (g_pLogObject)
					g_pLogObject->Record("Error:RKU_ReadLBA failed,err=%d", iRet);

				printf("Read LBA failed!\r\n");
				goto Exit_ReadLBA;
			}
		}
		bSuccess = true;
	} else {
		printf("Read LBA quit,Creating comm object failed!\r\n");
	}
Exit_ReadLBA:
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	if (file)
		fclose(file);
	return bSuccess;
}
bool write_lba(STRUCT_RKDEVICE_DESC &dev, UINT uiBegin, char *szFile)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKUsbComm *pComm = NULL;
	FILE *file = NULL;
	bool bRet, bFirst = true, bSuccess = false;
	int iRet;
	long long iTotalWrite = 0, iFileSize = 0;
	UINT iWrite = 0, iRead = 0;
	UINT uiLen;
	int nSectorSize = 512;
	BYTE pBuf[nSectorSize * DEFAULT_RW_LBA];

	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		file = fopen(szFile, "rb");
		if( !file ) {
			printf("Write LBA failed,err=%d,can't open file: %s\r\n", errno, szFile);
			goto Exit_WriteLBA;
		}

		iRet = fseeko(file, 0, SEEK_END);
		iFileSize = ftello(file);
		fseeko(file, 0, SEEK_SET);
		while(iTotalWrite < iFileSize) {
			memset(pBuf, 0, nSectorSize * DEFAULT_RW_LBA);
			iWrite = iRead= fread(pBuf, 1, nSectorSize * DEFAULT_RW_LBA, file);
			uiLen = ((iWrite % 512) == 0) ? (iWrite / 512) : (iWrite / 512 + 1);
			iRet = pComm->RKU_WriteLBA( uiBegin, uiLen, pBuf);
			if(ERR_SUCCESS == iRet) {
				uiBegin += uiLen;
				iTotalWrite += iWrite;
				if (bFirst) {
					if (iTotalWrite >= 1024)
						printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
					else
						printf("Write LBA from file %lld%%)\r\n", iTotalWrite * 100 / iFileSize);
					bFirst = false;
				} else {
					CURSOR_MOVEUP_LINE(1);
					CURSOR_DEL_LINE;
					printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
				}
			} else {
				if (g_pLogObject)
					g_pLogObject->Record("Error:RKU_WriteLBA failed,err=%d", iRet);

				printf("Write LBA failed!\r\n");
				goto Exit_WriteLBA;
			}
		}
		bSuccess = true;
	} else {
		printf("Write LBA quit,Creating comm object failed!\r\n");
	}
Exit_WriteLBA:
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	if (file)
		fclose(file);
	return bSuccess;
}

void split_item(STRING_VECTOR &vecItems, char *pszItems)
{
	string strItem;
	char szItem[100];
	char *pos = NULL, *pStart;
	pStart = pszItems;
	pos = strchr(pStart, ',');
	while(pos != NULL) {
		memset(szItem, 0, 100);
		strncpy(szItem, pStart, pos - pStart);
		strItem = szItem;
		vecItems.push_back(strItem);
		pStart = pos + 1;
		if (*pStart == 0)
			break;
		pos = strchr(pStart, ',');
	}
	if (strlen(pStart) > 0) {
		memset(szItem, 0, 100);
		strncpy(szItem, pStart, strlen(pStart));
		strItem = szItem;
		vecItems.push_back(strItem);
	}
}
bool handle_command(int argc, char* argv[], CRKScan *pScan)
{
	string strCmd;
	strCmd = argv[1];
	ssize_t cnt;
	bool bRet,bSuccess = false;
	int ret;
	STRUCT_RKDEVICE_DESC dev;

	transform(strCmd.begin(), strCmd.end(), strCmd.begin(), (int(*)(int))toupper);
	if(strcmp(strCmd.c_str(), "-H") == 0) {
		usage();
		return true;
	} else if(strcmp(strCmd.c_str(), "-V") == 0) {
		printf("rkDevelopTool ver 1.0\r\n");
		return true;
	}
	cnt = pScan->Search(RKUSB_MASKROM | RKUSB_LOADER);
	if (cnt < 1) {
		ERROR_COLOR_ATTR;
		printf("No found any rockusb device,please plug device in!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	} else if (cnt > 1) {
		ERROR_COLOR_ATTR;
		printf("Found many rockusb devices,please plug device out!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}

	bRet = pScan->GetDevice(dev, 0);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Getting information of rockusb device failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}

	if(strcmp(strCmd.c_str(), "RD") == 0) {
		if ((argc != 2) && (argc != 3))
			printf("Parameter of [RD] command is invalid,please check help!\r\n");
		else {
			if (argc == 2)
				bSuccess = reset_device(dev);
			else {
				UINT uiSubCode;
				char *pszEnd;
				uiSubCode = strtoul(argv[2], &pszEnd, 0);
				if (*pszEnd)
					printf("Subcode is invalid,please check!\r\n");
				else {
					if (uiSubCode <= 5)
						bSuccess = reset_device(dev, uiSubCode);
					else
						printf("Subcode is invalid,please check!\r\n");
				}
			}
		}
	} else if(strcmp(strCmd.c_str(), "TD") == 0) {
		bSuccess = test_device(dev);
	} else if (strcmp(strCmd.c_str(), "RID") == 0) {//Read Flash ID
		bSuccess = read_flash_id(dev);
	} else if (strcmp(strCmd.c_str(), "RFI") == 0){//Read Flash Info
		bSuccess = read_flash_info(dev);
	} else if (strcmp(strCmd.c_str(), "RCI") == 0) {//Read Chip Info
		bSuccess = read_chip_info(dev);
	} else if(strcmp(strCmd.c_str(), "DB") == 0) {
		if (argc > 2) {
			string strLoader;
			strLoader = argv[2];
			bSuccess = download_boot(dev, (char *)strLoader.c_str());
		} else if (argc == 2) {
			ret = find_config_item("loader");
			if (ret == -1)
				printf("No found loader item from config!\r\n");
			else
				bSuccess = download_boot(dev, g_ConfigItemVec[ret].szItemValue);
		} else
			printf("Parameter of [DB] command is invalid,please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "EF") == 0) {
		if (argc == 2) {
			bSuccess = erase_flash(dev);
		} else
			printf("Parameter of [EF] command is invalid,please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "WL") == 0) {
		if (argc == 4) {
			UINT uiBegin;
			char *pszEnd;
			uiBegin = strtoul(argv[2], &pszEnd, 0);
			if (*pszEnd)
				printf("Begin is invalid,please check!\r\n");
			else
				bSuccess = write_lba(dev, uiBegin, argv[3]);
		} else
			printf("Parameter of [WL] command is invalid,please check help!\r\n");
	} else if (strcmp(strCmd.c_str(), "RL") == 0) {//Read LBA
		char *pszEnd;
		UINT uiBegin, uiLen;
		if (argc != 5)
			printf("Parameter of [RL] command is invalid,please check help!\r\n");
		else {
			uiBegin = strtoul(argv[2], &pszEnd, 0);
			if (*pszEnd)
				printf("Begin is invalid,please check!\r\n");
			else {
				uiLen = strtoul(argv[3], &pszEnd, 0);
				if (*pszEnd)
					printf("Len is invalid,please check!\r\n");
				else {
					bSuccess = read_lba(dev, uiBegin, uiLen, argv[4]);
				}
			}
		}
	} else {
		printf("command is invalid,please press upgrade_tool -h to check usage!\r\n");
	}
	return bSuccess;
}



int main(int argc, char* argv[])
{
	CRKScan *pScan = NULL;
	int ret;
	char szProgramProcPath[100];
	char szProgramDir[256];
	string strLogDir,strConfigFile;
	struct stat statBuf;

	g_ConfigItemVec.clear();
	sprintf(szProgramProcPath, "/proc/%d/exe", getpid());
	if (readlink(szProgramProcPath, szProgramDir, 256) == -1)
		strcpy(szProgramDir, ".");
	else {
		char *pSlash;
		pSlash = strrchr(szProgramDir, '/');
		if (pSlash)
			*pSlash = '\0';
	}
	strLogDir = szProgramDir;
	strLogDir +=  "/log/";
	strConfigFile = szProgramDir;
	strConfigFile += "/config.ini";
	if (opendir(strLogDir.c_str()) == NULL)
		mkdir(strLogDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
	g_pLogObject = new CRKLog(strLogDir.c_str(), "log");

	if(stat(strConfigFile.c_str(), &statBuf) < 0) {
		if (g_pLogObject) {
			g_pLogObject->Record("Error:failed to stat config.ini,err=%d", errno);
		}
	} else if (S_ISREG(statBuf.st_mode)) {
		parse_config_file(strConfigFile.c_str(), g_ConfigItemVec);
	}

	ret = libusb_init(NULL);
	if (ret < 0) {
		if (g_pLogObject) {
			g_pLogObject->Record("Error:libusb_init failed,err=%d", ret);
			delete g_pLogObject;
		}
		return -1;
	}

	pScan = new CRKScan();
	if (!pScan) {
		if (g_pLogObject) {
			g_pLogObject->Record("Error:failed to Create object for searching device");
			delete g_pLogObject;
		}
		libusb_exit(NULL);
		return -2;
	}
	pScan->SetVidPid();

	if (argc == 1)
		usage();
	else if (!handle_command(argc, argv, pScan))
			return -0xFF;
	if (pScan)
		delete pScan;
	if (g_pLogObject)
		delete g_pLogObject;
	libusb_exit(NULL);
	return 0;
}
