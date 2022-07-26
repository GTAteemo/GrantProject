#pragma once
PVOID ViewMatrixReader() {
	while (true) {
		if (!Globals::GameObjectManager || !Globals::FPSCamera)
		{
			Sleep(100); //TEST
			continue;
		}
		if (!Utils::valid_pointer(Globals::FPSCamera))
			continue;
		Utils::Camera::GetViewMatrix();
		Sleep(1);
	}
}