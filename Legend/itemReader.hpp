#pragma once
//撤离点和物资一样，一起读了
void ExfiltrationReader()
{
	ULONG64 ExitController = mem.Read<ULONG64>(Globals::LocalGameWorld + 0x18);
	ULONG64 exfiltrationPoint;
	if (Globals::Player::IsPmc)
	{
		exfiltrationPoint = mem.Read<ULONG64>(ExitController + 0x20);
	}
	else
	{
		exfiltrationPoint = mem.Read<ULONG64>(ExitController + 0x28);
	}
	int ExitCount = mem.Read<int>(exfiltrationPoint + 0x18);

	constexpr auto BUFFER_SIZE = 24;
	auto exit_buffer_cache = mem.Read<array<ULONG64, BUFFER_SIZE>>(exfiltrationPoint + 0x20);
	
	ExfilTrationTemp.clear();

	for (int i = 0; i < ExitCount; i++)
	{
		if (!exit_buffer_cache[i])
		{
			continue;
		}
		ExfilTration exit;
		exit.instance = exit_buffer_cache[i];
		exit.ReadData();
		ExfilTrationTemp.push_back(exit);
	}

	ExfilTrationList = ExfilTrationTemp;

}

PVOID ItemReader()
{
	Sleep(200);
	while (true) {
		if (!Globals::GameObjectManager || !Globals::FPSCamera)
		{
			Sleep(100); //TEST
			continue;
		}
		if (!Utils::valid_pointer(Globals::LocalGameWorld))
		{
			Sleep(100); //TEST
			continue;
		}
		try
		{
			if (!ItemsTemp.empty())
			{
				ItemsTemp.clear();
			}
			uint64_t LootList = mem.Read<uint64_t>(Globals::LocalGameWorld + 0x68);
			if (!Utils::valid_pointer(LootList))
				continue;
			Globals::Loot::LootCount = mem.Read<int>(LootList + 0x18);
			if (Globals::Loot::LootCount <= 0)
				continue;
			for (int i = 0; i < Globals::Loot::LootCount; i++)
			{
				Item item(i);
				if (!item.Init())
					continue;
				item.GetName();
				item.GetLocation();
				item.GetId();
				/*if (item.isContainer)
				{
					cout << item.name << endl;
					cout << hex << item.m_lItemInteractive << endl;
					cout << "====================" << endl;
				}*/
				item.ReadContainer();
				item.GetDistance();

				if (item.id == "55d7217a4bdc2d86028b456d")
				{
					item.name = "Dead";
					item.role = 3;
				}
				else if (!item.isContainer)
				{
					if (!reals[item.id].is_null())
					{
						string name = reals[item.id]["shortName"];
						item.name = name;
						item.role = 1;
					}
					if (!foods[item.id].is_null())
					{
						string name = foods[item.id]["shortName"];
						item.name = name;
						item.role = 2;
						item.color = Col.blue;
					}
					if (!expensives[item.id].is_null())
					{
						string name = expensives[item.id]["shortName"];
						item.name = name;
						item.role = 3;
						item.color = Col.purple;
					}
					if (!always[item.id].is_null())
					{
						string name = always[item.id]["shortName"];
						item.name = name;
						item.role = 4;
						item.color = Col.red;
						item.haveExp = true;
					}
				}
				else if (item.isContainer)
				{
					for (int i = 0; i < item.inItems.size(); i++)
					{
						if (!reals[item.inItems[i].id].is_null())
						{
							string name = reals[item.inItems[i].id]["shortName"];
							item.inItems[i].name = name;
							item.inItems[i].role = 1;
						}
						if (!foods[item.inItems[i].id].is_null())
						{
							string name = foods[item.inItems[i].id]["shortName"];
							item.inItems[i].name = name;
							item.inItems[i].role = 2;
							item.inItems[i].color = Col.blue;
						}
						if (!expensives[item.inItems[i].id].is_null())
						{
							string name = expensives[item.inItems[i].id]["shortName"];
							item.inItems[i].name = name;
							item.inItems[i].role = 3;
							item.inItems[i].color = Col.purple;
						}
						if (!always[item.inItems[i].id].is_null())
						{
							string name = always[item.inItems[i].id]["shortName"];
							item.inItems[i].name = name;
							item.inItems[i].color = Col.red;
							item.inItems[i].role = 4;
							item.haveExp = true;
						}
					}
				}
				ItemsTemp.push_back(item);
			}

			ItemsList = ItemsTemp;
		}
		catch (const char*& e)
		{
			cout << e << endl;
			continue;
		}


		ExfiltrationReader();

		Sleep(2000);
	}
}
