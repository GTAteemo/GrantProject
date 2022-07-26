#pragma once
#include "Imgui/imgui.h"
#include "xorstr.hpp"
#include <WinUser.h>
#include "globals.hpp"

void DrawBackground()
{


	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.09f, 0.09f, 0.09f, 0.40f / 1.f * 2.f));
	static auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Button] = ImVec4(0.24f, 0.40f, 0.95f, 0.00f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.40f, 0.95f, 0.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 0.00f);
	ImGui::Begin((_xor_("EFT")), NULL, flags);
	{
		ImGui::SetWindowSize(ImVec2(450, GetSystemMetrics(SM_CYSCREEN)), ImGuiCond_Once);
		{
			if (GetAsyncKeyState(VK_INSERT) & 1) {

			}
			ImGui::SetCursorPos(ImVec2(GetSystemMetrics(SM_CYSCREEN) / 2 - 400, GetSystemMetrics(SM_CYSCREEN) / 2 - 150));
			ImGui::Separator();
			if (ImGui::Button(_xor_("A I M B O T"), ImVec2(-1.f, 100.f))) {
				Globals::Vars::Menu::Aimbot = !Globals::Vars::Menu::Aimbot;
				Globals::Vars::Menu::Visuals = false;
				Globals::Vars::Menu::Others = false;

			}
			ImGui::Separator();
			if (ImGui::Button(_xor_("V I S U A L S"), ImVec2(-1.f, 100.f))) {
				Globals::Vars::Menu::Visuals = !Globals::Vars::Menu::Visuals;
				Globals::Vars::Menu::Others = false;
				Globals::Vars::Menu::Aimbot = false;
			}
			ImGui::Separator();
			if (ImGui::Button(_xor_("M E M O R Y"), ImVec2(-1.f, 100.f))) {
				Globals::Vars::Menu::Others = !Globals::Vars::Menu::Others;
				Globals::Vars::Menu::Visuals = false;
				Globals::Vars::Menu::Aimbot = false;
			}
			ImGui::Separator();

		}
	}
	ImGui::End();
}

void DrawMenu()
{
	static auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
	//	ImGui::SetNextWindowPos(ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2));
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1, 0.640, 0.120, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1, 0.640, 0.120, 1.00f);
	if (Globals::Vars::Menu::Aimbot) {
		ImGui::SetNextWindowPos(ImVec2(450, 0));
		ImGui::Begin((_xor_("Aimbot")), NULL, flags);
		{
			ImGui::SetWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN) - 450, GetSystemMetrics(SM_CYSCREEN)), ImGuiCond_Once);
			{
				ImGui::SetCursorPos(ImVec2((GetSystemMetrics(SM_CXSCREEN) - 450) - 200, 50));
				ImGui::Button(_xor_("A I M B O T"));
				//ImGui::Checkbox(_xor_("Aimbot Enabled"), &Globals::Vars::Aimbot);
				ImGui::SliderInt(_xor_("FOV"), &Globals::Vars::AimbotFov, 1, 800, _xor_("%d"));
			}
		}
		ImGui::End();
	}


	if (Globals::Vars::Menu::Visuals) {
		ImGui::SetNextWindowPos(ImVec2(450, 0));
		ImGui::Begin((_xor_("Visuals")), NULL, flags);
		{
			ImGui::SetWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN) - 450, GetSystemMetrics(SM_CYSCREEN)), ImGuiCond_Once);
			{
				ImGui::SetCursorPos(ImVec2((GetSystemMetrics(SM_CXSCREEN) - 450) - 200, 50));
				ImGui::Button(_xor_("V I S U A L S"));
				ImGui::Checkbox(_xor_("Player ESP Enabled"), &Globals::Vars::PlayerEsp);
				ImGui::SliderFloat(_xor_("Max Distance"), &Globals::Vars::PlayerDistance, 0, 1000, "%0.0f", 1.f);

				ImGui::Checkbox(_xor_("Expensive Loot"), &Globals::Vars::ExpensiveLoot);
				ImGui::Checkbox(_xor_("Food Loot"), &Globals::Vars::Foods);
				ImGui::Checkbox(_xor_("All Loot"), &Globals::Vars::Others);
				ImGui::SliderFloat(_xor_("Loot Max Distance"), &Globals::Vars::ItemDistance, 0, 1000, "%0.0f", 1.f);
				ImGui::Checkbox(_xor_("DrawMinimap"), &Globals::Vars::DrawMinimap);
			}
		}
		ImGui::End();
	}


	if (Globals::Vars::Menu::Others) {
		ImGui::SetNextWindowPos(ImVec2(450, 0));
		ImGui::Begin((_xor_("Otbers")), NULL, flags);
		{
			ImGui::SetWindowSize(ImVec2(GetSystemMetrics(SM_CXSCREEN) - 450, GetSystemMetrics(SM_CYSCREEN)), ImGuiCond_Once);
			{
				ImGui::SetCursorPos(ImVec2((GetSystemMetrics(SM_CXSCREEN) - 450) - 200, 50));
				ImGui::Button(_xor_("M E M O R Y"));
			}
		}
		ImGui::End();
	}
}