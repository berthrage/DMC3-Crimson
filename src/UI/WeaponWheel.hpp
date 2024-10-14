#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>

#include "BatchedSprites.hpp"
#include "GenericAnimation.hpp"

namespace WW
{
	enum class WheelThemes
	{
		Neutral = 0,
		Dante,
		Vergil,

		Size
	};

	enum class WeaponIDs
	{
		RebellionDormant = 0,
		RebellionAwakened,
		ForceEdge,
		Cerberus,
		AgniRudra,
		Nevan,
		Beowulf,
		Yamato,
		ForceEdgeYamato,

		EbonyIvory,
		Shotgun,
		Artemis,
		Spiral,
		KalinaAnn,

		Size
	};

	class WeaponWheel
	{
	public:
		WeaponWheel(ID3D11Device* pD3D11Device, ID3D11DeviceContext* pD3D11DeviceContext, UINT width, UINT height,
			std::vector<WeaponIDs> weapons, WheelThemes themeID = WheelThemes::Neutral);
		virtual ~WeaponWheel();

		void SetWeapons(std::vector<WeaponIDs> weapons);
		void SetWheelTheme(WheelThemes themeID);
		void SetActiveSlot(size_t slot);

		void OnUpdate(double ts);
		bool OnDraw();

		ID3D11ShaderResourceView* GetSRV();

		const std::string& GetLastRenderingErrorMsg();
		HRESULT GetLastRenderingErrorCode();

	private:
		void InitializeAnimations();

		void SetInactiveOpacity(float opacity);
		void SetActiveWeaponOpacity(float opacity);

		void UpdateSlotStates();

	private:
		UINT m_Width{ 0 }, m_Height{ 0 }, m_Top{ 0 }, m_Left{ 0 };

		size_t m_CurrentActiveSlot{ 0 };
		std::vector<WeaponIDs> m_Weapons{};
		std::vector<size_t> m_PanelIDs{};

		WheelThemes m_ThemeID{ WheelThemes::Neutral };

		ID3D11Device* m_pD3D11Device;
		ID3D11DeviceContext* m_pD3D11DeviceContext;

		std::unique_ptr<Graphics::BatchedSprites> m_pSpriteBatch;

		std::unique_ptr<GenericAnimation> m_pWheelFadeAnimation, m_pActiveWeaponFadeAnimation, m_pWeaponSwitchBrightnessAnimation,
			m_pWeaponSwitchScaleAnimation;

		bool m_RunWheelFadeAnim{ false }, m_RunActiveWeaponFadeAnim{ false }, m_RunWeaponSwitchBrightnessAnim{ false },
			m_RunWeaponSwitchScaleAnim{ false };

		double m_SinceLatestChangeMs{ 0.0f };

		bool m_AlreadyTriggeredWheelFadeAnim{ false },
			m_AlreadyTriggeredActiveWeaponFadeAnim{ false },
			m_AlreadyTriggeredSwitchBrightnessAnim{ false },
			m_AlreadyTriggeredSwitchScaleAnim{ false };

		static constexpr double s_FadeDelay{ 583.3 };

		static constexpr double s_WheelFadeoutDur{ 133.3 }, s_ActiveWeaponFadeoutDur{ 433.3 }, s_SwitchBrightnessAnimDur{ 215.8 },
			s_SwitchScaleAnimDur{ 166.6 };
	};
}