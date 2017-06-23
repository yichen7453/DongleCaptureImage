#include "stdafx.h"
#include "winbio_ctl.h"

static WINBIO_SESSION_HANDLE sessionHandle = NULL;

SIZE_T EnumBiometricUnits()
{
	HRESULT hr = S_OK;
	PWINBIO_UNIT_SCHEMA unitSchema = NULL;
	SIZE_T unitCount = 0;
	SIZE_T index = 0;

	hr = WinBioEnumBiometricUnits(
		WINBIO_TYPE_FINGERPRINT,	// Type of biometric unit
		&unitSchema,				// Array of unit schemas
		&unitCount					// Count of unit schemas
		);

	if (FAILED(hr)) {
		printf("\n WinBioEnumBiometricUnits failed. hr = 0x%x\n", hr);
	}

	return unitCount;
}

HRESULT OpenSesseion()
{
	HRESULT hr = S_OK;

	hr = WinBioOpenSession(
		WINBIO_TYPE_FINGERPRINT,	// Service provider
		WINBIO_POOL_SYSTEM,			// Pool type
		WINBIO_FLAG_RAW,			// Raw access
		NULL,						// Array of biometric unit IDs
		0,							// Count of biometric unit IDs
		WINBIO_DB_DEFAULT,			// Default database
		&sessionHandle				// [out] Session handle
		);

	if (FAILED(hr)) {
		printf("\n WinBioOpenSession failed. hr = 0x%x\n", hr);
	}

	return hr;
}

HRESULT CaptureSampleFromService(WINBIO_UNIT_ID *UnitId, PWINBIO_BIR *Sample, SIZE_T *SampleSize, WINBIO_REJECT_DETAIL *RejectDetail)
{
	HRESULT hr = S_OK;
	WINBIO_UNIT_ID unitId = NULL;
	PWINBIO_BIR sample = NULL;
	SIZE_T sampleSize = 0;
	WINBIO_REJECT_DETAIL rejectDetail = 0;

	hr = WinBioCaptureSample(
		sessionHandle,
		WINBIO_NO_PURPOSE_AVAILABLE,
		WINBIO_DATA_FLAG_RAW,
		&unitId,
		&sample,
		&sampleSize,
		&rejectDetail
		);

	if (FAILED(hr)) {
		if (hr == WINBIO_E_BAD_CAPTURE) {
			printf("\n Bad capture; reason: %d\n", rejectDetail);
		}
		else {
			printf("\n WinBioCaptureSample failed. hr = 0x%x\n", hr);
		}
	}

	*UnitId = unitId;
	*Sample = sample;
	*SampleSize = sampleSize;
	*RejectDetail = rejectDetail;

	return hr;
}

HRESULT CancelSession()
{
	HRESULT hr = S_OK;

	hr = WinBioCancel(sessionHandle);

	if (FAILED(hr)) {
		printf("\n WinBioCancel failed. hr = 0x%x\n", hr);
	}

	return hr;
}

HRESULT FreeAddress(PVOID address)
{
	HRESULT hr = S_OK;

	hr = WinBioFree(address);

	if (FAILED(hr)) {
		printf("\n WinBioFree failed. hr = 0x%x\n", hr);
	}

	return hr;
}