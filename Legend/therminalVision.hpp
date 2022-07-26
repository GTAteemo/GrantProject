#pragma once
PVOID ThermalVision()
{
	Sleep(1500);
	while (true)
	{
		if (!Globals::GameObjectManager || !Globals::FPSCamera)
		{
			Sleep(100); //TEST
			continue;
		}
		if (!Utils::valid_pointer(Globals::FPSCamera))
		{
			Sleep(100); //TEST
			continue;
		}

		static bool ThermalVisonSwitch = false;
		if (GetAsyncKeyState(VK_F12) && 0x1)
		{
			Sleep(200);
			uint64_t thermalVision = Utils::GetComponentFromGameObject(Globals::FPSCamera, "ThermalVision");
			if (!Utils::valid_pointer(thermalVision))
				continue;
			ThermalVisonSwitch = !ThermalVisonSwitch;
			mem.Write<bool>(thermalVision + 0xE0, ThermalVisonSwitch, sizeof(bool)); // Enable
			mem.Write<bool>(thermalVision + 0xE1, false, sizeof(bool)); // IsNoisy
			mem.Write<bool>(thermalVision + 0xE2, false, sizeof(bool)); // IsFpSstuck
			mem.Write<bool>(thermalVision + 0xE3, false, sizeof(bool)); // MotionBlur
			mem.Write<bool>(thermalVision + 0xE4, false, sizeof(bool)); // IsGlitched
			mem.Write<bool>(thermalVision + 0xE5, false, sizeof(bool)); // Pixellated
		}
		Sleep(2);
	}
}