#pragma once
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
					}
					if (!expensives[item.id].is_null())
					{
						string name = expensives[item.id]["shortName"];
						item.name = name;
						item.role = 3;
					}
					if (!always[item.id].is_null())
					{
						string name = always[item.id]["shortName"];
						item.name = name;
						item.role = 4;
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
						}
						if (!expensives[item.inItems[i].id].is_null())
						{
							string name = expensives[item.inItems[i].id]["shortName"];
							item.inItems[i].name = name;
							item.inItems[i].role = 3;
						}
						if (!always[item.inItems[i].id].is_null())
						{
							string name = always[item.inItems[i].id]["shortName"];
							item.inItems[i].name = name;
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
		Sleep(2000);
	}
}
