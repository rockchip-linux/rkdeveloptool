/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include   <unistd.h>
#include   <dirent.h>
#include "config.h"
#include "DefineHeader.h"
#include "gpt.h"
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
extern UINT CRC_32(unsigned char* pData, UINT ulSize);
extern unsigned short CRC_16(unsigned char* aData, UINT aSize);
extern void P_RC4(unsigned char* buf, unsigned short len);
extern unsigned int crc32_le(unsigned int crc, unsigned char *p, unsigned int len);
/*
u8 test_gpt_head[] = {
	0x45, 0x46, 0x49, 0x20, 0x50, 0x41, 0x52, 0x54, 0x00, 0x00, 0x01, 0x00, 0x5C, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xFF, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xDE, 0xFF, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x49, 0x94, 0xEC, 0x23, 0xE8, 0x58, 0x4B,
	0xAE, 0xB7, 0xA9, 0x46, 0x51, 0xD0, 0x08, 0xF8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x51, 0xEA, 0xFE, 0x08};
*/

void usage()
{
	printf("\r\n---------------------Tool Usage ---------------------\r\n");
	printf("Help:\t\t\t-h or --version\r\n");
	printf("Version:\t\t-v or --version\r\n");
	printf("DownloadBoot:\t\tdb <Loader>\r\n");
	printf("UpgradeLoader:\t\tul <Loader>\r\n");
	printf("ReadLBA:\t\trl  <BeginSec> <SectorLen> <File>\r\n");
	printf("WriteLBA:\t\twl  <BeginSec> <File>\r\n");
	printf("WriteGPT:\t\tgpt <gpt partition table>\r\n");
	printf("EraseFlash:\t\tef \r\n");
	printf("TestDevice:\t\ttd\r\n");
	printf("ResetDevice:\t\trd [subcode]\r\n");
	printf("ReadFlashID:\t\trid\r\n");
	printf("ReadFlashInfo:\t\trfi\r\n");
	printf("ReadChipInfo:\t\trci\r\n");
	printf("PackBootLoader:\t\tpack\r\n");
	printf("UnpackBootLoader:\tunpack <boot loader>\r\n");
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
		strLine.erase(0, strLine.find_first_not_of(" "));
		strLine.erase(strLine.find_last_not_of(" ") + 1);
		if (strLine.size()==0 )
			continue;
		if (strLine[0] == '#')
			continue;
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
bool ParsePartitionInfo(string &strPartInfo, string &strName, UINT &uiOffset, UINT &uiLen)
{
	string::size_type pos,prevPos;
	string strOffset,strLen;
	int iCount;
	prevPos = pos = 0;
	if (strPartInfo.size() <= 0) {
		return false;
	}
	pos = strPartInfo.find('@');
	if (pos == string::npos) {
		return false;
	}
	strLen = strPartInfo.substr(prevPos, pos - prevPos);
	strLen.erase(0, strLen.find_first_not_of(" "));
	strLen.erase(strLen.find_last_not_of(" ") + 1);
	if (strchr(strLen.c_str(), '-')) {
		uiLen = 0xFFFFFFFF;
	} else {
		iCount = sscanf(strLen.c_str(), "0x%x", &uiLen);
		if (iCount != 1) {
			return false;
		}
	}

	prevPos = pos + 1;
	pos = strPartInfo.find('(',prevPos);
	if (pos == string::npos) {
		return false;
	}
	strOffset = strPartInfo.substr(prevPos, pos - prevPos);
	strOffset.erase(0, strOffset.find_first_not_of(" "));
	strOffset.erase(strOffset.find_last_not_of(" ") + 1);
	iCount = sscanf(strOffset.c_str(), "0x%x", &uiOffset);
	if (iCount != 1) {
		return false;
	}
	prevPos = pos + 1;
	pos = strPartInfo.find(')', prevPos);
	if (pos == string::npos) {
		return false;
	}
	strName = strPartInfo.substr(prevPos, pos - prevPos);
	strName.erase(0, strName.find_first_not_of(" "));
	strName.erase(strName.find_last_not_of(" ") + 1);

	return true;
}

bool parse_parameter(char *pParameter, PARAM_ITEM_VECTOR &vecItem)
{
	stringstream paramStream(pParameter);
	bool bRet,bFind = false;
	string strLine, strPartition, strPartInfo, strPartName;
	string::size_type line_size, pos, posColon, posComma;
	UINT uiPartOffset, uiPartSize;
	STRUCT_PARAM_ITEM item;
	vecItem.clear();
	while (!paramStream.eof()) {
		getline(paramStream,strLine);
		line_size = strLine.size();
		if (line_size == 0)
			continue;
		if (strLine[line_size - 1] == '\r'){
			strLine = strLine.substr(0, line_size - 1);
		}
		strLine.erase(0, strLine.find_first_not_of(" "));
		strLine.erase(strLine.find_last_not_of(" ") + 1);
		if (strLine.size()==0 )
			continue;
		if (strLine[0] == '#')
			continue;
		pos = strLine.find("mtdparts");
		if (pos == string::npos) {
			continue;
		}
		bFind = true;
		posColon = strLine.find(':', pos);
		if (posColon == string::npos) {
			continue;
		}
		strPartition = strLine.substr(posColon + 1);
		pos = 0;
		posComma = strPartition.find(',', pos);
		while (posComma != string::npos) {
			strPartInfo = strPartition.substr(pos, posComma - pos);
			bRet = ParsePartitionInfo(strPartInfo, strPartName, uiPartOffset, uiPartSize);
			if (bRet) {
				strcpy(item.szItemName, strPartName.c_str());
				item.uiItemOffset = uiPartOffset;
				item.uiItemSize = uiPartSize;
				vecItem.push_back(item);
			}
			pos = posComma + 1;
			posComma = strPartition.find(',', pos);
		}
		strPartInfo = strPartition.substr(pos);
		if (strPartInfo.size() > 0) {
			bRet = ParsePartitionInfo(strPartInfo, strPartName, uiPartOffset, uiPartSize);
			if (bRet) {
				strcpy(item.szItemName, strPartName.c_str());
				item.uiItemOffset = uiPartOffset;
				item.uiItemSize = uiPartSize;
				vecItem.push_back(item);
			}
		}
		break;
	}
	return bFind;

}
bool parse_parameter_file(char *pParamFile, PARAM_ITEM_VECTOR &vecItem)
{
	FILE *file = NULL;
	file = fopen(pParamFile, "rb");
	if( !file ) {
		if (g_pLogObject)
			g_pLogObject->Record("parse_parameter_file failed, err=%d, can't open file: %s\r\n", errno, pParamFile);
		return false;
	}
	int iFileSize;
	fseek(file, 0, SEEK_END);
	iFileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *pParamBuf = NULL;
	pParamBuf = new char[iFileSize];
	if (!pParamBuf) {
		fclose(file);
		return false;
	}
	int iRead;
	iRead = fread(pParamBuf, 1, iFileSize, file);
	if (iRead != iFileSize) {
		if (g_pLogObject)
			g_pLogObject->Record("parse_parameter_file failed, err=%d, read=%d, total=%d\r\n", errno,iRead,iFileSize);
		fclose(file);
		delete []pParamBuf;
		return false;
	}
	fclose(file);
	bool bRet;
	bRet = parse_parameter(pParamBuf, vecItem);
	delete []pParamBuf;
	return bRet;
}
void gen_rand_uuid(unsigned char *uuid_bin)
{
	efi_guid_t id;
	unsigned int *ptr = (unsigned int *)&id;
	unsigned int i;

	/* Set all fields randomly */
	for (i = 0; i < sizeof(id) / sizeof(*ptr); i++)
		*(ptr + i) = cpu_to_be32(rand());

	id.uuid.time_hi_and_version = (id.uuid.time_hi_and_version & 0x0FFF) | 0x4000;
	id.uuid.clock_seq_hi_and_reserved = id.uuid.clock_seq_hi_and_reserved | 0x80;

	memcpy(uuid_bin, id.raw, sizeof(id));
}

void create_gpt_buffer(u8 *gpt, PARAM_ITEM_VECTOR &vecParts, u64 diskSectors)
{
	legacy_mbr *mbr = (legacy_mbr *)gpt;
	gpt_header *gptHead = (gpt_header *)(gpt + SECTOR_SIZE);
	gpt_entry *gptEntry = (gpt_entry *)(gpt + 2 * SECTOR_SIZE);
	u32 i,j;
	string strPartName;
	string::size_type colonPos;
	/*1.protective mbr*/
	memset(gpt, 0, SECTOR_SIZE);
	mbr->signature = MSDOS_MBR_SIGNATURE;
	mbr->partition_record[0].sys_ind = EFI_PMBR_OSTYPE_EFI_GPT;
	mbr->partition_record[0].start_sect = 1;
	mbr->partition_record[0].nr_sects = (u32)-1;
	/*2.gpt header*/
	memset(gpt + SECTOR_SIZE, 0, SECTOR_SIZE);
	gptHead->signature = cpu_to_le64(GPT_HEADER_SIGNATURE);
	gptHead->revision = cpu_to_le32(GPT_HEADER_REVISION_V1);
	gptHead->header_size = cpu_to_le32(sizeof(gpt_header));
	gptHead->my_lba = cpu_to_le64(1);
	gptHead->alternate_lba = cpu_to_le64(diskSectors - 1);
	gptHead->first_usable_lba = cpu_to_le64(34);
	gptHead->last_usable_lba = cpu_to_le64(diskSectors - 34);
	gptHead->partition_entry_lba = cpu_to_le64(2);
	gptHead->num_partition_entries = cpu_to_le32(GPT_ENTRY_NUMBERS);
	gptHead->sizeof_partition_entry = cpu_to_le32(GPT_ENTRY_SIZE);
	gptHead->header_crc32 = 0;
	gptHead->partition_entry_array_crc32 = 0;
	gen_rand_uuid(gptHead->disk_guid.raw);

	/*3.gpt partition entry*/
	memset(gpt + 2 * SECTOR_SIZE, 0, 32 * SECTOR_SIZE);
	for (i = 0; i < vecParts.size(); i++) {
		gen_rand_uuid(gptEntry->partition_type_guid.raw);
		gen_rand_uuid(gptEntry->unique_partition_guid.raw);
		gptEntry->starting_lba = cpu_to_le64(vecParts[i].uiItemOffset);
		gptEntry->ending_lba = cpu_to_le64(gptEntry->starting_lba + vecParts[i].uiItemSize - 1);
		gptEntry->attributes.raw = 0;
		strPartName = vecParts[i].szItemName;
		colonPos = strPartName.find_first_of(':');
		if (colonPos != string::npos) {
			if (strPartName.find("bootable") != string::npos)
				gptEntry->attributes.raw = PART_PROPERTY_BOOTABLE;
			strPartName = strPartName.substr(0, colonPos);
			vecParts[i].szItemName[strPartName.size()] = 0;
		}
		for (j = 0; j < strlen(vecParts[i].szItemName); j++)
			gptEntry->partition_name[j] = vecParts[i].szItemName[j];
		gptEntry++;
	}

	gptHead->partition_entry_array_crc32 = cpu_to_le32(crc32_le(0, gpt + 2 * SECTOR_SIZE, GPT_ENTRY_SIZE * GPT_ENTRY_NUMBERS));
	gptHead->header_crc32 = cpu_to_le32(crc32_le(0, gpt + SECTOR_SIZE, sizeof(gpt_header)));

}
bool MakeSector0(PBYTE pSector, USHORT usFlashDataSec, USHORT usFlashBootSec)
{
	PRK28_IDB_SEC0 pSec0;
	memset(pSector, 0, SECTOR_SIZE);
	pSec0 = (PRK28_IDB_SEC0)pSector;

	pSec0->dwTag = 0x0FF0AA55;
	pSec0->uiRc4Flag = 1;
	pSec0->usBootCode1Offset = 0x4;
	pSec0->usBootCode2Offset = 0x4;
	pSec0->usBootDataSize = usFlashDataSec;
	pSec0->usBootCodeSize = usFlashDataSec + usFlashBootSec;
	return true;
}


bool MakeSector1(PBYTE pSector)
{
	PRK28_IDB_SEC1 pSec1;
	memset(pSector, 0, SECTOR_SIZE);
	pSec1 = (PRK28_IDB_SEC1)pSector;

	pSec1->usSysReservedBlock = 0xC;
	pSec1->usDisk0Size = 0xFFFF;
	pSec1->uiChipTag = 0x38324B52;
	return true;
}

bool MakeSector2(PBYTE pSector)
{
	PRK28_IDB_SEC2 pSec2;
	memset(pSector, 0, SECTOR_SIZE);
	pSec2 = (PRK28_IDB_SEC2)pSector;

	strcpy(pSec2->szVcTag, "VC");
	strcpy(pSec2->szCrcTag, "CRC");
	return true;
}

bool MakeSector3(PBYTE pSector)
{
	memset(pSector,0,SECTOR_SIZE);
	return true;
}

int MakeIDBlockData(PBYTE pDDR, PBYTE pLoader, PBYTE lpIDBlock, USHORT usFlashDataSec, USHORT usFlashBootSec, DWORD dwLoaderDataSize, DWORD dwLoaderSize)
{
	RK28_IDB_SEC0 sector0Info;
	RK28_IDB_SEC1 sector1Info;
	RK28_IDB_SEC2 sector2Info;
	RK28_IDB_SEC3 sector3Info;
	UINT i;

	MakeSector0((PBYTE)&sector0Info, usFlashDataSec, usFlashBootSec);
	MakeSector1((PBYTE)&sector1Info);
	if (!MakeSector2((PBYTE)&sector2Info)) {
		return -6;
	}
	if (!MakeSector3((PBYTE)&sector3Info)) {
		return -7;
	}
	sector2Info.usSec0Crc = CRC_16((PBYTE)&sector0Info, SECTOR_SIZE);
	sector2Info.usSec1Crc = CRC_16((PBYTE)&sector1Info, SECTOR_SIZE);
	sector2Info.usSec3Crc = CRC_16((PBYTE)&sector3Info, SECTOR_SIZE);

	memcpy(lpIDBlock, &sector0Info, SECTOR_SIZE);
	memcpy(lpIDBlock + SECTOR_SIZE, &sector1Info, SECTOR_SIZE);
	memcpy(lpIDBlock + SECTOR_SIZE * 3, &sector3Info, SECTOR_SIZE);
	memcpy(lpIDBlock + SECTOR_SIZE * 4, pDDR, dwLoaderDataSize);
	memcpy(lpIDBlock + SECTOR_SIZE * (4 + usFlashDataSec), pLoader, dwLoaderSize);

	sector2Info.uiBootCodeCrc = CRC_32((PBYTE)(lpIDBlock + SECTOR_SIZE * 4), sector0Info.usBootCodeSize * SECTOR_SIZE);
	memcpy(lpIDBlock + SECTOR_SIZE * 2, &sector2Info, SECTOR_SIZE);
	for(i = 0; i < 4; i++) {
		if(i == 1) {
			continue;
		} else {
			P_RC4(lpIDBlock + SECTOR_SIZE * i, SECTOR_SIZE);
		}
	}
	return 0;
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
bool write_gpt(STRUCT_RKDEVICE_DESC &dev, char *szParameter)
{
	u8 flash_info[SECTOR_SIZE], master_gpt[34 * SECTOR_SIZE], backup_gpt[33 * SECTOR_SIZE];
	u32 total_size_sector;
	CRKComm *pComm = NULL;
	PARAM_ITEM_VECTOR vecItems;
	int iRet;
	bool bRet, bSuccess = false;
	if (!check_device_type(dev, RKUSB_MASKROM))
		return false;

	pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Creating Comm Object failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	printf("Write gpt...\r\n");
	//1.get flash info
	iRet = pComm->RKU_ReadFlashInfo(flash_info);
	if (iRet != ERR_SUCCESS) {
		ERROR_COLOR_ATTR;
		printf("Reading Flash Info failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	total_size_sector = *(u32 *)flash_info;
	//2.get partition from parameter
	bRet = parse_parameter_file(szParameter, vecItems);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Parsing parameter failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	vecItems[vecItems.size()-1].uiItemSize = total_size_sector - 34;
	//3.generate gpt info
	create_gpt_buffer(master_gpt, vecItems, total_size_sector);
	memcpy(backup_gpt, master_gpt + 2* SECTOR_SIZE, 32 * SECTOR_SIZE);
	memcpy(backup_gpt + 32 * SECTOR_SIZE, master_gpt + SECTOR_SIZE, SECTOR_SIZE);
	//4. write gpt
	iRet = pComm->RKU_WriteLBA(0, 34, master_gpt);
	if (iRet != ERR_SUCCESS) {
		ERROR_COLOR_ATTR;
		printf("Writing master gpt failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	iRet = pComm->RKU_WriteLBA(total_size_sector - 34, 33, backup_gpt);
	if (iRet != ERR_SUCCESS) {
		ERROR_COLOR_ATTR;
		printf("Writing backup gpt failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	bSuccess = true;
	CURSOR_MOVEUP_LINE(1);
	CURSOR_DEL_LINE;
	printf("Write gpt ok.\r\n");
	return bSuccess;
}

#include "boot_merger.h"
#define ENTRY_ALIGN  (2048)
options gOpts;


char gSubfix[MAX_LINE_LEN] = OUT_SUBFIX;
char* gConfigPath;
uint8_t gBuf[MAX_MERGE_SIZE];

static inline void fixPath(char* path) {
	int i, len = strlen(path);
	for(i=0; i<len; i++) {
		if (path[i] == '\\')
			path[i] = '/';
		else if (path[i] == '\r' || path[i] == '\n')
			path[i] = '\0';
	}
}

static bool parseChip(FILE* file) {
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_NAME "=%s", gOpts.chip) != 1) {
		return false;
	}
	printf("chip:%s\n", gOpts.chip);
	return true;
}

static bool parseVersion(FILE* file) {
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_MAJOR "=%d", &gOpts.major) != 1)
		return false;
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_MINOR "=%d", &gOpts.minor) != 1)
		return false;
	printf("major:%d, minor:%d\n", gOpts.major, gOpts.minor);
	return true;
}

static bool parse471(FILE* file) {
	int i, index, pos;
	char buf[MAX_LINE_LEN];

	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_NUM "=%d", &gOpts.code471Num) != 1)
		return false;
	printf("num:%d\n", gOpts.code471Num);
	if (!gOpts.code471Num)
		return true;
	if (gOpts.code471Num < 0)
		return false;
	gOpts.code471Path = (line_t*) malloc(sizeof(line_t) * gOpts.code471Num);
	for (i=0; i<gOpts.code471Num; i++) {
		if (SCANF_EAT(file) != 0) {
			return false;
		}
		if (fscanf(file, OPT_PATH "%d=%[^\r^\n]", &index, buf)
				!= 2)
			return false;
		index--;
		fixPath(buf);
		strcpy((char*)gOpts.code471Path[index], buf);
		printf("path%i:%s\n", index, gOpts.code471Path[index]);
	}
	pos = ftell(file);
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_SLEEP "=%d", &gOpts.code471Sleep) != 1)
		fseek(file, pos, SEEK_SET);
	printf("sleep:%d\n", gOpts.code471Sleep);
	return true;
}

static bool parse472(FILE* file) {
	int i, index, pos;
	char buf[MAX_LINE_LEN];

	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_NUM "=%d", &gOpts.code472Num) != 1)
		return false;
	printf("num:%d\n", gOpts.code472Num);
	if (!gOpts.code472Num)
		return true;
	if (gOpts.code472Num < 0)
		return false;
	gOpts.code472Path = (line_t*) malloc(sizeof(line_t) * gOpts.code472Num);
	for (i=0; i<gOpts.code472Num; i++) {
		if (SCANF_EAT(file) != 0) {
			return false;
		}
		if (fscanf(file, OPT_PATH "%d=%[^\r^\n]", &index, buf)
				!= 2)
			return false;
		fixPath(buf);
		index--;
		strcpy((char*)gOpts.code472Path[index], buf);
		printf("path%i:%s\n", index, gOpts.code472Path[index]);
	}
	pos = ftell(file);
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_SLEEP "=%d", &gOpts.code472Sleep) != 1)
		fseek(file, pos, SEEK_SET);
	printf("sleep:%d\n", gOpts.code472Sleep);
	return true;
}

static bool parseLoader(FILE* file) {
	int i, j, index, pos;
	char buf[MAX_LINE_LEN];
	char buf2[MAX_LINE_LEN];

	if (SCANF_EAT(file) != 0) {
		return false;
	}
	pos = ftell(file);
	if (fscanf(file, OPT_NUM "=%d", &gOpts.loaderNum) != 1) {
		fseek(file, pos, SEEK_SET);
		if(fscanf(file, OPT_LOADER_NUM "=%d", &gOpts.loaderNum) != 1) {
			return false;
		}
	}
	printf("num:%d\n", gOpts.loaderNum);
	if (!gOpts.loaderNum)
		return false;
	if (gOpts.loaderNum < 0)
		return false;
	gOpts.loader = (name_entry*) malloc(sizeof(name_entry) * gOpts.loaderNum);
	for (i=0; i<gOpts.loaderNum; i++) {
		if (SCANF_EAT(file) != 0) {
			return false;
		}
		if (fscanf(file, OPT_LOADER_NAME "%d=%s", &index, buf)
				!= 2)
			return false;
		index--;
		strcpy(gOpts.loader[index].name, buf);
		printf("name%d:%s\n", index, gOpts.loader[index].name);
	}
	for (i=0; i<gOpts.loaderNum; i++) {
		if (SCANF_EAT(file) != 0) {
			return false;
		}
		if (fscanf(file, "%[^=]=%[^\r^\n]", buf, buf2)
				!= 2)
			return false;
		for (j=0; j<gOpts.loaderNum; j++) {
			if (!strcmp(gOpts.loader[j].name, buf)) {
				fixPath(buf2);
				strcpy(gOpts.loader[j].path, buf2);
				printf("%s=%s\n", gOpts.loader[j].name, gOpts.loader[j].path);
				break;
			}
		}
		if (j >= gOpts.loaderNum) {
			return false;
		}
	}
	return true;
}

static bool parseOut(FILE* file) {
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_OUT_PATH "=%[^\r^\n]", gOpts.outPath) != 1)
		return false;
	fixPath(gOpts.outPath);
	printf("out:%s\n", gOpts.outPath);
	return true;
}


void printOpts(FILE* out) {
	int i;
	fprintf(out, SEC_CHIP "\n" OPT_NAME "=%s\n", gOpts.chip);
	fprintf(out, SEC_VERSION "\n" OPT_MAJOR "=%d\n" OPT_MINOR
			"=%d\n", gOpts.major, gOpts.minor);

	fprintf(out, SEC_471 "\n" OPT_NUM "=%d\n", gOpts.code471Num);
	for (i=0 ;i<gOpts.code471Num ;i++) {
		fprintf(out, OPT_PATH "%d=%s\n", i+1, gOpts.code471Path[i]);
	}
	if (gOpts.code471Sleep > 0)
		fprintf(out, OPT_SLEEP "=%d\n", gOpts.code471Sleep);

	fprintf(out, SEC_472 "\n" OPT_NUM "=%d\n", gOpts.code472Num);
	for (i=0 ;i<gOpts.code472Num ;i++) {
		fprintf(out, OPT_PATH "%d=%s\n", i+1, gOpts.code472Path[i]);
	}
	if (gOpts.code472Sleep > 0)
		fprintf(out, OPT_SLEEP "=%d\n", gOpts.code472Sleep);

	fprintf(out, SEC_LOADER "\n" OPT_NUM "=%d\n", gOpts.loaderNum);
	for (i=0 ;i<gOpts.loaderNum ;i++) {
		fprintf(out, OPT_LOADER_NAME "%d=%s\n", i+1, gOpts.loader[i].name);
	}
	for (i=0 ;i<gOpts.loaderNum ;i++) {
		fprintf(out, "%s=%s\n", gOpts.loader[i].name, gOpts.loader[i].path);
	}

	fprintf(out, SEC_OUT "\n" OPT_OUT_PATH "=%s\n", gOpts.outPath);
}

static bool parseOpts(void) {
	bool ret = false;
	bool chipOk = false;
	bool versionOk = false;
	bool code471Ok = true;
	bool code472Ok = true;
	bool loaderOk = false;
	bool outOk = false;
	char buf[MAX_LINE_LEN];

	char* configPath = (gConfigPath == (char*)NULL)? (char*)DEF_CONFIG_FILE: gConfigPath;
	FILE* file;
	file = fopen(configPath, "r");
	if (!file) {
		fprintf(stderr, "config(%s) not found!\n", configPath);
		if (configPath == (char*)DEF_CONFIG_FILE) {
			file = fopen(DEF_CONFIG_FILE, "w");
			if (file) {
				fprintf(stderr, "create defconfig\n");
				printOpts(file);
			}
		}
		goto end;
	}

	printf("start parse\n");

	if (SCANF_EAT(file) != 0) {
		goto end;
	}
	while(fscanf(file, "%s", buf) == 1) {
		if (!strcmp(buf, SEC_CHIP)) {
			chipOk = parseChip(file);
			if (!chipOk) {
				printf("parseChip failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_VERSION)) {
			versionOk = parseVersion(file);
			if (!versionOk) {
				printf("parseVersion failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_471)) {
			code471Ok = parse471(file);
			if (!code471Ok) {
				printf("parse471 failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_472)) {
			code472Ok = parse472(file);
			if (!code472Ok) {
				printf("parse472 failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_LOADER)) {
			loaderOk = parseLoader(file);
			if (!loaderOk) {
				printf("parseLoader failed!\n");
				goto end;
			}
		} else if (!strcmp(buf, SEC_OUT)) {
			outOk = parseOut(file);
			if (!outOk) {
				printf("parseOut failed!\n");
				goto end;
			}
		} else if (buf[0] == '#') {
			continue;
		} else {
			printf("unknown sec: %s!\n", buf);
			goto end;
		}
		if (SCANF_EAT(file) != 0) {
			goto end;
		}
	}

	if (chipOk && versionOk && code471Ok && code472Ok
			&& loaderOk && outOk)
		ret = true;
end:
	if (file)
		fclose(file);
	return ret;
}

bool initOpts(void) {
	//set default opts
	gOpts.major = DEF_MAJOR;
	gOpts.minor = DEF_MINOR;
	strcpy(gOpts.chip, DEF_CHIP);
	gOpts.code471Sleep = DEF_CODE471_SLEEP;
	gOpts.code472Sleep = DEF_CODE472_SLEEP;
	gOpts.code471Num = DEF_CODE471_NUM;
	gOpts.code471Path = (line_t*) malloc(sizeof(line_t) * gOpts.code471Num);
	strcpy((char*)gOpts.code471Path[0], DEF_CODE471_PATH);
	gOpts.code472Num = DEF_CODE472_NUM;
	gOpts.code472Path = (line_t*) malloc(sizeof(line_t) * gOpts.code472Num);
	strcpy((char*)gOpts.code472Path[0], DEF_CODE472_PATH);
	gOpts.loaderNum = DEF_LOADER_NUM;
	gOpts.loader = (name_entry*) malloc(sizeof(name_entry) * gOpts.loaderNum);
	strcpy(gOpts.loader[0].name, DEF_LOADER0);
	strcpy(gOpts.loader[0].path, DEF_LOADER0_PATH);
	strcpy(gOpts.loader[1].name, DEF_LOADER1);
	strcpy(gOpts.loader[1].path, DEF_LOADER1_PATH);
	strcpy(gOpts.outPath, DEF_OUT_PATH);

	return parseOpts();
}

/************merge code****************/

static inline uint32_t getBCD(unsigned short value) {
	uint8_t tmp[2] = {0};
	int i;
	uint32_t ret;
	//if (value > 0xFFFF) {
	//	return 0;
	//}
	for(i=0; i < 2; i++) {
		tmp[i] = (((value/10)%10)<<4) | (value%10);
		value /= 100;
	}
	ret = ((uint16_t)(tmp[1] << 8)) | tmp[0];

	printf("ret:%x\n",ret);
	return ret&0xFF;
}

static inline void str2wide(const char* str, uint16_t* wide, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		wide[i] = (uint16_t) str[i];
	}
	wide[len] = 0;
}

static inline void getName(char* path, uint16_t* dst) {
	char* end;
	char* start;
	int len;
	if (!path || !dst)
		return;
	start = strrchr(path, '/');
	if (!start)
		start = path;
	else
		start++;
	end = strrchr(path, '.');
	if (!end)
		end = path + strlen(path);
	len = end - start;
	if (len >= MAX_NAME_LEN)
		len = MAX_NAME_LEN -1;
	str2wide(start, dst, len);


		char name[MAX_NAME_LEN];
		memset(name, 0, sizeof(name));
		memcpy(name, start, len);
		printf("path:%s, name:%s\n", path, name);

}

static inline bool getFileSize(const char *path, uint32_t* size) {
	struct stat st;
	if(stat(path, &st) < 0)
		return false;
	*size = st.st_size;
	printf("path:%s, size:%d\n", path, *size);
	return true;
}

static inline rk_time getTime(void) {
	rk_time rkTime;

	struct tm *tm;
	time_t tt = time(NULL);
	tm = localtime(&tt);
	rkTime.year = tm->tm_year + 1900;
	rkTime.month = tm->tm_mon + 1;
	rkTime.day = tm->tm_mday;
	rkTime.hour = tm->tm_hour;
	rkTime.minute = tm->tm_min;
	rkTime.second = tm->tm_sec;
	printf("%d-%d-%d %02d:%02d:%02d\n",
			rkTime.year, rkTime.month, rkTime.day,
			rkTime.hour, rkTime.minute, rkTime.second);
	return rkTime;
}

static bool writeFile(FILE* outFile, const char* path, bool fix) {
	bool ret = false;
	uint32_t size = 0, fixSize = 0;
	uint8_t* buf;

	FILE* inFile = fopen(path, "rb");
	if (!inFile)
		goto end;

	if (!getFileSize(path, &size))
		goto end;
	if (fix) {
		fixSize = ((size - 1) / SMALL_PACKET + 1) * SMALL_PACKET;
		uint32_t tmp = fixSize % ENTRY_ALIGN;
		tmp = tmp ? (ENTRY_ALIGN - tmp): 0;
		fixSize +=tmp;
		memset(gBuf, 0, fixSize);
	} else {
		memset(gBuf, 0, size+ENTRY_ALIGN);
	}
	if (!fread(gBuf, size, 1, inFile))
		goto end;

	if (fix) {

		buf = gBuf;
		size = fixSize;
		while(1) {
			P_RC4(buf, fixSize < SMALL_PACKET ? fixSize : SMALL_PACKET);
			buf += SMALL_PACKET;
			if (fixSize <= SMALL_PACKET)
				break;
			fixSize -= SMALL_PACKET;
		}
	} else {
		uint32_t tmp = size % ENTRY_ALIGN;
		tmp = tmp ? (ENTRY_ALIGN - tmp): 0;
		size +=tmp;
		P_RC4(gBuf, size);
	}

	if (!fwrite(gBuf, size, 1, outFile))
		goto end;
	ret = true;
end:
	if (inFile)
		fclose(inFile);
	if (!ret)
		printf("write entry(%s) failed\n", path);
	return ret;
}

static bool saveEntry(FILE* outFile, char* path, rk_entry_type type,
		uint16_t delay, uint32_t* offset, char* fixName, bool fix) {
	printf("write:%s\n", path);
	uint32_t size;
	rk_boot_entry entry;
	memset(&entry, 0, sizeof(rk_boot_entry));

	printf("write:%s\n", path);

	getName(fixName ? fixName: path, entry.name);
	entry.size = sizeof(rk_boot_entry);
	entry.type = type;
	entry.dataOffset = *offset;
	if (!getFileSize(path, &size)) {
		printf("save entry(%s) failed:\n\tcannot get file size.\n", path);
		return false;
	}
	if (fix)
		size = ((size - 1) / SMALL_PACKET + 1) * SMALL_PACKET;
	uint32_t tmp = size % ENTRY_ALIGN;
	size += tmp ? (ENTRY_ALIGN - tmp): 0;
	printf("align size:%d\n", size);
	entry.dataSize = size;
	entry.dataDelay = delay;
	*offset += size;
	fwrite(&entry, sizeof(rk_boot_entry), 1, outFile);
	return true;
}

static inline uint32_t convertChipType(const char* chip) {
	char buffer[5];
	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "%s", chip);
	return buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
}

static inline uint32_t getChipType(const char* chip) {
	printf("chip:%s\n", chip);
	int chipType = RKNONE_DEVICE;
	if(!chip) {
		goto end;
	}
	if (!strcmp(chip, CHIP_RK28)) {
		chipType = RK28_DEVICE;
	} else if (!strcmp(chip, CHIP_RK28)) {
		chipType = RK28_DEVICE;
	} else if (!strcmp(chip, CHIP_RK281X)) {
		chipType = RK281X_DEVICE;
	} else if (!strcmp(chip, CHIP_RKPANDA)) {
		chipType = RKPANDA_DEVICE;
	} else if (!strcmp(chip, CHIP_RK27)) {
		chipType = RK27_DEVICE;
	} else if (!strcmp(chip, CHIP_RKNANO)) {
		chipType = RKNANO_DEVICE;
	} else if (!strcmp(chip, CHIP_RKSMART)) {
		chipType = RKSMART_DEVICE;
	} else if (!strcmp(chip, CHIP_RKCROWN)) {
		chipType = RKCROWN_DEVICE;
	} else if (!strcmp(chip, CHIP_RKCAYMAN)) {
		chipType = RKCAYMAN_DEVICE;
	} else if (!strcmp(chip, CHIP_RK29)) {
		chipType = RK29_DEVICE;
	} else if (!strcmp(chip, CHIP_RK292X)) {
		chipType = RK292X_DEVICE;
	} else if (!strcmp(chip, CHIP_RK30)) {
		chipType = RK30_DEVICE;
	} else if (!strcmp(chip, CHIP_RK30B)) {
		chipType = RK30B_DEVICE;
	} else if (!strcmp(chip, CHIP_RK31)) {
		chipType = RK31_DEVICE;
	} else if (!strcmp(chip, CHIP_RK32)) {
		chipType = RK32_DEVICE;
	} else {
		chipType = convertChipType(chip + 2);
	}

end:
	printf("type:0x%x\n", chipType);
	if (chipType == RKNONE_DEVICE) {
		printf("chip type not support!\n");
	}
	return chipType;
}

static inline void getBoothdr(rk_boot_header* hdr) {
	memset(hdr, 0, sizeof(rk_boot_header));
	hdr->tag = TAG;
	hdr->size = sizeof(rk_boot_header);
	hdr->version = (getBCD(gOpts.major) << 8) | getBCD(gOpts.minor);
	hdr->mergerVersion = MERGER_VERSION;
	hdr->releaseTime = getTime();
	hdr->chipType = getChipType(gOpts.chip);

	hdr->code471Num = gOpts.code471Num;
	hdr->code471Offset = sizeof(rk_boot_header);
	hdr->code471Size = sizeof(rk_boot_entry);

	hdr->code472Num = gOpts.code472Num;
	hdr->code472Offset = hdr->code471Offset + gOpts.code471Num * hdr->code471Size;
	hdr->code472Size = sizeof(rk_boot_entry);

	hdr->loaderNum = gOpts.loaderNum;
	hdr->loaderOffset = hdr->code472Offset + gOpts.code472Num * hdr->code472Size;
	hdr->loaderSize = sizeof(rk_boot_entry);
#ifndef USE_P_RC4
	hdr->rc4Flag = 1;
#endif
}

static inline uint32_t getCrc(const char* path) {
	uint32_t size = 0;
	uint32_t crc = 0;

	FILE* file = fopen(path, "rb");
	getFileSize(path, &size);
	if (!file)
		goto end;
	if (!fread(gBuf, size, 1, file))
		goto end;
	crc = CRC_32(gBuf, size);
	printf("crc:0x%08x\n", crc);
end:
	if (file)
		fclose(file);
	return crc;
}

bool mergeBoot(void) {
	uint32_t dataOffset;
	bool ret = false;
	int i;
	FILE* outFile;
	uint32_t crc;
	rk_boot_header hdr;

	if (!initOpts())
		return false;
	{
		char* subfix = strstr(gOpts.outPath, OUT_SUBFIX);
		char version[MAX_LINE_LEN];
		snprintf(version, sizeof(version), "%s", gSubfix);
		if (subfix && !strcmp(subfix, OUT_SUBFIX)) {
			subfix[0] = '\0';
		}
		strcat(gOpts.outPath, version);
		printf("fix opt:%s\n", gOpts.outPath);
	}

	printf("---------------\nUSING CONFIG:\n");
	printOpts(stdout);
	printf("---------------\n\n");


	outFile = fopen(gOpts.outPath, "wb+");
	if (!outFile) {
		printf("open out file(%s) failed\n", gOpts.outPath);
		goto end;
	}

	getBoothdr(&hdr);
	printf("write hdr\n");
	fwrite(&hdr, 1, sizeof(rk_boot_header), outFile);

	dataOffset = sizeof(rk_boot_header) +
		(gOpts.code471Num + gOpts.code472Num + gOpts.loaderNum) *
		sizeof(rk_boot_entry);

	printf("write code 471 entry\n");
	for (i=0; i<gOpts.code471Num; i++) {
		if (!saveEntry(outFile, (char*)gOpts.code471Path[i], ENTRY_471, gOpts.code471Sleep,
					&dataOffset, NULL, false))
			goto end;
	}
	printf("write code 472 entry\n");
	for (i=0; i<gOpts.code472Num; i++) {
		if (!saveEntry(outFile, (char*)gOpts.code472Path[i], ENTRY_472, gOpts.code472Sleep,
					&dataOffset, NULL, false))
			goto end;
	}
	printf("write loader entry\n");
	for (i=0; i<gOpts.loaderNum; i++) {
		if (!saveEntry(outFile, gOpts.loader[i].path, ENTRY_LOADER, 0,
					&dataOffset, gOpts.loader[i].name, true))
			goto end;
	}

	printf("write code 471\n");
	for (i=0; i<gOpts.code471Num; i++) {
		if (!writeFile(outFile, (char*)gOpts.code471Path[i], false))
			goto end;
	}
	printf("write code 472\n");
	for (i=0; i<gOpts.code472Num; i++) {
		if (!writeFile(outFile, (char*)gOpts.code472Path[i], false))
			goto end;
	}
	printf("write loader\n");
	for (i=0; i<gOpts.loaderNum; i++) {
		if (!writeFile(outFile, gOpts.loader[i].path, true))
			goto end;
	}
	fflush(outFile);

	printf("write crc\n");
	crc = getCrc(gOpts.outPath);
	if (!fwrite(&crc, sizeof(crc), 1, outFile))
		goto end;
	printf("done\n");
	ret = true;
end:
	if (outFile)
		fclose(outFile);
	return ret;
}

/************merge code end************/
/************unpack code***************/

static inline void wide2str(const uint16_t* wide, char* str, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		str[i] = (char) (wide[i] & 0xFF);
	}
	str[len] = 0;
}

static bool unpackEntry(rk_boot_entry* entry, const char* name,
		FILE* inFile) {
	bool ret = false;
	int size, i;
	FILE* outFile = fopen(name, "wb+");
	if (!outFile)
		goto end;
	printf("unpack entry(%s)\n", name);
	fseek(inFile, entry->dataOffset, SEEK_SET);
	size = entry->dataSize;
	if (!fread(gBuf, size, 1, inFile))
		goto end;
	if (entry->type == ENTRY_LOADER) {
		for(i=0; i<size/SMALL_PACKET; i++)
			P_RC4(gBuf + i * SMALL_PACKET, SMALL_PACKET);
		if (size % SMALL_PACKET)
		{
			P_RC4(gBuf + i * SMALL_PACKET, size - SMALL_PACKET * 512);
		}
	} else {
		P_RC4(gBuf, size);
	}
	if (!fwrite(gBuf, size, 1, outFile))
		goto end;
	ret = true;
end:
	if (outFile)
		fclose(outFile);
	return ret;
}

bool unpackBoot(char* path) {
	bool ret = false;
	FILE* inFile = fopen(path, "rb");
	int entryNum, i;
	char name[MAX_NAME_LEN];
	rk_boot_entry* entrys;
	if (!inFile) {
		fprintf(stderr, "loader(%s) not found\n", path);
		goto end;
	}

	rk_boot_header hdr;
	if (!fread(&hdr, sizeof(rk_boot_header), 1, inFile)) {
		fprintf(stderr, "read header failed\n");
		goto end;
	}
	printf("471 num:%d, 472 num:%d, loader num:%d\n", hdr.code471Num, hdr.code472Num, hdr.loaderNum);
	entryNum = hdr.code471Num + hdr.code472Num + hdr.loaderNum;
	entrys = (rk_boot_entry*) malloc(sizeof(rk_boot_entry) * entryNum);
	if (!fread(entrys, sizeof(rk_boot_entry) * entryNum, 1, inFile)) {
		fprintf(stderr, "read data failed\n");
		goto end;
	}

	printf("entry num:%d\n", entryNum);
	for (i=0; i<entryNum; i++) {
		wide2str(entrys[i].name, name, MAX_NAME_LEN);

		printf("entry:t=%d, name=%s, off=%d, size=%d\n",
				entrys[i].type, name, entrys[i].dataOffset,
				entrys[i].dataSize);
		if (!unpackEntry(entrys + i, name, inFile)) {
			fprintf(stderr, "unpack entry(%s) failed\n", name);
			goto end;
		}
	}
	printf("done\n");
	ret = true;
end:
	if (inFile)
		fclose(inFile);
	return ret;
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
bool upgrade_loader(STRUCT_RKDEVICE_DESC &dev, char *szLoader)
{
	if (!check_device_type(dev, RKUSB_MASKROM))
		return false;
	CRKImage *pImage = NULL;
	CRKBoot *pBoot = NULL;
	CRKComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	char index;
	USHORT usFlashDataSec, usFlashBootSec;
	DWORD dwLoaderSize, dwLoaderDataSize, dwDelay, dwSectorNum;
	char loaderCodeName[] = "FlashBoot";
	char loaderDataName[] = "FlashData";
	PBYTE loaderCodeBuffer = NULL;
	PBYTE loaderDataBuffer = NULL;
	PBYTE pIDBData = NULL;
	pImage = new CRKImage(szLoader, bRet);
	if (!bRet){
		ERROR_COLOR_ATTR;
		printf("Open loader failed,exit upgrade loader!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		goto Exit_UpgradeLoader;
	} else {
		pBoot = (CRKBoot *)pImage->m_bootObject;
		dev.emDeviceType = pBoot->SupportDevice;
		pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
		if (!bRet) {
			ERROR_COLOR_ATTR;
			printf("Creating Comm Object failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			goto Exit_UpgradeLoader;
		}

		printf("Upgrade loader...\r\n");
		index = pBoot->GetIndexByName(ENTRYLOADER, loaderCodeName);
		if (index == -1) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR:upgrade_loader-->Get LoaderCode Entry failed");
			}
			goto Exit_UpgradeLoader;
		}
		bRet = pBoot->GetEntryProperty(ENTRYLOADER, index, dwLoaderSize, dwDelay);
		if (!bRet) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR:upgrade_loader-->Get LoaderCode Entry Size failed");
			}
			goto Exit_UpgradeLoader;
		}

		loaderCodeBuffer = new BYTE[dwLoaderSize];
		memset(loaderCodeBuffer, 0, dwLoaderSize);
		if (!pBoot->GetEntryData(ENTRYLOADER, index, loaderCodeBuffer)) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR:upgrade_loader-->Get LoaderCode Data failed");
			}
			goto Exit_UpgradeLoader;
		}

		index = pBoot->GetIndexByName(ENTRYLOADER, loaderDataName);
		if (index == -1) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR:upgrade_loader-->Get LoaderData Entry failed");
			}
			delete []loaderCodeBuffer;
			return -4;
		}

		bRet = pBoot->GetEntryProperty(ENTRYLOADER, index, dwLoaderDataSize, dwDelay);
		if (!bRet) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR:upgrade_loader-->Get LoaderData Entry Size failed");
			}
			goto Exit_UpgradeLoader;
		}

		loaderDataBuffer = new BYTE[dwLoaderDataSize];
		memset(loaderDataBuffer, 0, dwLoaderDataSize);
		if (!pBoot->GetEntryData(ENTRYLOADER,index,loaderDataBuffer)) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR:upgrade_loader-->Get LoaderData Data failed");
			}
			goto Exit_UpgradeLoader;
		}

		usFlashDataSec = (ALIGN(dwLoaderDataSize, 2048)) / SECTOR_SIZE;
		usFlashBootSec = (ALIGN(dwLoaderSize, 2048)) / SECTOR_SIZE;
		dwSectorNum = 4 + usFlashDataSec + usFlashBootSec;
		pIDBData = new BYTE[dwSectorNum*SECTOR_SIZE];
		if (!pIDBData) {
			ERROR_COLOR_ATTR;
			printf("New memory failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			goto Exit_UpgradeLoader;
		}
		memset(pIDBData, 0, dwSectorNum * SECTOR_SIZE);
		iRet = MakeIDBlockData(loaderDataBuffer, loaderCodeBuffer, pIDBData, usFlashDataSec, usFlashBootSec, dwLoaderDataSize, dwLoaderSize);
		if (iRet != 0) {
			ERROR_COLOR_ATTR;
			printf("Make idblock failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			goto Exit_UpgradeLoader;
		}
		iRet = pComm->RKU_WriteLBA(64, dwSectorNum, pIDBData);
		CURSOR_MOVEUP_LINE(1);
		CURSOR_DEL_LINE;
		if (iRet == ERR_SUCCESS) {
			pComm->Reset_Usb_Device();
			bSuccess = true;
			printf("Upgrade loader ok.\r\n");
		} else {
			printf("Upgrade loader failed!\r\n");
			goto Exit_UpgradeLoader;
		}
	}
Exit_UpgradeLoader:
	if (pImage)
		delete pImage;
	if (pComm)
		delete pComm;
	if (loaderCodeBuffer)
		delete []loaderCodeBuffer;
	if (loaderDataBuffer)
		delete []loaderDataBuffer;
	if (pIDBData)
		delete []pIDBData;
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
	char *s;
	int i, ret;
	STRUCT_RKDEVICE_DESC dev;

	transform(strCmd.begin(), strCmd.end(), strCmd.begin(), (int(*)(int))toupper);
	s = (char*)strCmd.c_str();
	for(i = 0; i < (int)strlen(s); i++)
	        s[i] = toupper(s[i]);

	if((strcmp(strCmd.c_str(), "-H") == 0) || (strcmp(strCmd.c_str(), "--HELP")) == 0){
		usage();
		return true;
	} else if((strcmp(strCmd.c_str(), "-V") == 0) || (strcmp(strCmd.c_str(), "--VERSION") == 0)) {
		printf("rkdeveloptool ver %s\r\n", PACKAGE_VERSION);
		return true;
	} else if (strcmp(strCmd.c_str(), "PACK") == 0) {//pack boot loader
		mergeBoot();

		return true;
	} else if (strcmp(strCmd.c_str(), "UNPACK") == 0) {//unpack boot loader
		string strLoader = argv[2];

		unpackBoot((char*)strLoader.c_str());
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
	} else if(strcmp(strCmd.c_str(), "GPT") == 0) {
		if (argc > 2) {
			string strParameter;
			strParameter = argv[2];
			bSuccess = write_gpt(dev, (char *)strParameter.c_str());
		} else
			printf("Parameter of [GPT] command is invalid,please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "UL") == 0) {
		if (argc > 2) {
			string strLoader;
			strLoader = argv[2];
			bSuccess = upgrade_loader(dev, (char *)strLoader.c_str());
		} else
			printf("Parameter of [UL] command is invalid,please check help!\r\n");
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
		printf("command is invalid,please press rkDevelopTool -h to check usage!\r\n");
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
