#pragma once

#include "kernelInterface.hpp"
#include <iostream>
#include "xorstr.hpp"

class Memory
{
public:
	~Memory() {};

	ULONG PID = 0x0;
	ULONG64 UnityPlayer = 0x0;
	KeInterface Driver = KeInterface(_xor_("\\\\.\\BrilliantMiracleKN"));

	bool init(char* processName)
	{
		if (Driver.hDriver == INVALID_HANDLE_VALUE)
		{
			std::cout << _xor_("Î´¼ÓÔØÇý¶¯") << std::endl;
			return false;
		}


		PID = Driver.GetTargetPid(processName, 1);
		if (!PID) return false;
		/*for (size_t i = 0; i < 100; i++)
		{
			char* name = Driver.GetModuleNameIndex(PID, i);
			ULONG64 addr = Driver.GetModuleIndex(PID, i);
			if (!addr) break;
			printf("Module Addr:%016I64x     Module Name: %s\n", addr, name);
		}*/
		return true;
	}



	ULONG64 GetModuleAddress(WCHAR* ModuleName)
	{
		return Driver.GetModuleADDR(ModuleName, PID);
	}


	template <typename type>
	type Read(ULONG64 addr) {
		if (!(addr && addr > 0xFFFFFF && addr < 0x7FFFFFFFFFFF))
			return type();
		return Driver.ReadVirtualMemory<type>(PID, addr);
	}

	template <typename type>
	bool Write(ULONG64 addr, type value, SIZE_T size) {
		if (!(addr && addr > 0xFFFFFF && addr < 0x7FFFFFFFFFFF))
			return false;
		return Driver.WriteVirtualMemory(PID, addr, value, size);
	}

	template <typename type>
	type Read(ULONG64 addr, PVOID buffer, SIZE_T size) {
		return Driver.ReadVirtualMemory<type>(PID, addr, buffer, size);
	}

	ULONG64 ReadChain(ULONG64 addr, const std::vector<ULONG64>& offsets)
	{
		ULONG64 result = Driver.ReadVirtualMemory<ULONG64>(PID, addr + offsets.at(0));
		for (int i = 1; i < offsets.size(); i++) {
			if (!(result && result > 0xFFFFFF && result < 0x7FFFFFFFFFFF))
				return result;
			result = Driver.ReadVirtualMemory<ULONG64>(PID, result + offsets.at(i));
		}
		return result;
	}

};

Memory mem;