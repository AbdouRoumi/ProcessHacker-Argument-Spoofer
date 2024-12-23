#include <Windows.h>
#include <stdio.h>
#include <winternl.h>


typedef NTSTATUS(NTAPI* fnNtQueryInformationProcess)(
	HANDLE           ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID            ProcessInformation,
	ULONG            ProcessInformationLength,
	PULONG           ReturnLength
	);