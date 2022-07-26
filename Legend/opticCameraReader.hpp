#pragma once
PVOID OpticCameraReader()
{
	while (true)
	{
		if (!Globals::GameObjectManager || !Globals::FPSCamera)
		{
			Sleep(100); //TEST
			continue;
		}
		//opt put end
		Globals::OpticCamera = Utils::GetObjectFromListv2("BaseOpticCamera(Clone)", Globals::GameObjectManager);
		Sleep(1);
	}
}