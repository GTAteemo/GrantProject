#pragma once
#include "memory.hpp"
#include "globals.hpp"
#include "utils.hpp"
#include "draw.hpp"


RGBA orangecol = Col.orange;
RGBA greencol = Col.green;
RGBA redcol = Col.red;
RGBA bluecol = Col.blue;
RGBA purplecol = Col.purple;

int playerLevels[79] =
{
	0, 1000, 4017, 8432, 14256, 21477, 30023, 39936, 51204, 63723,
	77563, 92713, 111881, 134674, 161139, 191417, 225194, 262366, 302484, 345751,
	391649, 440444, 492366, 547896, 609066, 675913, 748474, 826786, 910885, 1000809,
	1096593, 1198275, 1309251, 1429580, 1559321, 1698532, 1847272, 2005600, 2173575, 2351255,
	2538699, 2735966, 2946585, 3170637, 3408202, 3659361, 3924195, 4202784, 4495210, 4801553,
	5121894, 5456314, 5809667, 6182063, 6573613, 6984426, 7414613, 7864284, 8333549, 8831052,
	9360623, 9928578, 10541848, 11206300, 11946977, 12789143, 13820522, 15229487, 17206065, 19706065,
	22706065, 26206065, 30206065, 34706065, 39706065, 45206065, 51206065, 58206065, 68206065
};


class Player {
private:
	ULONG64 list_base;
	uintptr_t transform;


	uintptr_t Index(int index) {
		return mem.Read<ULONG64>(list_base + 0x20 + (index * 0x08));
	}
public:
	uintptr_t instance;
	FVector headPosition;
	FVector location;
	string name;

	RGBA color = Col.white;

	bool readed = false;

	float health = 0;
	float maxHealth = 0;
	int role;
	int side;
	float distance;
	bool isPlayer;
	bool isPMC;
	bool isAiming;
	int experience = 0;
	string GroupId;
	string MemberCategory;
	float KD;
	float BulletSpeed = 0.f;
	float headDistance;

	//ptrs
	ULONG64 Profile_Ptr;
	ULONG64 Information_Ptr;
	ULONG64 Settings_Ptr;
	ULONG64 PlayerBody_Ptr;
	ULONG64 BoneInfo_Ptr;
	ULONG64 ProceduralWeaponAnimation_Ptr;

	int roleInt;
	string weapon;
	string weaponId;

	FVector Velocity;
	int velocityIndex = 0;
	FVector VelocityCal;
	FVector ViewAngles;
	FVector vHeadBone, vHip, vNeck
		, vUpperArmLeft, vUpperArmRight, vLeftHand
		, vRightHand, vLeftHand1, vRightHand1
		, vRightThigh, vLeftThigh, vRightCalf
		, vLeftCalf, vLeftFoot, vRightFoot, Pelvis;
	//explicit Player(int index) {
		//instance = Index(index);
	//}

	bool Init() {


		//list_base = mem->Read<ULONG64>(SDK::Globals::g_onlineUsers + 0x10);
		//if (!Utils::valid_pointer(list_base))
		//	return false;


		return true;
	}
	//必须先读，按顺序来，保存一下指针
	ULONG64 GetProfile()
	{
		return Profile_Ptr = mem.Read<ULONG64>(instance + 0x4F0);
	}
	ULONG64 GetInfomation()
	{
		return Information_Ptr = mem.Read<ULONG64>(Profile_Ptr + 0x28);
	}
	ULONG64 GetSetting()
	{
		return Settings_Ptr = mem.Read<ULONG64>(Information_Ptr + 0x48);
	}
	ULONG64 GetBoneMatrix()
	{
		return BoneInfo_Ptr = Utils::Player::Bones::getbone_matrix(instance);
	}
	ULONG64 GetProceduralWeaponAnimation()
	{
		return ProceduralWeaponAnimation_Ptr = mem.Read<ULONG64>(instance + 0x198);
	}


	bool IsAiming()
	{
		ULONG64 m_pbreath = mem.ReadChain(instance, { 0x198, 0x28 });
		return isAiming = mem.Read<bool>(m_pbreath + 0xA0);
	}

	void GetRootPosition() {
		if (!Utils::valid_pointer(BoneInfo_Ptr))
			return;
		ULONG64 bone = mem.ReadChain(BoneInfo_Ptr, { 0x20, 0x10, 0x38 });
		if (!Utils::valid_pointer(bone))
			return;
		location = mem.Read<FVector>(bone + 0x90);
	}
	void GetHeadPosition() {
		headPosition = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 133);
	}
	void GetDistance() {
		distance = location.Distance(Globals::Player::LocalPlayerPos);
	}
	void GetHeadDistance()
	{
		headDistance = headPosition.Distance(Globals::Player::LocalHeadPos);
	}


	ULONG64 GetPlayerBody()
	{
		return PlayerBody_Ptr = mem.Read<ULONG64>(instance + 0xA8);
	}
	// 1 USEC 2 BEAR 4 SCAV
	int GetSide()
	{
		return side = mem.Read<int>(Information_Ptr + 0x68);
	}
	bool IsPlayer()
	{
		return isPlayer = mem.Read<int>(Information_Ptr + 0x6C);
	}
	int GetExperience()
	{
		auto intCount = mem.Read<int>(Information_Ptr + 0x88);
		for (int i = 0; i < 79; i++)
		{
			if (intCount < playerLevels[i])
			{
				return experience = i;
			}
		}
		return experience = 0;
	}
	string GetMemberCategory()
	{
		MemberCategory = Utils::Text::string_to_utf8("Default ");
		int memCate = mem.Read<int>(Information_Ptr + 0x84);
		switch (memCate)
		{
		case 0:
			MemberCategory = Utils::Text::string_to_utf8("白边");
			break;
		case 1:
			MemberCategory = Utils::Text::string_to_utf8("开发者");
			break;
		case 2:
			MemberCategory = Utils::Text::string_to_utf8("黑边");
			break;
		case 4:
			MemberCategory = Utils::Text::string_to_utf8("商人");
			break;
		case 8:
			MemberCategory = Utils::Text::string_to_utf8("Group");
			break;
		case 16:
			MemberCategory = Utils::Text::string_to_utf8("系统");
			break;
		case 32:
			MemberCategory = Utils::Text::string_to_utf8("ChatModerator");
			break;
		case 64:
			MemberCategory = Utils::Text::string_to_utf8("ChatModeratorWithPermanentBan");
			break;
		case 128:
			MemberCategory = Utils::Text::string_to_utf8("测试");
			break;
		case 256:
			MemberCategory = Utils::Text::string_to_utf8("夏尔巴人");
			break;
		case 512:
			MemberCategory = Utils::Text::string_to_utf8("使者");
			break;
		default:
			MemberCategory = Utils::Text::string_to_utf8("白边");
			break;
		}

		return  MemberCategory;
	}
	string GetGroup()
	{
		GroupId = "null";
		ULONG64 nick_ptr = mem.Read<ULONG64>(Information_Ptr + 0x20);
		if (!Utils::valid_pointer(nick_ptr))
			return GroupId;
		GroupId = Utils::Text::read_str(nick_ptr);
		return GroupId;
	}

	//Boss identify
	int GetRole()
	{
		return role = mem.Read<int>(Settings_Ptr + 0x10);
	}

	bool IsPMC()
	{
		return (side == 1 || side == 2);
	}

	string GetName()
	{
		name = "null";
		ULONG64 nick_ptr = mem.Read<ULONG64>(Information_Ptr + 0x10);
		if (!Utils::valid_pointer(nick_ptr))
			return name;

		if (IsPMC())
		{
			name = Utils::Text::read_str(nick_ptr) + (side == 1 ? "(USEC)" : "(BEAR)");
		}
		else
		{
			if (isPlayer)
			{
				name = _xor_("Scav(Player)");
			}
			else
			{
				switch (role)
				{
				case 1:
					name = _xor_("Scav");
					break;
				case 2:
					name = _xor_("Scav");
					break;
				case 4:
					name = _xor_("Boss Test");
					break;
				case 8:
					name = _xor_("Reshala"); // Rashalla
					break;
				case 16:
					name = _xor_("Follower Test");
					break;
				case 32:
					name = _xor_("Reshala Guard");
					break;
				case 64:
					name = _xor_("Killa");
					break;
				case 128:
					name = _xor_("Shturman");
					break;
				case 256:
					name = _xor_("Shturman Guard");
					break;
				case 512:
					name = _xor_("PMC Raider");
					break;
				case 1024:
					name = _xor_("Cursed Scav");
					break;
				case 2048:
					name = _xor_("Gluhar");
					break;
				case 4096:
					name = _xor_("Gluhar Assault");
					break;
				case 8192:
					name = _xor_("Gluhar Security");
					break;
				case 16384:
					name = _xor_("Gluhar Scout");
					break;
				case 32768:
					name = _xor_("Gluhar Sniper");
					break;
				case 65536:
					name = _xor_("Sanitar Guard");
					break;
				case 131072:
					name = _xor_("Sanitar");
					break;
				case 262144:
					name = _xor_("Test");
					break;
				case 524288:
					name = _xor_("Assault Raider");
					break;
				case 1048576:
					name = _xor_("Sectant Warrior");
					break;
				case 2097152:
					name = _xor_("Sectant Priest");
					break;
				case 4194304:
					name = _xor_("Tagilla");
					break;
				case 8388608:
					name = _xor_("Tagilla Guard");
					break;
				case 16777216:
					name = _xor_("Ex-PMC");
					break;
				case 33554432:
					name = _xor_("SANTA!!!!");
					break;
				case 67108864:
					name = _xor_("bossKnight");
					break;
				case 134217728:
					name = _xor_("followerBigPipe");
					break;
				case 268435456:
					name = _xor_("followerBirdEye");
					break;
				default:
					cout << role << endl;
					name = _xor_("Other");
					break;
				}
			}
		}

		return name;
	}



	void GetHealth()
	{
		health = 0;
		maxHealth = 0;
		ULONG64 m_pHealthController = mem.Read<ULONG64>(instance + 0x528);
		ULONG64 m_pHealthBody = mem.Read<ULONG64>(m_pHealthController + 0x50);
		ULONG64 m_pBodyController = mem.Read<ULONG64>(m_pHealthBody + 0x18);
		for (int i = 0; i < 7; i++) {
			auto body_part = mem.Read<ULONG64>(m_pBodyController + 0x30 + (i * 0x18));
			auto health_container = mem.Read<ULONG64>(body_part + 0x10);
			float hp = mem.Read<float>(health_container + 0x10);
			float health_max = mem.Read<float>(health_container + 0x14);
			health += hp;
			maxHealth += health_max;
		}
	}

	void GetWeaponName() {
		ULONG64 ProceduralWeaponAnimation = mem.Read<ULONG64>(instance + 0x198);
		if (!Utils::valid_pointer(ProceduralWeaponAnimation))
			return;
		ULONG64 weaponTemp = mem.ReadChain(ProceduralWeaponAnimation, { 0x48, 0x28, 0x40 });
		if (!Utils::valid_pointer(weaponTemp))
			return;
		ULONG64 EngineString = mem.Read<ULONG64>(weaponTemp + 0x18);
		if (!Utils::valid_pointer(EngineString))
			return;
		weapon = Utils::Text::read_str(EngineString);
		EngineString = mem.Read<ULONG64>(weaponTemp + 0x50);
		if (!Utils::valid_pointer(EngineString))
			return;
		weaponId = Utils::Text::read_str(EngineString);

	}

	void GetBones() {
		vHeadBone = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 133);


		vHip = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 14);


		vNeck = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 132);


		vUpperArmLeft = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 90);


		vUpperArmRight = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 111);


		vLeftHand = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 91);


		vRightHand = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 112);


		vLeftHand1 = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 92);


		vRightHand1 = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 113);


		vRightThigh = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 20);


		vLeftThigh = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 15);


		vRightCalf = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 22);


		vLeftCalf = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 17);


		vLeftFoot = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 18);


		vRightFoot = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 23);


		Pelvis = Utils::Player::Bones::get_bone_position(BoneInfo_Ptr, 14);

	}

	FVector GetViewAngle()
	{
		ULONG64 MovementContext = mem.Read<ULONG64>(instance + 0x40);
		if (!Utils::valid_pointer(MovementContext))
			return FVector{};
		return ViewAngles = mem.Read<FVector>(MovementContext + 0x22C); //last on in 3 vector
	}


	void MaxLevel()
	{
		ULONG64 Physical = mem.Read<ULONG64>(instance + 0x500);
		ULONG64 Stamina = mem.Read<ULONG64>(Physical + 0x38);
		mem.Write(Stamina + 0x48, 1000.f, sizeof(float));
		ULONG64 HandStamina = mem.Read<ULONG64>(Physical + 0x40);
		mem.Write(HandStamina + 0x48, 1000.f, sizeof(float));
		ULONG64 OxgenStamina = mem.Read<ULONG64>(Physical + 0x48);
		mem.Write(OxgenStamina + 0x48, 1000.f, sizeof(float));
	}


	float GetKD()
	{

		ULONG64 stats = mem.Read<ULONG64>(Profile_Ptr + 0xE8);
		ULONG64 overallCounters = mem.Read<ULONG64>(stats + 0x18);
		ULONG64 counters = mem.Read<ULONG64>(overallCounters + 0x10);
		ULONG64 count = mem.Read<uint32_t>(counters + 0x40);
		if (count < 0 || count > 1000)
			return 0.f;

		ULONG64 entries = mem.Read<ULONG64>(counters + 0x18);
		ULONG64 keys = mem.Read<ULONG64>(counters + 0x28);

		if (!entries)
			return 0.f;

		uint32_t kills(0), deaths(0);

		auto arrayBase = entries + 0x28;

		for (auto i = 0; i < count; i++)
		{
			auto key = mem.Read<ULONG64>(arrayBase + i * 0x18 + 0x18);
			auto hash = mem.Read<uint32_t>(key + 0x18);
			auto value = mem.Read<ULONG64>(arrayBase + i * 0x18 + 0x20);

			auto keySet = mem.Read<ULONG64>(key + 0x10);
			auto keySetSlots = mem.Read<ULONG64>(keySet + 0x18);
			auto setArrayBase = keySetSlots + 0x28;
			auto first = mem.Read<ULONG64>(setArrayBase);
			auto statName = Utils::Text::read_str(first);
			if (statName == "Kills")
				kills = value;
			else if (statName == "Deaths")
				deaths = value;
		}
		return KD = (deaths == 0) ? float(kills) : float(kills) / float(deaths);
	}

	//enegy and hydration  useless
	void GetVelocity()
	{
		ULONG64 MotionReact = mem.Read<ULONG64>(ProceduralWeaponAnimation_Ptr + 0x38);
		Velocity = mem.Read<FVector>(MotionReact + 0x44);
	}

	float GetBulletSpeed() {

		ULONG64 active_weapon = mem.Read<ULONG64>(ProceduralWeaponAnimation_Ptr + 0x48);
		active_weapon = mem.Read<ULONG64>(active_weapon + 0x28);
		//[Class] EFT.InventoryLogic.Weapon
		if (active_weapon) {
			//// EFT.Player->HandsController (as FirearmController)->Item (as Weapon)->Chambers (Slot[])->Slot[0] (Slot)->ContainedItem (as GClassXXXX)->ItemTemplate (as AmmoTemplate)->InitialSpeed (float)
			//auto ammo_template = mem.ReadChain(active_weapon, { 0x98, 0x20, 0x38, 0x38 });
			//if (!ammo_template) {
			//	//EFT.Player->HandsController(as FirearmController)->Item(as Weapon)->ItemTemplate(as WeaponTemplate)->_defAmmoTemplate(AmmoTemplate)->InitialSpeed(float)
			//	
			//}
			ULONG64 ammo_template = mem.ReadChain(active_weapon, { 0x98, 0x20, 0x38, 0x40 });
			if (!ammo_template)
			{
				ammo_template = mem.ReadChain(active_weapon, { 0x40, 0x168 });
			}
			float initial_speed = mem.Read <float>(ammo_template + 0x18C);
			return BulletSpeed = initial_speed;
		}

		return BulletSpeed = 0.f;
	}
	/*enum Parts {
		vHeadBone = 133,
		vHip = 14,
		vNeck = 32,
		vUpperArmLeft = 90,
		vUpperArmRight = 111,
		vLeftHand = 91,
		vRightHand = 112,
		vLeftHand1 = 92,
		vRightHand1 = 113,
		vRightThigh = 20,
		vLeftThigh = 15,
		vRightCalf = 22,
		vLeftCalf = 17,
		vLeftFoot = 18,
		vRightFoot = 23,
		Pelvis = 14
	};*/
};