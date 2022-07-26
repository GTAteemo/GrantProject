#pragma once
#include <ntifs.h>
#include <windef.h>
#include "vector.h"

// We store every value here.
ULONG TkfID, ControllerID, ProtectController;
ULONG64 UnityPlayerDll, UnityPlayerDllSize;

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;
vector CSRSSList;
BOOLEAN IsManualMapped;


// Constants
#define EnableProcessHiding 1 // Used to hide our process.
#define EnableDriverHiding 1 // Used to hide our driver.

#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#define PROCESS_VM_OPERATION 0x0008
#define PROCESS_VM_READ 0x0010
#define PROCESS_VM_WRITE 0x0020
#define PROCESS_TERMINATE 0x0001

typedef struct _OB_REG_CONTEXT {
	USHORT Version;
	UNICODE_STRING Altitude;
	USHORT ulIndex;
	OB_OPERATION_REGISTRATION* OperationRegistration;
} REG_CONTEXT, * PREG_CONTEXT;