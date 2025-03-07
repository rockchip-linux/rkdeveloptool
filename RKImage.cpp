/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include "RKImage.h"

UINT CRKImage::GetVersion()
{
	return m_version;
}
UINT CRKImage::GetMergeVersion()
{
	return m_mergeVersion;
}
STRUCT_RKTIME CRKImage::GetReleaseTime()
{
	return m_releaseTime;
}
ENUM_RKDEVICE_TYPE CRKImage::GetSupportDevice()
{
	return m_supportDevice;
}
ENUM_OS_TYPE CRKImage::GetOsType()
{
	UINT *pOsType;
	pOsType = (UINT *)&m_reserved[4];
	return (ENUM_OS_TYPE)*pOsType;
}

USHORT CRKImage::GetBackupSize()
{
	USHORT *pBackupSize;
	pBackupSize = (USHORT *)&m_reserved[12];
	return *pBackupSize;
}
UINT CRKImage::GetBootOffset()
{
	return m_bootOffset;
}
UINT CRKImage::GetBootSize()
{
	return m_bootSize;
}
UINT CRKImage::GetFWOffset()
{
	return m_fwOffset;
}
long long CRKImage::GetFWSize()
{
	return m_fwSize;
}
bool CRKImage::SaveBootFile(string filename)
{
	FILE *file = NULL;
	int iRead;
	file = fopen(filename.c_str(), "wb+");
	if (!file) {
		return false;
	}
	BYTE buffer[1024];
	DWORD dwBufferSize = 1024;
	DWORD dwBootSize = m_bootSize;
	DWORD dwReadSize;
	fseek(m_pFile, m_bootOffset, SEEK_SET);
	do {
		dwReadSize = (dwBootSize >= 1024) ? dwBufferSize : dwBootSize;
		iRead = fread(buffer, 1, dwReadSize, m_pFile);
		if (iRead != (int)dwReadSize) {
			fclose(file);
			return false;
		}
		fwrite(buffer, 1, dwReadSize, file);
		dwBootSize -= dwReadSize;
	} while(dwBootSize > 0);
	fclose(file);
	return true;
}
bool CRKImage::SaveFWFile(string filename)
{
	FILE *file = NULL;
	int iRead;
	file = fopen(filename.c_str(), "wb+");
	if (!file) {
		return false;
	}
	BYTE buffer[1024];
	DWORD dwBufferSize = 1024;
	long long dwFWSize = m_fwSize;
	DWORD dwReadSize;
	fseeko(m_pFile, m_fwOffset, SEEK_SET);
	do {
		dwReadSize = (dwFWSize >= 1024) ? dwBufferSize : dwFWSize;
		iRead = fread(buffer, 1, dwReadSize, m_pFile);
		if (iRead != (int)dwReadSize) {
			fclose(file);
			return false;
		}
		fwrite(buffer, 1, dwReadSize, file);
		dwFWSize -= dwReadSize;
	} while (dwFWSize > 0);
	fclose(file);
	return true;
}
bool CRKImage::GetData(long long dwOffset, DWORD dwSize, PBYTE lpBuffer)
{
	if ( (dwOffset < 0) || (dwSize == 0) ) {
		return false;
	}
	if ( dwOffset+dwSize > m_fileSize) {
		return false;
	}
	fseeko(m_pFile, dwOffset, SEEK_SET);
	UINT uiActualRead;
	uiActualRead = fread(lpBuffer,1, dwSize, m_pFile);
	if (dwSize != uiActualRead){
		return false;
	}
	return true;
}
void CRKImage::GetReservedData(PBYTE &lpData, USHORT &usSize)
{
	lpData = m_reserved;
	usSize = IMAGE_RESERVED_SIZE;
}

CRKImage::CRKImage(string filename, bool &bCheck)
{
	Version.setContainer(this);
	Version.getter(&CRKImage::GetVersion);
	MergeVersion.setContainer(this);
	MergeVersion.getter(&CRKImage::GetMergeVersion);
	ReleaseTime.setContainer(this);
	ReleaseTime.getter(&CRKImage::GetReleaseTime);
	SupportDevice.setContainer(this);
	SupportDevice.getter(&CRKImage::GetSupportDevice);
	OsType.setContainer(this);
	OsType.getter(&CRKImage::GetOsType);
	BackupSize.setContainer(this);
	BackupSize.getter(&CRKImage::GetBackupSize);
	BootOffset.setContainer(this);
	BootOffset.getter(&CRKImage::GetBootOffset);
	BootSize.setContainer(this);
	BootSize.getter(&CRKImage::GetBootSize);
	FWOffset.setContainer(this);
	FWOffset.getter(&CRKImage::GetFWOffset);
	FWSize.setContainer(this);
	FWSize.getter(&CRKImage::GetFWSize);
	SignFlag.setContainer(this);
	SignFlag.getter(&CRKImage::GetSignFlag);
	struct stat statBuf;
	m_bootObject = NULL;
	m_pFile = NULL;
	m_bSignFlag = false;

	m_signMd5Size = 0;
	memset(m_md5, 0, 32);
	memset(m_signMd5, 0, 256);

	char szName[256];
	strcpy(szName, filename.c_str());
	if(stat(szName, &statBuf) < 0) {
		bCheck = false;
		return;
	}
	if (S_ISDIR(statBuf.st_mode)) {
		bCheck = false;
		return;
	}
	m_fileSize = statBuf.st_size;

	bool bOnlyBootFile=false;
	transform(filename.begin(), filename.end(), filename.begin(), (int(*)(int))tolower);
	if (filename.find(".bin") != string::npos) {
		bOnlyBootFile = true;
	}

	m_pFile = fopen(szName, "rb");
	if (!m_pFile) {
		bCheck = false;
		return;
	}

	int nMd5DataSize, iRead;
	long long ulFwSize;
	STRUCT_RKIMAGE_HEAD imageHead;
	if (!bOnlyBootFile) {
		fseeko(m_pFile, 0, SEEK_SET);
		iRead = fread((PBYTE)(&imageHead), 1, sizeof(STRUCT_RKIMAGE_HEAD), m_pFile);
		if (iRead != sizeof(STRUCT_RKIMAGE_HEAD)) {
			bCheck = false;
			return;
		}
		if ( imageHead.uiTag != 0x57464B52 ) {
			bCheck = false;
			return;
		}
		if ((imageHead.reserved[14] == 'H') && (imageHead.reserved[15] == 'I')) {
			ulFwSize = *((DWORD *)(&imageHead.reserved[16]));
			ulFwSize <<= 32;
			ulFwSize += imageHead.dwFWOffset;
			ulFwSize += imageHead.dwFWSize;
		} else
			ulFwSize = imageHead.dwFWOffset + imageHead.dwFWSize;
		nMd5DataSize = GetImageSize() - ulFwSize;
		if (nMd5DataSize >= 160) {
			m_bSignFlag = true;
			m_signMd5Size = nMd5DataSize - 32;
			fseeko(m_pFile, ulFwSize, SEEK_SET);
			iRead = fread(m_md5, 1, 32, m_pFile);
			if (iRead != 32) {
				bCheck = false;
				return;
			}
			iRead = fread(m_signMd5, 1, nMd5DataSize - 32, m_pFile);
			if (iRead != (nMd5DataSize - 32)) {
				bCheck = false;
				return;
			}
		} else {
			fseeko(m_pFile, -32, SEEK_END);
			iRead = fread(m_md5, 1, 32, m_pFile);
			if (iRead != 32) {
				bCheck = false;
				return;
			}
		}

		m_version = imageHead.dwVersion;
		m_mergeVersion = imageHead.dwMergeVersion;
		m_releaseTime.usYear = imageHead.stReleaseTime.usYear;
		m_releaseTime.ucMonth = imageHead.stReleaseTime.ucMonth;
		m_releaseTime.ucDay = imageHead.stReleaseTime.ucDay;
		m_releaseTime.ucHour = imageHead.stReleaseTime.ucHour;
		m_releaseTime.ucMinute = imageHead.stReleaseTime.ucMinute;
		m_releaseTime.ucSecond = imageHead.stReleaseTime.ucSecond;
		m_supportDevice = imageHead.emSupportChip;
		m_bootOffset = imageHead.dwBootOffset;
		m_bootSize = imageHead.dwBootSize;
		m_fwOffset = imageHead.dwFWOffset;
		m_fwSize = ulFwSize - m_fwOffset;
		memcpy(m_reserved, imageHead.reserved, IMAGE_RESERVED_SIZE);
	} else {
		m_bootOffset = 0;
		m_bootSize = m_fileSize;
	}

	PBYTE lpBoot;
	lpBoot = new BYTE[m_bootSize];
	fseeko(m_pFile, m_bootOffset, SEEK_SET);
	iRead = fread(lpBoot, 1, m_bootSize, m_pFile);
	if (iRead != (int)m_bootSize) {
		bCheck = false;
		return;
	}
	bool bRet;
	m_bootObject = new CRKBoot(lpBoot, m_bootSize, bRet);
	if (!bRet) {
		bCheck = false;
		return;
	}
	if (bOnlyBootFile) {
		m_supportDevice = m_bootObject->SupportDevice;
		UINT *pOsType;
		pOsType = (UINT *)&m_reserved[4];
		*pOsType = (UINT)RK_OS;
		fclose(m_pFile);
		m_pFile = NULL;
	}
	bCheck = true;
}
CRKImage::~CRKImage()
{
	if (m_pFile) {
		fclose(m_pFile);
		m_pFile = NULL;
	}
	if (m_bootObject) {
		delete m_bootObject;
		m_bootObject = NULL;
	}
}

long long CRKImage::GetImageSize()
{
	return m_fileSize;
}
int CRKImage::GetMd5Data(PBYTE &lpMd5, PBYTE &lpSignMd5)
{
	lpMd5 = m_md5;
	lpSignMd5 = m_signMd5;
	return m_signMd5Size;
}
bool CRKImage::GetSignFlag()
{
	return m_bSignFlag;
}
