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
#define NORMAL_COLOR_ATTR  printf("%c[0m", 0x1B);
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
	printf("Help:\t\t\t-h or --help\r\n");
	printf("Version:\t\t-v or --version\r\n");
	printf("ListDevice:\t\tld\r\n");
	printf("DownloadBoot:\t\tdb <Loader>\r\n");
	printf("UpgradeLoader:\t\tul <Loader>\r\n");
	printf("ReadLBA:\t\trl  <BeginSec> <SectorLen> <File>\r\n");
	printf("WriteLBA:\t\twl  <BeginSec> <File>\r\n");
	printf("WriteLBA:\t\twlx  <PartitionName> <File>\r\n");
	printf("WriteGPT:\t\tgpt <gpt partition table>\r\n");
	printf("WriteParameter:\t\tprm <parameter>\r\n");
	printf("PrintPartition:\t\tppt \r\n");
	printf("EraseFlash:\t\tef \r\n");
	printf("TestDevice:\t\ttd\r\n");
	printf("ResetDevice:\t\trd [subcode]\r\n");
	printf("ReadFlashID:\t\trid\r\n");
	printf("ReadFlashInfo:\t\trfi\r\n");
	printf("ReadChipInfo:\t\trci\r\n");
	printf("ReadCapability:\t\trcb\r\n");
	printf("PackBootLoader:\t\tpack\r\n");
	printf("UnpackBootLoader:\tunpack <boot loader>\r\n");
	printf("TagSPL:\t\t\ttagspl <tag> <U-Boot SPL>\r\n");
	printf("-------------------------------------------------------\r\n\r\n");
}
void ProgressInfoProc(DWORD deviceLayer, ENUM_PROGRESS_PROMPT promptID, long long totalValue, long long currentValue, ENUM_CALL_STEP emCall)
{
	string strInfoText="";
	char szText[256];
	switch (promptID) {
	case TESTDEVICE_PROGRESS:
		snprintf(szText, sizeof(szText), "Test Device total %lld, current %lld", totalValue, currentValue);
		strInfoText = szText;
		break;
	case LOWERFORMAT_PROGRESS:
		snprintf(szText, sizeof(szText), "Lowerformat Device total %lld, current %lld", totalValue, currentValue);
		strInfoText = szText;
		break;
	case DOWNLOADIMAGE_PROGRESS:
		snprintf(szText, sizeof(szText), "Download Image total %lldK, current %lldK", totalValue/1024, currentValue/1024);
		strInfoText = szText;
		break;
	case CHECKIMAGE_PROGRESS:
		snprintf(szText, sizeof(szText), "Check Image total %lldK, current %lldK", totalValue/1024, currentValue/1024);
		strInfoText = szText;
		break;
	case TAGBADBLOCK_PROGRESS:
		snprintf(szText, sizeof(szText), "Tag Bad Block total %lld, current %lld", totalValue, currentValue);
		strInfoText = szText;
		break;
	case TESTBLOCK_PROGRESS:
		snprintf(szText, sizeof(szText), "Test Block total %lld, current %lld", totalValue, currentValue);
		strInfoText = szText;
		break;
	case ERASEFLASH_PROGRESS:
		snprintf(szText, sizeof(szText), "Erase Flash total %lld, current %lld", totalValue, currentValue);
		strInfoText = szText;
		break;
	case ERASESYSTEM_PROGRESS:
		snprintf(szText, sizeof(szText), "Erase System partition total %lld, current %lld", totalValue, currentValue);
		strInfoText = szText;
		break;
	case ERASEUSERDATA_PROGRESS:
		snprintf(szText, sizeof(szText), "<LocationID=%x> Erase Userdata partition total %lld, current %lld", deviceLayer, totalValue, currentValue);
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

int find_config_item(CONFIG_ITEM_VECTOR &vecItems, const char *pszName)
{
	unsigned int i;
	for(i = 0; i < vecItems.size(); i++){
		if (strcasecmp(pszName, vecItems[i].szItemName) == 0){
			return i;
		}
	}
	return -1;
}
void string_to_uuid(string strUUid, char *uuid)
{
	unsigned int i;
	char value;
	memset(uuid, 0, 16);
	for (i =0; i < strUUid.size(); i++) {
		value = 0;
		if ((strUUid[i] >= '0')&&(strUUid[i] <= '9'))
			value = strUUid[i] - '0';
		if ((strUUid[i] >= 'a')&&(strUUid[i] <= 'f'))
			value = strUUid[i] - 'a' + 10;
		if ((strUUid[i] >= 'A')&&(strUUid[i] <= 'F'))
			value = strUUid[i] - 'A' + 10;
		if ((i % 2) == 0)
			uuid[i / 2] += (value << 4);
		else
			uuid[i / 2] += value;
	}
	unsigned int *p32;
	unsigned short *p16;
	p32 = (unsigned int*)uuid;
	*p32 = cpu_to_be32(*p32);
	p16 = (unsigned short *)(uuid + 4);
	*p16 = cpu_to_be16(*p16);
	p16 = (unsigned short *)(uuid + 6);
	*p16 = cpu_to_be16(*p16);
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
			g_pLogObject->Record("%s failed, err=%d, can't open file: %s\r\n", __func__, errno, pConfigFile);
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
			g_pLogObject->Record("%s failed, err=%d, read=%d, total=%d\r\n", __func__, errno, iRead, iFileSize);
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
bool ParseUuidInfo(string &strUuidInfo, string &strName, string &strUUid)
{
	string::size_type pos(0);
	
	if (strUuidInfo.size() <= 0) {
		return false;
	}
	pos = strUuidInfo.find('=');
	if (pos == string::npos) {
		return false;
	}
	strName = strUuidInfo.substr(0, pos);
	strName.erase(0, strName.find_first_not_of(" "));
	strName.erase(strName.find_last_not_of(" ") + 1);

	strUUid = strUuidInfo.substr(pos+1);
	strUUid.erase(0, strUUid.find_first_not_of(" "));
	strUUid.erase(strUUid.find_last_not_of(" ") + 1);
	
	while(true) { 
		pos = 0;
		if( (pos = strUUid.find("-")) != string::npos) 
			strUUid.replace(pos,1,""); 
		else 
			break; 
	}
	if (strUUid.size() != 32)
		return false;
	return true;
}


bool parse_parameter(char *pParameter, PARAM_ITEM_VECTOR &vecItem, CONFIG_ITEM_VECTOR &vecUuidItem)
{
	stringstream paramStream(pParameter);
	bool bRet,bFind = false;
	string strLine, strPartition, strPartInfo, strPartName, strUUid;
	string::size_type line_size, pos, posColon, posComma;
	UINT uiPartOffset, uiPartSize;
	STRUCT_PARAM_ITEM item;
	STRUCT_CONFIG_ITEM uuid_item;
	vecItem.clear();
	vecUuidItem.clear();
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
		pos = strLine.find("uuid:");
		if (pos != string::npos) {
			strPartInfo = strLine.substr(pos+5);
			bRet = ParseUuidInfo(strPartInfo, strPartName, strUUid);
			if (bRet) {
				strcpy(uuid_item.szItemName, strPartName.c_str());
				string_to_uuid(strUUid,uuid_item.szItemValue);
				vecUuidItem.push_back(uuid_item);
			}
			continue;
		}
			
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
	}
	return bFind;

}
bool parse_parameter_file(char *pParamFile, PARAM_ITEM_VECTOR &vecItem, CONFIG_ITEM_VECTOR &vecUuidItem)
{
	FILE *file = NULL;
	file = fopen(pParamFile, "rb");
	if( !file ) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, err=%d, can't open file: %s\r\n", __func__, errno, pParamFile);
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
			g_pLogObject->Record("%s failed, err=%d, read=%d, total=%d\r\n", __func__, errno,iRead,iFileSize);
		fclose(file);
		delete []pParamBuf;
		return false;
	}
	fclose(file);
	bool bRet;
	bRet = parse_parameter(pParamBuf, vecItem, vecUuidItem);
	delete []pParamBuf;
	return bRet;
}
bool is_sparse_image(char *szImage)
{
	FILE *file = NULL;
	sparse_header head;
	u32 uiRead;
	file = fopen(szImage, "rb");
	if( !file ) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, err=%d, can't open file: %s\r\n", __func__, errno, szImage);
		return false;
	}
	uiRead = fread(&head, 1, sizeof(head), file);
	if (uiRead != sizeof(head)) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, err=%d, read=%d, total=%d\r\n", __func__, errno, uiRead, sizeof(head));
		fclose(file);
		return false;
	}
	fclose(file);
	if (head.magic!=SPARSE_HEADER_MAGIC)
	{
		return false;
	}
	return true;
	
}
bool is_ubifs_image(char *szImage)
{
	FILE *file = NULL;
	u32 magic;
	u32 uiRead;
	file = fopen(szImage, "rb");
	if( !file ) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, err=%d, can't open file: %s\r\n", __func__, errno, szImage);
		return false;
	}
	uiRead = fread(&magic, 1, sizeof(magic), file);
	if (uiRead != sizeof(magic)) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, err=%d, read=%d, total=%d\r\n", __func__, errno, uiRead, sizeof(magic));
		fclose(file);
		return false;
	}
	fclose(file);
	if (magic!=UBI_HEADER_MAGIC)
	{
		return false;
	}
	return true;
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

void prepare_gpt_backup(u8 *master, u8 *backup)
{
	gpt_header *gptMasterHead = (gpt_header *)(master + SECTOR_SIZE);
	gpt_header *gptBackupHead = (gpt_header *)(backup + 32 * SECTOR_SIZE);
	u32 calc_crc32;
	u64 val;

	/* recalculate the values for the Backup GPT Header */
	val = le64_to_cpu(gptMasterHead->my_lba);
	gptBackupHead->my_lba = gptMasterHead->alternate_lba;
	gptBackupHead->alternate_lba = cpu_to_le64(val);
	gptBackupHead->partition_entry_lba = cpu_to_le64(le64_to_cpu(gptMasterHead->last_usable_lba) + 1); 
	gptBackupHead->header_crc32 = 0;

	calc_crc32 = crc32_le(0, (unsigned char *)gptBackupHead, le32_to_cpu(gptBackupHead->header_size));
	gptBackupHead->header_crc32 = cpu_to_le32(calc_crc32);
}
bool get_lba_from_gpt(u8 *master, char *pszName, u64 *lba, u64 *lba_end)
{
	gpt_header *gptMasterHead = (gpt_header *)(master + SECTOR_SIZE);
	gpt_entry  *gptEntry  = NULL;
	u32 i,j;
	u8 zerobuf[GPT_ENTRY_SIZE];
	bool bFound = false;
	memset(zerobuf,0,GPT_ENTRY_SIZE);

	for (i = 0; i < le32_to_cpu(gptMasterHead->num_partition_entries); i++) {
		gptEntry = (gpt_entry *)(master + 2 * SECTOR_SIZE + i * GPT_ENTRY_SIZE);
		if (memcmp(zerobuf, (u8 *)gptEntry, GPT_ENTRY_SIZE) == 0)
			break;
		for (j = 0; j < strlen(pszName); j++)
			if (gptEntry->partition_name[j] != pszName[j])
				break;
		if (gptEntry->partition_name[j] != 0)
			continue;
		if (j == strlen(pszName)) {
			bFound = true;
			break;
		}
	}
	if (bFound) {
		*lba = le64_to_cpu(gptEntry->starting_lba);
		if (gptMasterHead->last_usable_lba == gptEntry->ending_lba)
			*lba_end = 0xFFFFFFFF;
		else
			*lba_end =  le64_to_cpu(gptEntry->ending_lba);
		return true;
	}
	return false;
}
bool get_lba_from_param(u8 *param, char *pszName, u32 *part_offset, u32 *part_size)
{
	u32 i;
	bool bFound = false, bRet;
	PARAM_ITEM_VECTOR vecItem;
	CONFIG_ITEM_VECTOR vecUuid;
	
	bRet = parse_parameter((char *)param, vecItem, vecUuid);
	if (!bRet)
		return false;

	for (i = 0; i < vecItem.size(); i++) {
		if (strcasecmp(pszName, vecItem[i].szItemName)==0) {
			bFound = true;
			break;
		}
	}
	if (bFound) {
		*part_offset = vecItem[i].uiItemOffset;
		*part_size =  vecItem[i].uiItemSize;
		return true;
	}
	return false;
}

void update_gpt_disksize(u8 *master, u8 *backup, u32 total_sector)
{
	gpt_header *gptMasterHead = (gpt_header *)(master + SECTOR_SIZE);
	gpt_entry  *gptLastPartEntry  = NULL;
	u32 i;
	u64 old_disksize;
	u8 zerobuf[GPT_ENTRY_SIZE];

	memset(zerobuf,0,GPT_ENTRY_SIZE);
	old_disksize = le64_to_cpu(gptMasterHead->alternate_lba) + 1;
	for (i = 0; i < le32_to_cpu(gptMasterHead->num_partition_entries); i++) {
		gptLastPartEntry = (gpt_entry *)(master + 2 * SECTOR_SIZE + i * GPT_ENTRY_SIZE);
		if (memcmp(zerobuf, (u8 *)gptLastPartEntry, GPT_ENTRY_SIZE) == 0)
			break;
	}
	i--;
	gptLastPartEntry = (gpt_entry *)(master + 2 * SECTOR_SIZE + i * sizeof(gpt_entry));

	gptMasterHead->alternate_lba = cpu_to_le64(total_sector - 1);
	gptMasterHead->last_usable_lba = cpu_to_le64(total_sector- 34);
	
	if (gptLastPartEntry->ending_lba == (old_disksize - 34)) {//grow partition 
		gptLastPartEntry->ending_lba = cpu_to_le64(total_sector- 34);
		gptMasterHead->partition_entry_array_crc32 = cpu_to_le32(crc32_le(0, master + 2 * SECTOR_SIZE, GPT_ENTRY_SIZE * GPT_ENTRY_NUMBERS));
	}
	gptMasterHead->header_crc32 = 0;
	gptMasterHead->header_crc32 = cpu_to_le32(crc32_le(0, master + SECTOR_SIZE, sizeof(gpt_header)));
	memcpy(backup,master + 2 * SECTOR_SIZE, GPT_ENTRY_SIZE * GPT_ENTRY_NUMBERS);
	memcpy(backup + GPT_ENTRY_SIZE * GPT_ENTRY_NUMBERS, master + SECTOR_SIZE, SECTOR_SIZE);
	prepare_gpt_backup(master, backup);
	
}
bool load_gpt_buffer(char *pParamFile, u8 *master, u8 *backup)
{
	FILE *file = NULL;
	file = fopen(pParamFile, "rb");
	if( !file ) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, err=%d, can't open file: %s\r\n", __func__, errno, pParamFile);
		return false;
	}
	int iFileSize;
	fseek(file, 0, SEEK_END);
	iFileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (iFileSize != 67 * SECTOR_SIZE) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed, wrong size file: %s\r\n", __func__, pParamFile);
		fclose(file);
		return false;
	}
	
	int iRead;
	iRead = fread(master, 1, 34 * SECTOR_SIZE, file);
	if (iRead != 34 * SECTOR_SIZE) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed,read master gpt err=%d, read=%d, total=%d\r\n", __func__, errno,iRead, 34 * SECTOR_SIZE);
		fclose(file);
		return false;
	}
	iRead = fread(backup, 1, 33 * SECTOR_SIZE, file);
	if (iRead != 33 * SECTOR_SIZE) {
		if (g_pLogObject)
			g_pLogObject->Record("%s failed,read backup gpt err=%d, read=%d, total=%d\r\n", __func__, errno,iRead, 33 * SECTOR_SIZE);
		fclose(file);
		return false;
	}
	fclose(file);
	return true;
}
void create_gpt_buffer(u8 *gpt, PARAM_ITEM_VECTOR &vecParts, CONFIG_ITEM_VECTOR &vecUuid, u64 diskSectors)
{
	legacy_mbr *mbr = (legacy_mbr *)gpt;
	gpt_header *gptHead = (gpt_header *)(gpt + SECTOR_SIZE);
	gpt_entry *gptEntry = (gpt_entry *)(gpt + 2 * SECTOR_SIZE);
	u32 i,j;
	int pos;
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
			if (strPartName.find("grow") != string::npos)
				gptEntry->ending_lba = cpu_to_le64(diskSectors - 34);
			strPartName = strPartName.substr(0, colonPos);
			vecParts[i].szItemName[strPartName.size()] = 0;
		}
		for (j = 0; j < strlen(vecParts[i].szItemName); j++)
			gptEntry->partition_name[j] = vecParts[i].szItemName[j];
		if ((pos = find_config_item(vecUuid, vecParts[i].szItemName)) != -1)
			memcpy(gptEntry->unique_partition_guid.raw, vecUuid[pos].szItemValue, 16);
		gptEntry++;
	}

	gptHead->partition_entry_array_crc32 = cpu_to_le32(crc32_le(0, gpt + 2 * SECTOR_SIZE, GPT_ENTRY_SIZE * GPT_ENTRY_NUMBERS));
	gptHead->header_crc32 = cpu_to_le32(crc32_le(0, gpt + SECTOR_SIZE, sizeof(gpt_header)));

}
bool MakeSector0(PBYTE pSector, USHORT usFlashDataSec, USHORT usFlashBootSec, bool rc4Flag)
{
	PRK28_IDB_SEC0 pSec0;
	memset(pSector, 0, SECTOR_SIZE);
	pSec0 = (PRK28_IDB_SEC0)pSector;

	pSec0->dwTag = 0x0FF0AA55;
	pSec0->uiRc4Flag = rc4Flag;
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

int MakeIDBlockData(PBYTE pDDR, PBYTE pLoader, PBYTE lpIDBlock, USHORT usFlashDataSec, USHORT usFlashBootSec, DWORD dwLoaderDataSize, DWORD dwLoaderSize, bool rc4Flag)
{
	RK28_IDB_SEC0 sector0Info;
	RK28_IDB_SEC1 sector1Info;
	RK28_IDB_SEC2 sector2Info;
	RK28_IDB_SEC3 sector3Info;
	UINT i;
	MakeSector0((PBYTE)&sector0Info, usFlashDataSec, usFlashBootSec, rc4Flag);
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

	if (rc4Flag) {
		for (i = 0; i < dwLoaderDataSize/SECTOR_SIZE; i++)
			P_RC4(pDDR + i * SECTOR_SIZE, SECTOR_SIZE);
		for (i = 0; i < dwLoaderSize/SECTOR_SIZE; i++)
			P_RC4(pLoader + i * SECTOR_SIZE, SECTOR_SIZE);
	}
	
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
	if (((UINT)dev.emUsbType & uiSupportType) == (UINT)dev.emUsbType)
		return true;
	else
	{
		ERROR_COLOR_ATTR;
		printf("The device does not support this operation!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return false;
	}
}
bool MakeParamBuffer(char *pParamFile, char* &pParamData)
{
	FILE *file=NULL;
	file = fopen(pParamFile, "rb");
	if( !file )
	{
		if (g_pLogObject)
			g_pLogObject->Record("MakeParamBuffer failed,err=%d,can't open file: %s\r\n", errno, pParamFile);
		return false;
	}
	int iFileSize;
	fseek(file,0,SEEK_END);
	iFileSize = ftell(file);
	fseek(file,0,SEEK_SET);
	char *pParamBuf=NULL;
	pParamBuf = new char[iFileSize + 12];
	if (!pParamBuf)
	{
		fclose(file);
		return false;
	}
	memset(pParamBuf,0,iFileSize+12);
	*(UINT *)(pParamBuf) = 0x4D524150;
	
	int iRead;
	iRead = fread(pParamBuf+8,1,iFileSize,file);
	if (iRead!=iFileSize)
	{
		if (g_pLogObject)
			g_pLogObject->Record("MakeParamBuffer failed,err=%d,read=%d,total=%d\r\n", errno, iRead, iFileSize);
		fclose(file);
		delete []pParamBuf;
		return false;
	}
	fclose(file);
	
	*(UINT *)(pParamBuf+4) = iFileSize;
	*(UINT *)(pParamBuf+8+iFileSize) = CRC_32( (PBYTE)pParamBuf+8, iFileSize);
	pParamData = pParamBuf;
	return true;
}

bool write_parameter(STRUCT_RKDEVICE_DESC &dev, char *szParameter)
{
	CRKComm *pComm = NULL;
	char *pParamBuf = NULL, writeBuf[512*1024];
	int iRet, nParamSec, nParamSize;
	bool bRet, bSuccess = false;
	if (!check_device_type(dev, RKUSB_MASKROM|RKUSB_LOADER))
		return false;

	pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Creating Comm Object failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	if (!MakeParamBuffer(szParameter, pParamBuf)) {
		ERROR_COLOR_ATTR;
		printf("Generating parameter failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	printf("Writing parameter...\r\n");
	nParamSize = *(UINT *)(pParamBuf+4) + 12;
	nParamSec = BYTE2SECTOR(nParamSize);
	if (nParamSec > 1024) {
		ERROR_COLOR_ATTR;
		printf("parameter is too large!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	memset(writeBuf, 0, nParamSec*512);
	memcpy(writeBuf, pParamBuf, nParamSize);
	iRet = pComm->RKU_WriteLBA(0x2000, nParamSec, (BYTE *)writeBuf);
	if (iRet != ERR_SUCCESS) {
		ERROR_COLOR_ATTR;
		printf("Writing parameter failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
		
	bSuccess = true;
	CURSOR_MOVEUP_LINE(1);
	CURSOR_DEL_LINE;
	printf("Writing parameter succeeded.\r\n");
	return bSuccess;
}

bool write_gpt(STRUCT_RKDEVICE_DESC &dev, char *szParameter)
{
	u8 flash_info[SECTOR_SIZE], master_gpt[34 * SECTOR_SIZE], backup_gpt[33 * SECTOR_SIZE];
	u32 total_size_sector;
	CRKComm *pComm = NULL;
	PARAM_ITEM_VECTOR vecItems;
	CONFIG_ITEM_VECTOR vecUuid;
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
	printf("Writing gpt...\r\n");
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
	if (strstr(szParameter, ".img")) {
		if (!load_gpt_buffer(szParameter, master_gpt, backup_gpt)) {
			ERROR_COLOR_ATTR;
			printf("Loading partition image failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			return bSuccess;
		}
		update_gpt_disksize(master_gpt, backup_gpt, total_size_sector);
	} else {
		//2.get partition from parameter
		bRet = parse_parameter_file(szParameter, vecItems, vecUuid);
		if (!bRet) {
			ERROR_COLOR_ATTR;
			printf("Parsing parameter failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			return bSuccess;
		}
		//3.generate gpt info
		create_gpt_buffer(master_gpt, vecItems, vecUuid, total_size_sector);
		memcpy(backup_gpt, master_gpt + 2* SECTOR_SIZE, 32 * SECTOR_SIZE);
		memcpy(backup_gpt + 32 * SECTOR_SIZE, master_gpt + SECTOR_SIZE, SECTOR_SIZE);
		prepare_gpt_backup(master_gpt, backup_gpt);
	}
	
	//4. write gpt
	iRet = pComm->RKU_WriteLBA(0, 34, master_gpt);
	if (iRet != ERR_SUCCESS) {
		ERROR_COLOR_ATTR;
		printf("Writing master gpt failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	iRet = pComm->RKU_WriteLBA(total_size_sector - 33, 33, backup_gpt);
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
	printf("Writing gpt succeeded.\r\n");
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
	printf("chip: %s\n", gOpts.chip);
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
	printf("major: %d, minor: %d\n", gOpts.major, gOpts.minor);
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
	printf("num: %d\n", gOpts.code471Num);
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
		printf("path%i: %s\n", index, gOpts.code471Path[index]);
	}
	pos = ftell(file);
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_SLEEP "=%d", &gOpts.code471Sleep) != 1)
		fseek(file, pos, SEEK_SET);
	printf("sleep: %d\n", gOpts.code471Sleep);
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
	printf("num: %d\n", gOpts.code472Num);
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
		printf("path%i: %s\n", index, gOpts.code472Path[index]);
	}
	pos = ftell(file);
	if (SCANF_EAT(file) != 0) {
		return false;
	}
	if (fscanf(file, OPT_SLEEP "=%d", &gOpts.code472Sleep) != 1)
		fseek(file, pos, SEEK_SET);
	printf("sleep: %d\n", gOpts.code472Sleep);
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
	printf("num: %d\n", gOpts.loaderNum);
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
		strcpy(gOpts.loader[index].name, buf);
		printf("name%d: %s\n", index, gOpts.loader[index].name);
		index++;
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
	printf("out: %s\n", gOpts.outPath);
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
		fprintf(stderr, "config (%s) not found!\n", configPath);
		if (strcmp(configPath, (char*)DEF_CONFIG_FILE) == 0) {
			file = fopen(DEF_CONFIG_FILE, "w");
			if (file) {
				fprintf(stderr, "creating defconfig\n");
				printOpts(file);
			}
		}
		goto end;
	}

	printf("Starting to parse...\n");

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

	printf("ret: %x\n",ret);
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
	if (!end || (end < start))
		end = path + strlen(path);
	len = end - start;
	if (len >= MAX_NAME_LEN)
		len = MAX_NAME_LEN -1;
	str2wide(start, dst, len);


		char name[MAX_NAME_LEN];
		memset(name, 0, sizeof(name));
		memcpy(name, start, len);
		printf("path: %s, name: %s\n", path, name);

}

static inline bool getFileSize(const char *path, uint32_t* size) {
	struct stat st;
	if(stat(path, &st) < 0)
		return false;
	*size = st.st_size;
	printf("path: %s, size: %d\n", path, *size);
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
		printf("writing entry (%s) failed\n", path);
	return ret;
}

static bool saveEntry(FILE* outFile, char* path, rk_entry_type type,
		uint16_t delay, uint32_t* offset, char* fixName, bool fix) {
	uint32_t size;
	rk_boot_entry entry;

	printf("writing: %s\n", path);
	memset(&entry, 0, sizeof(rk_boot_entry));
	getName(fixName ? fixName: path, entry.name);
	entry.size = sizeof(rk_boot_entry);
	entry.type = type;
	entry.dataOffset = *offset;
	if (!getFileSize(path, &size)) {
		printf("Saving entry (%s) failed:\n\tCannot get file size.\n", path);
		return false;
	}
	if (fix)
		size = ((size - 1) / SMALL_PACKET + 1) * SMALL_PACKET;
	uint32_t tmp = size % ENTRY_ALIGN;
	size += tmp ? (ENTRY_ALIGN - tmp): 0;
	printf("alignment size: %d\n", size);
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
	printf("chip: %s\n", chip);
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
	printf("type: 0x%x\n", chipType);
	if (chipType == RKNONE_DEVICE) {
		printf("chip type not supported!\n");
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
	printf("crc: 0x%08x\n", crc);
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
		printf("fix opt: %s\n", gOpts.outPath);
	}

	printf("---------------\nUSING CONFIG:\n");
	printOpts(stdout);
	printf("---------------\n\n");


	outFile = fopen(gOpts.outPath, "wb+");
	if (!outFile) {
		printf("Opening output file (%s) failed\n", gOpts.outPath);
		goto end;
	}

	getBoothdr(&hdr);
	printf("Writing header...\n");
	fwrite(&hdr, 1, sizeof(rk_boot_header), outFile);

	dataOffset = sizeof(rk_boot_header) +
		(gOpts.code471Num + gOpts.code472Num + gOpts.loaderNum) *
		sizeof(rk_boot_entry);

	printf("Writing code 471 entry...\n");
	for (i=0; i<gOpts.code471Num; i++) {
		if (!saveEntry(outFile, (char*)gOpts.code471Path[i], ENTRY_471, gOpts.code471Sleep,
					&dataOffset, NULL, false))
			goto end;
	}
	printf("Writing code 472 entry...\n");
	for (i=0; i<gOpts.code472Num; i++) {
		if (!saveEntry(outFile, (char*)gOpts.code472Path[i], ENTRY_472, gOpts.code472Sleep,
					&dataOffset, NULL, false))
			goto end;
	}
	printf("Writing loader entry...\n");
	for (i=0; i<gOpts.loaderNum; i++) {
		if (!saveEntry(outFile, gOpts.loader[i].path, ENTRY_LOADER, 0,
					&dataOffset, gOpts.loader[i].name, true))
			goto end;
	}

	printf("Writing code 471...\n");
	for (i=0; i<gOpts.code471Num; i++) {
		if (!writeFile(outFile, (char*)gOpts.code471Path[i], false))
			goto end;
	}
	printf("Writing code 472...\n");
	for (i=0; i<gOpts.code472Num; i++) {
		if (!writeFile(outFile, (char*)gOpts.code472Path[i], false))
			goto end;
	}
	printf("Writing loader...\n");
	for (i=0; i<gOpts.loaderNum; i++) {
		if (!writeFile(outFile, gOpts.loader[i].path, true))
			goto end;
	}
	fflush(outFile);

	printf("Writing crc...\n");
	crc = getCrc(gOpts.outPath);
	if (!fwrite(&crc, sizeof(crc), 1, outFile))
		goto end;
	printf("Done.\n");
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
}

static bool unpackEntry(rk_boot_entry* entry, const char* name,
		FILE* inFile) {
	bool ret = false;
	int size, i;
	FILE* outFile = fopen(name, "wb+");
	if (!outFile)
		goto end;
	printf("unpacking entry (%s)\n", name);
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
		fprintf(stderr, "loader (%s) not found\n", path);
		goto end;
	}

	rk_boot_header hdr;
	if (!fread(&hdr, sizeof(rk_boot_header), 1, inFile)) {
		fprintf(stderr, "reading header failed\n");
		goto end;
	}
	printf("471 num:%d, 472 num:%d, loader num:%d\n", hdr.code471Num, hdr.code472Num, hdr.loaderNum);
	entryNum = hdr.code471Num + hdr.code472Num + hdr.loaderNum;
	entrys = (rk_boot_entry*) malloc(sizeof(rk_boot_entry) * entryNum);
	if (!fread(entrys, sizeof(rk_boot_entry) * entryNum, 1, inFile)) {
		fprintf(stderr, "reading data failed\n");
		goto end;
	}

	printf("entry num: %d\n", entryNum);
	for (i=0; i<entryNum; i++) {
		wide2str(entrys[i].name, name, MAX_NAME_LEN);

		printf("entry: t=%d, name=%s, off=%d, size=%d\n",
				entrys[i].type, name, entrys[i].dataOffset,
				entrys[i].dataSize);
		if (!unpackEntry(entrys + i, name, inFile)) {
			fprintf(stderr, "unpacking entry (%s) failed\n", name);
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
		printf("Opening loader failed, exiting download boot!");
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
		printf("Downloading bootloader...\r\n");
		iRet = pDevice->DownloadBoot();

		CURSOR_MOVEUP_LINE(1);
		CURSOR_DEL_LINE;
		if (iRet == 0) {
			bSuccess = true;
			printf("Downloading bootloader succeeded.\r\n");
		}
		else
			printf("Downloading bootloader failed!\r\n");

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
	bool bRet,bNewIDBlock=false, bSuccess = false;
	int iRet;
	unsigned int i;
	signed char index;
	USHORT usFlashDataSec, usFlashBootSec, usFlashHeadSec;
	DWORD dwLoaderSize, dwLoaderDataSize, dwLoaderHeadSize, dwDelay, dwSectorNum;
	char loaderCodeName[] = "FlashBoot";
	char loaderDataName[] = "FlashData";
	char loaderHeadName[] = "FlashHead";
	PBYTE loaderCodeBuffer = NULL;
	PBYTE loaderDataBuffer = NULL;
	PBYTE loaderHeadBuffer = NULL;
	PBYTE pIDBData = NULL;
	BYTE capability[8];
	pImage = new CRKImage(szLoader, bRet);
	if (!bRet){
		ERROR_COLOR_ATTR;
		printf("Opening loader failed, exiting upgrade loader!");
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

		printf("Upgrading loader...\r\n");
		index = pBoot->GetIndexByName(ENTRYLOADER, loaderCodeName);
		if (index == -1) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR: %s --> Get LoaderCode Entry failed", __func__);
			}
			goto Exit_UpgradeLoader;
		}
		bRet = pBoot->GetEntryProperty(ENTRYLOADER, index, dwLoaderSize, dwDelay);
		if (!bRet) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR: %s --> Get LoaderCode Entry Size failed", __func__);
			}
			goto Exit_UpgradeLoader;
		}

		loaderCodeBuffer = new BYTE[dwLoaderSize];
		memset(loaderCodeBuffer, 0, dwLoaderSize);
		if (!pBoot->GetEntryData(ENTRYLOADER, index, loaderCodeBuffer)) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR: %s --> Get LoaderCode Data failed", __func__);
			}
			goto Exit_UpgradeLoader;
		}

		index = pBoot->GetIndexByName(ENTRYLOADER, loaderDataName);
		if (index == -1) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR: %s --> Get LoaderData Entry failed", __func__);
			}
			delete []loaderCodeBuffer;
			return -4;
		}

		bRet = pBoot->GetEntryProperty(ENTRYLOADER, index, dwLoaderDataSize, dwDelay);
		if (!bRet) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR: %s --> Get LoaderData Entry Size failed", __func__);
			}
			goto Exit_UpgradeLoader;
		}

		loaderDataBuffer = new BYTE[dwLoaderDataSize];
		memset(loaderDataBuffer, 0, dwLoaderDataSize);
		if (!pBoot->GetEntryData(ENTRYLOADER,index,loaderDataBuffer)) {
			if (g_pLogObject) {
				g_pLogObject->Record("ERROR: %s --> Get LoaderData Data failed", __func__);
			}
			goto Exit_UpgradeLoader;
		}

		index = pBoot->GetIndexByName(ENTRYLOADER, loaderHeadName);
		if (index != -1) {
			bRet = pBoot->GetEntryProperty(ENTRYLOADER, index, dwLoaderHeadSize, dwDelay);
			if (!bRet) {
				if (g_pLogObject) {
					g_pLogObject->Record("ERROR: %s --> Get LoaderHead Entry Size failed", __func__);
				}
				goto Exit_UpgradeLoader;
			}

			loaderHeadBuffer= new BYTE[dwLoaderHeadSize];
			memset(loaderHeadBuffer, 0, dwLoaderHeadSize);
			if (!pBoot->GetEntryData(ENTRYLOADER,index,loaderHeadBuffer)) {
				if (g_pLogObject) {
					g_pLogObject->Record("ERROR: %s --> Get LoaderHead Data failed", __func__);
				}
				goto Exit_UpgradeLoader;
			}
			
			iRet = pComm->RKU_ReadCapability(capability);
			if (iRet != ERR_SUCCESS)
			{
				if (g_pLogObject)
					g_pLogObject->Record("ERROR: %s --> read capability failed", __func__);
				goto Exit_UpgradeLoader;
			}
			if ((capability[1] & 1) == 0) {
				if (g_pLogObject)
					g_pLogObject->Record("ERROR: %s --> device did not support to upgrade the loader", __func__);
				ERROR_COLOR_ATTR;
				printf("Device not support to upgrade the loader!");
				NORMAL_COLOR_ATTR;
				printf("\r\n");
				goto Exit_UpgradeLoader;
			}
			bNewIDBlock = true;
		}

		usFlashDataSec = (ALIGN(dwLoaderDataSize, 2048)) / SECTOR_SIZE;
		usFlashBootSec = (ALIGN(dwLoaderSize, 2048)) / SECTOR_SIZE;
		if (bNewIDBlock) {
			usFlashHeadSec = (ALIGN(dwLoaderHeadSize, 2048)) / SECTOR_SIZE;
			dwSectorNum = usFlashHeadSec + usFlashDataSec + usFlashBootSec;
		} else
			dwSectorNum = 4 + usFlashDataSec + usFlashBootSec;
		pIDBData = new BYTE[dwSectorNum*SECTOR_SIZE];
		if (!pIDBData) {
			ERROR_COLOR_ATTR;
			printf("Allocating memory failed!");
			NORMAL_COLOR_ATTR;
			printf("\r\n");
			goto Exit_UpgradeLoader;
		}
		memset(pIDBData, 0, dwSectorNum * SECTOR_SIZE);
		if (bNewIDBlock) {
			if (pBoot->Rc4DisableFlag)
			{//close rc4 encryption
				for (i=0;i<dwLoaderHeadSize/SECTOR_SIZE;i++)
				{
					P_RC4(loaderHeadBuffer+SECTOR_SIZE*i,SECTOR_SIZE);
				}
				for (i=0;i<dwLoaderDataSize/SECTOR_SIZE;i++)
				{
					P_RC4(loaderDataBuffer+SECTOR_SIZE*i,SECTOR_SIZE);
				}
				for (i=0;i<dwLoaderSize/SECTOR_SIZE;i++)
				{
					P_RC4(loaderCodeBuffer+SECTOR_SIZE*i,SECTOR_SIZE);
				}
			}
			memcpy(pIDBData, loaderHeadBuffer, dwLoaderHeadSize);
			memcpy(pIDBData+SECTOR_SIZE*usFlashHeadSec, loaderDataBuffer, dwLoaderDataSize);
			memcpy(pIDBData+SECTOR_SIZE*(usFlashHeadSec+usFlashDataSec), loaderCodeBuffer, dwLoaderSize);
		} else {
			iRet = MakeIDBlockData(loaderDataBuffer, loaderCodeBuffer, pIDBData, usFlashDataSec, usFlashBootSec, dwLoaderDataSize, dwLoaderSize, pBoot->Rc4DisableFlag);
			if (iRet != 0) {
				ERROR_COLOR_ATTR;
				printf("Making idblock failed!");
				NORMAL_COLOR_ATTR;
				printf("\r\n");
				goto Exit_UpgradeLoader;
			}
		}
		
		iRet = pComm->RKU_WriteLBA(64, dwSectorNum, pIDBData);
		CURSOR_MOVEUP_LINE(1);
		CURSOR_DEL_LINE;
		if (iRet == ERR_SUCCESS) {
			//pComm->Reset_Usb_Device();
			bSuccess = true;
			printf("Upgrading loader succeeded.\r\n");
		} else {
			printf("Upgrading loader failed!\r\n");
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
	if (loaderHeadBuffer)
		delete []loaderHeadBuffer;
	if (pIDBData)
		delete []pIDBData;
	return bSuccess;
}
bool print_gpt(STRUCT_RKDEVICE_DESC &dev)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	u8 master_gpt[34 * SECTOR_SIZE];
	gpt_header *gptHead = (gpt_header *)(master_gpt + SECTOR_SIZE);
	bool bRet, bSuccess = false;
	int iRet;
	gpt_entry  *gptEntry  = NULL;
	u32 i,j;
	u8 zerobuf[GPT_ENTRY_SIZE];
	memset(zerobuf,0,GPT_ENTRY_SIZE);
	CRKComm *pComm = NULL;
	char partName[36];
	pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Creating Comm Object failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	iRet = pComm->RKU_ReadLBA( 0, 34, master_gpt);
	if(ERR_SUCCESS == iRet) {
		if (gptHead->signature != le64_to_cpu(GPT_HEADER_SIGNATURE)) {
			goto Exit_PrintGpt;
		}
			
	} else {
		if (g_pLogObject)
				g_pLogObject->Record("Error: read gpt failed, err=%d", iRet);
		printf("Read GPT failed!\r\n");
		goto Exit_PrintGpt;
	}
	
	printf("**********Partition Info(GPT)**********\r\n");
	printf("NO  LBA       Name                \r\n");
	for (i = 0; i < le32_to_cpu(gptHead->num_partition_entries); i++) {
		gptEntry = (gpt_entry *)(master_gpt + 2 * SECTOR_SIZE + i * GPT_ENTRY_SIZE);
		if (memcmp(zerobuf, (u8 *)gptEntry, GPT_ENTRY_SIZE) == 0)
			break;
		memset(partName, 0 , 36);
		j = 0;
		while (gptEntry->partition_name[j]) {
			partName[j] = (char)gptEntry->partition_name[j];
			j++;
		}
		printf("%02d  %08X  %s\r\n", i, (u32)le64_to_cpu(gptEntry->starting_lba), partName);
	}
	bSuccess = true;
Exit_PrintGpt:
	if (pComm)
		delete pComm;
	return bSuccess;
}
bool print_parameter(STRUCT_RKDEVICE_DESC &dev)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	u8 param_buf[512 * SECTOR_SIZE];
	bool bRet, bSuccess = false;
	int iRet;
	u32 i, nParamSize;
	CRKComm *pComm = NULL;
	PARAM_ITEM_VECTOR vecParamItem;
	CONFIG_ITEM_VECTOR vecUuidItem;
	pComm = new CRKUsbComm(dev, g_pLogObject, bRet);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Creating Comm Object failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
	iRet = pComm->RKU_ReadLBA( 0x2000, 512, param_buf);
	if(ERR_SUCCESS == iRet) {
		if (*(u32 *)param_buf != 0x4D524150) {
			goto Exit_PrintParam;
		}
			
	} else {
		if (g_pLogObject)
				g_pLogObject->Record("Error: read parameter failed, err=%d", iRet);
		printf("Read parameter failed!\r\n");
		goto Exit_PrintParam;
	}
	nParamSize = *(u32 *)(param_buf + 4);
	memset(param_buf+8+nParamSize, 0, 512*SECTOR_SIZE - nParamSize - 8);
	
	bRet = parse_parameter((char *)(param_buf+8), vecParamItem, vecUuidItem);
	if (!bRet) {
		if (g_pLogObject)
				g_pLogObject->Record("Error: parse parameter failed");
		printf("Parse parameter failed!\r\n");
		goto Exit_PrintParam;
	}
	printf("**********Partition Info(parameter)**********\r\n");
	printf("NO  LBA       Name                \r\n");
	for (i = 0; i < vecParamItem.size(); i++) {
		printf("%02d  %08X  %s\r\n", i, vecParamItem[i].uiItemOffset, vecParamItem[i].szItemName);
	}
	bSuccess = true;
Exit_PrintParam:
	if (pComm)
		delete pComm;
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

	printf("Starting to erase flash...\r\n");
	bRet = pDevice->GetFlashInfo();
	if (!bRet) {
		if (pDevice)
			delete pDevice;
		if (pScan)
			delete pScan;
		ERROR_COLOR_ATTR;
		printf("Getting flash info from device failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}
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
		printf("Erasing flash complete.\r\n");
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
				g_pLogObject->Record("Error: RKU_TestDeviceReady failed, err=%d", iRet);
			printf("Test Device failed!\r\n");
		} else {
			bSuccess = true;
			printf("Test Device OK.\r\n");
		}
	} else {
		printf("Test Device quit, creating comm object failed!\r\n");
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
				g_pLogObject->Record("Error: RKU_ResetDevice failed, err=%d", iRet);
			printf("Reset Device failed!\r\n");
		} else {
			bSuccess = true;
			printf("Reset Device OK.\r\n");
		}
	} else {
		printf("Reset Device quit, creating comm object failed!\r\n");
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
				g_pLogObject->Record("Error: RKU_ReadFlashID failed, err=%d", iRet);
			printf("Reading flash ID failed!\r\n");
		} else {
			printf("Flash ID: %02X %02X %02X %02X %02X\r\n", flashID[0], flashID[1], flashID[2], flashID[3], flashID[4]);
			bSuccess = true;
		}
	} else {
		printf("Read Flash ID quit, creating comm object failed!\r\n");
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
				g_pLogObject->Record("Error: RKU_ReadFlashInfo failed, err=%d", iRet);
			printf("Read Flash Info failed!\r\n");
		} else {
			printf("Flash Info:\r\n");
			if (info.bManufCode <= 7) {
				printf("\tManufacturer: %s, value=%02X\r\n", szManufName[info.bManufCode], info.bManufCode);
			}
			else
				printf("\tManufacturer: %s, value=%02X\r\n", "Unknown", info.bManufCode);

			printf("\tFlash Size: %d MB\r\n", info.uiFlashSize / 2 / 1024);
			printf("\tFlash Size: %d Sectors\r\n", info.uiFlashSize);
			printf("\tBlock Size: %d KB\r\n", info.usBlockSize / 2);
			printf("\tPage Size: %d KB\r\n", info.bPageSize / 2);
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
		printf("Read Flash Info quit, creating comm object failed!\r\n");
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
				g_pLogObject->Record("Error: RKU_ReadChipInfo failed, err=%d", iRet);
			printf("Read Chip Info failed!\r\n");
		} else {
			string strChipInfo;
			g_pLogObject->PrintBuffer(strChipInfo, chipInfo, 16, 16);
			printf("Chip Info: %s\r\n", strChipInfo.c_str());
			bSuccess = true;
		}
	} else {
		printf("Read Chip Info quit, creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}
bool read_capability(STRUCT_RKDEVICE_DESC &dev)
{
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return bSuccess;

	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		
		BYTE capability[8];
		iRet = pComm->RKU_ReadCapability(capability);
		if (iRet != ERR_SUCCESS)
		{
			if (g_pLogObject)
				g_pLogObject->Record("Error:read_capability failed,err=%d", iRet);
			printf("Read capability Fail!\r\n");
		} else {
			printf("Capability:%02X %02X %02X %02X %02X %02X %02X %02X \r\n",
			capability[0], capability[1], capability[2], capability[3],
			capability[4], capability[5], capability[6], capability[7]);
			if (capability[0] & 1)
			{
				printf("Direct LBA:\tenabled\r\n");
			}

			if (capability[0] & 2)
			{
				printf("Vendor Storage:\tenabled\r\n");
			}
				
			if (capability[0] & 4)
			{
				printf("First 4m Access:\tenabled\r\n");
			}
			if (capability[0] & 8)
			{
				printf("Read LBA:\tenabled\r\n");
			}

			if (capability[0] & 20)
			{
				printf("Read Com Log:\tenabled\r\n");
			}

			if (capability[0] & 40)
			{
				printf("Read IDB Config:\tenabled\r\n");
			}

			if (capability[0] & 80)
			{
				printf("Read Secure Mode:\tenabled\r\n");
			}

			if (capability[1] & 1)
			{
				printf("New IDB:\tenabled\r\n");
			}
			bSuccess = true;
		}
	} else {
		printf("Read capability quit, creating comm object failed!\r\n");
	}
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}
bool read_param(STRUCT_RKDEVICE_DESC &dev, u8 *pParam)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		iRet = pComm->RKU_ReadLBA( 0x2000, 512, pParam);
		if(ERR_SUCCESS == iRet) {
			if (*(u32 *)pParam != 0x4D524150) {
				goto Exit_ReadParam;
			}
		} else {
			if (g_pLogObject)
					g_pLogObject->Record("Error: read parameter failed, err=%d", iRet);
			printf("Read parameter failed!\r\n");
			goto Exit_ReadParam;
		}
		bSuccess = true;
	}
Exit_ReadParam:
	if (pComm) {
		delete pComm;
		pComm = NULL;
	}
	return bSuccess;
}


bool read_gpt(STRUCT_RKDEVICE_DESC &dev, u8 *pGpt)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	gpt_header *gptHead = (gpt_header *)(pGpt + SECTOR_SIZE);
	CRKUsbComm *pComm = NULL;
	bool bRet, bSuccess = false;
	int iRet;
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		iRet = pComm->RKU_ReadLBA( 0, 34, pGpt);
		if(ERR_SUCCESS == iRet) {
			if (gptHead->signature != le64_to_cpu(GPT_HEADER_SIGNATURE)) {
				goto Exit_ReadGPT;
			}
		} else {
			if (g_pLogObject)
					g_pLogObject->Record("Error: read gpt failed, err=%d", iRet);
			printf("Read GPT failed!\r\n");
			goto Exit_ReadGPT;
		}
		bSuccess = true;
	}
Exit_ReadGPT:
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
				printf("Read LBA failed, err=%d, can't open file: %s\r\n", errno, szFile);
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
							printf("Read LBA to file (%d%%)\r\n", (iTotalRead / 1024) * 100 / ((uiLen + iTotalRead) / 1024));
						else
							printf("Read LBA to file (%d%%)\r\n", iTotalRead * 100 / (uiLen + iTotalRead));
						bFirst = false;
					} else {
						CURSOR_MOVEUP_LINE(1);
						CURSOR_DEL_LINE;
						if (iTotalRead >= 1024)
							printf("Read LBA to file (%d%%)\r\n", (iTotalRead / 1024) * 100 / ((uiLen + iTotalRead) / 1024));
						else
							printf("Read LBA to file (%d%%)\r\n", iTotalRead * 100 / (uiLen + iTotalRead));
					}
				}
				else
					PrintData(pBuf, nSectorSize * iRead);
			} else {
				if (g_pLogObject)
					g_pLogObject->Record("Error: RKU_ReadLBA failed, err=%d", iRet);

				printf("Read LBA failed!\r\n");
				goto Exit_ReadLBA;
			}
		}
		bSuccess = true;
	} else {
		printf("Read LBA quit, creating comm object failed!\r\n");
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
bool erase_ubi_block(STRUCT_RKDEVICE_DESC &dev, u32 uiOffset, u32 uiPartSize)
{
	STRUCT_FLASHINFO_CMD info;
	CRKComm *pComm = NULL;
	BYTE flashID[5];
	bool bRet,bSuccess=false;
	UINT uiReadCount,uiStartBlock,uiEraseBlock,uiBlockCount,uiErasePos;
	int iRet;
	DWORD *pID=NULL;

	printf("Erase ubi in, offset=0x%08x,size=0x%08x!\r\n",uiOffset,uiPartSize);
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (!bRet)
	{
		printf("Erase ubi quit, creating comm object failed!\r\n");
		goto EXIT_UBI_ERASE;
	}
	iRet = pComm->RKU_ReadFlashID(flashID);
	if(iRet!=ERR_SUCCESS)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record("Error:EraseUBIBlock-->RKU_ReadFlashID failed,RetCode(%d)",iRet);
		}
		goto EXIT_UBI_ERASE;
	}
	pID = (DWORD *)flashID;

	if (*pID==0x434d4d45)//emmc
	{
		bSuccess = true;
		goto EXIT_UBI_ERASE;
	}

	iRet = pComm->RKU_ReadFlashInfo((BYTE *)&info,&uiReadCount);
	if (iRet!=ERR_SUCCESS)
	{
		if (g_pLogObject)
			g_pLogObject->Record("Error:EraseUBIBlock-->RKU_ReadFlashInfo err=%d", iRet);
		goto EXIT_UBI_ERASE;
	}
	if (uiPartSize==0xFFFFFFFF)
		uiPartSize = info.uiFlashSize - uiOffset;

	uiStartBlock = uiOffset / info.usBlockSize;
	uiEraseBlock = (uiPartSize + info.usBlockSize -1) / info.usBlockSize;


	printf("Erase block start, offset=0x%08x,count=0x%08x!\r\n",uiStartBlock,uiEraseBlock);
	uiErasePos=uiStartBlock;
	while (uiEraseBlock>0)
	{
		uiBlockCount = (uiEraseBlock<MAX_ERASE_BLOCKS)?uiEraseBlock:MAX_ERASE_BLOCKS;

		iRet = pComm->RKU_EraseBlock(0,uiErasePos,uiBlockCount,ERASE_FORCE);
		if ((iRet!=ERR_SUCCESS)&&(iRet!=ERR_FOUND_BAD_BLOCK))
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record("Error:EraseUBIBlock-->RKU_EraseBlock failed,RetCode(%d)",iRet);
			}
			goto EXIT_UBI_ERASE;
		}

		uiErasePos += uiBlockCount;
		uiEraseBlock -= uiBlockCount;
	}
	bSuccess = true;
EXIT_UBI_ERASE:
	if (pComm)
		delete pComm;
	return bSuccess;
}
bool erase_partition(CRKUsbComm *pComm, UINT uiOffset, UINT uiSize)
{
	UINT uiErase=1024*32;
	bool bSuccess = true;
	int iRet;
	while (uiSize)
	{
		if (uiSize>=uiErase)
		{
			iRet = pComm->RKU_EraseLBA(uiOffset,uiErase);
			uiSize -= uiErase;
			uiOffset += uiErase;
		}
		else
		{
			iRet = pComm->RKU_EraseLBA(uiOffset,uiSize);
			uiSize = 0;
			uiOffset += uiSize;
		}
		if (iRet!=ERR_SUCCESS)
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record("ERROR:erase_partition failed,err=%d",iRet);
			}
			bSuccess = false;
			break;
		}
	}
	return bSuccess;

}
bool EatSparseChunk(FILE *file, chunk_header &chunk)
{
	UINT uiRead;
	uiRead = fread(&chunk, 1, sizeof(chunk_header), file);
	if (uiRead != sizeof(chunk_header)) {
		if (g_pLogObject)
		{
			g_pLogObject->Record("Error:EatSparseChunk failed,err=%d", errno);
		}
		return false;
	}
	return true;
}
bool EatSparseData(FILE *file, PBYTE pBuf, DWORD dwSize)
{
	UINT uiRead;
	uiRead = fread(pBuf, 1, dwSize, file);
	if (uiRead!=dwSize)
	{
		if (g_pLogObject)
		{
			g_pLogObject->Record("Error:EatSparseData failed,err=%d",errno);
		}
		return false;
	}
	return true;
}

bool write_sparse_lba(STRUCT_RKDEVICE_DESC &dev, UINT uiBegin, UINT uiSize, char *szFile)
{
	if (!check_device_type(dev, RKUSB_LOADER | RKUSB_MASKROM))
		return false;
	CRKUsbComm *pComm = NULL;
	FILE *file = NULL;
	bool bRet, bSuccess = false, bFirst = true;
	int iRet;
	u64 iTotalWrite = 0, iFileSize = 0,dwChunkDataSize;
	UINT iRead = 0, uiTransferSec, curChunk, i;
	UINT dwMaxReadWriteBytes, dwTransferBytes, dwFillByte, dwCrc;
	BYTE pBuf[SECTOR_SIZE * DEFAULT_RW_LBA];
	sparse_header header;
	chunk_header  chunk;
	dwMaxReadWriteBytes = DEFAULT_RW_LBA * SECTOR_SIZE;
	pComm =  new CRKUsbComm(dev, g_pLogObject, bRet);
	if (bRet) {
		
		file = fopen(szFile, "rb");
		if( !file ) {
			printf("%s failed, err=%d, can't open file: %s\r\n", __func__, errno, szFile);
			goto Exit_WriteSparseLBA;
		}
		fseeko(file, 0, SEEK_SET);
		iRead = fread(&header, 1, sizeof(header), file);
		if (iRead != sizeof(sparse_header))
		{
			if (g_pLogObject)
			{
				g_pLogObject->Record("ERROR:%s-->read sparse header failed,file=%s,err=%d", __func__, szFile, errno);
			}
			goto Exit_WriteSparseLBA;
		}
		iFileSize = header.blk_sz * (u64)header.total_blks;
		iTotalWrite = 0;
		curChunk = 0;
		if (uiSize==(u32)-1)
			uiSize = ALIGN(iFileSize, SECTOR_SIZE);
		bRet = erase_partition(pComm, uiBegin, uiSize);
		if (!bRet) {
			printf("%s failed, erase partition error\r\n", __func__);
			goto Exit_WriteSparseLBA;
		}
		while(curChunk < header.total_chunks) 
		{
			if (!EatSparseChunk(file, chunk)) {
				goto Exit_WriteSparseLBA;
			}
			curChunk++;
			switch (chunk.chunk_type) {
			case CHUNK_TYPE_RAW:
				dwChunkDataSize = chunk.total_sz - sizeof(chunk_header);
				while (dwChunkDataSize) {
					memset(pBuf, 0, dwMaxReadWriteBytes);
					if (dwChunkDataSize >= dwMaxReadWriteBytes) {
						dwTransferBytes = dwMaxReadWriteBytes;
						uiTransferSec = DEFAULT_RW_LBA;
					} else {
						dwTransferBytes = dwChunkDataSize;
						uiTransferSec = ((dwTransferBytes % SECTOR_SIZE == 0) ? (dwTransferBytes / SECTOR_SIZE) : (dwTransferBytes / SECTOR_SIZE + 1));
					}
					if (!EatSparseData(file, pBuf, dwTransferBytes)) {
						goto Exit_WriteSparseLBA;
					}
					iRet = pComm->RKU_WriteLBA(uiBegin, uiTransferSec, pBuf);
					if( ERR_SUCCESS == iRet ) {
						dwChunkDataSize -= dwTransferBytes;
						iTotalWrite += dwTransferBytes;
						uiBegin += uiTransferSec;
					} else {
						if (g_pLogObject) {
							g_pLogObject->Record("ERROR:%s-->RKU_WriteLBA failed,Written(%d),RetCode(%d)",  __func__, iTotalWrite, iRet);
						}
						goto Exit_WriteSparseLBA;
					}
					if (bFirst) {
						if (iTotalWrite >= 1024)
							printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
						else
							printf("Write LBA from file (%lld%%)\r\n", iTotalWrite * 100 / iFileSize);
						bFirst = false;
					} else {
						CURSOR_MOVEUP_LINE(1);
						CURSOR_DEL_LINE;
						printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
					}
				}
				break;
			case CHUNK_TYPE_FILL:
				dwChunkDataSize = (u64)chunk.chunk_sz * header.blk_sz;
				if (!EatSparseData(file, (PBYTE)&dwFillByte, 4)) {
					goto Exit_WriteSparseLBA;
				}
				while (dwChunkDataSize) {
					memset(pBuf, 0, dwMaxReadWriteBytes);
					if (dwChunkDataSize >= dwMaxReadWriteBytes) {
						dwTransferBytes = dwMaxReadWriteBytes;
						uiTransferSec = DEFAULT_RW_LBA;
					} else {
						dwTransferBytes = dwChunkDataSize;
						uiTransferSec = ((dwTransferBytes % SECTOR_SIZE == 0) ? (dwTransferBytes / SECTOR_SIZE) : (dwTransferBytes / SECTOR_SIZE + 1));
					}
					for (i = 0; i < dwTransferBytes / 4; i++) {
						*(DWORD *)(pBuf + i * 4) = dwFillByte;
					}
					iRet = pComm->RKU_WriteLBA(uiBegin, uiTransferSec, pBuf);
					if( ERR_SUCCESS == iRet ) {
						dwChunkDataSize -= dwTransferBytes;
						iTotalWrite += dwTransferBytes;
						uiBegin += uiTransferSec;
					} else {
						if (g_pLogObject) {
							g_pLogObject->Record("ERROR:%s-->RKU_WriteLBA failed,Written(%d),RetCode(%d)" ,__func__, iTotalWrite, iRet);
						}
						goto Exit_WriteSparseLBA;
					}
					if (bFirst) {
						if (iTotalWrite >= 1024)
							printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
						else
							printf("Write LBA from file (%lld%%)\r\n", iTotalWrite * 100 / iFileSize);
						bFirst = false;
					} else {
						CURSOR_MOVEUP_LINE(1);
						CURSOR_DEL_LINE;
						printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
					}
				}
				break;
			case CHUNK_TYPE_DONT_CARE:
				dwChunkDataSize = (u64)chunk.chunk_sz * header.blk_sz;
				iTotalWrite += dwChunkDataSize;
				uiTransferSec = ((dwChunkDataSize % SECTOR_SIZE == 0) ? (dwChunkDataSize / SECTOR_SIZE) : (dwChunkDataSize / SECTOR_SIZE + 1));
				uiBegin += uiTransferSec;
				if (bFirst) {
					if (iTotalWrite >= 1024)
						printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
					else
						printf("Write LBA from file (%lld%%)\r\n", iTotalWrite * 100 / iFileSize);
					bFirst = false;
				} else {
					CURSOR_MOVEUP_LINE(1);
					CURSOR_DEL_LINE;
					printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
				}
				break;
			case CHUNK_TYPE_CRC32:
				EatSparseData(file,(PBYTE)&dwCrc,4);
				break;
			}
		}
		bSuccess = true;
	} else {
		printf("Write LBA quit, creating comm object failed!\r\n");
	}
Exit_WriteSparseLBA:
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
			printf("Write LBA failed, err=%d, can't open file: %s\r\n", errno, szFile);
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
						printf("Write LBA from file (%lld%%)\r\n", iTotalWrite * 100 / iFileSize);
					bFirst = false;
				} else {
					CURSOR_MOVEUP_LINE(1);
					CURSOR_DEL_LINE;
					printf("Write LBA from file (%lld%%)\r\n", (iTotalWrite / 1024) * 100 / (iFileSize / 1024));
				}
			} else {
				if (g_pLogObject)
					g_pLogObject->Record("Error: RKU_WriteLBA failed, err=%d", iRet);

				printf("Write LBA failed!\r\n");
				goto Exit_WriteLBA;
			}
		}
		bSuccess = true;
	} else {
		printf("Write LBA quit, creating comm object failed!\r\n");
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
		memset(szItem, 0, sizeof(szItem));
		strncpy(szItem, pStart, pos - pStart);
		strItem = szItem;
		vecItems.push_back(strItem);
		pStart = pos + 1;
		if (*pStart == 0)
			break;
		pos = strchr(pStart, ',');
	}
	if (strlen(pStart) > 0) {
		memset(szItem, 0, sizeof(szItem));
		strncpy(szItem, pStart, sizeof(szItem)-1);
		strItem = szItem;
		vecItems.push_back(strItem);
	}
}

void tag_spl(char *tag, char *spl)
{
	FILE *file = NULL;
	int len;

	if(!tag || !spl)
		return;
	len = strlen(tag);
	printf("tag len=%d\n",len);
	file = fopen(spl, "rb");
	if( !file ){
		return;
	}
	int iFileSize;
	fseek(file, 0, SEEK_END);
	iFileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char *Buf = NULL;
	Buf = new char[iFileSize + len + 1];
	if (!Buf){
		fclose(file);
		return;
	}
	memset(Buf, 0, iFileSize + 1);
	memcpy(Buf, tag, len);
	int iRead;
	iRead = fread(Buf+len, 1, iFileSize, file);
	if (iRead != iFileSize){
		fclose(file);
		delete []Buf;
		return;
	}
	fclose(file);

	len = strlen(spl);
	char *taggedspl = new char[len + 5];
	strcpy(taggedspl, spl);
	strcpy(taggedspl + len, ".tag");
	taggedspl[len+4] = 0;
	printf("Writing tagged spl to %s\n", taggedspl);

	file = fopen(taggedspl, "wb");
	if( !file ){
		delete []taggedspl;
		delete []Buf;
		return;
	}
	fwrite(Buf, 1, iFileSize+len, file);
	fclose(file);
	delete []taggedspl;
	delete []Buf;
	printf("done\n");
	return;
}
void list_device(CRKScan *pScan)
{
	STRUCT_RKDEVICE_DESC desc;
	string strDevType;
	int i,cnt;
	cnt = pScan->DEVICE_COUNTS;
	if (cnt == 0) {
		printf("not found any devices!\r\n");
		return;
	}
	for (i=0;i<cnt;i++)
	{
		pScan->GetDevice(desc, i);
		if (desc.emUsbType==RKUSB_MASKROM)
			strDevType = "Maskrom";
		else if (desc.emUsbType==RKUSB_LOADER)
			strDevType = "Loader";
		else
			strDevType = "Unknown";
		printf("DevNo=%d\tVid=0x%x,Pid=0x%x,LocationID=%x\t%s\r\n",i+1,desc.usVid,
		       desc.usPid,desc.uiLocationID,strDevType.c_str());
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
	u8 master_gpt[34 * SECTOR_SIZE], param_buffer[512 * SECTOR_SIZE];
	u64 lba, lba_end;
	u32 part_size, part_offset;

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
	} else if (strcmp(strCmd.c_str(), "TAGSPL") == 0) {//tag u-boot spl
		if (argc == 4) {
			string tag = argv[2];
			string spl = argv[3];
			printf("tag %s to %s\n", tag.c_str(), spl.c_str());
			tag_spl((char*)tag.c_str(), (char*)spl.c_str());
			return true;
		}
		printf("tagspl: parameter error\n");
		usage();
	}
	cnt = pScan->Search(RKUSB_MASKROM | RKUSB_LOADER);
	if(strcmp(strCmd.c_str(), "LD") == 0) {
		list_device(pScan);
		return (cnt>0)?true:false;
	}
	
	if (cnt < 1) {
		ERROR_COLOR_ATTR;
		printf("Did not find any rockusb device, please plug device in!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	} else if (cnt > 1) {
		ERROR_COLOR_ATTR;
		printf("Found too many rockusb devices, please plug devices out!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}

	bRet = pScan->GetDevice(dev, 0);
	if (!bRet) {
		ERROR_COLOR_ATTR;
		printf("Getting information about rockusb device failed!");
		NORMAL_COLOR_ATTR;
		printf("\r\n");
		return bSuccess;
	}

	if(strcmp(strCmd.c_str(), "RD") == 0) {
		if ((argc != 2) && (argc != 3))
			printf("Parameter of [RD] command is invalid, please check help!\r\n");
		else {
			if (argc == 2)
				bSuccess = reset_device(dev);
			else {
				UINT uiSubCode;
				char *pszEnd;
				uiSubCode = strtoul(argv[2], &pszEnd, 0);
				if (*pszEnd)
					printf("Subcode is invalid, please check!\r\n");
				else {
					if (uiSubCode <= 5)
						bSuccess = reset_device(dev, uiSubCode);
					else
						printf("Subcode is invalid, please check!\r\n");
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
	} else if (strcmp(strCmd.c_str(), "RCB") == 0) {//Read Capability
		bSuccess = read_capability(dev);
	} else if(strcmp(strCmd.c_str(), "DB") == 0) {
		if (argc > 2) {
			string strLoader;
			strLoader = argv[2];
			bSuccess = download_boot(dev, (char *)strLoader.c_str());
		} else if (argc == 2) {
			ret = find_config_item(g_ConfigItemVec, "loader");
			if (ret == -1)
				printf("Did not find loader item in config!\r\n");
			else
				bSuccess = download_boot(dev, g_ConfigItemVec[ret].szItemValue);
		} else
			printf("Parameter of [DB] command is invalid, please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "GPT") == 0) {
		if (argc > 2) {
			string strParameter;
			strParameter = argv[2];
			bSuccess = write_gpt(dev, (char *)strParameter.c_str());
		} else
			printf("Parameter of [GPT] command is invalid, please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "PRM") == 0) {
		if (argc > 2) {
			string strParameter;
			strParameter = argv[2];
			bSuccess = write_parameter(dev, (char *)strParameter.c_str());
		} else
			printf("Parameter of [PRM] command is invalid, please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "UL") == 0) {
		if (argc > 2) {
			string strLoader;
			strLoader = argv[2];
			bSuccess = upgrade_loader(dev, (char *)strLoader.c_str());
		} else
			printf("Parameter of [UL] command is invalid, please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "EF") == 0) {
		if (argc == 2) {
			bSuccess = erase_flash(dev);
		} else
			printf("Parameter of [EF] command is invalid, please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "WL") == 0) {
		if (argc == 4) {
			UINT uiBegin;
			char *pszEnd;
			uiBegin = strtoul(argv[2], &pszEnd, 0);
			if (*pszEnd)
				printf("Begin is invalid, please check!\r\n");
			else {
				if (is_sparse_image(argv[3]))
						bSuccess = write_sparse_lba(dev, (u32)uiBegin, (u32)-1, argv[3]);
				else {
					bSuccess = true;
					if (is_ubifs_image(argv[3]))
						bSuccess = erase_ubi_block(dev, (u32)uiBegin, (u32)-1);
					if (bSuccess)
						bSuccess = write_lba(dev, (u32)uiBegin, argv[3]);
					else
						printf("Failure of Erase for writing ubi image!\r\n");
				}
			}
		} else
			printf("Parameter of [WL] command is invalid, please check help!\r\n");
	} else if(strcmp(strCmd.c_str(), "WLX") == 0) {
		if (argc == 4) {
			bRet = read_gpt(dev, master_gpt);
			if (bRet) {
				bRet = get_lba_from_gpt(master_gpt, argv[2], &lba, &lba_end);
				if (bRet) {
					if (is_sparse_image(argv[3]))
						bSuccess = write_sparse_lba(dev, (u32)lba, (u32)(lba_end - lba + 1), argv[3]);
					else {
						bSuccess = true;
						if (is_ubifs_image(argv[3]))
						{
							if (lba_end == 0xFFFFFFFF)
								bSuccess = erase_ubi_block(dev, (u32)lba, (u32)lba_end);
							else
								bSuccess = erase_ubi_block(dev, (u32)lba, (u32)(lba_end - lba + 1));
						}
						if (bSuccess)
							bSuccess = write_lba(dev, (u32)lba, argv[3]);
						else
							printf("Failure of Erase for writing ubi image!\r\n");
					}
				} else
					printf("No found %s partition\r\n", argv[2]);
			} else {
				bRet = read_param(dev, param_buffer);
				if (bRet) {
					bRet = get_lba_from_param(param_buffer+8, argv[2], &part_offset, &part_size);
					if (bRet) {
						if (is_sparse_image(argv[3]))
							bSuccess = write_sparse_lba(dev, part_offset, part_size, argv[3]);
						else {

							bSuccess = true;
							if (is_ubifs_image(argv[3]))
								bSuccess = erase_ubi_block(dev, part_offset, part_size);
							if (bSuccess)
								bSuccess = write_lba(dev, part_offset, argv[3]);
							else
								printf("Failure of Erase for writing ubi image!\r\n");
						}
					} else
						printf("No found %s partition\r\n", argv[2]);
				}
				else
					printf("Not found any partition table!\r\n");
			}
			
		} else
			printf("Parameter of [WLX] command is invalid, please check help!\r\n");
	} else if (strcmp(strCmd.c_str(), "RL") == 0) {//Read LBA
		char *pszEnd;
		UINT uiBegin, uiLen;
		if (argc != 5)
			printf("Parameter of [RL] command is invalid, please check help!\r\n");
		else {
			uiBegin = strtoul(argv[2], &pszEnd, 0);
			if (*pszEnd)
				printf("Begin is invalid, please check!\r\n");
			else {
				uiLen = strtoul(argv[3], &pszEnd, 0);
				if (*pszEnd)
					printf("Len is invalid, please check!\r\n");
				else {
					bSuccess = read_lba(dev, uiBegin, uiLen, argv[4]);
				}
			}
		}
	} else if(strcmp(strCmd.c_str(), "PPT") == 0) {
		if (argc == 2) {
			bSuccess = print_gpt(dev);
			if (!bSuccess) {
				bSuccess = print_parameter(dev);
				if (!bSuccess)
					printf("Not found any partition table!\r\n");
			}
		} else
			printf("Parameter of [PPT] command is invalid, please check help!\r\n");
	} else {
		printf("command is invalid!\r\n");
		usage();
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
	snprintf(szProgramProcPath, sizeof(szProgramProcPath), "/proc/%d/exe", getpid());
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
	g_pLogObject = new CRKLog(strLogDir.c_str(), "log",true);

	if(stat(strConfigFile.c_str(), &statBuf) < 0) {
		if (g_pLogObject) {
			g_pLogObject->Record("Error: failed to stat config.ini, err=%d", errno);
		}
	} else if (S_ISREG(statBuf.st_mode)) {
		parse_config_file(strConfigFile.c_str(), g_ConfigItemVec);
	}

	ret = libusb_init(NULL);
	if (ret < 0) {
		if (g_pLogObject) {
			g_pLogObject->Record("Error: libusb_init failed, err=%d", ret);
			delete g_pLogObject;
		}
		return -1;
	}

	pScan = new CRKScan();
	if (!pScan) {
		if (g_pLogObject) {
			g_pLogObject->Record("Error: failed to create object for searching device");
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
