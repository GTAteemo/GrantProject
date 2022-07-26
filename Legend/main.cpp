#pragma once

#include "includes.hpp"
#include "threads.hpp"
#include "memory.hpp"
#include "GenerateString.hpp"
#include "window.hpp"
#include "globals.hpp"
#include "xorstr.hpp"

using namespace std;
int main()
{
	SetConsoleTitle(RandomString(10).c_str());

	std::ifstream i(_xor_("expnew.json"));
	i >> expensives;
	std::ifstream i2(_xor_("foods.json"));
	i2 >> foods;
	std::ifstream i3(_xor_("real.json"));
	i3 >> reals;
	std::ifstream i4(_xor_("always.json"));
	i4 >> always;

	/*_xor_(*/
	char* processName = "EscapeFromTarkov.exe";
	if (!mem.init(processName))
	{
		printf(_xor_("未能打开游戏!\n"));
		getchar();
		return 0;
	}
	ULONG64 UModule = mem.GetModuleAddress(L"UnityPlayer.dll");
	Globals::GameObjectManager = mem.Read<ULONG64>(UModule + Offsets::GameObjectManager);
	
	thread trhead1(&BaseValues);
	thread trhead2(&PlayerReader);
	thread trhead3(&ViewMatrixReader);
	thread trhead4(&OpticCameraReader);
	thread thread5(&ItemReader);
	thread trhead6(&Violent);
	thread thread7(&BonesReader);
	thread thread8(&ThermalVision);
	//thread trhead9 (&TestThread);

	Overlay.Name = RandomString(10).c_str();
	SetupWindow();
	DirectXInit();
	MainLoop();

	getchar();
	return 0;
}


