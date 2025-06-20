﻿#pragma once
#include "../ThirdParty/ImGui/imgui.h"
#include "../ThirdParty/ImGui/imgui_internal.h"
#include "WebAPICalls.hpp"
#include <vector>
#include <array>
#include <string>
#include <iostream>

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }

namespace UI {
	constexpr ImU32 SwapColorEndianness(ImU32 col) {
		ImU8 r = (ImU8)((col & 0xFF000000) >> 24);
		ImU8 g = (ImU8)((col & 0x00FF0000) >> 16);
		ImU8 b = (ImU8)((col & 0x0000FF00) >> 8);
		ImU8 a = (ImU8)(col & 0x000000FF);

		ImU32 res = 0;

		res |= ImU32(r);
		res |= ImU32(g) << 8;
		res |= ImU32(b) << 16;
		res |= ImU32(a) << 24;

		return res;
	}

	inline ImVec4 ColorToImVec4(uint32_t color) {
		float r = ((color >> 24) & 0xFF) / 255.0f;
		float g = ((color >> 16) & 0xFF) / 255.0f;
		float b = ((color >> 8) & 0xFF) / 255.0f;
		float a = (color & 0xFF) / 255.0f;
		return ImVec4(r, g, b, a);
	}

	class Roboto_t {
	public:
		auto& operator[](size_t index)
		{
			assert(index >= 10);
			assert(index <= m_FontSizes.size() + 10);

			return m_FontSizes[index - 10];
		}

	protected:
		// Starts from soize 10 to 50
		std::array<ImFont*, 40> m_FontSizes;

		friend void SetStyleCrimson();
	};

	class RussoOne_t {
	public:
		auto& operator[](size_t index)
		{
			assert(index >= 10);
			assert(index <= m_FontSizes.size() + 10);

			return m_FontSizes[index - 10];
		}

	protected:
		// Starts from soize 10 to 50
		std::array<ImFont*, 40> m_FontSizes;

		friend void SetStyleCrimson();
	};

	extern Roboto_t g_ImGuiFont_Roboto;

	extern RussoOne_t g_ImGuiFont_RussoOne;

	extern RussoOne_t g_ImGuiFont_RedOrbRusso;

	extern RussoOne_t g_ImGuiFont_RedOrbRussoBackdrop;

	extern ImFont* g_ImGuiFont_RussoOne256;

	struct UIContext {
		enum class MainTabs {
			GameMode = 0,
			Player,
			Quickplay,
			MusicSwitcher,
			Options,
			CheatsAndDebug,

			Size,
			None,
		} SelectedTab{ 0 };

		enum class OptionsSubTabs {
			Gameplay = 0,
			Camera,
			Hotkeys,
			Interface,
			Sound,
			Visual,
			System,

			Size,
			None,
		} SelectedOptionsSubTab{ 0 };

		enum class CheatsAndDebugSubTabs {
			CommonCheats = 0,
			CharacterCheats,
			Teleporter,
			EnemySpawner,
			FMODJukebox,

			Size,
			None
		} SelectedCheatsAndDebugSubTab{ 0 };

		enum class GameModes {
			Vanilla = 0,
			StyleSwitcher,
			Crimson,
			Custom,

			Size,
			None
		} SelectedGameMode{ 0 };

		bool NewVersionAvailable = false;

		struct {
			uint32_t Major;
			uint32_t Minor;
			char	 PatchLetter;
		} LatestVersion;

		struct {
			uint32_t Major = 0;
			uint32_t Minor = 3;
			char	 PatchLetter = 'b';
		} CurrentVersion;

		struct {
			uint32_t Day = 20;
			uint32_t Month = 5;
			uint32_t Year = 2025;
		} LatestUpdateDate;

		std::string LatestVersionURL{};

		size_t DefaultFontSize = 18.0f;

		std::string TierNames[(size_t)PatreonTiers_t::Size]{};

		std::vector<std::string> PatronsDT{
			"Loading..."
		};

		std::vector<std::string> PatronsSDT{
			"Loading..."
		};

		std::vector<std::string> PatronsLDK{
			"Loading..."
		};

		std::vector<const char*> SpecialThanksNames{
			"serpentiem",
			"Bibic<3",
			"Zuzu",
			"Sammy",
			"Bini",
			"Johnny",
			"Matt",
		};
	};

	extern UIContext g_UIContext;

	void ResetID(int from = 0);

	int GetUniqueINTID();

	inline void PushID() {
		ImGui::PushID(GetUniqueINTID());
	}

	inline void PopID() {
		ImGui::PopID();
	}

	void SetStyleCrimson();

	bool ButtonEx(const char* label, const ImVec2& size_arg, ImU32 hover_text_color, ImGuiButtonFlags flags);

	bool TabButton(const char* label, bool isActive, bool isEnabled, bool hasSubtab, const ImVec2& size = ImVec2(0, 0));

	bool InfoButton(const char* label, const ImVec2& size = ImVec2(0, 0));

	// Name must be unique because it's used to generate an id for the item
	bool CloseButton(const char* name, const ImVec2 size);

	float CalcMaxPopupHeightFromItemCount(int items_count);

	bool BeginCombo(const char* label, const char* preview_value, ImVec2 preview_align = { 0.0f, 0.0f }, float arrow_scale = 1.0f, ImGuiComboFlags flags = 0);

	void SeparatorEx(float length = 0.0f, float align = 0.0f, ImGuiSeparatorFlags flags = ImGuiSeparatorFlags_Horizontal);

#pragma region SerpCode
	bool Selectable(const char* label, bool* selected, ImGuiSelectableFlags flags = 0);

	template <typename varType, uint8_t count>
	bool Combo(const char* label, const char* (&names)[count], varType& var, ImGuiComboFlags flags = 0) {
		bool update = false;

		PushID();

		if (BeginCombo(label, names[var], ImVec2{ 0.0f, 0.0f }, 0.6f, flags)) {
			for (varType i = 0; i < count; i++) {
				bool selected = (i == var) ? true : false;

				PushID();

				if (Selectable(names[i], &selected)) {
					update = true;
					var = i;
				}

				PopID();
			}

			ImGui::EndCombo();
		}

		PopID();

		if (update) {
			::GUI::save = true;
		}

		return update;
	}


	template <typename varType, uint8_t count>
	bool Combo2(const char* label, const char* (&names)[count], varType& var, varType& var2, ImGuiComboFlags flags = 0) {
		auto update = Combo(label, names, var2, flags);

		if (update) {
			var = var2;
		}

		return update;
	}

	template <typename varType>
	bool ComboVectorString(const char* label, std::vector<std::string>(&names), varType& var, ImGuiComboFlags flags = 0) {
		bool update = false;

		std::vector<const char*> namescStr;

		for (const auto& name : names) {
			namescStr.push_back(name.c_str());
		}

		PushID();


		if (BeginCombo(label, var.c_str(), ImVec2{0.0f, 0.0f}, 0.6f, flags)) {
			for (int i = 0; i < names.size(); i++) {
				bool selected = (strcmp(namescStr[i], var.c_str()) == 0) ? true : false;

				PushID();

				if (Selectable(namescStr[i], &selected)) {
					update = true;
					var = namescStr[i];
				}

				PopID();
			}

			ImGui::EndCombo();
		}
		

		PopID();

		if (update) {
			::GUI::save = true;
		}

		return update;
	}

	template <typename varType>
	bool ComboVectorString2(const char* label, std::vector<std::string>(&names), varType& var, varType& var2, ImGuiComboFlags flags = 0) {
		auto update = ComboVectorString(label, names, var2, flags);

		if (update) {
			var = var2;
		}

		return update;
	}

	template <typename varType, uint8_t mapItemCount>
	bool ComboMap(const char* label,
		const char* (&names)[mapItemCount], // @Todo: Use mapItemNames.
		const varType(&map)[mapItemCount], // @Todo: Use mapItems.
		uint8_t& index, varType& var, ImGuiComboFlags flags = 0) {
		bool update = false;
		PushID();
		if (BeginCombo(label, names[index], ImVec2{ 0.0f, 0.0f }, 0.6f, flags)) {
			for (uint8_t mapIndex = 0; mapIndex < mapItemCount; mapIndex++) // @Todo: mapItemIndex.
			{
				auto& mapItem = map[mapIndex];
				bool selected = (mapIndex == index) ? true : false; // @Todo: Redundant.
				// @Todo: Remove Push and Pop.
				PushID();
				if (Selectable(names[mapIndex], &selected)) {
					update = true;
					index = mapIndex;
					var = mapItem;
				}
				PopID();
			}
			ImGui::EndCombo();
		}
		PopID();

		if (update) {
			::GUI::save = true;
		}

		//     if constexpr (debug) {
		//         ImGui::Text("value %u", var);
		//         ImGui::Text("index %u", index);
		//     }

		return update;
	}

	template <typename varType, uint8_t mapItemCount>
	bool ComboMap2(const char* label, const char* (&names)[mapItemCount], const varType(&map)[mapItemCount], uint8_t& index, varType& var,
		varType& var2, ImGuiComboFlags flags = 0) {
		auto update = ComboMap(label, names, map, index, var2, flags);

		if (update) {
			var = var2;
		}

		return update;
	}

	template <typename varType, uint8_t mapItemCount>
	bool ComboMapValue(const char* label,
		const char* (&names)[mapItemCount],
		const varType(&map)[mapItemCount],
		varType& var,
		ImGuiComboFlags flags = 0) {
		using namespace ImGui;
		bool update = false;

		int currentIndex = -1;
		for (uint8_t i = 0; i < mapItemCount; ++i) {
			if (map[i] == var) {
				currentIndex = i;
				break;
			}
		}

		const char* currentLabel = (currentIndex >= 0) ? names[currentIndex] : "Unknown";

		PushID();

		if (BeginCombo(label, currentLabel, ImVec2{ 0.0f, 0.0f }, 0.6f, flags)) {
			for (uint8_t i = 0; i < mapItemCount; ++i) {
				bool selected = (map[i] == var);

				PushID();
				if (Selectable(names[i], &selected)) {
					var = map[i];
					update = true;
				}
				PopID();
			}
			EndCombo();		}

		PopID();

		if (update) {
			::GUI::save = true;
		}

		return update;
	}

	template <typename varType, uint8_t mapItemCount>
	bool ComboMapValue2(const char* label,
		const char* (&names)[mapItemCount],
		const varType(&map)[mapItemCount],
		varType& var,
		varType& var2,
		ImGuiComboFlags flags = 0) {
		auto update = ComboMapValue(label, names, map, var2, flags);

		if (update) {
			var = var2;
		}

		return update;
	}

	template <typename varType>
	bool ComboMapVector(const char* label,
		const std::vector<std::string>& names,
		const std::vector<varType>& map,
		varType& var,
		ImGuiComboFlags flags = 0) {
		using namespace ImGui;
		bool update = false;

		std::vector<const char*> namesCStr;
		for (const auto& name : names) {
			namesCStr.push_back(name.c_str());
		}

		PushID();

		int selectedIndex = -1;
		for (int i = 0; i < map.size(); ++i) {
			if (map[i] == var) {
				selectedIndex = i;
				break;
			}
		}

		const char* previewValue = (selectedIndex >= 0 && selectedIndex < namesCStr.size()) ? namesCStr[selectedIndex] : "Unknown";

		if (BeginCombo(label, previewValue, ImVec2{ 0.0f, 0.0f }, 0.6f, flags)) {
			for (int i = 0; i < namesCStr.size(); i++) {
				bool isSelected = (var == map[i]);

				PushID();

				if (ImGui::Selectable(namesCStr[i], isSelected)) {
					var = map[i];
					update = true;
				}

				if (isSelected) {
					SetItemDefaultFocus();
				}

				PopID();
			}

			EndCombo();
		}

		PopID();

		if (update) {
			::GUI::save = true;
		}

		return update;
	}

	template <typename varType>
	bool ComboMapVector2(const char* label,
		const std::vector<std::string>& names,
		const std::vector<varType>& map,
		varType& var,
		varType& var2,
		ImGuiComboFlags flags = 0) {
		auto update = ComboMapVector(label, names, map, var2, flags);

		if (update) {
			var = var2;
		}

		return update;
	}





#pragma endregion
}
