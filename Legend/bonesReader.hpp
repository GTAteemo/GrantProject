#pragma once
PVOID BonesReader() {
	while (true)
	{
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
			ULONG64 RegisteredPlayers = mem.Read<ULONG64>(Globals::LocalGameWorld + 0x88);
			int PlayerCount = mem.Read<int>(RegisteredPlayers + 0x18);
			ULONG64 list_base = mem.Read<ULONG64>(RegisteredPlayers + 0x10);
			if (PlayerCount <= 0 || PlayerCount > 127 || !Utils::valid_pointer(list_base))
				continue;
			constexpr auto BUFFER_SIZE = 256;
			ULONG64 player_buffer[BUFFER_SIZE];
			auto player_buffer_chace = mem.Read<array<ULONG64, BUFFER_SIZE>>(list_base + 0x20);
			for (int i = 0; i < BUFFER_SIZE; i++)
			{
				player_buffer[i] = player_buffer_chace[i];
			}
			PlayerBoneseTemp.clear();

			for (int i = 0; i < PlayerCount; i++) {
				if (!Utils::valid_pointer(player_buffer[i]))
					continue;
				PlayerBoneseTemp.push_back(player_buffer[i]);
			}

			vector<Player> cleanTemp;
			for (ULONG64 address : PlayerBoneseTemp)
			{
				for (int i = 0; i < CurrentPlayerBonese.size(); i++)
				{
					if (CurrentPlayerBonese[i].instance == address)
					{
						cleanTemp.push_back(CurrentPlayerBonese[i]);
						break;
					}
				}
			}
			CurrentPlayerBonese = cleanTemp;
			for (ULONG64 address : PlayerBoneseTemp)
			{
				bool finded = false;
				for (Player player : CurrentPlayerBonese)
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
						CurrentPlayerBonese.push_back(player);
					}
				}
			}
			for (int i = 0; i < CurrentPlayerList.size(); i++) {
				if (mem.Read<bool>(CurrentPlayerList[i].instance + 0x807))
					continue;
				if (!CurrentPlayerList[i].readed)
				{
					CurrentPlayerList[i].GetBoneMatrix();
				}
				CurrentPlayerList[i].GetHeadPosition();
				CurrentPlayerList[i].GetRootPosition();
				CurrentPlayerList[i].GetDistance();
				if (!Utils::Camera::WorldToScreenv(CurrentPlayerList[i].headPosition, locationS) || CurrentPlayerList[i].distance > Globals::Vars::PlayerDistance) continue;
				CurrentPlayerList[i].GetBones();
			}
			PlayerBonese = CurrentPlayerList;
		}
		catch (const char& e)
		{
			cout << e << endl;
			exit(4);
		}
		Sleep(1);
	}
}