#include "stdafx.h"
#include "winsrv_ctl.h"

TCHAR szCommand[10];
TCHAR szSvcName[80];

SC_HANDLE schSCManager;
SC_HANDLE schService;


BOOL __stdcall stopDependentServices(void);


// stop the service if possible
VOID __stdcall doStopSrv()
{
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	char* service = "WbioSrvc";

	int index = MultiByteToWideChar(CP_ACP, 0, service, -1, NULL, 0);

	MultiByteToWideChar(CP_ACP, 0, service, -1, szSvcName, index);

	// Get a handle to the SCM database.
	schSCManager = OpenSCManager(
		NULL,					// local computer
		NULL,					// servicesActive database
		SC_MANAGER_ALL_ACCESS	// full access rights
		);

	if (NULL == schSCManager) {
		printf("\n OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to hte service.
	schService = OpenService(
		schSCManager,		// SCM database
		szSvcName,			// name of service
		SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS
		);
	if (schService == NULL) {
		printf("\n OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Make sure the service is not already stopped.
	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssStatus,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded)) {

		printf("\n QueryServiceStatusEx failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	if (ssStatus.dwCurrentState == SERVICE_STOPPED) {
		printf("\n Service is already stopped.\n");
		goto stop_cleanup;
	}

	// If a stop is pending, wait for it.
	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
		printf("\n Service stop pending...\n");

		dwWaitTime = ssStatus.dwWaitHint / 10;

		//printf("\n dwWaitTime (%s)\n", dwWaitTime);

		if (dwWaitTime < 1000) {
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000) {
			dwWaitTime = 10000;
		}

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssStatus,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded)) {

			printf("\n QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED) {
			printf("\n Service stopped scuuessfully.\n");
			goto stop_cleanup;
		}

		if (GetTickCount() - dwStartTime > dwWaitTime) {
			printf("\n Service stop time out.\n");
			goto stop_cleanup;
		}
	}

	// If the service is running, dependencies must be stopped first.
	//StopDependentServices();

	// Send a stop code to the service 
	if (!ControlService(
		schService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssStatus)) {

		printf("\n ControlService failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	// Wait fot the service to stop.
	while (ssStatus.dwCurrentState != SERVICE_STOPPED) {
		printf("\n ssStatus.dwCurrentState (0x%08x)\n", ssStatus.dwCurrentState);
		//Sleep(ssStatus.dwWaitHint);
		Sleep(10);
		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssStatus,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			printf("\n QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOPPED) {
			break;
		}

		if (GetTickCount() - dwStartTime > dwTimeout) {
			printf("\n Wait timed out\n");
			goto stop_cleanup;
		}
	}
	printf("\n Service stopped successfully.\n");

stop_cleanup:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

BOOL __stdcall stopDependentServices()
{
	DWORD i;
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS   lpDependencies = NULL;
	ENUM_SERVICE_STATUS     ess;
	SC_HANDLE               hDepService;
	SERVICE_STATUS_PROCESS  ssp;

	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000; // 30-second time-out

	// Pass a zero-length buffer to get the required buffer size.
	if (EnumDependentServices(schService, SERVICE_ACTIVE,
		lpDependencies, 0, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent
		// services, so do nothing.
		printf("\n EnumDependentServices true\n");
		return TRUE;
	}
	else
	{
		printf("\n EnumDependentServices false\n");

		if (GetLastError() != ERROR_MORE_DATA)
			return FALSE; // Unexpected error	

		// Allocate a buffer for the dependencies.
		lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

		if (!lpDependencies)
			return FALSE;

		__try {
			// Enumerate the dependencies.
			if (!EnumDependentServices(schService, SERVICE_ACTIVE,
				lpDependencies, dwBytesNeeded, &dwBytesNeeded,
				&dwCount))
				return FALSE;

			for (i = 0; i < dwCount; i++)
			{
				ess = *(lpDependencies + i);
				// Open the service.
				hDepService = OpenService(schSCManager,
					ess.lpServiceName,
					SERVICE_STOP | SERVICE_QUERY_STATUS);

				if (!hDepService)
					return FALSE;

				__try {
					// Send a stop code.
					if (!ControlService(hDepService,
						SERVICE_CONTROL_STOP,
						(LPSERVICE_STATUS)&ssp))
						return FALSE;

					// Wait for the service to stop.
					while (ssp.dwCurrentState != SERVICE_STOPPED)
					{
						Sleep(ssp.dwWaitHint);
						if (!QueryServiceStatusEx(
							hDepService,
							SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ssp,
							sizeof(SERVICE_STATUS_PROCESS),
							&dwBytesNeeded))
							return FALSE;

						if (ssp.dwCurrentState == SERVICE_STOPPED)
							break;

						if (GetTickCount() - dwStartTime > dwTimeout)
							return FALSE;
					}
				}
				__finally
				{
					// Always release the service handle.
					CloseServiceHandle(hDepService);
				}
			}
		}
		__finally
		{
			// Always free the enumeration buffer.
			HeapFree(GetProcessHeap(), 0, lpDependencies);
		}
	}
	return TRUE;
}