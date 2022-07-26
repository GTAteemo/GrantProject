#pragma once
#include <ntifs.h>
#include "gstructs.h"
#include "vector.h"
#include "ntos.h"

ULONG GetWindowsBuildNumber();

int GetCorrectOffset(CHAR* Name, ULONG BuildNumber);

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size);

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size);

NTSTATUS FindProcessByName(CHAR* process_name, vector* list);

NTSTATUS write_kernel_memory(HANDLE pid, uintptr_t address, PVOID buffer, SIZE_T size);

MODULEENTRY GetProcessModule(PEPROCESS Process, LPCWSTR ModuleName);
MODULEENTRY64 GetProcessModule64(PEPROCESS Process, LPCWSTR module_name);
MODULEENTRY64 GetAllProcessModule64(PEPROCESS Process, int index);