#ifndef RKIMAGE_HEADER
#define RKIMAGE_HEADER
#include "DefineHeader.h"
#include "RKBoot.h"
#define  IMAGE_RESERVED_SIZE 61
#pragma pack(1)
typedef struct {
	UINT uiTag;
	USHORT usSize;
	DWORD  dwVersion;
	DWORD  dwMergeVersion;
	STRUCT_RKTIME stReleaseTime;
	ENUM_RKDEVICE_TYPE emSupportChip;
	DWORD  dwBootOffset;
	DWORD  dwBootSize;
	DWORD  dwFWOffset;
	DWORD  dwFWSize;
	BYTE   reserved[IMAGE_RESERVED_SIZE];
} STRUCT_RKIMAGE_HEAD, *PSTRUCT_RKIMAGE_HEAD;
#pragma pack()
class CRKImage
{
public:
	UINT GetVersion();
 	property<CRKImage, UINT, READ_ONLY> Version;
	UINT GetMergeVersion();
 	property<CRKImage, UINT, READ_ONLY> MergeVersion;
	STRUCT_RKTIME GetReleaseTime();
 	property<CRKImage, STRUCT_RKTIME, READ_ONLY> ReleaseTime;
	ENUM_RKDEVICE_TYPE GetSupportDevice();
 	property<CRKImage, ENUM_RKDEVICE_TYPE, READ_ONLY> SupportDevice;
	ENUM_OS_TYPE GetOsType();
 	property<CRKImage, ENUM_OS_TYPE, READ_ONLY> OsType;

	unsigned short GetBackupSize();
 	property<CRKImage, unsigned short, READ_ONLY> BackupSize;
	UINT GetBootOffset();
 	property<CRKImage, UINT, READ_ONLY> BootOffset;
	UINT GetBootSize();
 	property<CRKImage, UINT, READ_ONLY> BootSize;
	UINT GetFWOffset();
 	property<CRKImage, UINT, READ_ONLY> FWOffset;
	long long GetFWSize();
 	property<CRKImage, long long, READ_ONLY> FWSize;
	bool GetSignFlag();
	property<CRKImage, bool, READ_ONLY> SignFlag;

	CRKBoot *m_bootObject;
	bool SaveBootFile(string filename);
	bool SaveFWFile(string filename);
	bool GetData(long long dwOffset, DWORD dwSize, PBYTE lpBuffer);
	void GetReservedData(PBYTE &lpData, USHORT &usSize);
	int GetMd5Data(PBYTE &lpMd5, PBYTE &lpSignMd5);
	long long GetImageSize();
	CRKImage(string filename, bool &bCheck);
	~CRKImage();
protected:

private:
	DWORD m_version;
	DWORD m_mergeVersion;
	STRUCT_RKTIME m_releaseTime;
	ENUM_RKDEVICE_TYPE m_supportDevice;
	DWORD m_bootOffset;
	DWORD m_bootSize;
	DWORD m_fwOffset;
	long long m_fwSize;

	BYTE  m_md5[32];
	BYTE  m_signMd5[256];
	BYTE  m_reserved[IMAGE_RESERVED_SIZE];
	bool  m_bSignFlag;
	int   m_signMd5Size;
	FILE *m_pFile;
	long long m_fileSize;
};
#endif