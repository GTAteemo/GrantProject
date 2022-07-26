#pragma once
#pragma warning( disable : 4101 4244)

#include "Communication.hpp"
#include <vector>



// The interface handler for IOCTL. Thanks Zeromem.
class KeInterface
{

public:
	HANDLE hDriver; // Handle to driver

					// Initializer
	KeInterface(LPCSTR RegistryPath)
	{
		hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	}


	ULONG GetTargetPid(CHAR* processName, int index)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return 0;
		KERNEL_GETID_REQUEST64 Id;
		Id.ProcessName = processName;
		Id.index = index;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_ID_REQUEST, &Id, sizeof(Id), &Id, sizeof(Id), &Bytes, NULL))
		{
			return Id.ProcessId;
		}
		return 0;
	}

	char* GetModuleNameIndex(ULONG processId, int index)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return 0;;
		KERNEL_GET_MODULENAME_INDEX_REQUEST64 GMI;
		ULONG64 Addr;
		GMI.ProcessId = processId;
		GMI.index = index;

		if (DeviceIoControl(hDriver, IO_GET_MODULENAME_INDEX_CONTROLLER, &GMI, sizeof(GMI), &GMI, sizeof(GMI), 0, NULL))
		{
			return GMI.ModuleName;
		}
		return "";
	}
	ULONG64 GetModuleIndex(ULONG processId, int index)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return 0x0;
		KERNEL_GET_MODULE_INDEX_REQUEST64 GMI;
		ULONG64 Addr;
		GMI.ProcessId = processId;
		GMI.index = index;

		if (DeviceIoControl(hDriver, IO_GET_MODULE_INDEX_CONTROLLER, &GMI, sizeof(GMI), &GMI, sizeof(GMI), 0, NULL))
		{
			return GMI.Address;
		}
		return 0x0;
	}
	ULONG64 GetModuleADDR(WCHAR* ModuleName, ULONG processId)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return 0x0;
		KERNEL_GET_MODULEADDR_INDEX_REQUEST64 GMI;
		ULONG64 Addr;
		GMI.ProcessId = processId;
		GMI.MODULENAME = ModuleName;

		if (DeviceIoControl(hDriver, IO_GET_MODULEADDR_INDEX_CONTROLLER, &GMI, sizeof(GMI), &GMI, sizeof(GMI), 0, NULL))
		{
			return GMI.Address;
		}
		return 0x0;
	}


	template <typename type>
	type ReadVirtualMemory(ULONG ProcessId, ULONG64 ReadAddress)
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return type();
		// allocate a buffer with specified type to allow our driver to write our wanted data inside this buffer
		type Buffer;

		SIZE_T Size = sizeof(type);
		DWORD Return, Bytes;
		KERNEL_READ_REQUEST64 ReadRequest;

		ReadRequest.ProcessId = ProcessId;
		ReadRequest.Address = ReadAddress;

		//send the 'address' of the buffer so our driver can know where to write the data
		ReadRequest.pBuff = &Buffer;

		ReadRequest.Size = Size;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
		{
			//return our buffer
			return Buffer;
		}
		return Buffer;
	}

	template <typename type>
	bool WriteVirtualMemory(ULONG ProcessId, ULONG64 WriteAddress, type WriteValue, SIZE_T WriteSize)
	{
		if (!WriteAddress)
		{
			return false;
		}
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;
		DWORD Bytes;


		KERNEL_WRITE_REQUEST64  WriteRequest;
		WriteRequest.ProcessId = ProcessId;
		WriteRequest.Address = WriteAddress;


		//send driver the 'address' of our specified type WriteValue so our driver can copy the data we want to write from this address to WriteAddress
		WriteRequest.pBuff = &WriteValue;

		WriteRequest.Size = WriteSize;

		if (DeviceIoControl(hDriver, IO_WRITE_REQUEST, &WriteRequest, sizeof(WriteRequest), 0, 0, &Bytes, NULL))
		{
			return true;
		}
		return false;
	}

	bool RequestProtection()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		return DeviceIoControl(hDriver, IO_PROTECT_CONTROLLER, 0, 0, 0, 0, 0, 0);
	}

	ULONG64 GetUModule()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		ULONG64 Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_MODULE_REQUEST, &Address, sizeof(Address),
			&Address, sizeof(Address), &Bytes, NULL))
		{
			return Address;
		}
		return false;
	}

	DWORD GetEngineModuleSize()
	{
		if (hDriver == INVALID_HANDLE_VALUE)
			return false;

		ULONG Address;
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_MODULE_REQUEST_LENGTH, &Address, sizeof(Address),
			&Address, sizeof(Address), &Bytes, NULL))
		{
			return Address;
		}
		return false;
	}

	template <typename type>
	type ReadVirtualMemory(ULONG ProcessId, ULONG64 ReadAddress, PVOID buffer, SIZE_T size)
	{

		// allocate a buffer with specified type to allow our driver to write our wanted data inside this buffer
		type Buffer;


		SIZE_T Size = size;
		DWORD Return, Bytes;
		KERNEL_READ_REQUEST64 ReadRequest;

		ReadRequest.ProcessId = ProcessId;
		ReadRequest.Address = ReadAddress;

		//send the 'address' of the buffer so our driver can know where to write the data
		ReadRequest.pBuff = &Buffer;

		ReadRequest.Size = Size;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
		{
			//return our buffer
			return Buffer;
		}
		return Buffer;
	}

	BOOL DataCompare(const BYTE* pData, const BYTE* pMask, const char* pszMask)
	{
		for (; *pszMask; ++pszMask, ++pData, ++pMask)
		{
			if (*pszMask == 'x' && *pData != *pMask)
			{
				return FALSE;
			}
		}
		return (*pszMask == 0);
	}

	DWORD Scan(ULONG ProcessId, DWORD dwStart, DWORD dwSize, const char* pSignature, const char* pMask)
	{
		BYTE* pRemote = new BYTE[dwSize];
		if (!ReadSpecial(ProcessId, dwStart, pRemote, dwSize))
		{
			delete[] pRemote;
			return NULL;
		}
		for (DWORD dwIndex = 0; dwIndex < dwSize; dwIndex++)
		{
			if (DataCompare((const BYTE*)(pRemote + dwIndex), (const BYTE*)pSignature, pMask))
			{
				delete[] pRemote;
				return (dwStart + dwIndex);
			}
		}
		delete[] pRemote;
		return NULL;
	}

	PVOID ReadBuffer(ULONG ProcessId, ULONG64 ReadAddress, PVOID buffer, SIZE_T size)
	{

		// allocate a buffer with specified type to allow our driver to write our wanted data inside this buffer
		PVOID Buffer;

		SIZE_T Size = size;
		DWORD Return, Bytes;
		KERNEL_READ_REQUEST64 ReadRequest;

		ReadRequest.ProcessId = ProcessId;
		ReadRequest.Address = ReadAddress;

		//send the 'address' of the buffer so our driver can know where to write the data
		ReadRequest.pBuff = &Buffer;

		ReadRequest.Size = Size;

		// send code to our driver with the arguments
		if (DeviceIoControl(hDriver, IO_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
		{
			//return our buffer
			buffer = Buffer;
			return Buffer;
		}
		return 0;
	}

	ULONG64 ReadChain(ULONG procId, ULONG64 read_address, const std::vector<ULONG64>& offsets)
	{
		ULONG64 result = ReadVirtualMemory<ULONG64>(procId, read_address + offsets.at(0));
		for (int i = 1; i < offsets.size(); i++) {
			if (!result) return 0;
			result = ReadVirtualMemory<ULONG64>(procId, result + offsets.at(i));
		}
		return result;
	}

	bool ReadSpecial(ULONG ProcessId, DWORD dwAddress, LPVOID lpBuffer, DWORD dwSize)
	{
		SIZE_T Out = NULL;

		KERNEL_READ_REQUEST ReadRequest;

		ReadRequest.ProcessId = ProcessId;
		ReadRequest.Address = dwAddress;

		ReadRequest.pBuff = &lpBuffer;

		ReadRequest.Size = dwSize;

		return (DeviceIoControl(hDriver, IO_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0) == TRUE);
	}
};