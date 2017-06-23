#include "stdafx.h"


typedef struct _DEVICE_DATA {

	BOOL		HandlesOpen;
	HANDLE		DeviceHandle;
	TCHAR		DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;


HRESULT OpenDevice(
	_Out_		PDEVICE_DATA DeviceData,
	_Out_opt_	PBOOL	FailureDeviceNotFound);

VOID CloseDevice(
	_Inout_ PDEVICE_DATA DeviceData);

BOOL CaptureSampleInit(
	HANDLE		hDeviceHandle);

BOOL CaptureSampleFromDriver(
	HANDLE		hDeviceHandle,
	PWINBIO_CAPTURE_DATA *data);

BOOL Reset(
	HANDLE		hDeviceHandle);

HRESULT RetrieveDevicePath(
	_Out_bytecap_(BufLen)	LPTSTR	DevicePath,
	_In_					ULONG	BufLen,
	_Out_opt_				PBOOL	FailureDeviceNotFound);