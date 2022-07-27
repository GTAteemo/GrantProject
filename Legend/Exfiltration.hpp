#pragma once
#include "memory.hpp"
#include "globals.hpp"
#include "utils.hpp"

class ExfilTration {
public:
	ULONG64 instance;
	FVector location;
	string name;
	bool active;

	void ReadData()
	{
		ULONG64 transform = mem.ReadChain(instance, { 0x10, 0x30, 0x30, 0x8, 0x28 });
		location = Utils::Player::Bones::GetPosition(transform);
		ULONG64 namePtr = mem.ReadChain(instance, { 0x58,0x10 });
		name = Utils::Text::read_str(namePtr);
	}
};