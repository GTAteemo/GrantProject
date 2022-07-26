#pragma once
#include <ntifs.h>
#include <ntdef.h>
#include <wingdi.h>

// Request to read virtual user memory (memory of a program) from kernel space
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0666 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request to write virtual user memory (memory of a program) from kernel space
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0667 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request to retrieve the process id of gogo process, from kernel space
#define IO_GET_ID_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0668 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request to retrieve the base address of client.dll in csgo.exe from kernel space
#define IO_GET_MODULE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0669 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Request size of Client.dll
#define IO_GET_MODULE_REQUEST_LENGTH CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0670 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Order driver to apply full protection on Controller
#define IO_PROTECT_CONTROLLER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0671 /* Our Custom Code */, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

// Request Module By index
#define IO_GET_MODULE_INDEX_CONTROLLER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0672 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request ModuleName By index
#define IO_GET_MODULENAME_INDEX_CONTROLLER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0673 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Request ModuleADDR
#define IO_GET_MODULEADDR_INDEX_CONTROLLER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0674 /* Our Custom Code */, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

// Declare the IoControl function.
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP irp);

BOOL call_kernel_function();

// datatype for read request
typedef struct _KERNEL_READ_REQUEST
{
	ULONG ProcessId;
	ULONG Address;
	PVOID pBuff;
	ULONG Size;

} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST
{
	ULONG ProcessId;
	ULONG Address;
	PVOID pBuff;
	ULONG Size;

} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

// datatype for read request
typedef struct _KERNEL_READ_REQUEST64
{
	ULONG ProcessId;
	ULONG64 Address;
	PVOID64 pBuff;
	ULONG64 Size;
	int x;
	int y;
	int w;
	int h;

} KERNEL_READ_REQUEST64, * PKERNEL_READ_REQUEST64;

typedef struct _KERNEL_WRITE_REQUEST64
{
	ULONG ProcessId;
	ULONG64 Address;
	PVOID64 pBuff;
	ULONG64 Size;
} KERNEL_WRITE_REQUEST64, * PKERNEL_WRITE_REQUEST64;

typedef struct _KERNEL_GETID_REQUEST64
{
	ULONG ProcessId;
	int index;
	CHAR* ProcessName;

} KERNEL_GETID_REQUEST64, * PKERNEL_GETID_REQUEST64;

typedef struct _KERNEL_GET_MODULENAME_INDEX_REQUEST64
{
	CHAR ModuleName[64];
	ULONG ProcessId;
	ULONG index;
} KERNEL_GET_MODULENAME_INDEX_REQUEST64, * PKERNEL_GET_MODULENAME_INDEX_REQUEST64;

typedef struct _KERNEL_GET_MODULE_INDEX_REQUEST64
{
	ULONG64 Address;
	ULONG ProcessId;
	ULONG index;
} KERNEL_GET_MODULE_INDEX_REQUEST64, * PKERNEL_GET_MODULE_INDEX_REQUEST64;

typedef struct _KERNEL_GET_MODULEADDR_INDEX_REQUEST64
{
	ULONG64 Address;
	WCHAR* MODULENAME;
	ULONG ProcessId;
} KERNEL_GET_MODULEADDR_INDEX_REQUEST64, * PKERNEL_GET_MODULEADDR_INDEX_REQUEST64;