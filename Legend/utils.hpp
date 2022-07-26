#pragma once
#include "memory.hpp"
#include "math.hpp"
#include "globals.hpp"

using namespace std;

struct GameObjectManager
{
	ULONG64 lastTaggedObject; //0x00
	ULONG64 taggedObjects; // 0x08
	ULONG64 lastMainCameraTaggedNode; //0x0010
	ULONG64 mainCameraTaggedNodes; // 0x18
	ULONG64 lastActiveObject; //0x0020
	ULONG64 activeObjects; //0x0028
}; //Size: 0x0010
struct BaseObject
{
	uint64_t previousObjectLink; //0x0000
	uint64_t nextObjectLink; //0x0008
	uint64_t object; //0x0010
};

namespace Utils {
	inline auto valid_pointer(ULONG64 pointer) -> bool {
		return (pointer && pointer > 0xFFFFFF && pointer < 0x7FFFFFFFFFFF);
	}

	struct BaseObject
	{
		uint64_t previousObjectLink; //0x0000
		uint64_t nextObjectLink; //0x0008
		uint64_t object; //0x0010
	};


	uint64_t GetObjectFromList(uint64_t listPtr, uint64_t lastObjectPtr, const char* objectName)
	{
		uint64_t classNamePtr = 0x0;
		BaseObject activeObject = mem.Read<BaseObject>(listPtr);
		BaseObject lastObject = mem.Read<BaseObject>(lastObjectPtr);
		//  std::cout << "-------------------" << std::endl;
		if (activeObject.object != 0x0)
		{
			while (activeObject.object != 0 && activeObject.object != lastObject.object)
			{
				if (!Utils::valid_pointer(activeObject.object) || !Utils::valid_pointer(activeObject.nextObjectLink) || !Utils::valid_pointer(activeObject.previousObjectLink))
					break;
				//printf("%d\n", CurrentCount);
				classNamePtr = mem.Read<uint64_t>(activeObject.object + 0x60);
				if (!Utils::valid_pointer(classNamePtr))
					break;
				char buf[256] = { 0 };
				auto cache = mem.Read<array<char, 256>>(classNamePtr);
				for (int i = 0; i < cache.size(); i++)
				{
					buf[i] = cache[i];
				}
				auto name = buf;
				//std::cout << name << std::endl;
				if (strcmp(name, objectName) == 0)
				{
					return activeObject.object;
				}
				activeObject = mem.Read<BaseObject>(activeObject.nextObjectLink);
				Sleep(5);
			}
		}
		if (lastObject.object != 0x0)
		{
			if (!Utils::valid_pointer(lastObject.object) || !Utils::valid_pointer(lastObject.nextObjectLink) || !Utils::valid_pointer(lastObject.previousObjectLink))
				return 0x0;
			classNamePtr = mem.Read<uint64_t>(lastObject.object + 0x60);
			char buf[256] = { 0 };
			auto cache = mem.Read<array<char, 256>>(classNamePtr);
			for (int i = 0; i < cache.size(); i++)
			{
				buf[i] = cache[i];
			}
			auto name = buf;
			//std::cout << "lasat:" << name << std::endl;
			if (strcmp(name, objectName) == 0)
			{
				return activeObject.object;
			}
		}
		//std::cout << "-------------------" << std::endl;
		return 0x0;
	}
	ULONG64 GetObjectFromListv2(char* Name, ULONG64 gom)
	{

		ULONG64 buffer = mem.Read<ULONG64>(gom + offsetof(GameObjectManager, lastTaggedObject));

		int loop_count = 10000;

		ULONG64 result = 0;

		for (int i = 0; (buffer && i < loop_count); i++)
		{
			ULONG64 object_ptr = mem.Read<ULONG64>(buffer + 0x10);
			ULONG64 object_name_ptr = mem.Read<ULONG64>(object_ptr + 0x60);
			char buf[256] = { 0 };
			auto cache = mem.Read<array<char, 256>>(object_name_ptr);

			for (int i = 0; i < cache.size(); i++)
			{
				buf[i] = cache[i];
			}
			auto object_name = buf;

			if (strstr(object_name, Name))
			{
				result = object_ptr;
				break;
			}
			if (strcmp(object_name, Name))
			{
				break;
			}


			buffer = mem.Read<ULONG64>(buffer + 0x0);
		}
		return result;
	}

	namespace Player {
		namespace Bones {

			FVector GetPosition(ULONG64 transform)
			{
				auto transform_internal = mem.Read<ULONG64>(transform + 0x10);

				auto matrices = mem.Read<ULONG64>(transform_internal + 0x38);
				auto index = mem.Read<int>(transform_internal + 0x40);

				ULONG64 matrix_list_base = mem.Read<uintptr_t>((uintptr_t)(matrices + 0x18));
				ULONG64 dependency_index_table_base = mem.Read<uintptr_t>((uintptr_t)(matrices + 0x20));


				static auto get_dependency_index = [](ULONG64 base, int32_t index)
				{
					index = mem.Read<int32_t>((uintptr_t)(base + index * 4));
					return index;
				};

				static auto get_matrix_blob = [](ULONG64 base, ULONG64 offs, float* blob, ULONG size) {

					blob = mem.Read<float*>((uintptr_t)(base + offs));

					return blob;
				};

				int32_t index_relation = mem.Read<int32_t>((uintptr_t)(dependency_index_table_base + index * 4));
				//int32_t index_relation = get_dependency_index(dependency_index_table_base, index);

				FVector ret_value;
				{
					float* base_matrix3x4 = (float*)malloc(64),
						* matrix3x4_buffer0 = (float*)((ULONG64)base_matrix3x4 + 16),
						* matrix3x4_buffer1 = (float*)((ULONG64)base_matrix3x4 + 32),
						* matrix3x4_buffer2 = (float*)((ULONG64)base_matrix3x4 + 48);

					auto base_matrix3x4_cache = mem.Read<array<float, 4>>(matrix_list_base + index * 48);
					for (int i = 0; i < 4; i++)
					{
						base_matrix3x4[i] = base_matrix3x4_cache[i];
					}
					// get_matrix_blob(matrix_list_base, index * 48, base_matrix3x4, 16);

					__m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
					__m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
					__m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };

					while (index_relation >= 0)
					{
						ULONG matrix_relation_index = 6 * index_relation;

						// paziuret kur tik 3 nureadina, ten translationas, kur 4 = quatas ir yra rotationas.

						auto matrix3x4_buffer2_cache = mem.Read<array<float, 4>>(matrix_list_base + 8 * matrix_relation_index);
						for (int i = 0; i < 4; i++)
						{
							matrix3x4_buffer2[i] = matrix3x4_buffer2_cache[i];
						}
						//get_matrix_blob(matrix_list_base, 8 * matrix_relation_index, matrix3x4_buffer2, 16);
						__m128 v_0 = *(__m128*)matrix3x4_buffer2;


						auto matrix3x4_buffer0_cache = mem.Read<array<float, 4>>(matrix_list_base + 8 * matrix_relation_index + 32);
						for (int i = 0; i < 4; i++)
						{
							matrix3x4_buffer0[i] = matrix3x4_buffer0_cache[i];
						}
						//get_matrix_blob(matrix_list_base, 8 * matrix_relation_index + 32, matrix3x4_buffer0, 16);
						__m128 v_1 = *(__m128*)matrix3x4_buffer0;


						auto matrix3x4_buffer1_cache = mem.Read<array<float, 4>>(matrix_list_base + 8 * matrix_relation_index + 16);
						for (int i = 0; i < 4; i++)
						{
							matrix3x4_buffer1[i] = matrix3x4_buffer1_cache[i];
						}
						//get_matrix_blob(matrix_list_base, 8 * matrix_relation_index + 16, matrix3x4_buffer1, 16);
						__m128i v9 = *(__m128i*)matrix3x4_buffer1;

						__m128* v3 = (__m128*)base_matrix3x4; // r10@1
						__m128 v10; // xmm9@2
						__m128 v11; // xmm3@2
						__m128 v12; // xmm8@2
						__m128 v13; // xmm4@2
						__m128 v14; // xmm2@2
						__m128 v15; // xmm5@2
						__m128 v16; // xmm6@2
						__m128 v17; // xmm7@2

						v10 = _mm_mul_ps(v_1, *v3);
						v11 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 0));
						v12 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 85));
						v13 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -114));
						v14 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -37));
						v15 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, -86));
						v16 = _mm_castsi128_ps(_mm_shuffle_epi32(v9, 113));

						v17 = _mm_add_ps(
							_mm_add_ps(
								_mm_add_ps(
									_mm_mul_ps(
										_mm_sub_ps(
											_mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1350), v13),
											_mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1360), v14)),
										_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), -86))),
									_mm_mul_ps(
										_mm_sub_ps(
											_mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1360), v14),
											_mm_mul_ps(_mm_mul_ps(v11, (__m128)xmmword_1410D1340), v16)),
										_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 85)))),
								_mm_add_ps(
									_mm_mul_ps(
										_mm_sub_ps(
											_mm_mul_ps(_mm_mul_ps(v12, (__m128)xmmword_1410D1340), v16),
											_mm_mul_ps(_mm_mul_ps(v15, (__m128)xmmword_1410D1350), v13)),
										_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(v10), 0))),
									v10)),
							v_0);

						*v3 = v17;

						index_relation = mem.Read<int32_t>((uintptr_t)(dependency_index_table_base + index_relation * 4));
						//index_relation = get_dependency_index(dependency_index_table_base, index_relation);
					}

					ret_value = *(FVector*)base_matrix3x4;
					delete[] base_matrix3x4;

				}

				return ret_value;
			}
			ULONG64 getbone_matrix(ULONG64 instance)
			{
				return mem.ReadChain(instance, { 0xA8 , 0x28, 0x28, 0x10 });
			}
			FVector get_bone_position(uint64_t bone_info, uint32_t index) {

				if (!bone_info)
					return FVector();
				ULONG64 bone_transform = mem.Read<ULONG64>(bone_info + 0x20 + (index * 0x8));
				if (!bone_transform)
					return FVector();
				if (!bone_transform)
					return  FVector();
				return GetPosition(bone_transform);
			}
		}
	}

	uint64_t GetComponentFromGameObject(uint64_t game_object, const char* component_name)
	{
		char Name[256];

		uint64_t Test = mem.Read<uint64_t>(game_object + 0x30);

		for (int i = 0x8; i < 0x1000; i += 0x10)
		{
			uint64_t Fields = mem.Read<uint64_t>(mem.Read<uint64_t>(Test + i) + 0x28);

			uint64_t NameChain = mem.ReadChain(Fields, { 0x0, 0x0, 0x48 });

			char buf[256] = { 0 };
			auto cache = mem.Read<array<char, 256>>(NameChain);

			for (int i = 0; i < cache.size(); i++)
			{
				buf[i] = cache[i];
			}
			auto object_name = buf;

			if (strstr(object_name, component_name))
			{
				return Fields;
			}
		}

		return 0;
	}


	DOUBLE PI = 3.14159265358;
	DOUBLE Deg = 180 / PI;
	Vector2 CalcViewAngles(FVector LocalPlayerPos, FVector TargetPos)
	{
		FVector diff = LocalPlayerPos - TargetPos;
		Vector2 ret;
		float length = diff.GetLength();

		ret.x = -atan2f(diff.x, -diff.y) * Deg;
		ret.y = asinf(diff.z / length) * Deg;
		/*ret.y = asinf(diff.y / length);
		ret.x = -atan2f(diff.x, -diff.z);*/

		return ret;
	}
	//Camera
	namespace Camera
	{

		D3DMATRIX GetViewMatrix()
		{
			D3DXMATRIX matrix;
			ULONG64 Camera = Globals::Player::LocalPlayerAiming && Globals::OpticCamera ? Globals::OpticCamera : Globals::FPSCamera;
			ULONG64 temp = mem.ReadChain(Camera, { 0x30, 0x18 });
			D3DXMATRIX temp_matrix = mem.Read<D3DXMATRIX>(temp + 0xDC);
			D3DXMatrixTranspose(&matrix, &temp_matrix);
			Globals::ViewMatrix = matrix;
		}

		bool WorldToScreenv(FVector& point3D, D2D1_POINT_2F& point2D)
		{
			D3DXVECTOR3 _point3D = D3DXVECTOR3(point3D.x, point3D.z, point3D.y);


			auto& matrix = Globals::ViewMatrix;

			D3DXVECTOR3 translationVector = D3DXVECTOR3(matrix._41, matrix._42, matrix._43);
			D3DXVECTOR3 up = D3DXVECTOR3(matrix._21, matrix._22, matrix._23);
			D3DXVECTOR3 right = D3DXVECTOR3(matrix._11, matrix._12, matrix._13);

			float w = D3DXVec3Dot(&translationVector, &_point3D) + matrix._44;

			if (w < 0.098f)
				return false;

			float y = D3DXVec3Dot(&up, &_point3D) + matrix._24;
			float x = D3DXVec3Dot(&right, &_point3D) + matrix._14;


			/*if (Globals::Player::LocalPlayerAiming)
			{
				x *= 0.9;
				y *= 0.9;
			}*/

			point2D.x = ((1920) / 2) * (1.f + x / w) - 19;
			point2D.y = ((1080) / 2) * (1.f - y / w) - 19;


			if ((point2D.x) <= 0 || (point2D.y <= 0) || (1920 <= point2D.x) || (1080 <= point2D.y))
			{
				return false;
			}
			return true;
		}
	}

	//text
	namespace Text
	{
		wstring read_wide_str(ULONG64 nickptr)
		{
			int length = mem.Read<int>(nickptr + 0x10);

			if (length > 255)
			{
				return L"long name";
			}
			auto* buffer = malloc(length);
			std::array<WCHAR, 256> name_buffer;
			auto cache = mem.Read<array<WCHAR, 256>>(nickptr + 0x14, buffer, length * 2);
			free(buffer);
			for (int i = 0; i < cache.size(); i++)
			{
				name_buffer[i] = cache[i];
			}
			name_buffer[length] = '\0';

			const wstring namestr = name_buffer.data();

			return namestr;
		}
		string read_str(ULONG64 nickptr)
		{
			int length = mem.Read<int>(nickptr + 0x10);
			if (length > 255)
			{
				return "longname";
			}
			try
			{
				auto* buffer = malloc(length);
				std::array<char, 256> name_buffer;
				auto cache = mem.Read<array<WCHAR, 256>>(nickptr + 0x14, buffer, length * 2);
				free(buffer);
				for (int i = 0; i < cache.size(); i++)
				{
					name_buffer[i] = cache[i];
				}
				name_buffer[length] = '\0';
				const string namestr = name_buffer.data();
				return namestr;
			}
			catch (const std::exception&)
			{

			}

			return "longname";
		}
		string read_str_item(ULONG64 nickptr)
		{
			int length = 24;
			auto* buffer = malloc(length);
			std::array<char, 256> name_buffer;
			auto cache = mem.Read<array<WCHAR, 256>>(nickptr + 0x14, buffer, length * 2);
			free(buffer);
			for (int i = 0; i < cache.size(); i++)
			{
				name_buffer[i] = cache[i];
			}
			name_buffer[length] = '\0';
			const string namestr = name_buffer.data();

			return namestr;
		}

		string wstring_convert_string(wstring str)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
			return  conv.to_bytes(str);
		}
		std::string wstring_to_utf8(const std::wstring& str) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.to_bytes(str);
		}

		std::wstring utf8_to_wstring(const std::string& str) {
			std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
			return myconv.from_bytes(str);
		}


		string utf8_to_string(const std::string& str)
		{
			int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
			wchar_t* pwBuf = new wchar_t[nwLen + 1];
			memset(pwBuf, 0, nwLen * 2 + 2);
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

			int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
			char* pBuf = new char[nLen + 1];
			memset(pBuf, 0, nLen + 1);
			WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

			std::string ret = pBuf;
			delete[]pBuf;
			delete[]pwBuf;

			return ret;
		}

		string string_to_utf8(const std::string& str)
		{
			int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
			wchar_t* pwBuf = new wchar_t[nwLen + 1];
			memset(pwBuf, 0, nwLen * 2 + 2);
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

			int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
			char* pBuf = new char[nLen + 1];
			memset(pBuf, 0, nLen + 1);
			WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

			std::string ret = pBuf;
			delete[]pwBuf;
			delete[]pBuf;

			return ret;
		}

	}
	namespace Calculate {
		float CalculateDistance2D(int x1, int y1, int x2, int y2) {
			return sqrtf(powf(x2 - x1, 2.0) + powf(y2 - y1, 2.0));
		}
	}
}