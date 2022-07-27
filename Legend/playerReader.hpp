#pragma once
PVOID PlayerReader()
{
	while (true)
	{
		if (!Globals::GameObjectManager || !Globals::FPSCamera)
		{
			Sleep(10); //TEST
			continue;
		}
		PlayerTemp.clear();
		GroupMapTemp.clear();
		if (!Utils::valid_pointer(Globals::LocalGameWorld))
			continue;
		Globals::RegisteredPlayers = mem.Read<ULONG64>(Globals::LocalGameWorld + 0x88);
		Globals::Player::PlayerCount = mem.Read<int>(Globals::RegisteredPlayers + 0x18);
		ULONG64 list_base = mem.Read<ULONG64>(Globals::RegisteredPlayers + 0x10);
		if (Globals::Player::PlayerCount <= 0 || Globals::Player::PlayerCount > 127 || !Utils::valid_pointer(list_base))
			continue;
		constexpr auto BUFFER_SIZE = 256;
		ULONG64 player_buffer[BUFFER_SIZE];
		auto player_buffer_chace = mem.Read<array<ULONG64, BUFFER_SIZE>>(list_base + 0x20);
		for (int i = 0; i < BUFFER_SIZE; i++)
		{
			player_buffer[i] = player_buffer_chace[i];
		}
		char mapCount = 'A';
		for (int i = 0; i < Globals::Player::PlayerCount; i++) {
			if (!Utils::valid_pointer(player_buffer[i]))
				continue;
			PlayerTemp.push_back(player_buffer[i]);
		}
		vector<Player> cleanTemp;
		for (ULONG64 address : PlayerTemp)
		{
			for (int i = 0; i < CurrentPlayerList.size();i++)
			{
				if (CurrentPlayerList[i].instance == address)
				{
					cleanTemp.push_back(CurrentPlayerList[i]);
					break;
				}
			}
		}
		CurrentPlayerList = cleanTemp;
		for (ULONG64 address : PlayerTemp)
		{
			bool finded = false;
			for (Player player : CurrentPlayerList)
			{
				if (player.instance == address)
				{
					finded = true;
					break;
				}
			}
			if (!finded)
			{
				Player player;
				player.instance = address;
				if (Utils::valid_pointer(address))
				{
					CurrentPlayerList.push_back(player);
				}
			}
		}		
		for (int i = 0; i < CurrentPlayerList.size(); i++) {
			if (!CurrentPlayerList[i].readed)
			{
				//Read once ptr
				CurrentPlayerList[i].GetProfile();
				CurrentPlayerList[i].GetInfomation();
				CurrentPlayerList[i].GetSetting();
				CurrentPlayerList[i].GetBoneMatrix();
				CurrentPlayerList[i].GetProceduralWeaponAnimation();
				//Read once Normal;
				CurrentPlayerList[i].IsPlayer();
				CurrentPlayerList[i].GetExperience();
				CurrentPlayerList[i].GetSide();
				CurrentPlayerList[i].GetRole();
				CurrentPlayerList[i].GetName();
				CurrentPlayerList[i].GetMemberCategory();
				CurrentPlayerList[i].GetGroup();
				CurrentPlayerList[i].GetKD();
				CurrentPlayerList[i].readed = true;
			}
			//Read EveryTime
			CurrentPlayerList[i].GetRootPosition();
			CurrentPlayerList[i].GetHeadPosition();
			CurrentPlayerList[i].GetDistance();
			CurrentPlayerList[i].GetHeadDistance();
			CurrentPlayerList[i].GetWeaponName();
			//Read in distance
			CurrentPlayerList[i].GetViewAngle();
			CurrentPlayerList[i].IsAiming();
			CurrentPlayerList[i].GetVelocity();
			CurrentPlayerList[i].GetHealth();
			CurrentPlayerList[i].IsAiming();
			/*
			* 预判，获取坐标后根据tick算  1000 tick = 1000ms = 1s
			*/
			{
				float curCount = GetTickCount();
				map <ULONG64, vector<float>> ::iterator it;
				it = TickCountMap.find(CurrentPlayerList[i].instance);
				map <ULONG64, vector<FVector>> ::iterator it2;
				it2 = DisCountMap.find(CurrentPlayerList[i].instance);
				if (it == TickCountMap.end()) {
					vector<float> counts; counts.push_back(curCount);
					vector<FVector> locations; locations.push_back(CurrentPlayerList[i].location);
					TickCountMap.insert(map<ULONG64, vector<float>>::value_type(CurrentPlayerList[i].instance, counts));
					DisCountMap.insert(map<ULONG64, vector<FVector>>::value_type(CurrentPlayerList[i].instance, locations));
				}
				else
				{
					vector<float> tickCounts = it->second;
					vector<FVector> locations = it2->second;
					if (tickCounts.size() < 6)
					{
						tickCounts.push_back(curCount);
						locations.push_back(CurrentPlayerList[i].location);
					}
					else
					{
						tickCounts[CurrentPlayerList[i].velocityIndex] = curCount;
						locations[CurrentPlayerList[i].velocityIndex] = CurrentPlayerList[i].location;
					}
					DisCountMap[CurrentPlayerList[i].instance] = locations;
					TickCountMap[CurrentPlayerList[i].instance] = tickCounts;

					if (tickCounts.size() == 6)
					{
						int nextIndex = CurrentPlayerList[i].velocityIndex + 1;
						if (nextIndex == 6) nextIndex = 0;

						double time = (tickCounts[CurrentPlayerList[i].velocityIndex] - tickCounts[nextIndex])/ 1000;
						FVector abs = (locations[CurrentPlayerList[i].velocityIndex] - locations[nextIndex]);
						abs.x /= time;
						abs.y /= time;
						abs.z /= time;
						CurrentPlayerList[i].VelocityCal = abs;
					}

				}
				CurrentPlayerList[i].velocityIndex++;
				if (CurrentPlayerList[i].velocityIndex == 6) CurrentPlayerList[i].velocityIndex = 0;
			}
			CurrentPlayerList[i].Velocity = CurrentPlayerList[i].VelocityCal;
			//组队
			map<std::string, char>::iterator it;
			it = GroupMapTemp.find(CurrentPlayerList[i].GroupId);
			if (it == GroupMapTemp.end()) {
				if (!CurrentPlayerList[i].GroupId._Equal(_xor_("null")))
				{
					GroupMapTemp.insert(map<std::string, char>::value_type(CurrentPlayerList[i].GroupId, mapCount));
				}
				else
				{
					GroupMapTemp.insert(map<std::string, char>::value_type(CurrentPlayerList[i].GroupId, '0'));
				}
				mapCount += 1;
			}
			if (!reals[CurrentPlayerList[i].weaponId].is_null())
			{
				string name = reals[CurrentPlayerList[i].weaponId]["shortName"];
				CurrentPlayerList[i].weapon = name;
			}
			else
			{
				CurrentPlayerList[i].weapon = "no weapon";
			}
			if (mem.Read<bool>(CurrentPlayerList[i].instance + (0x807)))
			{
				cout << hex << CurrentPlayerList[i].instance << endl;
				Globals::Player::LocalPlayer = CurrentPlayerList[i].instance;
				Globals::Player::LocalPlayerPos = CurrentPlayerList[i].location;
				Globals::Player::LocalHeadPos = CurrentPlayerList[i].headPosition;
				Globals::Player::LocalPlayerAiming = CurrentPlayerList[i].isAiming;
				Globals::Player::LocalPlayerViewAngles = CurrentPlayerList[i].ViewAngles;
				if (CurrentPlayerList[i].GroupId._Equal(_xor_("null")))
				{
					CurrentPlayerList[i].GroupId = "me";
				}
				Globals::Player::LocalPlayerGroup = CurrentPlayerList[i].GroupId;
				// 无限耐力
				CurrentPlayerList[i].MaxLevel();
				Globals::Player::BulletSpeed = CurrentPlayerList[i].GetBulletSpeed();
			}
			if (CurrentPlayerList[i].GroupId == Globals::Player::LocalPlayerGroup)
			{
				CurrentPlayerList[i].color = Col.green;
			}
		}
		GroupMap = GroupMapTemp;;
		PlayerList = CurrentPlayerList;
		TargetList = CurrentPlayerList;
		Sleep(1);
	}
}
