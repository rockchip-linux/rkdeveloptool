#ifndef RKSCAN_HEADER
#define RKSCAN_HEADER
#include "DefineHeader.h"
#include "RKLog.h"

typedef struct {
	USHORT usVid;
	USHORT usPid;
	ENUM_RKDEVICE_TYPE emDeviceType;
} STRUCT_DEVICE_CONFIG, *PSTRUCT_DEVICE_CONFIG;

typedef vector<STRUCT_DEVICE_CONFIG> RKDEVICE_CONFIG_SET;
class CRKScan
{
public:
	UINT GetMSC_TIMEOUT();
	void SetMSC_TIMEOUT(UINT value);
 	property<CRKScan, UINT, READ_WRITE> MSC_TIMEOUT;

	UINT GetRKUSB_TIMEOUT();
	void SetRKUSB_TIMEOUT(UINT value);
 	property<CRKScan, UINT, READ_WRITE> RKUSB_TIMEOUT;

	int GetDEVICE_COUNTS();
 	property<CRKScan, int, READ_ONLY> DEVICE_COUNTS;

	CRKScan(UINT uiMscTimeout = 30, UINT uiRKusbTimeout = 20);
	void SetVidPid(USHORT mscVid = 0, USHORT mscPid = 0);
	void AddRockusbVidPid(USHORT newVid, USHORT newPid, USHORT oldVid, USHORT oldPid);
	bool FindRockusbVidPid(ENUM_RKDEVICE_TYPE type, USHORT &usVid, USHORT &usPid);
	int Search(UINT type);
	bool Wait(STRUCT_RKDEVICE_DESC &device, ENUM_RKUSB_TYPE usbType, USHORT usVid = 0, USHORT usPid = 0);
	bool MutexWaitPrepare(UINT_VECTOR &vecExistedDevice, DWORD uiOfflineDevice);
	bool MutexWait(UINT_VECTOR &vecExistedDevice, STRUCT_RKDEVICE_DESC &device, ENUM_RKUSB_TYPE usbType, USHORT usVid = 0, USHORT usPid = 0);
	int GetPos(UINT locationID);
	bool GetDevice(STRUCT_RKDEVICE_DESC &device, int pos);
	bool SetLogObject(CRKLog *pLog);
	~CRKScan();
private:
	UINT   m_waitRKusbSecond;
	UINT   m_waitMscSecond;
	CRKLog *m_log;
	RKDEVICE_DESC_SET m_list;
	RKDEVICE_CONFIG_SET m_deviceConfigSet;
	RKDEVICE_CONFIG_SET m_deviceMscConfigSet;
	int FindConfigSetPos(RKDEVICE_CONFIG_SET &devConfigSet, USHORT vid, USHORT pid);
	int FindWaitSetPos(const RKDEVICE_CONFIG_SET &waitDeviceSet, USHORT vid, USHORT pid);
	void EnumerateUsbDevice(RKDEVICE_DESC_SET &list, UINT &uiTotalMatchDevices);
	void FreeDeviceList(RKDEVICE_DESC_SET &list);
	bool IsRockusbDevice(ENUM_RKDEVICE_TYPE &type, USHORT vid, USHORT pid);
};
#endif