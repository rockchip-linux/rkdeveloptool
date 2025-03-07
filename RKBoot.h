#ifndef RKBOOT_HEADER
#define RKBOOT_HEADER
#include "DefineHeader.h"

#define  BOOT_RESERVED_SIZE 57
#pragma pack(1)
typedef struct  {
	UINT uiTag;
	USHORT usSize;
	DWORD  dwVersion;
	DWORD  dwMergeVersion;
	STRUCT_RKTIME stReleaseTime;
	ENUM_RKDEVICE_TYPE emSupportChip;
	UCHAR uc471EntryCount;
	DWORD dw471EntryOffset;
	UCHAR uc471EntrySize;
	UCHAR uc472EntryCount;
	DWORD dw472EntryOffset;
	UCHAR uc472EntrySize;
	UCHAR ucLoaderEntryCount;
	DWORD dwLoaderEntryOffset;
	UCHAR ucLoaderEntrySize;
	UCHAR ucSignFlag;
	UCHAR ucRc4Flag;
	UCHAR reserved[BOOT_RESERVED_SIZE];
} STRUCT_RKBOOT_HEAD, *PSTRUCT_RKBOOT_HEAD;

typedef struct  {
	UCHAR ucSize;
	ENUM_RKBOOTENTRY emType;
	WCHAR szName[20];
	DWORD dwDataOffset;
	DWORD dwDataSize;
	DWORD dwDataDelay;
} STRUCT_RKBOOT_ENTRY, *PSTRUCT_RKBOOT_ENTRY;


#pragma pack()
class CRKBoot {
public:
	bool GetRc4DisableFlag();
 	property<CRKBoot, bool, READ_ONLY> Rc4DisableFlag;
	bool GetSignFlag();
 	property<CRKBoot, bool, READ_ONLY> SignFlag;
	UINT GetVersion();
 	property<CRKBoot, UINT, READ_ONLY> Version;
	UINT GetMergeVersion();
 	property<CRKBoot, UINT, READ_ONLY> MergeVersion;
	STRUCT_RKTIME GetReleaseTime();
 	property<CRKBoot, STRUCT_RKTIME, READ_ONLY> ReleaseTime;
	ENUM_RKDEVICE_TYPE GetSupportDevice();
 	property<CRKBoot, ENUM_RKDEVICE_TYPE, READ_ONLY> SupportDevice;
	unsigned char GetEntry471Count();
 	property<CRKBoot, unsigned char, READ_ONLY> Entry471Count;
	unsigned char GetEntry472Count();
 	property<CRKBoot, unsigned char, READ_ONLY> Entry472Count;
	unsigned char GetEntryLoaderCount();
 	property<CRKBoot, unsigned char, READ_ONLY> EntryLoaderCount;
	bool CrcCheck();
	bool SaveEntryFile(ENUM_RKBOOTENTRY type, UCHAR ucIndex, string fileName);
	bool GetEntryProperty(ENUM_RKBOOTENTRY type, UCHAR ucIndex, DWORD &dwSize, DWORD &dwDelay, char *pName = NULL);
	char GetIndexByName(ENUM_RKBOOTENTRY type, char *pName);
	bool GetEntryData(ENUM_RKBOOTENTRY type, UCHAR ucIndex, PBYTE lpData);
	CRKBoot(PBYTE lpBootData, DWORD dwBootSize, bool &bCheck);
	~CRKBoot();
protected:
private:
	bool m_bRc4Disable;
	bool m_bSignFlag;
	DWORD m_version;
	DWORD m_mergeVersion;
	STRUCT_RKTIME m_releaseTime;
	ENUM_RKDEVICE_TYPE m_supportDevice;
	DWORD m_471Offset;
	UCHAR m_471Size;
	UCHAR m_471Count;
	DWORD m_472Offset;
	UCHAR m_472Size;
	UCHAR m_472Count;
	DWORD m_loaderOffset;
	UCHAR m_loaderSize;
	UCHAR m_loaderCount;
	BYTE  m_crc[4];
	PBYTE m_BootData;
	DWORD m_BootSize;
	USHORT m_BootHeadSize;
	void WCHAR_To_wchar(WCHAR *src, wchar_t *dst, int len);
	void WCHAR_To_char(WCHAR *src, char *dst, int len);
};

#endif
