#include "ImGuiExtra.hpp"
#include "Config.hpp"

#include <format>

#include "UI/Texture2DD3D11.hpp"

#include "UI/EmbeddedImages.hpp"

namespace UI {
#include "Fonts/RobotoMedium.hpp"
#include "Fonts/RussoOneRegular.hpp"

	Roboto_t g_ImGuiFont_Roboto;

	RussoOne_t g_ImGuiFont_RussoOne;

	UIContext g_UIContext;

	ImFont* g_ImGuiFont_RussoOne256 = nullptr;

	static int g_UIID{ 0 };

	void ResetID(int from) {
		g_UIID = from;
	}

	int GetUniqueINTID() {
		return g_UIID++;
	}

	void SetStyleCrimson() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		//style.DisabledAlpha = 0.600f;
		style.WindowPadding = ImVec2(6.0f, 3.0f);
		style.WindowRounding = 0.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 0.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(5.0f, 1.0f);
		style.FrameRounding = 3.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(8.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		style.CellPadding = ImVec2(4.0f, 2.0f);
		style.IndentSpacing = 21.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 13.0f;
		style.ScrollbarRounding = 16.0f;
		style.GrabMinSize = 20.0f;
		style.GrabRounding = 2.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 1.0f;
		style.TabMinWidthForCloseButton = 0.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.858f, 0.929f, 0.886f, 0.280f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.207f, 0.156f, 0.168f, queuedConfig.GUI.transparencyValue);
		style.Colors[ImGuiCol_WindowBgText] = ImVec4(0.207f, 0.156f, 0.168f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.200f, 0.219f, 0.266f, 0.899f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.537f, 0.478f, 0.254f, 0.162f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.247f, 0.247f, 0.247f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.529f, 0.182f, 0.256f, 0.749f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.788f, 0.049f, 0.205f, 0.733f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.592f, 0.047f, 0.156f, 0.650f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.592f, 0.047f, 0.156f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.200f, 0.219f, 0.266f, 0.75f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.200f, 0.219f, 0.266f, 0.0f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.772f, 0.215f, 0.215f, 0.780f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.788f, 0.062f, 0.211f, 1.0f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.886f, 0.070f, 0.235f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.855f, 0.106f, 0.325f, 1.0f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.980f, 0.278f, 0.261f, 1.0f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.199f, 0.215f, 0.266f, 0.490f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.529f, 0.184f, 0.254f, 0.858f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.788f, 0.062f, 0.211f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.690f, 0.054f, 0.184f, 0.494f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.690f, 0.054f, 0.184f, 0.654f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.690f, 0.054f, 0.184f, 1.0f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.870f, 0.109f, 0.298f, 1.0f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.870f, 0.109f, 0.298f, 1.0f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.870f, 0.109f, 0.298f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.827f, 0.214f, 0.214f, 0.039f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.827f, 0.215f, 0.215f, 0.780f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.827f, 0.215f, 0.215f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.592f, 0.047f, 0.156f, 0.862f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.803f, 0.043f, 0.203f, 0.823f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.855f, 0.106f, 0.325f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.066f, 0.101f, 0.145f, 0.972f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.423f, 0.133f, 0.133f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.858f, 0.929f, 0.886f, 0.629f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.858f, 0.929f, 0.886f, 0.629f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.776f, 0.146f, 0.130f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.188f, 0.188f, 0.200f, 1.0f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.309f, 0.309f, 0.349f, 1.0f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.227f, 0.227f, 0.247f, 1.0f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.059f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.785f, 0.105f, 0.026f, 0.430f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.899f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.976f, 0.295f, 0.258f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800f, 0.800f, 0.800f, 0.200f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800f, 0.800f, 0.800f, 0.349f);

		// Fonts
		auto& io = ImGui::GetIO();

		// Add custom ranges to the font
		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddText((const char*)u8"•");
		builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
		builder.BuildRanges(&ranges);

		for (size_t i = 0; i < g_ImGuiFont_Roboto.m_FontSizes.size(); i++) {
			g_ImGuiFont_Roboto.m_FontSizes[i] = io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium_compressed_data, RobotoMedium_compressed_size, float(i + 10), nullptr, ranges.Data);
		}

		for (size_t i = 0; i < g_ImGuiFont_RussoOne.m_FontSizes.size(); i++) {
			g_ImGuiFont_RussoOne.m_FontSizes[i] = io.Fonts->AddFontFromMemoryCompressedTTF(RussoOneRegular_compressed_data, RussoOneRegular_compressed_size, float(i + 10), nullptr, ranges.Data);
		}

		g_ImGuiFont_RussoOne256 = io.Fonts->AddFontFromMemoryCompressedTTF(RussoOneRegular_compressed_data, RussoOneRegular_compressed_size, 256.0f, nullptr, ranges.Data);

		io.FontDefault = g_ImGuiFont_Roboto[18];

		io.Fonts->Build();
	}

	bool ButtonEx(const char* label, const ImVec2& size_arg, ImU32 hover_text_color, ImGuiButtonFlags flags)
	{
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
			pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
		ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ItemSize(size, style.FramePadding.y);
		if (!ItemAdd(bb, id))
			return false;

		if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat)
			flags |= ImGuiButtonFlags_Repeat;
		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

		if (hovered) {
			PushStyleColor(ImGuiCol_Text, hover_text_color);
		}

		// Render
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		RenderNavHighlight(bb, id);
		RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

		if (hovered) {
			PopStyleColor();
		}

		// Automatically close popups
		//if (pressed && !(flags & ImGuiButtonFlags_DontClosePopups) && (window->Flags & ImGuiWindowFlags_Popup))
		//    CloseCurrentPopup();

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.LastItemStatusFlags);
		return pressed;
	}

	bool TabButton(const char* label, bool isActive, bool isEnabled, bool hasSubtab, const ImVec2& size) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

		ImGui::PushStyleColor(ImGuiCol_ButtonActive, SwapColorEndianness(0xFF6A8FFF));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, isActive ? SwapColorEndianness(0xD81B57FF) : SwapColorEndianness(0x2E2E2EFF));
		ImGui::PushStyleColor(ImGuiCol_Button, isActive ? (hasSubtab ? SwapColorEndianness(0xB3173EFF) : SwapColorEndianness(0xD81B57FF)) : 0x00000000);
		ImGui::PushStyleColor(ImGuiCol_Text, isEnabled ? 0xFFFFFFFF : SwapColorEndianness(0x625F5FFF));

		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !isEnabled);
		bool ret = ButtonEx(label, size, SwapColorEndianness(0xFFFFFFFF), ImGuiButtonFlags_None);
		ImGui::PopItemFlag();

		ImGui::PopStyleColor(4);

		ImGui::PopStyleVar(2);

		if (isActive && hasSubtab) {
			ImGui::GetItemID();

			auto window = ImGui::GetCurrentWindowRead();

			const float bCenterX = (window->DC.LastItemRect.Min.x + window->DC.LastItemRect.Max.x) * 0.5f;
			ImVec2 subTabArrowCenterPos{ bCenterX,  window->DC.LastItemRect.Max.y };

			float triSideLen = 10;
			float triHeight = std::sinf(60.0f) * triSideLen * 2.0f;

			// Sub tab arrow
			ImGui::GetWindowDrawList()->AddTriangleFilled(ImVec2{ subTabArrowCenterPos.x - triSideLen * 0.5f,
																  subTabArrowCenterPos.y + triHeight * 0.5f },
				ImVec2{ subTabArrowCenterPos.x + triSideLen * 0.5f,
						subTabArrowCenterPos.y + triHeight * 0.5f },
				ImVec2{ subTabArrowCenterPos.x,
						subTabArrowCenterPos.y - triHeight * 0.5f },
				0xFFFFFFFF);
		}

		return ret;
	}

	bool InfoButton(const char* label, const ImVec2& size) {
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);

		ImGui::PushStyleColor(ImGuiCol_ButtonActive, SwapColorEndianness(0xFF6A8FFF));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, SwapColorEndianness(0xDA1B53FF));
		ImGui::PushStyleColor(ImGuiCol_Button, 0xFFFFFFFF);
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_WindowBgText));

		bool ret = ButtonEx(label, size, SwapColorEndianness(0xFFFFFFFF), ImGuiButtonFlags_None);

		ImGui::PopStyleColor(4);

		ImGui::PopStyleVar(1);

		return ret;
	}

	// Name must be unique because it's used to generate an id for the item
	bool CloseButton(const char* name, const ImVec2 size) {
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		ImGuiID id = window->GetID(name);
		ImGuiStyle& style = ImGui::GetStyle();

		ImVec2 pos = { window->DC.CursorPos.x, window->DC.CursorPos.y };

		// We intentionally allow interaction when clipped so that a mechanical Alt,Right,Validate sequence close a window.
		// (this isn't the regular behavior of buttons, but it doesn't affect the user much because navigation tends to keep items visible).
		const ImRect bb(pos, pos + size);
		ItemSize(bb.Max - bb.Min, style.FramePadding.y);
		bool isClipped = !ItemAdd(bb, id);

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held);
		if (isClipped)
			return pressed;

		// Render
		ImU32 col = GetColorU32(held ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
		ImVec2 center = bb.GetCenter();

		if (hovered)
			window->DrawList->AddRectFilled(bb.Min, bb.Max, col, style.FrameRounding);

		ImU32 crossCol = 0xFFFFFFFF;

		ImVec2 topLeftLeft, topLeftRight, topRightRight, topRightLeft,
			bottomLeftLeft, bottomLeftRight, bottomRightRight, bottomRightLeft;

		ImVec2 innerPadding = { bb.GetWidth() * 0.1f, bb.GetHeight() * 0.1f };

		float crossLineThickness = (bb.GetHeight() - (innerPadding.y * 2.0)) * 0.2f;

		topLeftLeft = { bb.Min.x + innerPadding.x * 1.5f, bb.Min.y + innerPadding.y };
		topLeftRight = { topLeftLeft.x + crossLineThickness, topLeftLeft.y };

		topRightRight = { bb.Max.x - innerPadding.x * 1.5f, bb.Min.y + innerPadding.y };
		topRightLeft = { topRightRight.x - crossLineThickness, topRightRight.y };

		bottomLeftLeft = { bb.Min.x + innerPadding.x * 1.5f , bb.Max.y - innerPadding.y };
		bottomLeftRight = { bottomLeftLeft.x + crossLineThickness, bottomLeftLeft.y };

		bottomRightRight = { bb.Max.x - innerPadding.x * 1.5f, bb.Max.y - innerPadding.y };
		bottomRightLeft = { bottomRightRight.x - crossLineThickness, bottomRightRight.y };

		window->DrawList->AddQuadFilled(topLeftLeft, topLeftRight, bottomRightRight, bottomRightLeft, crossCol);
		window->DrawList->AddQuadFilled(topRightLeft, topRightRight, bottomLeftRight, bottomLeftLeft, crossCol);

		return pressed;
	}

	float CalcMaxPopupHeightFromItemCount(int items_count)
	{
		ImGuiContext& g = *GImGui;
		if (items_count <= 0)
			return FLT_MAX;
		return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
	}

	bool BeginCombo(const char* label, const char* preview_value, ImVec2 preview_align /*= { 0.0f, 0.0f }*/, float arrow_scale /*= 1.0f*/, ImGuiComboFlags flags /*= 0*/)
	{
		using namespace ImGui;

		// Always consume the SetNextWindowSizeConstraint() call in our early return paths
		ImGuiContext& g = *GImGui;
		bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
		g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
		const ImVec2 label_size = CalcTextSize(label, NULL, true);
		const float expected_w = CalcItemWidth();
		const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
		ItemSize(total_bb, style.FramePadding.y);
		if (!ItemAdd(total_bb, id, &frame_bb))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
		bool popup_open = IsPopupOpen(id, ImGuiPopupFlags_None);

		const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
		RenderNavHighlight(frame_bb, id);
		if (!(flags & ImGuiComboFlags_NoPreview))
			window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, frame_col, style.FrameRounding, ImDrawCornerFlags_All);
		if (!(flags & ImGuiComboFlags_NoArrowButton))
		{
			ImU32 text_col = GetColorU32(ImGuiCol_Text);

			if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
			{
				ImVec2 arrow_pos{ value_x2 + style.FramePadding.y + arrow_size * 0.5f - arrow_size * arrow_scale * 0.5f, frame_bb.Min.y + style.FramePadding.y + arrow_size * 0.5f - arrow_size * arrow_scale * 0.5f };
				RenderArrow(window->DrawList, arrow_pos, text_col, ImGuiDir_Down, arrow_scale);
			}
		}
		RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);

		auto frame_center = frame_bb.GetCenter();
		if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
			RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, preview_value, NULL, NULL, preview_align);

		if (label_size.x > 0)
			RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

		if ((pressed || g.NavActivateId == id) && !popup_open)
		{
			if (window->DC.NavLayerCurrent == 0)
				window->NavLastIds[0] = id;
			OpenPopupEx(id, ImGuiPopupFlags_None);
			popup_open = true;
		}

		if (!popup_open)
			return false;

		if (has_window_size_constraint)
		{
			g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
			g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
		}
		else
		{
			if ((flags & ImGuiComboFlags_HeightMask_) == 0)
				flags |= ImGuiComboFlags_HeightRegular;
			IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
			int popup_max_height_in_items = -1;
			if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
			else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
			else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
			SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
		}

		char name[16];
		ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

		// Position the window given a custom constraint (peak into expected window size so we can position it)
		// This might be easier to express with an hypothetical SetNextWindowPosConstraints() function.
		if (ImGuiWindow* popup_window = FindWindowByName(name))
			if (popup_window->WasActive)
			{
				// Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
				ImVec2 size_expected = CalcWindowExpectedSize(popup_window);
				if (flags & ImGuiComboFlags_PopupAlignLeft)
					popup_window->AutoPosLastDirection = ImGuiDir_Left; // "Below, Toward Left"
				else
					popup_window->AutoPosLastDirection = ImGuiDir_Down; // "Below, Toward Right (default)"
				ImRect r_outer = GetWindowAllowedExtentRect(popup_window);
				ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
				SetNextWindowPos(pos);
			}

		// We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

		// Horizontally align ourselves with the framed text
		PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
		bool ret = Begin(name, NULL, window_flags);
		PopStyleVar();
		if (!ret)
		{
			EndPopup();
			IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
			return false;
		}
		return true;
	}

	void SeparatorEx(float length /*= 0.0f*/, float align /*= 0.0f*/, ImGuiSeparatorFlags flags /*= ImGuiSeparatorFlags_Horizontal*/)
	{
		using namespace ImGui;

		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		IM_ASSERT(ImIsPowerOfTwo(flags & (ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_Vertical)));   // Check that only 1 option is selected

		float thickness_draw = 1.0f;
		float thickness_layout = 0.0f;

		ImRect content_region = window->ContentRegionRect;

		if (flags & ImGuiSeparatorFlags_Vertical)
		{
			// Vertical separator, for menu bars (use current line height). Not exposed because it is misleading and it doesn't have an effect on regular layout.
			float y1 = 0.0f;
			float y2 = 0.0f;

			if (length > 0.0f) {
				y1 = content_region.Min.y + (content_region.GetSize().y - length) * align;
				y2 = y1 + length;
			}
			else {
				y1 = window->DC.CursorPos.y;
				y2 = window->DC.CursorPos.y + window->DC.CurrLineSize.y;
			}

			const ImRect bb(ImVec2(window->DC.CursorPos.x, y1), ImVec2(window->DC.CursorPos.x + thickness_draw, y2));
			ItemSize(ImVec2(thickness_layout, 0.0f));
			if (!ItemAdd(bb, 0))
				return;

			// Draw
			window->DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Min.x, bb.Max.y), GetColorU32(ImGuiCol_Separator));
			if (g.LogEnabled)
				LogText(" |");
		}
		else if (flags & ImGuiSeparatorFlags_Horizontal)
		{
			// Horizontal Separator
			float x1 = 0.0f;
			float x2 = 0.0f;

			if (length > 0.0f) {
				x1 = content_region.Min.x + (content_region.GetSize().x - length) * align;
				x2 = x1 + length;
			}
			else {
				x1 = window->DC.CursorPos.x;
				x2 = window->Pos.x + window->Size.x;
			}

			// FIXME-WORKRECT: old hack (#205) until we decide of consistent behavior with WorkRect/Indent and Separator
			if (g.GroupStack.Size > 0 && g.GroupStack.back().WindowID == window->ID)
				x1 += window->DC.Indent.x;

			ImGuiOldColumns* columns = (flags & ImGuiSeparatorFlags_SpanAllColumns) ? window->DC.CurrentColumns : NULL;
			if (columns)
				PushColumnsBackground();

			// We don't provide our width to the layout so that it doesn't get feed back into AutoFit
			const ImRect bb(ImVec2(x1, window->DC.CursorPos.y), ImVec2(x2, window->DC.CursorPos.y + thickness_draw));
			ItemSize(ImVec2(0.0f, thickness_layout));
			const bool item_visible = ItemAdd(bb, 0);
			if (item_visible)
			{
				// Draw
				window->DrawList->AddLine(bb.Min, ImVec2(bb.Max.x, bb.Min.y), GetColorU32(ImGuiCol_Separator));
				if (g.LogEnabled)
					LogRenderedText(&bb.Min, "--------------------------------");
			}
			if (columns)
			{
				PopColumnsBackground();
				columns->LineMinY = window->DC.CursorPos.y;
			}
		}
	}

#pragma region SerpCode
	bool Selectable(const char* label, bool* selected, ImGuiSelectableFlags flags) {
		UI::PushID();
		auto update = ImGui::Selectable(label, selected, flags);
		UI::PopID();
		return update;
	}
#pragma endregion

}