#pragma once

PVOID BaseValues()
{
	while (true)
	{

		if (!Globals::GameObjectManager)
			cout << _xor_("GOM NOT") << endl;
		//cout << "GOM:" << hex << Globals::GameObjectManager << endl;
		auto last_maincamera_tagged_objects = mem.Read<std::array<uint64_t, 2>>(Globals::GameObjectManager + 0x10);
		if (!Utils::valid_pointer(last_maincamera_tagged_objects[0]) || !Utils::valid_pointer(last_maincamera_tagged_objects[1]))
		{
			Sleep(100); //TEST
			continue;
		}
		Globals::FPSCamera = Utils::GetObjectFromList(last_maincamera_tagged_objects[1], last_maincamera_tagged_objects[0], "FPS Camera");
		if (!Utils::valid_pointer(Globals::FPSCamera))
		{
			cout << "no fps" << endl;
			PlayerTemp.clear();
			PlayerList.clear();
			TargetList.clear();
			ItemsTemp.clear();
			ItemsList.clear();
			PlayerBoneseTemp.clear();
			PlayerBonese.clear();
			CurrentPlayerList.clear();
			Globals::GameWorld = 0x0;
			Globals::LocalGameWorld = 0x0;
			Globals::RegisteredPlayers = 0x0;
			Globals::TargetPlayer::closet = 0;
			Globals::TargetPlayer::TargetHeadPos = FVector{};
			Globals::TargetPlayer::TargetLocation = FVector{};
			Sleep(100); //TEST
			continue;
		}

		//cout << "FPS:" << hex << Globals::FPSCamera << endl;
		auto last_active_objects = mem.Read<std::array<uint64_t, 2>>(Globals::GameObjectManager + 0x20);
		if (!Utils::valid_pointer(last_active_objects[0]) || !Utils::valid_pointer(last_active_objects[1]))
		{
			Sleep(100); //TEST
			continue;
		}
		Globals::GameWorld = Utils::GetObjectFromList(last_active_objects[1], last_active_objects[0], "GameWorld");
		if (!Globals::GameWorld) {
			cout << _xor_("no game world") << endl;
			Sleep(100); //TEST
			continue;
		}
		//cout << "GameWorld" << hex << Globals::GameWorld << endl;
		Globals::LocalGameWorld = mem.ReadChain(Globals::GameWorld, { 0x30 , 0x18 , 0x28 });
		if (!Globals::LocalGameWorld)
		{
			cout << _xor_("no local game world") << endl;
			continue;
		}
		//cout << "LocalGame:" << hex << Globals::LocalGameWorld << endl;
		Sleep(100); //TEST
	}
}