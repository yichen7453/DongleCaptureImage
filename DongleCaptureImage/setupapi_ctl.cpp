#include "stdafx.h"
#include "setupapi_ctl.h"

#define countof(x) (sizeof(x) / sizeof(x[0]))

static struct {
	WINBIO_CAPTURE_PARAMETERS	captureParams;
	PWINBIO_CAPTURE_DATA		captureData;
	DWORD						captureDataSize;
} Global;

HRESULT RetrieveDevicePath(
	_Out_bytecap_(BufLen)	LPTSTR	DevicePath,
	_In_					ULONG	BufLen,
	_Out_opt_				PBOOL	FailureDeviceNotFound);


HRESULT OpenDevice(
	_Out_		PDEVICE_DATA DeviceData,
	_Out_opt_	PBOOL	FailureDeviceNotFound)
{
	printf("\n --> openDevice() <-- \n");

	HRESULT hr = S_OK;

	DeviceData->HandlesOpen = FALSE;

	hr = RetrieveDevicePath(DeviceData->DevicePath, sizeof(DeviceData->DevicePath), FailureDeviceNotFound);

	if (FAILED(hr)) {
		return hr;
	}

	printf("\n DeviceName = (%ls)\n", DeviceData->DevicePath);

	DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
											GENERIC_WRITE | GENERIC_READ,
											FILE_SHARE_WRITE | FILE_SHARE_READ,
											NULL,
											OPEN_EXISTING,
											FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
											NULL);

	if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("\n Failed to open the device, error - %d\n", GetLastError());
		return hr;
	}
	else {
		printf("\n Open device success.\n");
		hr = S_OK;
	}

	DeviceData->HandlesOpen = TRUE;

	return hr;
}

VOID CloseDevice(
	_Inout_ PDEVICE_DATA DeviceData)
{
	if (FALSE == DeviceData->HandlesOpen) {
		return;
	}

	CloseHandle(DeviceData->DeviceHandle);
	DeviceData->HandlesOpen = FALSE;

	return;
}

BOOL CaptureSampleInit(
	HANDLE	hDeviceHandle)
{
	printf("\n --> captureSampleInit() <-- \n");

	BOOL	bResult = TRUE;
	DWORD	bytesReturned = 0;
	DWORD	allocationSize = 0;

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	Global.captureParams = { 0 };
	Global.captureData = { 0 };
	Global.captureDataSize = { 0 };

	Global.captureParams.PayloadSize = sizeof(WINBIO_CAPTURE_PARAMETERS);
	Global.captureParams.Purpose = WINBIO_PURPOSE_VERIFY;
	Global.captureParams.Format.Owner = WINBIO_ANSI_381_FORMAT_OWNER;
	Global.captureParams.Format.Type = WINBIO_ANSI_381_FORMAT_TYPE;
	CopyMemory(&Global.captureParams.VendorFormat, &GUID_DEVINTERFACE_BIOMETRIC_READER, sizeof(WINBIO_UUID));
	Global.captureParams.Flags = WINBIO_DATA_FLAG_RAW;

	while (true) {
		if (Global.captureData == NULL) {
			bResult = DeviceIoControl(hDeviceHandle, IOCTL_BIOMETRIC_CAPTURE_DATA, &Global.captureParams, sizeof(WINBIO_CAPTURE_PARAMETERS), &allocationSize, sizeof(DWORD), &bytesReturned, NULL);

			if (!bResult && GetLastError() == ERROR_IO_PENDING) {
				SetLastError(ERROR_SUCCESS);

				bResult = GetOverlappedResult(hDeviceHandle, NULL, &bytesReturned, TRUE);
			}

			if (allocationSize < sizeof(WINBIO_CAPTURE_DATA)) {
				allocationSize = sizeof(WINBIO_CAPTURE_DATA);
			}

			Global.captureData = (PWINBIO_CAPTURE_DATA)LocalAlloc(LMEM_FIXED, allocationSize);
			if (!Global.captureData) {
				Global.captureDataSize = 0;
			}
			Global.captureDataSize = allocationSize;

			break;
		}
	}

	return bResult;
}

BOOL CaptureSampleFromDriver(
	HANDLE		hDeviceHandle,
	PWINBIO_CAPTURE_DATA *data)
{
	printf("\n --> CaptureSampleWithDriver() <-- \n");

	DWORD bytesReturned	= 0;

	BOOL  bResult = FALSE;

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	bResult = DeviceIoControl(hDeviceHandle, IOCTL_BIOMETRIC_CAPTURE_DATA, &Global.captureParams, sizeof(WINBIO_CAPTURE_PARAMETERS), Global.captureData, Global.captureDataSize, &bytesReturned, NULL);

	if (bResult && Global.captureData != NULL) {
		//printf("\n Capture data success!\n");
		//printf("\n captureData PayloadSize: %u\n", Global.captureData->PayloadSize);
		*data = Global.captureData;
	}

	return bResult;
}

BOOL Reset(
	HANDLE	hDeviceHandle)
{
	printf("\n --> Reset() <-- \n");

	WINBIO_BLANK_PAYLOAD blankPayload = { 0 };
	DWORD	bytesReturned = 0;
	BOOL	bResult = TRUE;

	if (hDeviceHandle == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	bResult = DeviceIoControl(hDeviceHandle, IOCTL_BIOMETRIC_RESET, NULL, 0, &blankPayload, sizeof(WINBIO_BLANK_PAYLOAD), &bytesReturned, NULL);

	return bResult;
}

HRESULT RetrieveDevicePath(
	_Out_bytecap_(BufLen)	LPTSTR	DevicePath,
	_In_					ULONG	BufLen,
	_Out_opt_				PBOOL	FailureDeviceNotFound)
{
	HDEVINFO							hardwareDeviceInfo;
	SP_DEVICE_INTERFACE_DATA			deviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	deviceInterfaceDetailData = NULL;
	ULONG								length, requiredLength = 0;
	BOOL								bResult = TRUE;
	HRESULT								hr;

	printf("\n  -->  GetDevicePath() <--  \n");

	if (NULL != FailureDeviceNotFound) {
		*FailureDeviceNotFound = FALSE;
	}

	LPGUID interfaceGuid = (LPGUID)&GUID_DEVINTERFACE_BIOMETRIC_READER;

	hardwareDeviceInfo = SetupDiGetClassDevs(interfaceGuid, NULL, NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));

	if (hardwareDeviceInfo == INVALID_HANDLE_VALUE) {
		printf(" SetupDiGetClassDevs failed!\n");
		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	bResult = SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0, interfaceGuid, 0, &deviceInterfaceData);

	if (FALSE == bResult) {		
		if (ERROR_NO_MORE_ITEMS == GetLastError() && NULL != FailureDeviceNotFound) {
			*FailureDeviceNotFound = TRUE;
		}
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf(" SetupDiEnumInterfaces, error - %d\n", GetLastError());
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		return hr;
	}

	bResult = SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInterfaceData, NULL, 0, &requiredLength, NULL);

	if (FALSE == bResult && ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		printf(" Get a failure with insufficient buffer.\n");
		return hr;
	}

	deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LMEM_FIXED, requiredLength);

	if (NULL == deviceInterfaceDetailData) {
		hr = E_OUTOFMEMORY;
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		printf(" Failed to allocate memory.\n");
		return hr;
	}

	deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	length = requiredLength;

	bResult = SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInterfaceData, deviceInterfaceDetailData, length, &requiredLength, NULL);

	if (FALSE == bResult) {
		hr = HRESULT_FROM_WIN32(GetLastError());
		LocalFree(deviceInterfaceDetailData);
		SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
		return hr;
	}

	hr = StringCchCopy(DevicePath, BufLen, deviceInterfaceDetailData->DevicePath);

	LocalFree(deviceInterfaceDetailData);

	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

	return hr;
}