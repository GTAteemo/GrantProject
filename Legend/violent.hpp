#pragma once

Vector2 PredictViewAnagle;
FVector Prodiction;
void Prediction(Player closetPlayer)
{
	//prediction
	float FlyDistanceVertical = closetPlayer.headPosition.z - Globals::Player::MyFirePortPos.z;
	float FlyDistanceHorizontal = float(sqrtf(powf(closetPlayer.headPosition.x - Globals::Player::MyFirePortPos.x, 2.0) + powf(closetPlayer.headPosition.y - Globals::Player::MyFirePortPos.y, 2.0)));
	PredictViewAnagle = Utils::CalcViewAngles(Globals::Player::MyFirePortPos, closetPlayer.headPosition);

	float BulletSpeedY = -Globals::Player::BulletSpeed * sin(PredictViewAnagle.y / Deg);
	float BulletSpeedX = Globals::Player::BulletSpeed * cos(PredictViewAnagle.y / Deg);
	float FlyingTime = FlyDistanceHorizontal / BulletSpeedX;
	float CurrentFlyDistanceVertical = BulletSpeedY * FlyingTime + (-9.81 * powf(FlyingTime, 2.0) / 2);
	while ((CurrentFlyDistanceVertical < FlyDistanceVertical))
	{
		BulletSpeedY = -Globals::Player::BulletSpeed * sin(PredictViewAnagle.y / Deg);
		BulletSpeedX = Globals::Player::BulletSpeed * cos(PredictViewAnagle.y / Deg);
		FlyingTime = FlyDistanceHorizontal / BulletSpeedX;
		CurrentFlyDistanceVertical = BulletSpeedY * FlyingTime + (-9.81 * powf(FlyingTime, 2.0) / 2);
		PredictViewAnagle.y -= 0.01f;
	}
	Prodiction = closetPlayer.headPosition + closetPlayer.Velocity * FlyingTime;
	Prodiction.z = Globals::Player::MyFirePortPos.z + FlyDistanceHorizontal * tan(-PredictViewAnagle.y / Deg);
}

PVOID Violent()
{
	float gravity = 9.81;
	while (true)
	{
		if (!Globals::LocalGameWorld || !Globals::FPSCamera)
		{
			Sleep(50);
			continue;
		}
		//find target
		D2D1_POINT_2F screenPos;
		float closetPlayerDis = -1;
		vector<Player> copyTargetList = TargetList;
		bool finded = false;
		Player closetPlayer;
		for (Player player : copyTargetList)
		{

			if (player.instance == Globals::Player::LocalPlayer)
			{
				continue;
			}
			if (!Utils::Camera::WorldToScreenv(player.headPosition, screenPos)) continue;
			if (player.headDistance > Globals::Vars::PlayerDistance) continue;
			if (closetPlayerDis < 0)
			{
				float curCloset = Utils::Calculate::CalculateDistance2D(WindowWidth / 2, WindowHeight / 2, screenPos.x, screenPos.y);
				if (curCloset < Globals::Vars::AimbotFov)
				{
					closetPlayerDis = curCloset;
					closetPlayer = player;
					finded = true;
				}
			}
			float curCloset = Utils::Calculate::CalculateDistance2D(WindowWidth / 2, WindowHeight / 2, screenPos.x, screenPos.y);
			if (curCloset < closetPlayerDis && closetPlayerDis >= 0)
			{
				if (curCloset < Globals::Vars::AimbotFov)
				{
					closetPlayerDis = curCloset;
					closetPlayer = player;
					finded = true;
				}
			}
		}
		
		//ProceduralWeaponAnimation
		ULONG64 ProceduralWeaponAnimation = mem.Read<ULONG64>(Globals::Player::LocalPlayer + 0x198);
		if (!Utils::valid_pointer(ProceduralWeaponAnimation))
			continue;
		// aimbot fireport position
		const auto FirArmController = mem.Read<ULONG64>(ProceduralWeaponAnimation + 0x80);
		if (!Utils::valid_pointer(FirArmController))
			continue;
		const auto fireport = mem.Read<ULONG64>(FirArmController + 0xC8);
		if (!Utils::valid_pointer(fireport))
			continue;
		const auto fireport_orginal_transform = mem.Read<ULONG64>(fireport + 0x10);
		if (!Utils::valid_pointer(fireport_orginal_transform))
			continue;
		FVector firePortPos = Utils::Player::Bones::GetPosition(fireport_orginal_transform);
		Globals::Player::MyFirePortPos = firePortPos;
		Prediction(closetPlayer);
		Globals::TargetPlayer::closet = closetPlayerDis;
		Globals::TargetPlayer::TargetHeadPos = Prodiction;
		Globals::TargetPlayer::TargetLocation = closetPlayer.location;

		//view angles
		Vector2 viewAangle = PredictViewAnagle;
		uintptr_t movement_context = mem.Read<ULONG64>(Globals::Player::LocalPlayer + 0x40);
		if (!Utils::valid_pointer(movement_context))
			continue;
		Vector2 myView = mem.Read<Vector2>(movement_context + 0x22C); // middle one in three vector
		if (myView.x > 180.f) myView.x -= 360.f;
		if (myView.x < -180.f) myView.x += 360.f;
		Globals::TargetPlayer::MyView = myView;
		Globals::TargetPlayer::TarView = viewAangle;


		if (Globals::Vars::Aimbot == 1)
		{
			ULONG64 Breath = mem.Read<ULONG64>(ProceduralWeaponAnimation + 0x28);
			mem.Write(Breath + 0xA4, 0.1f, sizeof(float)); //Intensity
			mem.Write(Breath + 0xA8, 0.1f, sizeof(float)); //_shakeIntensity
			mem.Write(Breath + 0xAC, 0.1f, sizeof(float)); //_breathIntensity
			mem.Write(Breath + 0xB0, 0.1f, sizeof(float)); //_breathFrequency
			mem.Write(ProceduralWeaponAnimation + 0x100, 1, sizeof(int)); //mask

			Vector2 fireDirect = { viewAangle.x - myView.x , myView.y - viewAangle.y };
			if (/*GetAsyncKeyState(VK_LBUTTON) && 0x1 &&*/ Globals::TargetPlayer::closet > 0 && (0 <= fireDirect.x <= 90 && 0 <= fireDirect.y <= 90))
			{
				if (fireDirect.x > 180.f) fireDirect -= 360.f;
				if (fireDirect.x < -180.f) fireDirect += 360.f;
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E0, fireDirect.x / 90, sizeof(float));
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E8, fireDirect.y / 90, sizeof(float));
			}
			else
			{
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E0, 0, sizeof(float));
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E4, -1, sizeof(float));
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E8, 0, sizeof(float));
			}
		}
		else
		{
			if (!(Globals::Vars::Aimbot == 1))
			{
				mem.Write(ProceduralWeaponAnimation + 0x100, 125, sizeof(int));
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E0, 0, sizeof(float));
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E4, -1, sizeof(float));
				mem.Write<float>(ProceduralWeaponAnimation + 0x1E8, 0, sizeof(float));
			}
		}

		Vector2 ViewAngleABS = viewAangle - myView;
		if (ViewAngleABS.x > 180.f) ViewAngleABS.x -= 360.f;
		if (ViewAngleABS.x < -180.f) ViewAngleABS.x += 360.f;
		float aimspeed = 0.5f;
		if (ViewAngleABS.x > aimspeed)
		{
			if (ViewAngleABS.x > 0)
			{
				myView.x += aimspeed;
			}
			else
			{
				myView.x -= aimspeed;
			}
		}
		else
		{
			myView.x = viewAangle.x;
		}
		if (ViewAngleABS.y > aimspeed)
		{
			if (ViewAngleABS.y > 0)
			{
				myView.y += aimspeed;
			}
			else
			{
				myView.y -= aimspeed;
			}
		}
		else
		{
			myView.y = viewAangle.y;
		}
		if (GetAsyncKeyState(VK_LBUTTON) && 0x1 && Globals::TargetPlayer::closet > 0 && (Globals::Vars::Aimbot == 2))
		{
			mem.Write<Vector2>(movement_context + 0x22C, myView, sizeof(Vector2));

			ULONG64 HandsContainer = mem.Read<ULONG64>(ProceduralWeaponAnimation + 0x18);
			ULONG64 Recoil = mem.Read<ULONG64>(HandsContainer + 0x40);
			mem.Write<float>(Recoil + 0x80, 0.0, sizeof(float)); // X
			mem.Write<float>(Recoil + 0x88, 0.3, sizeof(float)); // y

			ULONG64 SwaySpring = mem.Read<ULONG64>(HandsContainer + 0x58);
			FVector _pPos = { 0,0,0 };
			mem.Write(SwaySpring + 0x10, _pPos, sizeof(FVector));
			mem.Write(SwaySpring + 0x1C, _pPos, sizeof(FVector));
			mem.Write(SwaySpring + 0x28, _pPos, sizeof(FVector));

			ULONG64 Breath = mem.Read<ULONG64>(ProceduralWeaponAnimation + 0x28);
			mem.Write(Breath + 0xA4, 0.1f, sizeof(float)); //Intensity
			mem.Write(Breath + 0xA8, 0.1f, sizeof(float)); //_shakeIntensity
			mem.Write(Breath + 0xAC, 0.1f, sizeof(float)); //_breathIntensity
			mem.Write(Breath + 0xB0, 0.1f, sizeof(float)); //_breathFrequency
			mem.Write(ProceduralWeaponAnimation + 0x100, 1, sizeof(int)); //mask
		}

		if (Globals::Vars::Aimbot == 3)
		{
			ULONG64 HandsContainer = mem.Read<ULONG64>(ProceduralWeaponAnimation + 0x18);
			ULONG64 Recoil = mem.Read<ULONG64>(HandsContainer + 0x40);
			mem.Write<float>(Recoil + 0x80, 0.3, sizeof(float)); // X
			mem.Write<float>(Recoil + 0x88, 0.3, sizeof(float)); // y
		}


		Sleep(1);
	}
}
