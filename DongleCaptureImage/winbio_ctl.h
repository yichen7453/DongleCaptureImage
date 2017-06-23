#include "stdafx.h"

SIZE_T EnumBiometricUnits();
HRESULT OpenSesseion();
HRESULT CaptureSampleFromService(WINBIO_UNIT_ID *UnitId, PWINBIO_BIR *Sample, SIZE_T *SampleSize, WINBIO_REJECT_DETAIL *RejectDetail);
HRESULT CancelSession();
HRESULT FreeAddress(PVOID address);