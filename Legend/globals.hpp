#pragma once

#include "includes.hpp"

int WindowWidth = 1920 - 38;
int WindowHeight = 1080 - 38;
int WindowLeft = 19;
int WindowTop = 19;

//app running
bool Running = true;
//mouse focus window
bool FocusWindow = false;



namespace Offsets
{
	ULONG64 GameObjectManager = 0x17FFD28;
}

namespace Globals {
	ULONG64 GameObjectManager;
	ULONG64 GameWorld;
	ULONG64 LocalGameWorld;
	ULONG64 FPSCamera;
	ULONG64 OpticCamera;
	ULONG64 RegisteredPlayers;

	D3DMATRIX ViewMatrix;
	D3DMATRIX OptViewMatrix;

	namespace Vars {
		bool ExpensiveLoot = false;
		bool Foods = false;
		bool Others = false;
		bool Recoil = false;
		bool Bones = true;
		bool PlayerEsp = true;
		bool DrawMinimap = true;
		//aimbot update
		int Aimbot = 0;


		int AimbotFov = 250;
		float PlayerDistance = 450;
		float ItemDistance = 300;

		namespace Menu {
			bool Aimbot = false;
			bool Visuals = true;
			bool Others = false;
		}
	}

	namespace Player {
		int PlayerCount;
		ULONG64 LocalPlayer;
		FVector LocalPlayerPos;
		FVector LocalHeadPos;
		FVector LocalPlayerViewAngles;
		float BulletSpeed;
		bool LocalPlayerAiming;
		std::string LocalPlayerGroup;
		FVector MyFirePortPos;
	}
	namespace Loot {
		int LootCount;
	}
	namespace TargetPlayer {
		FVector TargetLocation;
		FVector TargetHeadPos;
		FVector PredictTargetHeadPos;
		int Part = 133;

		float closet;

		Vector2 MyView;
		Vector2 TarView;

	}
}