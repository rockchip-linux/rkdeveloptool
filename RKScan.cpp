/*
 * (C) Copyright 2017 Fuzhou Rockchip Electronics Co., Ltd
 * Seth Liu 2017.03.01
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "RKScan.h"
#define BUSID(id)  ((id & 0x0000FF00) >> 8)
int CRKScan::GetDEVICE_COUNTS()
{
	return m_list.size();
}

UINT CRKScan::GetMSC_TIMEOUT()
{
	return m_waitMscSecond;
}

UINT CRKScan::GetRKUSB_TIMEOUT()
{
	return m_waitRKusbSecond;
}

void CRKScan::SetMSC_TIMEOUT(UINT value)
{
	m_waitMscSecond = value;
}

void CRKScan::SetRKUSB_TIMEOUT(UINT value)
{
	m_waitRKusbSecond = value;
}

CRKScan::CRKScan(UINT uiMscTimeout, UINT uiRKusbTimeout)
{
	DEVICE_COUNTS.setContainer(this);
    DEVICE_COUNTS.getter(&CRKScan::GetDEVICE_COUNTS);

 	MSC_TIMEOUT.setContainer(this);
    MSC_TIMEOUT.getter(&CRKScan::GetMSC_TIMEOUT);
 	MSC_TIMEOUT.setter(&CRKScan::SetMSC_TIMEOUT);

	RKUSB_TIMEOUT.setContainer(this);
   	RKUSB_TIMEOUT.getter(&CRKScan::GetRKUSB_TIMEOUT);
 	RKUSB_TIMEOUT.setter(&CRKScan::SetRKUSB_TIMEOUT);

	m_waitMscSecond = uiMscTimeout;
	m_waitRKusbSecond = uiRKusbTimeout;
	m_log = NULL;
	m_list.clear();
	m_deviceConfigSet.clear();
	m_deviceMscConfigSet.clear();
}
bool CRKScan::FindRockusbVidPid(ENUM_RKDEVICE_TYPE type, USHORT &usVid, USHORT &usPid)
{
	UINT i;
	bool bRet = false;
	for (i = 0; i < m_deviceConfigSet.size(); i++) {
		if (m_deviceConfigSet[i].emDeviceType == type) {
			usVid = m_deviceConfigSet[i].usVid;
			usPid = m_deviceConfigSet[i].usPid;
			bRet = true;
			break;
		}
	}
	return bRet;
}
void CRKScan::AddRockusbVidPid(USHORT newVid, USHORT newPid, USHORT oldVid, USHORT oldPid)
{
	if ((newVid == 0) || (newPid == 0) || (oldVid == 0) || (oldPid == 0)) {
		return;
	}
	STRUCT_DEVICE_CONFIG config;
	unsigned int i;
	for (i = 0; i < m_deviceConfigSet.size(); i++) {
		if ((m_deviceConfigSet[i].usVid == oldVid) && (m_deviceConfigSet[i].usPid == oldPid)) {
			config.usVid = newVid;
			config.usPid = newPid;
			config.emDeviceType = m_deviceConfigSet[i].emDeviceType;
			break;
		}
	}
	if (i < m_deviceConfigSet.size())
		m_deviceConfigSet.push_back(config);
}

void CRKScan::SetVidPid(USHORT mscVid, USHORT mscPid)
{
	STRUCT_DEVICE_CONFIG config;
	m_deviceConfigSet.clear();

	config.emDeviceType = RK27_DEVICE;
	config.usPid = 0x3201;
	config.usVid = 0x071B;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK28_DEVICE;
	config.usPid = 0x3228;
	config.usVid = 0x071B;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RKNANO_DEVICE;
	config.usPid = 0x3226;
	config.usVid = 0x071B;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RKCROWN_DEVICE;
	config.usPid = 0x261A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK281X_DEVICE;
	config.usPid = 0x281A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RKCAYMAN_DEVICE;
	config.usPid = 0x273A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK29_DEVICE;
	config.usPid = 0x290A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RKPANDA_DEVICE;
	config.usPid = 0x282B;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RKSMART_DEVICE;
	config.usPid = 0x262C;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK292X_DEVICE;
	config.usPid = 0x292A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK30_DEVICE;
	config.usPid = 0x300A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK30B_DEVICE;
	config.usPid = 0x300B;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK31_DEVICE;
	config.usPid = 0x310B;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK31_DEVICE;
	config.usPid = 0x310C;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	config.emDeviceType = RK32_DEVICE;
	config.usPid = 0x320A;
	config.usVid = 0x2207;
	m_deviceConfigSet.push_back(config);

	m_deviceMscConfigSet.clear();

	config.emDeviceType = RKNONE_DEVICE;
	config.usPid = 0x3203;
	config.usVid = 0x071B;
	m_deviceMscConfigSet.push_back(config);

	config.emDeviceType = RKNONE_DEVICE;
	config.usPid = 0x3205;
	config.usVid = 0x071B;
	m_deviceMscConfigSet.push_back(config);

	config.emDeviceType = RKNONE_DEVICE;
	config.usPid = 0x2910;
	config.usVid = 0x0BB4;
	m_deviceMscConfigSet.push_back(config);

	config.emDeviceType = RKNONE_DEVICE;
	config.usPid = 0x0000;
	config.usVid = 0x2207;
	m_deviceMscConfigSet.push_back(config);

	config.emDeviceType = RKNONE_DEVICE;
	config.usPid = 0x0010;
	config.usVid = 0x2207;
	m_deviceMscConfigSet.push_back(config);

	if ((mscVid != 0) || (mscPid != 0)) {
		if (FindConfigSetPos(m_deviceMscConfigSet, mscVid, mscPid) == -1) {
			config.emDeviceType = RKNONE_DEVICE;
			config.usPid = mscPid;
			config.usVid = mscVid;
			m_deviceMscConfigSet.push_back(config);
		}
	}
}
int CRKScan::FindWaitSetPos(const RKDEVICE_CONFIG_SET &waitDeviceSet, USHORT vid, USHORT pid)
{
	int pos=-1;
	UINT i;
	for ( i = 0; i < waitDeviceSet.size(); i++ ) {
		if ( (vid == waitDeviceSet[i].usVid) && (pid == waitDeviceSet[i].usPid) ) {
			pos = i;
			break;
		}
	}
	return pos;
}
int CRKScan::FindConfigSetPos(RKDEVICE_CONFIG_SET &devConfigSet, USHORT vid, USHORT pid)
{
	int pos = -1;
	UINT i;
	for ( i = 0; i < devConfigSet.size(); i++ ) {
		if ( (vid == devConfigSet[i].usVid) && (pid == devConfigSet[i].usPid) ) {
			pos = i;
			break;
		}
	}
	return pos;
}
void CRKScan::EnumerateUsbDevice(RKDEVICE_DESC_SET &list, UINT &uiTotalMatchDevices)
{
	STRUCT_RKDEVICE_DESC desc;
	struct libusb_device_descriptor descriptor;
	int ret,i,cnt;

	uiTotalMatchDevices = 0;
	libusb_device **pDevs = NULL;
	libusb_device *dev;
	ret = libusb_get_device_list(NULL, &pDevs);
	if (ret < 0) {
		if (m_log)
			m_log->Record("Error:EnumerateUsbDevice-->get_device_list failed,err=%d!", ret);
		return;
	}
	cnt = ret;
	for (i = 0; i < cnt; i++) {
		dev = pDevs[i];
		if (dev) {
			ret = libusb_get_device_descriptor (dev, &descriptor);
			if (ret < 0) {
				libusb_free_device_list(pDevs, 1);
				if (m_log)
					m_log->Record("Error:EnumerateUsbDevice-->get_device_descriptor failed,err=%d!", ret);
				return;
			}
			desc.emDeviceType = RKNONE_DEVICE;
			desc.emUsbType = RKUSB_NONE;
			desc.pUsbHandle = (void *)dev;
			desc.usbcdUsb = descriptor.bcdUSB;
			desc.usVid = descriptor.idVendor;
			desc.usPid = descriptor.idProduct;
			desc.uiLocationID = libusb_get_bus_number(dev);
			desc.uiLocationID <<= 8;
			desc.uiLocationID += libusb_get_port_number(dev);
			libusb_ref_device(dev);
			uiTotalMatchDevices++;
			list.push_back(desc);
		}

	}
	libusb_free_device_list(pDevs, 1);

}

void CRKScan::FreeDeviceList(RKDEVICE_DESC_SET &list)
{
	device_list_iter iter;
	 for (iter = list.begin(); iter != list.end(); iter++) {
		if ((*iter).pUsbHandle) {
			libusb_unref_device((libusb_device *)((*iter).pUsbHandle));
			(*iter).pUsbHandle = NULL;
		}
	}
   	list.clear();
}
bool CRKScan::IsRockusbDevice(ENUM_RKDEVICE_TYPE &type, USHORT vid, USHORT pid)
{
	int iPos;
	iPos = FindConfigSetPos(m_deviceConfigSet, vid, pid);
	if (iPos != -1) {
		type = m_deviceConfigSet[iPos].emDeviceType;
		return true;
	}
	if (vid == 0x2207) {
		if ((pid >> 8) > 0) {
			type = RKNONE_DEVICE;
			return true;
		}
	}
	return false;
}
int CRKScan::Search(UINT type)
{
	device_list_iter iter,new_iter;
	ENUM_RKDEVICE_TYPE devType;
	UINT uiTotalDevice;
	int iPos;

	FreeDeviceList(m_list);
    EnumerateUsbDevice( m_list, uiTotalDevice );

	for ( iter = m_list.begin(); iter != m_list.end(); ) {
		if( (iPos = FindConfigSetPos(m_deviceMscConfigSet, (*iter).usVid, (*iter).usPid)) != -1 ) {
			(*iter).emDeviceType = RKNONE_DEVICE;
			iter++;
			continue;
		} else if (IsRockusbDevice(devType, (*iter).usVid, (*iter).usPid) ) {
			(*iter).emDeviceType = devType;
			iter++;
			continue;
		} else {
			if ((*iter).pUsbHandle) {
				libusb_unref_device((libusb_device *)((*iter).pUsbHandle));
				(*iter).pUsbHandle = NULL;
			}
			iter = m_list.erase(iter);
			uiTotalDevice--;
		}
	}

	if (m_list.size() <= 0) {
		return 0;
	}

	if ( (type & RKUSB_MASKROM) == 0 ) {
		for ( iter = m_list.begin(); iter != m_list.end(); ) {
			if( (IsRockusbDevice(devType, (*iter).usVid, (*iter).usPid)) && (((*iter).usbcdUsb & 0x1) == 0) ) {
	            if ((*iter).pUsbHandle) {
					libusb_unref_device((libusb_device *)((*iter).pUsbHandle));
					(*iter).pUsbHandle = NULL;
				}
				iter = m_list.erase(iter);
	            uiTotalDevice--;
			} else {
				iter++;
				continue;
			}
		}
	}
	if (m_list.size() <= 0) {
		return 0;
	}

	if ( (type & RKUSB_LOADER) == 0 ) {
		for ( iter = m_list.begin(); iter != m_list.end(); ) {
			if( (IsRockusbDevice(devType, (*iter).usVid, (*iter).usPid)) && (((*iter).usbcdUsb & 0x1) == 1) ) {
	            if ((*iter).pUsbHandle) {
					libusb_unref_device((libusb_device *)((*iter).pUsbHandle));
					(*iter).pUsbHandle = NULL;
				}
				iter = m_list.erase(iter);
	            uiTotalDevice--;
			} else {
				iter++;
				continue;
			}
		}
	}
	if (m_list.size() <= 0) {
		return 0;
	}

	if ( (type & RKUSB_MSC) == 0 ) {
		for ( iter = m_list.begin(); iter != m_list.end(); ) {
			if(FindConfigSetPos(m_deviceMscConfigSet, (*iter).usVid, (*iter).usPid) != -1) {
                if ((*iter).pUsbHandle) {
					libusb_unref_device((libusb_device *)((*iter).pUsbHandle));
					(*iter).pUsbHandle = NULL;
				}
				iter = m_list.erase(iter);
                uiTotalDevice--;
			} else {
				iter++;
				continue;
			}
		}
	}
	if (m_list.size() <= 0) {
		return 0;
	}

	for ( iter = m_list.begin(); iter != m_list.end(); iter++ ) {
		if (FindConfigSetPos(m_deviceMscConfigSet, (*iter).usVid, (*iter).usPid) != -1) {
			(*iter).emUsbType = RKUSB_MSC;
		} else {
			USHORT usTemp;
			usTemp = (*iter).usbcdUsb;
			usTemp= usTemp & 0x1;
			if ( usTemp == 0 )
				(*iter).emUsbType = RKUSB_MASKROM;
			else
				(*iter).emUsbType = RKUSB_LOADER;
		}
	}
	return m_list.size();
}
bool CRKScan::MutexWait(UINT_VECTOR &vecExistedDevice, STRUCT_RKDEVICE_DESC &device, ENUM_RKUSB_TYPE usbType, USHORT usVid, USHORT usPid)
{
	device_list_iter iter;
	int uiWaitSecond;
	int iFoundCount = 0;
	UINT iRet,i;
	bool bFound = false;
	if (usbType == RKUSB_MSC)
		uiWaitSecond = m_waitMscSecond;
	else
		uiWaitSecond = m_waitRKusbSecond;
	time_t tmInit, tmNow;
	time(&tmInit);
	device.uiLocationID = 0;
	while( difftime(time(&tmNow), tmInit) <= uiWaitSecond ) {
		iRet = Search(RKUSB_MASKROM | RKUSB_LOADER | RKUSB_MSC);
		if ( iRet == vecExistedDevice.size() + 1 ) {
			for (i = 0; i < vecExistedDevice.size(); i++) {
				for (iter = m_list.begin(); iter != m_list.end(); ) {
					if ((*iter).uiLocationID == vecExistedDevice[i]) {
						iter = m_list.erase(iter);
					} else
						iter++;
				}
			}
			if (m_list.size() != 1) {
				device.uiLocationID = 0;
				iFoundCount = 0;
			} else {
				iter = m_list.begin();
				if (device.uiLocationID == 0) {
					iFoundCount++;
					device.uiLocationID = (*iter).uiLocationID;
				} else {
					if (device.uiLocationID == (*iter).uiLocationID) {
						iFoundCount++;
					} else {
						device.uiLocationID = 0;
						iFoundCount = 0;
					}
				}
			}
		} else {
			device.uiLocationID = 0;
			iFoundCount = 0;
		}
		if (iFoundCount >= 10) {
			bFound = true;
			break;
		}
	}
	if (!bFound) {
		return false;
	}
	bFound = Wait(device, usbType, usVid, usPid);
	return bFound;
}

bool CRKScan::MutexWaitPrepare(UINT_VECTOR &vecExistedDevice, DWORD uiOfflineDevice)
{
	int iRet, iRet2;
	device_list_iter iter;
	time_t timeInit, timeNow;
	time(&timeInit);
	iRet = iRet2 =0;
	while ((time(&timeNow) - timeInit) <= 3) {
		iRet = Search(RKUSB_MASKROM | RKUSB_LOADER | RKUSB_MSC);
		usleep(20000);
		iRet2 = Search(RKUSB_MASKROM | RKUSB_LOADER | RKUSB_MSC);
		if (iRet2 == iRet) {
			break;
		}
	}
	if ((iRet <= 0) || (iRet2 != iRet)) {
		return false;
	}
	vecExistedDevice.clear();
	bool bFound = false;
	for ( iter = m_list.begin(); iter != m_list.end(); iter++ ) {
		if ((*iter).uiLocationID != uiOfflineDevice) {
			vecExistedDevice.push_back((*iter).uiLocationID);
		} else
			bFound = true;
	}
	if (!bFound) {
		return false;
	}
	return true;

}

bool CRKScan::Wait(STRUCT_RKDEVICE_DESC &device, ENUM_RKUSB_TYPE usbType, USHORT usVid, USHORT usPid)
{
	RKDEVICE_DESC_SET deviceList;
	ENUM_RKUSB_TYPE curDeviceType;
	ENUM_RKDEVICE_TYPE devType;
	device_list_iter iter;
	UINT totalDevice;
	int uiWaitSecond;
	int iFoundCount = 0;
	bool bRet = false;
	if (usbType == RKUSB_MSC)
		uiWaitSecond = m_waitMscSecond;
	else
		uiWaitSecond = m_waitRKusbSecond;
	time_t tmInit, tmNow;
	time(&tmInit);
	while( difftime(time(&tmNow), tmInit) <= uiWaitSecond ) {
		FreeDeviceList(deviceList);
		EnumerateUsbDevice(deviceList, totalDevice);
		for ( iter = deviceList.begin(); iter != deviceList.end(); iter++ ) {
			if ((BUSID((*iter).uiLocationID) != BUSID(device.uiLocationID)) ||
				((BUSID((*iter).uiLocationID) == BUSID(device.uiLocationID)) && ((*iter).uiLocationID >= device.uiLocationID))) {
				if ((usVid != 0) || (usPid != 0)) {
					if ( ((*iter).usVid != usVid) || ((*iter).usPid != usPid) )
						continue;
				}
				if (IsRockusbDevice(devType, (*iter).usVid, (*iter).usPid)) {
					if ( ((*iter).usbcdUsb & 0x0001) == 0 )
						curDeviceType = RKUSB_MASKROM;
					else
						curDeviceType = RKUSB_LOADER;
				} else if ( FindConfigSetPos(m_deviceMscConfigSet, (*iter).usVid, (*iter).usPid) != -1 ) {
					curDeviceType = RKUSB_MSC;
				} else
					curDeviceType = RKUSB_NONE;

				if ( curDeviceType == usbType ) {
					iFoundCount++;
					break;
				}
			}
		}
		if ( iter == deviceList.end() ) {
			iFoundCount = 0;
		}
		if ( iFoundCount >= 8 ) {
			device.usVid = (*iter).usVid;
			device.usPid = (*iter).usPid;
			device.uiLocationID = (*iter).uiLocationID;
			device.pUsbHandle= (*iter).pUsbHandle;
			device.emUsbType = usbType;
			device.usbcdUsb = (*iter).usbcdUsb;
			libusb_ref_device((libusb_device *)device.pUsbHandle);

			if (usbType == RKUSB_MSC) {
				device.emDeviceType = RKNONE_DEVICE;
			} else {
				if (IsRockusbDevice(devType, device.usVid, device.usPid))
					device.emDeviceType = devType;
			}
			bRet = true;
			break;
		}
		usleep(50000);
	}

	FreeDeviceList(deviceList);
	return bRet;
}
int CRKScan::GetPos(UINT locationID)
{
	device_list_iter iter;
	int pos = 0;
	bool bFound = false;
	for (iter = m_list.begin(); iter != m_list.end(); iter++) {
		if (locationID == (*iter).uiLocationID) {
			bFound=true;
			break;
		}
		pos++;
	}
	return (bFound ? pos : -1);
}
bool CRKScan::GetDevice(STRUCT_RKDEVICE_DESC &device, int pos)
{
	if ( (pos < 0) || (pos >= (int)m_list.size()) ) {
		return false;
	}
	device_list_iter iter;
	for (iter = m_list.begin(); iter != m_list.end(); iter++) {
		if (pos == 0) {
			break;
		}
		pos--;
	}
	device.usVid = (*iter).usVid;
	device.usPid = (*iter).usPid;
	device.emDeviceType = (*iter).emDeviceType;
	device.emUsbType = (*iter).emUsbType;
	device.uiLocationID = (*iter).uiLocationID;
	device.pUsbHandle= (*iter).pUsbHandle;
	device.usbcdUsb = (*iter).usbcdUsb;
	return true;
}

bool CRKScan::SetLogObject(CRKLog *pLog)
{
	if (pLog) {
		if (m_log) {
			delete m_log;
		}
		m_log = pLog;
	} else
		return false;
	return true;
}
CRKScan::~CRKScan()
{
	FreeDeviceList(m_list);
	if (m_log) {
		delete m_log;
		m_log = NULL;
	}
}
