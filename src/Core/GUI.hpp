#pragma once

#include "Core.hpp"
#include "../../ThirdParty/ImGui/imgui.h"
#include "../../ThirdParty/ImGui/imgui_internal.h"
#include <d3d11.h>
#include <d3d10.h>

#include "DebugSwitch.hpp"

template <typename T>
struct GetImGuiDataType
{
	static constexpr ImGuiDataType value =
		(TypeMatch<T, int8  >::value) ? ImGuiDataType_S8 :
		(TypeMatch<T, int16 >::value) ? ImGuiDataType_S16 :
		(TypeMatch<T, int32 >::value) ? ImGuiDataType_S32 :
		(TypeMatch<T, int64 >::value) ? ImGuiDataType_S64 :
		(TypeMatch<T, uint8 >::value) ? ImGuiDataType_U8 :
		(TypeMatch<T, uint16>::value) ? ImGuiDataType_U16 :
		(TypeMatch<T, uint32>::value) ? ImGuiDataType_U32 :
		(TypeMatch<T, uint64>::value) ? ImGuiDataType_U64 :
		(TypeMatch<T, float >::value) ? ImGuiDataType_Float :
		(TypeMatch<T, double>::value) ? ImGuiDataType_Double :
		(TypeMatch<T, bool  >::value) ? ImGuiDataType_U8 :
		0;
};

namespace GUI {
	extern int   id;
	extern bool  save;
	extern float saveTimer;
	extern float saveTimeout; // in ms
};

// extern bool    GUI_hide;
// extern float   GUI_hideTimeout;

enum
{
	SectionFlags_NoNewLine = 1 << 0,
};

inline void GUI_PushId()
{
	ImGui::PushID(::GUI::id);
	::GUI::id++;
}

inline void GUI_PopId()
{
	ImGui::PopID();
}

bool GUI_Button
(
	const char* label,
	const ImVec2& size = ImVec2(0, 0)
);

bool GUI_ResetButton();

bool GUI_Checkbox
(
	const char* label,
	bool& var
);

bool GUI_Checkbox2
(
	const char* label,
	bool& var,
	bool& var2
);

// @Extend
template <typename T>
bool GUI_Input
(
	const char* label,
	T& var,
	T step = 1,
	const char* format = 0,
	ImGuiInputTextFlags flags = 0
)
{
	GUI_PushId();
	auto update = ImGui::InputScalar
	(
		label,
		GetImGuiDataType<T>::value,
		&var,
		(step == 0) ? 0 : &step,
		(step == 0) ? 0 : &step,
		format,
		flags
	);
	GUI_PopId();

	if (update)
	{
		::GUI::save = true;
	}

	return update;
}

// @Extend
template <typename T>
bool GUI_Input2
(
	const char* label,
	T& var,
	T& var2,
	T step = 1,
	const char* format = 0,
	ImGuiInputTextFlags flags = 0
)
{
	auto update = GUI_Input
	(
		label,
		var2,
		step,
		format,
		flags
	);

	if (update)
	{
		var = var2;
	}

	return update;
}

template <typename T>
bool GUI_InputDefault
(
	const char* label,
	T& var,
	T& defaultVar,
	const T step = 1,
	const char* format = 0,
	ImGuiInputTextFlags flags = 0
)
{
	auto& window = *ImGui::GetCurrentWindow();
	auto& style = ImGui::GetStyle();

	auto buttonSize = ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight());

	auto itemWidth = window.DC.ItemWidth;

	if (window.DC.ItemWidthStack.Size > 0)
	{
		window.DC.ItemWidth -= (buttonSize.x + style.ItemInnerSpacing.x);
		window.DC.ItemWidthStack.back() = window.DC.ItemWidth;
	}

	GUI_PushId();
	auto update = ImGui::InputScalar
	(
		"",
		GetImGuiDataType<T>::value,
		&var,
		(step == 0) ? 0 : &step,
		(step == 0) ? 0 : &step,
		format,
		flags
	);
	GUI_PopId();

	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	if (GUI_Button("D", buttonSize))
	{
		update = true;
		var = defaultVar;
	}

	ImGui::SameLine(0, style.ItemInnerSpacing.x);
	ImGui::Text(label);

	if (window.DC.ItemWidthStack.Size > 0)
	{
		window.DC.ItemWidth = itemWidth;
		window.DC.ItemWidthStack.back() = window.DC.ItemWidth;
	}

	if (update)
	{
		::GUI::save = true;
	}

	return update;
}

template <typename T>
bool GUI_InputDefault2
(
	const char* label,
	T& var,
	T& var2,
	T& defaultVar,
	const T step = 1,
	const char* format = 0,
	ImGuiInputTextFlags flags = 0
)
{
	auto update = GUI_InputDefault
	(
		label,
		var2,
		defaultVar,
		step,
		format,
		flags
	);

	if (update)
	{
		var = var2;
	}

	return update;
}

template <typename T>
bool GUI_Slider
(
	const char* label,
	T& var,
	const T min,
	const T max
)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiIO& io = ImGui::GetIO();

	GUI_PushId();
	auto update = ImGui::SliderScalar
	(
		label,
		GetImGuiDataType<T>::value,
		&var,
		&min,
		&max,
		"%u"
	);
	if (ImGui::IsItemHovered())
	{
		window->Flags |= ImGuiWindowFlags_NoScrollbar;
		window->Flags |= ImGuiWindowFlags_NoScrollWithMouse;
		if (io.MouseWheel < 0)
		{
			if (var > min)
			{
				var--;
				update = true;
			}
		}
		else if (io.MouseWheel > 0)
		{
			if (var < max)
			{
				var++;
				update = true;
			}
		}
	}
	GUI_PopId();

	if (update)
	{
		::GUI::save = true;
	}

	return update;
}

template <typename T>
bool GUI_Slider2
(
	const char* label,
	T& var,
	T& var2,
	const T min,
	const T max
)
{
	auto update = GUI_Slider
	(
		label,
		var2,
		min,
		max
	);

	if (update)
	{
		var = var2;
	}

	return update;
}

bool GUI_Selectable
(
	const char* label,
	bool* selected,
	ImGuiSelectableFlags flags = 0
);

template
<
	typename varType,
	uint8 count
>
bool GUI_Combo
(
	const char* label,
	const char* (&names)[count],
	varType& var,
	ImGuiComboFlags flags = 0
)
{
	bool update = false;

	GUI_PushId();

	if (ImGui::BeginCombo(label, names[var], flags))
	{
		old_for_all(varType, index, count)
		{
			bool selected = (index == var) ? true : false;

			GUI_PushId();

			if (GUI_Selectable(names[index], &selected))
			{
				update = true;
				var = index;
			}

			GUI_PopId();
		}

		ImGui::EndCombo();
	}

	GUI_PopId();

	if (update)
	{
		::GUI::save = true;
	}

	return update;
}

template
<
	typename varType,
	uint8 count
>
bool GUI_Combo2
(
	const char* label,
	const char* (&names)[count],
	varType& var,
	varType& var2,
	ImGuiComboFlags flags = 0
)
{
	auto update = GUI_Combo
	(
		label,
		names,
		var2,
		flags
	);

	if (update)
	{
		var = var2;
	}

	return update;
}

template
<
	typename varType,
	uint8 mapItemCount
>
bool GUI_ComboMap
(
	const char* label,
	const char* (&names)[mapItemCount], // @Todo: Use mapItemNames.
	const varType(&map)[mapItemCount], // @Todo: Use mapItems.
	uint8& index,
	varType& var,
	ImGuiComboFlags flags = 0
)
{
	bool update = false;
	GUI_PushId();
	if (ImGui::BeginCombo(label, names[index], flags))
	{
		old_for_all(uint8, mapIndex, mapItemCount) // @Todo: mapItemIndex.
		{
			auto& mapItem = map[mapIndex];
			bool selected = (mapIndex == index) ? true : false; // @Todo: Redundant.
			// @Todo: Remove Push and Pop.
			GUI_PushId();
			if (GUI_Selectable(names[mapIndex], &selected))
			{
				update = true;
				index = mapIndex;
				var = mapItem;
			}
			GUI_PopId();
		}
		ImGui::EndCombo();
	}
	GUI_PopId();

	if (update)
	{
		::GUI::save = true;
	}

	if constexpr (debug)
	{
		ImGui::Text("value %u", var);
		ImGui::Text("index %u", index);
	}

	return update;
}

template
<
	typename varType,
	uint8 mapItemCount
>
bool GUI_ComboMap2
(
	const char* label,
	const char* (&names)[mapItemCount],
	const varType(&map)[mapItemCount],
	uint8& index,
	varType& var,
	varType& var2,
	ImGuiComboFlags flags = 0
)
{
	auto update = GUI_ComboMap
	(
		label,
		names,
		map,
		index,
		var2,
		flags
	);

	if (update)
	{
		var = var2;
	}

	return update;
}

inline void GUI_SectionStart(const char* label)
{
	ImGui::Text(label);
	ImGui::Text("");
}

inline void GUI_PushDisable(bool condition)
{
	if (condition)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
}

inline void GUI_SectionEnd(byte64 flags = 0)
{
	if (!(flags & SectionFlags_NoNewLine))
	{
		ImGui::Text("");
	}

	ImGui::Separator();
}

inline void GUI_PopDisable(bool condition)
{
	if (condition)
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}
}

bool GUI_ColorEdit4
(
	const char* label,
	uint8(&var)[4],
	float(&var2)[4],
	ImGuiColorEditFlags flags = 0
);

bool GUI_Color
(
	const char* label,
	uint8(&var)[4],
	float(&var2)[4]
);

bool GUI_Color2
(
	const char* label,
	uint8(&var)[4],
	uint8(&var2)[4],
	float(&var3)[4]
);

template<uint8 count>
bool GUI_ColorPalette
(
	const char* label,
	uint8(&vars)[count][4],
	float(&vars2)[count][4]
)
{
	bool update = false;
	auto& style = ImGui::GetStyle();

	old_for_all(uint8, index, count)
	{
		if (GUI_ColorEdit4("", vars[index], vars2[index], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaPreview))
		{
			update = true;
		}

		ImGui::SameLine(0, style.ItemInnerSpacing.x);
	}

	ImGui::Text(label);

	return update;
}

template<uint8 count>
bool GUI_ColorPalette2
(
	const char* label,
	uint8(&vars)[count][4],
	uint8(&vars2)[count][4],
	float(&vars3)[count][4]
)
{
	auto update = GUI_ColorPalette
	(
		label,
		vars2,
		vars3
	);

	if (update)
	{
		memcpy(vars, vars2, sizeof(vars2));

		if constexpr (debug)
		{
			Log("GUI_ColorPalette memcpy size %u", sizeof(vars2));
		}
	}

	return update;
}

// @Todo: Create RadioButton2.
template <typename varType>
bool GUI_RadioButton
(
	const char* label,
	varType& var,
	varType value
)
{
	bool update = false;

	GUI_PushId();

	bool selected = (var == value);

	if (ImGui::RadioButton(label, selected))
	{
		update = true;
		var = value;
	}

	GUI_PopId();

	return update;
}

bool GUI_Color
(
	const char* label,
	float(&var)[4],
	ImGuiColorEditFlags flags = 0
);

bool GUI_Color2
(
	const char* label,
	float(&var)[4],
	float(&var2)[4],
	ImGuiColorEditFlags flags = 0
);

ID3D11ShaderResourceView* CreateTexture
(
	const char* filename,
	ID3D11Device* device
);

void TooltipHelper
(
	const char* name,
	const char* description,
	float x = 2048.0f
);

void DescriptionHelper
(
	const char* description,
	float width = 500.0f
);

void CenterCursorX(float width);

void CenterText(const char* name);

bool CenterButton(const char* name);

bool ScrollbarY();

#pragma region Key Bindings

struct KeyBinding
{
	struct Data
	{
		bool run = false;

		char buffer[512] = {};
		offset_t pos = 0;

		uint32 lastKeys32 = 0;
	};


	const char* name = "";

	Data main = {};
	Data popup = {};

	bool showPopup = false;

	KeyData popupKeyData = {};

	KeyData& activeKeyData;
	KeyData& queuedKeyData;
	KeyData& defaultKeyData;

	bool executes[3] = {};
	bool executes2[256] = {};
	bool executes3[1] = {};

	byte32 flags = 0;

	typedef void(*func_t)();

	func_t func = 0;

	KeyBinding
	(
		const char* _name,
		KeyData& _activeKeyData,
		KeyData& _queuedKeyData,
		KeyData& _defaultKeyData,
		func_t    _func = 0,
		byte32    _flags = 0
	);

	void UpdateBuffer
	(
		Data& data,
		KeyData& keyData
	);

	void Main();

	void Popup();

	void UpdateKeyData(byte8* state);

	void Check(byte8* state);
};

#pragma endregion