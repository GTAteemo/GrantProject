#pragma warning( disable : 4100 4047 4024 4022 4201 4311 4057 4213 4189 4081 4189 4706 4214 4459 4273)

#include "data.h"
#include "messages.h"
#include "communication.h"
#include "memory.h"



NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

// IOCTL Call Handler function

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS Status;
	ULONG BytesIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	// Code received from user space
	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	if (ControlCode == IO_READ_REQUEST)
	{
		// Get the input buffer & format it to our struct
		PKERNEL_READ_REQUEST64 ReadInput = (PKERNEL_READ_REQUEST64)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;

		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(ReadInput->ProcessId, &Process))) {

			//read from ReadInput->Address and write it to pBuff so we can use the data in our controller
			KeReadVirtualMemory(Process, ReadInput->Address, ReadInput->pBuff, ReadInput->Size);
		}
		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_READ_REQUEST);
	}
	else if (ControlCode == IO_WRITE_REQUEST)
	{
		// Get the input buffer & format it to our struct
		PKERNEL_WRITE_REQUEST64 WriteInput = (PKERNEL_WRITE_REQUEST64)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(WriteInput->ProcessId, &Process))) {
			// copy the value of pBuff to WriteInput->Address

			PVOID kernelBuff = ExAllocatePool(NonPagedPool, WriteInput->Size);

			if (!kernelBuff)
			{
				Status = STATUS_UNSUCCESSFUL;
			}

			if (!memcpy(kernelBuff, WriteInput->pBuff, WriteInput->Size))
			{
				Status = STATUS_UNSUCCESSFUL;
			}
			else
			{
				
				write_kernel_memory((HANDLE)WriteInput->ProcessId, WriteInput->Address, kernelBuff, WriteInput->Size);
				ExFreePool(kernelBuff);
			}

		}
		//DebugMessageNormal("Write Size:  %d \n", WriteInput->Size);

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(KERNEL_WRITE_REQUEST);
		
	}
	else if (ControlCode == IO_GET_ID_REQUEST)
	{
		PKERNEL_GETID_REQUEST64 OutPut = (PKERNEL_GETID_REQUEST64)Irp->AssociatedIrp.SystemBuffer;


		vector processes;
		vector_init(&processes);
		FindProcessByName(OutPut->ProcessName, &processes);

		if (vector_total(&processes) > 0);
		{
			if( OutPut->index < processes.total)
			{
				PEPROCESS proc = (PEPROCESS)vector_get(&processes, OutPut->index);
				OutPut->ProcessId = (ULONG)PsGetProcessId(proc);
			}
			else
			{
				OutPut->ProcessId = 0;
			}
		}
		vector_free(&processes);

		
		
		Status = STATUS_SUCCESS;
		BytesIO = sizeof(PKERNEL_GETID_REQUEST64);
	}
	else if (ControlCode == IO_GET_MODULENAME_INDEX_CONTROLLER)
	{
		PKERNEL_GET_MODULENAME_INDEX_REQUEST64 MoudleInput = (PKERNEL_GET_MODULENAME_INDEX_REQUEST64)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		
		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(MoudleInput->ProcessId, &Process))) {
			MODULEENTRY64 moduleEntry = GetAllProcessModule64(Process, MoudleInput->index);
			UNICODE_STRING UnicodeString1;
			RtlInitUnicodeString(&UnicodeString1, moduleEntry.ModuleName);
			ANSI_STRING AnsiString1;
			NTSTATUS nStatus = RtlUnicodeStringToAnsiString(&AnsiString1, &UnicodeString1, TRUE);
			if (NT_SUCCESS(nStatus))
			{
				RtlCopyMemory(MoudleInput->ModuleName, AnsiString1.Buffer, sizeof(AnsiString1.Buffer));
			}
			//Ïú»ÙAnsiString1
			RtlFreeAnsiString(&AnsiString1);

		}

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(PKERNEL_GET_MODULENAME_INDEX_REQUEST64);
	}
	else if (ControlCode == IO_GET_MODULEADDR_INDEX_CONTROLLER)
	{
		PKERNEL_GET_MODULEADDR_INDEX_REQUEST64 MoudleInput = (PKERNEL_GET_MODULEADDR_INDEX_REQUEST64)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		
		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(MoudleInput->ProcessId, &Process))) {

			WCHAR clone[260];
			wcscpy(clone, MoudleInput->MODULENAME);
			MODULEENTRY64 moduleEntry = GetProcessModule64(Process, clone);
			MoudleInput->Address = moduleEntry.Address;
		}

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(PKERNEL_GET_MODULENAME_INDEX_REQUEST64);
	}
	else if (ControlCode == IO_GET_MODULE_INDEX_CONTROLLER)
	{
		PKERNEL_GET_MODULE_INDEX_REQUEST64 MoudleInput = (PKERNEL_GET_MODULE_INDEX_REQUEST64)Irp->AssociatedIrp.SystemBuffer;

		PEPROCESS Process;
		
		// Get our process
		if (NT_SUCCESS(PsLookupProcessByProcessId(MoudleInput->ProcessId, &Process))) {
			MODULEENTRY64 moduleEntry = GetAllProcessModule64(Process, MoudleInput->index);
			MoudleInput->Address = moduleEntry.Address;
		}

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(PKERNEL_GET_MODULE_INDEX_REQUEST64);
	}
	/*MoudleInput->Address = (ULONG64)moduleEntry.Address;*/
	else if (ControlCode == IO_GET_MODULE_REQUEST)
	{
		PULONG64 OutPut = (PULONG64)Irp->AssociatedIrp.SystemBuffer;
		*OutPut = UnityPlayerDll;
		
		Status = STATUS_SUCCESS;
		BytesIO = sizeof(*OutPut);
	}
	else if (ControlCode == IO_GET_MODULE_REQUEST_LENGTH)
	{
		PULONG64 OutPut = (PULONG64)Irp->AssociatedIrp.SystemBuffer;
		*OutPut = UnityPlayerDllSize;

		Status = STATUS_SUCCESS;
		BytesIO = sizeof(*OutPut);
	}
	else if (ControlCode == IO_PROTECT_CONTROLLER)
	{
		ProtectController = 1;
		DebugMessageNormal("IO_PROTECT_CONTROLLER received. Controller is marked ready, and fully protected. \n");
		Status = STATUS_SUCCESS;
	}
	else
	{
		// if the code is unknown
		Status = STATUS_INVALID_PARAMETER;
	}

	// Complete the request
	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIO;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}


//if (0)
//{
//	vector processes;
//	vector_init(&processes);
//	FindProcessByName("EscapeFrom", &processes);
//
//
//	if (vector_total(&processes) > 0)
//	{
//
//		//// First should be good.
//		for (int i = 0; i < processes.total; i++)
//		{
//			PEPROCESS proc = (PEPROCESS)vector_get(&processes, i);
//			TkfID = (ULONG)PsGetProcessId(proc);
//			MODULEENTRY64 UnityEntry = { 0,0 };
//			UnityEntry = GetProcessModule64(proc, L"UnityPlayer.dll");
//			if (UnityEntry.Address)
//			{
//				UnityPlayerDll = UnityEntry.Address;
//				UnityPlayerDllSize = UnityEntry.Size;
//				break;
//			}
//		}
//
//	}
//	vector_free(&processes);
//}