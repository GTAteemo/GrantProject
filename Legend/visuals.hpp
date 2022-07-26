#pragma once
#include "Threads.hPP"
#include "Draw.hPP"

#include <D3dx9tex.h>
#pragma comment(lib, "D3dx9")


D2D1_POINT_2F player_screenHead{};
D2D1_POINT_2F player_screen;
std::wstring name;
char nameBuf[256];
char weaponBuf[256];
char distBuf[256];
char healBuf[256];
char expBuf[256];
char memBuf[256];
char GroupBuf[256];
char KDBuf[256];

D2D1_POINT_2F vHeadBoneOut;
D2D1_POINT_2F vHipOut;
D2D1_POINT_2F vNeckOut;
D2D1_POINT_2F vUpperArmLeftOut;
D2D1_POINT_2F vUpperArmRightOut;
D2D1_POINT_2F vLeftHandOut;
D2D1_POINT_2F vRightHandOut;
D2D1_POINT_2F vLeftHandOut1;
D2D1_POINT_2F vRightHandOut1;
D2D1_POINT_2F vRightThighOut;
D2D1_POINT_2F vLeftThighOut;
D2D1_POINT_2F vRightCalfOut;
D2D1_POINT_2F vLeftCalfOut;
D2D1_POINT_2F vLeftFootOut;
D2D1_POINT_2F vRightFootOut;
D2D1_POINT_2F PelvisOut;

char volocity[256];
vector<string> DrawPlayerTexts;
void DrawPlayers() {

	sprintf_s(nameBuf, "[当前实体数量:%d]", Globals::Player::PlayerCount);
	DrawStrokeText(100, 80, &Col.white, nameBuf);
	for (auto player : PlayerList) {
		DrawPlayerTexts.clear();
		if (player.instance == Globals::Player::LocalPlayer)
		{
			//DrawHaertArray(WindowWidth / 2 - 35 * 10, WindowHeight - 100, 30, 5, player.health / player.maxHealth * 100.f, &Col.red);
			sprintf_s(volocity, "[x:%0.0f y:%0.0f z:%0.0f]", player.Velocity.x, player.Velocity.y, player.Velocity.z);
			DrawStrokeText(100, 160, &Col.white, volocity);
			continue;
		}
		if (!Utils::Camera::WorldToScreenv(player.headPosition, player_screenHead))
		{
			continue;
		}
		if (player.distance > Globals::Vars::PlayerDistance) continue;
		Utils::Camera::WorldToScreenv(player.location, player_screen);

		//预判
		float FlyDistanceVertical = player.headPosition.z - Globals::Player::MyFirePortPos.z;
		float FlyDistanceHorizontal = float(sqrtf(powf(player.headPosition.x - Globals::Player::MyFirePortPos.x, 2.0) + powf(player.headPosition.y - Globals::Player::MyFirePortPos.y, 2.0)));
		Vector2 PredictViewAnagle = Utils::CalcViewAngles(Globals::Player::MyFirePortPos, player.headPosition);
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
		FVector Prodiction;
		Prodiction = player.headPosition + player.Velocity * FlyingTime;
		Prodiction.z = Globals::Player::MyFirePortPos.z + FlyDistanceHorizontal * tan(-PredictViewAnagle.y / Deg);
		D2D1_POINT_2F PridectLocation;
		if (Utils::Camera::WorldToScreenv(Prodiction, PridectLocation))
		{
			DrawCircle(PridectLocation.x, PridectLocation.y, 4, &Col.red, 5);
		}
		float BoxHeight = (float)(player_screen.y - player_screenHead.y);
		float BoxWidth = BoxHeight / 2.0f;
		//DrawNormalBox(player_screen.x - (BoxWidth / 2), player_screenHead.y, BoxWidth, BoxHeight, 1, &Col.red);

		DrawCircle(player_screenHead.x, player_screenHead.y, BoxHeight / 15, &Col.yellow, 100);
		sprintf(nameBuf, u8"%s", player.name.c_str());
		DrawPlayerTexts.push_back(nameBuf);
		sprintf(healBuf, u8"血量:%0.0f", player.health);
		DrawPlayerTexts.push_back(healBuf);
		sprintf_s(distBuf, u8"距离:[%0.f米]", player.distance);
		DrawPlayerTexts.push_back(distBuf);
		sprintf_s(expBuf, u8"等级:%d", player.experience);
		DrawPlayerTexts.push_back(expBuf);
		if (player.isPlayer)
		{
			sprintf(KDBuf, u8"KD:%0.1f", player.KD);
			DrawPlayerTexts.push_back(KDBuf);
		}
		sprintf(weaponBuf, u8"武器:%s", player.weapon.c_str());
		DrawPlayerTexts.push_back(weaponBuf);
		sprintf(memBuf, u8"分类:%s", player.MemberCategory.c_str());
		DrawPlayerTexts.push_back(memBuf);

		if (player.GroupId == Globals::Player::LocalPlayerGroup)
		{
			DrawPlayerTexts.push_back(u8"傻逼队友");
		}
		else
		{
			map<std::string, char>::iterator iter;
			iter = GroupMap.find(player.GroupId);
			if (iter != GroupMap.end()) {
				char gp = iter->second;
				sprintf(GroupBuf, u8"队伍:%c", gp);
				DrawPlayerTexts.push_back(GroupBuf);
			}
		}

		DrawProgressBar(player_screenHead.x, player_screenHead.y - 15, player.health, player.maxHealth);
		DrawRectCenter(DrawPlayerTexts, player_screen.x, player_screen.y + 5, &Col.darkgreen);
		DrawPlayerAimLine(player.headPosition, player.ViewAngles, 3);
	}

	for (Player player : PlayerBonese)
	{
		if (player.distance > Globals::Vars::PlayerDistance) continue;
		if (Globals::Player::LocalPlayer == player.instance) continue;
		if (Globals::Vars::Bones && Utils::Camera::WorldToScreenv(player.vHeadBone, vHeadBoneOut))
		{
			Utils::Camera::WorldToScreenv(player.vHip, vHipOut);
			Utils::Camera::WorldToScreenv(player.vNeck, vNeckOut);
			Utils::Camera::WorldToScreenv(player.vUpperArmLeft, vUpperArmLeftOut);
			Utils::Camera::WorldToScreenv(player.vUpperArmRight, vUpperArmRightOut);
			Utils::Camera::WorldToScreenv(player.vLeftHand, vLeftHandOut);
			Utils::Camera::WorldToScreenv(player.vRightHand, vRightHandOut);
			Utils::Camera::WorldToScreenv(player.vLeftHand1, vLeftHandOut1);
			Utils::Camera::WorldToScreenv(player.vRightHand1, vRightHandOut1);
			Utils::Camera::WorldToScreenv(player.vRightThigh, vRightThighOut);
			Utils::Camera::WorldToScreenv(player.vLeftThigh, vLeftThighOut);
			Utils::Camera::WorldToScreenv(player.vRightCalf, vRightCalfOut);
			Utils::Camera::WorldToScreenv(player.vLeftCalf, vLeftCalfOut);
			Utils::Camera::WorldToScreenv(player.vLeftFoot, vLeftFootOut);
			Utils::Camera::WorldToScreenv(player.vRightFoot, vRightFootOut);
			Utils::Camera::WorldToScreenv(player.Pelvis, PelvisOut);

			DrawLine(vNeckOut.x, vNeckOut.y, vHeadBoneOut.x, vHeadBoneOut.y, &Col.green, 1);
			DrawLine(PelvisOut.x, PelvisOut.y, vNeckOut.x, vNeckOut.y, &Col.green, 1);

			DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &Col.green, 1);
			DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &Col.green, 1);

			DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &Col.green, 1);
			DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &Col.green, 1);

			DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &Col.green, 1);
			DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &Col.green, 1);

			DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &Col.green, 1);
			DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &Col.green, 1);

			DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &Col.green, 1);
			DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &Col.green, 1);

			DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &Col.green, 1);
			DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &Col.green, 1);
		}
	}
	//return;
}


D2D1_POINT_2F item_screen;
char itemBuf[256];
char disBuf[256];
vector<string> ItemTexts;
void DrawItems() {


	for (auto& item : ItemsList) {
		ItemTexts.clear();
		item.GetDistance();
		if ((item.distance > Globals::Vars::ItemDistance) && (!item.haveExp)) continue;
		if (!Utils::Camera::WorldToScreenv(item.location, item_screen)) continue;
		if (item.isContainer)
		{		
			bool showContainer = false;
			for (InItem inItem : item.inItems)
			{
				if (inItem.role == 4)
				{
					showContainer = true;
					sprintf_s(itemBuf, u8"%s", inItem.name.c_str());
					ItemTexts.push_back(itemBuf);
				}
				else if (inItem.role == 3 && Globals::Vars::ExpensiveLoot)
				{
					showContainer = true;
					sprintf_s(itemBuf, u8"%s", inItem.name.c_str());
					ItemTexts.push_back(itemBuf);
				}
				else if (inItem.role == 2 && Globals::Vars::Foods)
				{
					showContainer = true;
					sprintf_s(itemBuf, u8"%s", inItem.name.c_str());
					ItemTexts.push_back(itemBuf);
				}
				else if (Globals::Vars::Others)
				{
					showContainer = true;
					sprintf_s(itemBuf, u8"%s", inItem.name.c_str());
					ItemTexts.push_back(itemBuf);
				}
			}
			if (showContainer)
			{
				sprintf_s(disBuf, u8"[. %0.0f]", item.distance);
				ItemTexts.push_back(disBuf);
				DrawRectCenter(ItemTexts, item_screen.x, item_screen.y + 5, &Col.white);
			}
		}
		else
		{
			if (item.role == 4)
			{
				sprintf_s(itemBuf, u8"[%s %0.0fm]", item.name.c_str(), item.distance);
				ItemTexts.push_back(itemBuf);
			}
			if (item.role == 3 && Globals::Vars::ExpensiveLoot)
			{
				sprintf_s(itemBuf, u8"[%s %0.0fm]", item.name.c_str(), item.distance);
				ItemTexts.push_back(itemBuf);
			}
			else if (item.role == 2 && Globals::Vars::Foods)
			{
				sprintf_s(itemBuf, u8"[%s %0.0fm]", item.name.c_str(), item.distance);
				ItemTexts.push_back(itemBuf);
			}
			else if (Globals::Vars::Others)
			{
				sprintf_s(itemBuf, u8"[%s %0.0fm]", item.name.c_str(), item.distance);
				ItemTexts.push_back(itemBuf);
			}
			if (ItemTexts.size() > 0)
			{
				DrawRectCenter(ItemTexts, item_screen.x, item_screen.y + 5, &Col.white);
			}
		}
	}
}

char myViewBuff[255];
char tarViewBuff[255];
//char MXBUF1[255];
//char MXBUF2[255];
//char MXBUF3[255];
//char MXBUF4[255];
D2D1_POINT_2F target_head_screen;
D2D1_POINT_2F pre_head_screen;
void DrawAimLine() {

	sprintf_s(myViewBuff, "[x:%0.0f y:%0.0f]", Globals::TargetPlayer::MyView.x, Globals::TargetPlayer::MyView.y);
	DrawStrokeText(100, 100, &Col.white, myViewBuff);
	sprintf_s(myViewBuff, "[x:%0.0f y:%0.0f]", Globals::Player::LocalPlayerPos.x, Globals::Player::LocalPlayerPos.y);
	DrawStrokeText(100, 120, &Col.white, myViewBuff);
	/*sprintf_s(MXBUF1, "[_11:%0.1f _12:%0.1f _13:%0.1f _14:%0.1f]", Globals::ViewMatrix._11, Globals::ViewMatrix._12, Globals::ViewMatrix._13, Globals::ViewMatrix._14);
	sprintf_s(MXBUF2, "[_21:%0.1f _22:%0.1f _23:%0.1f _24:%0.1f]", Globals::ViewMatrix._21, Globals::ViewMatrix._22, Globals::ViewMatrix._23, Globals::ViewMatrix._24);
	sprintf_s(MXBUF3, "[_31:%0.1f _32:%0.1f _33:%0.1f _34:%0.1f]", Globals::ViewMatrix._31, Globals::ViewMatrix._32, Globals::ViewMatrix._33, Globals::ViewMatrix._34);
	sprintf_s(MXBUF4, "[_41:%0.1f _42:%0.1f _43:%0.1f _44:%0.1f]", Globals::ViewMatrix._41, Globals::ViewMatrix._42, Globals::ViewMatrix._43, Globals::ViewMatrix._44);*/
	/*DrawStrokeText(100, 140, &Col.white, MXBUF1);
	DrawStrokeText(100, 160, &Col.white, MXBUF2);
	DrawStrokeText(100, 180, &Col.white, MXBUF3);
	DrawStrokeText(100, 200, &Col.white, MXBUF4);*/
	string aimopen;
	if (Globals::Vars::Aimbot == 0)
	{
		aimopen = "自瞄关闭";
	}
	else if (Globals::Vars::Aimbot == 1)
	{
		aimopen = "暴力自瞄";
	}
	else if (Globals::Vars::Aimbot == 2)
	{
		aimopen = "咸粽粽同款自瞄";
	}
	else if (Globals::Vars::Aimbot == 3)
	{
		aimopen = "自动压抢拉满！";
	}
	sprintf_s(tarViewBuff, "%s", aimopen);
	DrawStrokeText(100, 140, &Col.white, tarViewBuff);
	if (!(Globals::Vars::Aimbot == 1)) return;
	DrawCircle(WindowWidth / 2, WindowHeight / 2, 3, &Col.red, 0);
	if (!(Globals::TargetPlayer::closet > 0)) return;
	Utils::Camera::WorldToScreenv(Globals::TargetPlayer::TargetHeadPos, target_head_screen);
	DrawLine2(ImVec2(target_head_screen.x, target_head_screen.y), ImVec2(WindowWidth / 2, WindowHeight / 2), ImColor(Col.blue.R, Col.blue.G, Col.blue.B, 255), 2);
	DrawCircle(target_head_screen.x, target_head_screen.y, 3, &Col.red, 5);
	DrawCircle(WindowWidth / 2, WindowHeight / 2, Globals::Vars::AimbotFov, &Col.blue, 0);
}

ImVec2 mapPos{ 400,400 };
float fov = 110.f;
float mapSize = 250.f;
float maxDistance = 300.f;
void DrawMiniMap() {
	if (!Globals::Vars::DrawMinimap)
		return;
	DrawMiniMap(mapPos, fov, mapSize);
	RGBA clr = Col.red;
	for (auto player : PlayerList) {
		if (player.instance == Globals::Player::LocalPlayer)
		{
			DrawCircleArror(ImVec2{ mapPos.x,mapPos.y }, 90.f, 7, 60.f, &Col.yellow);
			continue;
		}
		if (player.GroupId == Globals::Player::LocalPlayerGroup)
		{
			clr = Col.green;
		}
		else 
		{
			if (player.side == 1 || player.side == 2)
			{
				clr = Col.orange;
			}
			else if (player.isPlayer)
			{
				clr = Col.pink;
			}
			else {
				clr = Col.red;
			}
		}
		FVector target = player.location;
		FVector mypos = Globals::Player::LocalPlayerPos;
		Vector2 delta = Vector2{ target.x - mypos.x, target.y - mypos.y };
		float deltaVecLength = sqrt(delta.x * delta.x + delta.y * delta.y);
		if (deltaVecLength > maxDistance)
		{
			deltaVecLength = maxDistance;
		}
		deltaVecLength = deltaVecLength * mapSize / maxDistance;
		float yaw = atan2(delta.x, delta.y) * (180 / PI);
		float viewy = Globals::Player::LocalPlayerViewAngles.x;
		float deg = yaw - viewy;
		while (deg > 180)
			deg -= 360;
		while (deg < -180)
			deg += 360;
		Vector2 dotpos;
		dotpos.x = sin(deg * PI / 180) * deltaVecLength;
		dotpos.y = -cos(deg * PI / 180) * deltaVecLength;
		float vdelta = player.ViewAngles.x - Globals::Player::LocalPlayerViewAngles.x;
		while (vdelta > 180)
			vdelta -= 360;
		while (vdelta < -180)
			vdelta += 360;

		DrawCircleArror(ImVec2{ mapPos.x + dotpos.x,mapPos.y + dotpos.y }, -vdelta + 90.f, 7, 60.f, &clr);
	}
}

char* myAimBuff;
using namespace Globals::TargetPlayer;
void  DrawAimPart() {

	if (Part == 133) {
		myAimBuff = "Head";
	}
	else if (Part == 17) {
		myAimBuff = "LeftCalf";
	}
	else if (Part == 22) {
		myAimBuff = "RightCalf";
	}
	else if (Part == 111) {
		myAimBuff = "UpperArmRight";
	}
	else if (Part == 90) {
		myAimBuff = "UpperArmLeft";
	}
	else if (Part == 66) {
		myAimBuff = "RibCage";
	}
	else if (Part == 14) {
		myAimBuff = "NiuNiu";
	}
	else {
		cout << " ?" << endl;
	}


	//DrawStrokeText(100, 140, &Col.white, myAimBuff);
}

void  DrawTest() {

	DrawsTestFunction();
}
