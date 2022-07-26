#pragma warning( disable : 4100 4047 4024 4022 4201 4311 4057 4213 4189 4081 4189 4706 4214 4459 4273 4457 4133)

#include "memory.h"
#include <ntdef.h>
#include <minwindef.h>
#include <ntstatus.h>
#include <stdlib.h>
#include "messages.h"
#include <xkeycheck.h>

ULONG GetWindowsBuildNumber()
{
	// Get the Windows Version table.
	RTL_OSVERSIONINFOEXW osversion;
	osversion.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	RtlGetVersion(&osversion);

	return osversion.dwBuildNumber;
}

// https://docs.microsoft.com/en-us/windows/release-information/
// https://www.vergiliusproject.com/kernels/x64/Windows 10 | 2016/1903 19H1 (May 2019 Update)/_EPROCESS
int GetCorrectOffset(CHAR* Name, ULONG BuildNumber)
{
	// 1903 & 1909
	if (BuildNumber == 18362 || BuildNumber == 18363)
	{
		if (strcmp(Name, "ImageFileName") == 0)
		{
			return 0x450;
		}
		if (strcmp(Name, "ActiveThreads") == 0)
		{
			return 0x498;
		}
		if (strcmp(Name, "ActiveProcessLinks") == 0)
		{
			return 0x2F0;
		}
	}
	// 2004 & 2009 & 2104
	else if (BuildNumber == 19041 || BuildNumber == 19042 || BuildNumber == 19043)
	{
		if (strcmp(Name, "ImageFileName") == 0)
		{
			return 0x5a8;
		}
		if (strcmp(Name, "ActiveThreads") == 0)
		{
			return 0x5f0;
		}
		if (strcmp(Name, "ActiveProcessLinks") == 0)
		{
			return 0x448;
		}
	}
	// 2004 & 2009 & 2104
	else if (19044)
	{
		if (strcmp(Name, "ImageFileName") == 0)
		{
			return 0x5a8;
		}
		if (strcmp(Name, "ActiveThreads") == 0)
		{
			return 0x5f0;
		}
		if (strcmp(Name, "ActiveProcessLinks") == 0)
		{
			return 0x448;
		}
	}

	return 0;
}

NTSTATUS KeReadVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{

	SIZE_T Bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(Process, (void*)SourceAddress, PsGetCurrentProcess(),
		(void*)TargetAddress, Size, KernelMode, &Bytes)))
	{
		return STATUS_SUCCESS;
	}
	return STATUS_ACCESS_DENIED;
}

NTSTATUS KeWriteVirtualMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
{
	SourceAddress = 0;
	SIZE_T Bytes;
	if (NT_SUCCESS(MmCopyVirtualMemory(PsGetCurrentProcess(), (void*)SourceAddress, Process,
		(void*)TargetAddress, Size, KernelMode, &Bytes)))
	{
		return STATUS_SUCCESS;
	}
	return STATUS_ACCESS_DENIED;
}

NTSTATUS write_kernel_memory(HANDLE pid, uintptr_t address, PVOID buffer, SIZE_T size)
{
	if (!address || !buffer || !size)
	{
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS status = STATUS_SUCCESS;
	PEPROCESS process;
	PsLookupProcessByProcessId((HANDLE)pid, &process);

	KAPC_STATE state;
	KeStackAttachProcess((PEPROCESS)process, &state);

	MEMORY_BASIC_INFORMATION info;

	status = ZwQueryVirtualMemory(ZwCurrentProcess(), (PVOID)address, MemoryBasicInformation, &info, sizeof(info), NULL);

	if (!NT_SUCCESS(status))
	{
		KeUnstackDetachProcess(&state);
		return STATUS_UNSUCCESSFUL;
	}

	if (((uintptr_t)info.BaseAddress + info.RegionSize) < (address + size))
	{
		KeUnstackDetachProcess(&state);
		return STATUS_UNSUCCESSFUL;
	}

	if (!(info.State & MEM_COMMIT) || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
	{
		KeUnstackDetachProcess(&state);
		return STATUS_UNSUCCESSFUL;
	}

	if ((info.Protect & PAGE_EXECUTE_READWRITE) || (info.Protect & PAGE_EXECUTE_WRITECOPY)
		|| (info.Protect & PAGE_READWRITE) || (info.Protect & PAGE_WRITECOPY))
	{
		RtlCopyMemory((void*)address, buffer, size);
	}
	KeUnstackDetachProcess(&state);
	return STATUS_SUCCESS;
}

NTSTATUS FindProcessByName(CHAR* process_name, vector* ls)
{
	ULONG BuildNumber = GetWindowsBuildNumber();
	PEPROCESS sys_process = PsInitialSystemProcess;
	PEPROCESS cur_entry = sys_process;
	
	int ImageFileNameOffset = GetCorrectOffset("ImageFileName", BuildNumber);
	int ActiveThreadsOffset = GetCorrectOffset("ActiveThreads", BuildNumber);
	int ActiveProcessLinksOffset = GetCorrectOffset("ActiveProcessLinks", BuildNumber);

	
	// Verify atleast one variable so we don't cause BSOD for unsupported builds.
	if (ImageFileNameOffset == 0)
	{
		DebugMessageNormal("Warning: Unsupported Windows build. Update EPROCESS offsets. See README.md.\n");
		return STATUS_UNSUCCESSFUL;
	}

	CHAR image_name[256];
	
	do
	{
		DWORD active_threads;
		RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + ActiveThreadsOffset) /*EPROCESS->ActiveThreads*/, sizeof(active_threads));
		if (active_threads)
		{
			PEPROCESS proc = (PEPROCESS)cur_entry;

			RtlCopyMemory((PVOID64)(&image_name), (PVOID64)((uintptr_t)cur_entry + ImageFileNameOffset) /*EPROCESS->ImageFileName*/, sizeof(image_name));
			if (strstr(process_name, image_name))
			{
				vector_add(ls, cur_entry);
			}
		}

		PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+ActiveProcessLinksOffset) /*EPROCESS->ActiveProcessLinks*/;
		cur_entry = (PEPROCESS)((uintptr_t)list->Flink - ActiveProcessLinksOffset);

	} while (cur_entry != sys_process);

	return STATUS_SUCCESS;
}

MODULEENTRY GetProcessModule(PEPROCESS Process, LPCWSTR ModuleName)
{
	KAPC_STATE KAPC = { 0 };
	MODULEENTRY ret = { 0, 0 };

	KeStackAttachProcess(Process, &KAPC);
	__try
	{
		PPEB32 peb32 = (PPEB32)PsGetProcessWow64Process(Process);
		if (!peb32 || !peb32->Ldr)
		{
			KeUnstackDetachProcess(&KAPC);
			return ret;
		}

		for (PLIST_ENTRY32 plist_entry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)peb32->Ldr)->InLoadOrderModuleList.Flink;
			plist_entry != &((PPEB_LDR_DATA32)peb32->Ldr)->InLoadOrderModuleList;
			plist_entry = (PLIST_ENTRY32)plist_entry->Flink)
		{
			PLDR_DATA_TABLE_ENTRY32 pentry = CONTAINING_RECORD(plist_entry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

			if (wcscmp((PWCH)pentry->BaseDllName.Buffer, ModuleName) == 0)
			{
				ret.Address = pentry->DllBase;
				ret.Size = pentry->SizeOfImage;
				break;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DebugMessageNormal("%s: Exception, Code: 0x%X\n", __FUNCTION__, GetExceptionCode());
	}

	KeUnstackDetachProcess(&KAPC);

	return ret;
}

MODULEENTRY64 GetProcessModule64(PEPROCESS Process, LPCWSTR module_name)
{

	KAPC_STATE KAPC = { 0 };
	MODULEENTRY64 ret = { 0, 0};

	KeStackAttachProcess(Process, &KAPC);


	try
	{
		PPEB pPeb = PsGetProcessPeb(Process);
		if (!pPeb)
		{
			KeUnstackDetachProcess(&KAPC);
			return ret;
		}
		PPEB_LDR_DATA64 pLdr = (PPEB_LDR_DATA64)pPeb->Ldr;
		if (!pLdr)
		{
			KeUnstackDetachProcess(&KAPC);
			return ret;
		}
		for (PLIST_ENTRY list = (PLIST_ENTRY)pLdr->ModuleListLoadOrder.Flink;
			list != &pLdr->ModuleListLoadOrder;
			list = (PLIST_ENTRY)list->Flink)
		{
			PLDR_DATA_TABLE_ENTRY64  pEntry = CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY64, InLoadOrderModuleList);

			
			if (wcscmp((PWCH)pEntry->BaseDllName.Buffer, module_name) == 0)
			{
				ret.Address = (ULONG64)pEntry->DllBase;
				ret.Size = (ULONG)pEntry->SizeOfImage;
				break;
			}
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DebugMessageNormal("%s: Exception, Code: 0x%X\n", __FUNCTION__, GetExceptionCode());
	}

	KeUnstackDetachProcess(&KAPC);
	return ret;
}

MODULEENTRY64 GetAllProcessModule64(PEPROCESS Process, int index)
{

	KAPC_STATE KAPC = { 0 };
	MODULEENTRY64 ret = { 0, 0 };

	KeStackAttachProcess(Process, &KAPC);


	try
	{
		PPEB pPeb = PsGetProcessPeb(Process);
		if (!pPeb)
		{
			KeUnstackDetachProcess(&KAPC);
			return ret;
		}
		PPEB_LDR_DATA64 pLdr = (PPEB_LDR_DATA64)pPeb->Ldr;
		if (!pLdr)
		{
			KeUnstackDetachProcess(&KAPC);
			return ret;
		}
		int i = 0;
		for (PLIST_ENTRY list = (PLIST_ENTRY)pLdr->ModuleListLoadOrder.Flink;
			list != &pLdr->ModuleListLoadOrder;
			list = (PLIST_ENTRY)list->Flink)
		{
			PLDR_DATA_TABLE_ENTRY64  pEntry = CONTAINING_RECORD(list, LDR_DATA_TABLE_ENTRY64, InLoadOrderModuleList);

			if (i==index)
			{
				ret.Address = (ULONG64)pEntry->DllBase;
				ret.Size = (ULONG)pEntry->SizeOfImage;
				wcscpy(ret.ModuleName, (PWCHAR)pEntry->BaseDllName.Buffer);
				break;
			}
			i++;
		}

	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DebugMessageNormal("%s: Exception, Code: 0x%X\n", __FUNCTION__, GetExceptionCode());
	}

	KeUnstackDetachProcess(&KAPC);
	return ret;
}