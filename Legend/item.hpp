#pragma once
#include "memory.hpp"
#include "globals.hpp"
#include "utils.hpp"

class InItem {
public:
	string id;
	string name;
	int role;
};

class Item {
private:
	uintptr_t LootArray;


	//std::vector<Item> ItemList;

	uintptr_t Index(int index) {
		return mem.Read<ULONG64>(LootArray + 0x20 + (index * 0x08));
	}
public:
	uintptr_t instance;
	FVector location;
	string name;
	string id;
	float distance;
	int  role = 0;
	bool isContainer;
	bool haveExp = false;

	vector<InItem> inItems;

	uint64_t m_lItemProfile; //0x10

	uint64_t m_lItemInteractive; //0x28 
	uint64_t m_lItemBasicInfo; //0x30

	uint64_t m_lItemLocalization; //0x30

	uint64_t m_lItemCoordinates; //0x8

	uint64_t m_lItemLocationContainerTransForm; //0x28
	uint64_t m_lItemLocationContainer; //0x38

	int price;
	uint64_t Template;

	explicit Item(int index) {
		uint64_t LootList = mem.Read<uint64_t>(Globals::LocalGameWorld + 0x68);
		if (!Utils::valid_pointer(LootList))
			return;


		LootArray = mem.Read<uint64_t>(LootList + 0x10);
		if (!Utils::valid_pointer(LootArray))
			return;

		instance = Index(index);
	}

	bool Init() {

		m_lItemProfile = mem.Read<uint64_t>(instance + 0x10);
		if (!Utils::valid_pointer(m_lItemProfile))
			return false;


		m_lItemBasicInfo = mem.Read<uint64_t>(m_lItemProfile + 0x30);
		if (!Utils::valid_pointer(m_lItemBasicInfo))
			return false;
		m_lItemInteractive = mem.Read<uint64_t>(m_lItemProfile + 0x28);
		if (!Utils::valid_pointer(m_lItemInteractive))
			return false;

		m_lItemLocalization = mem.Read<uint64_t>(m_lItemBasicInfo + 0x30);
		if (!Utils::valid_pointer(m_lItemLocalization))
			return false;

		m_lItemCoordinates = mem.Read<uint64_t>(m_lItemLocalization + 0x8);
		if (!Utils::valid_pointer(m_lItemCoordinates))
			return false;

		m_lItemLocationContainerTransForm = mem.Read<uint64_t>(m_lItemCoordinates + 0x28);
		if (!Utils::valid_pointer(m_lItemLocationContainerTransForm))
			return false;
		m_lItemLocationContainer = mem.Read<uint64_t>(m_lItemCoordinates + 0x38);
		if (!Utils::valid_pointer(m_lItemLocationContainer))
			return false;
		//Template = mem->ReadChain(instance, { 0x10, 0x28, 0x50, 0x40 });
		//if (Template <= 0)
			//return false;
		return true;
	}

	void GetName()
	{
		std::array<char, 64> name_buffer;
		if (auto name_ptr = mem.ReadChain(m_lItemInteractive, { 0x10,0x30, 0x60 }))
		{
			name_buffer = mem.Read<array<char, 64>>(name_ptr);
		}
		name_buffer[63] = '\0';
		name = name_buffer.data();
	}
	void GetLocation() {
		if (m_lItemLocationContainerTransForm)
		{
			location = Utils::Player::Bones::GetPosition(m_lItemLocationContainerTransForm);
		}
		else
		{
			location = mem.Read<FVector>(m_lItemLocationContainer + 0x90);
		}
	}
	void GetId() {
		auto temp2 = mem.ReadChain(m_lItemInteractive, { 0xB0, 0x40 });
		ULONG64 lootId = mem.Read<ULONG64>(temp2 + 0x50);
		if (!Utils::valid_pointer(lootId))
		{
			id = "Container";
			isContainer = true;
			return;
		}
		id = Utils::Text::read_str_item(lootId);


	}
	void ReadContainer() {
		try
		{
			isContainer = false;
			if (id._Equal(_xor_("55d7217a4bdc2d86028b456d")))
				return;
			if (ULONG64 itemOwner = mem.Read<ULONG64>(m_lItemInteractive + 0x108))
			{
				if (!Utils::valid_pointer(itemOwner)) return;
				ULONG64 itemcontainer = mem.Read<ULONG64>(itemOwner + 0xB8);
				if (!Utils::valid_pointer(itemcontainer)) return;
				ULONG64 grids = mem.Read<ULONG64>(itemcontainer + 0x70);
				if (!Utils::valid_pointer(grids)) return;
				if (grids)
				{
					int gridscount = mem.Read<int>(grids + 0x18);
					if (0 < gridscount && gridscount <= 60)
					{
						ULONG64 dict = mem.ReadChain(grids, { 0x20, 0x40, 0x10 });
						if (!Utils::valid_pointer(dict)) return;
						if (dict)
						{
							int dictcount = mem.Read<int>(dict + 0x40);
							int dictcountMax = mem.Read<int>(dict + 0x44);
							//cout << name << "gridscoun:" << gridscount << "  max: " << dictcount << endl;
							if (0 < dictcount && dictcount < 48 && dictcountMax < 96)
							{
								auto entries = mem.Read<ULONG64>(dict + 0x18);
								if (!Utils::valid_pointer(entries)) return;
								if (entries)
								{
									isContainer = true;
									for (int j = 0; j < dictcount; j++) {
										auto containerItemPtr = mem.Read<ULONG64>(entries + 0x28 + (0x18 * j));  if (!Utils::valid_pointer(containerItemPtr)) return;
										auto itemTemplatePtr = mem.Read<ULONG64>(containerItemPtr + 0x40);  if (!Utils::valid_pointer(itemTemplatePtr)) return;
										ULONG64 lootId = mem.Read<ULONG64>(itemTemplatePtr + 0x50);  if (!Utils::valid_pointer(lootId)) return;
										string lootD = Utils::Text::read_str(lootId);
										//string lootid = wstring_convert_string(lootD);
										InItem inItem;
										if (lootD != "")
										{
											inItem.id = lootD;
											inItems.emplace_back(inItem);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		catch (const char& e)
		{
			cout << _xor_("read container:") << e << endl;
		}
	}
	void GetDistance()
	{
		distance = location.Distance(Globals::Player::LocalHeadPos);
	}
	//void IsQuestItem() {
	//	questItem = mem->Read<bool>(Template + 0x94);
	//}
};