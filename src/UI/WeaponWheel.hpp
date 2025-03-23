#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>

#include "BatchedSprites.hpp"
#include "GenericAnimation.hpp"

struct Transform {
	glm::vec3 Translation;
	glm::vec3 Rotation;
	glm::vec3 Scale;
};


namespace WW
{
	constexpr size_t CHARACTER_COUNT = 3;
	enum class TextureID 
	{
		ColoredDantePanel1Active = 0,
		ColoredDantePanel2Active,
		ColoredDantePanel3Active,
		ColoredDantePanel4Active,
		ColoredDantePanel5Active,

		ColoredDantePanel1Inactive,
		ColoredDantePanel2Inactive,
		ColoredDantePanel3Inactive,
		ColoredDantePanel4Inactive,
		ColoredDantePanel5Inactive,

		ColoredDanteArrow1,
		ColoredDanteArrow2,
		ColoredDanteArrow3,
		ColoredDanteArrow4,
		ColoredDanteArrow5,
		ColoredDanteCenter,

		ColoredVergilPanel1Active,
		ColoredVergilPanel2Active,
		ColoredVergilPanel3Active,
		ColoredVergilPanel4Active,
		ColoredVergilPanel5Active,

		ColoredVergilPanel1Inactive,
		ColoredVergilPanel2Inactive,
		ColoredVergilPanel3Inactive,
		ColoredVergilPanel4Inactive,
		ColoredVergilPanel5Inactive,

		ColoredVergilArrow1,
		ColoredVergilArrow2,
		ColoredVergilArrow3,
		ColoredVergilArrow4,
		ColoredVergilArrow5,
		ColoredVergilCenter,

		ColorlessPanel1Active,
		ColorlessPanel2Active,
		ColorlessPanel3Active,
		ColorlessPanel4Active,
		ColorlessPanel5Active,

		ColorlessPanel1Inactive,
		ColorlessPanel2Inactive,
		ColorlessPanel3Inactive,
		ColorlessPanel4Inactive,
		ColorlessPanel5Inactive,

		ColorlessArrow1,
		ColorlessArrow2,
		ColorlessArrow3,
		ColorlessArrow4,
		ColorlessArrow5,
		ColorlessCenter,

		Char1Slot1Inactive,
		Char1Slot1Active,
		Char1Slot1Duplicate,

		Char1Slot2Inactive,
		Char1Slot2Active,
		Char1Slot2Duplicate,

		Char1Slot3Inactive,
		Char1Slot3Active,
		Char1Slot3Duplicate,

		Char1Slot4Inactive,
		Char1Slot4Active,
		Char1Slot4Duplicate,

		Char1Slot5Inactive,
		Char1Slot5Active,
		Char1Slot5Duplicate,

		Char2Slot1Inactive,
		Char2Slot1Active,
		Char2Slot1Duplicate,

		Char2Slot2Inactive,
		Char2Slot2Active,
		Char2Slot2Duplicate,

		Char2Slot3Inactive,
		Char2Slot3Active,
		Char2Slot3Duplicate,

		Char2Slot4Inactive,
		Char2Slot4Active,
		Char2Slot4Duplicate,

		Char2Slot5Inactive,
		Char2Slot5Active,
		Char2Slot5Duplicate,

		Char3Slot1Inactive,
		Char3Slot1Active,
		Char3Slot1Duplicate,

		Char3Slot2Inactive,
		Char3Slot2Active,
		Char3Slot2Duplicate,

		Char3Slot3Inactive,
		Char3Slot3Active,
		Char3Slot3Duplicate,

		Char3Slot4Inactive,
		Char3Slot4Active,
		Char3Slot4Duplicate,

		Char3Slot5Inactive,
		Char3Slot5Active,
		Char3Slot5Duplicate,

		Size
	};

	enum class WeaponTextureID 
	{
		RebellionAwakenedActive,
		RebellionDormantActive,
		CerberusActive,
		AgniRudraActive,
		NevanActive,
		BeowulfActive,
		ForceEdgeActive,

		EbonyIvoryActive,
		ShotgunActive,
		ArtemisActive,
		SpiralActive,
		KalinaActive,

		YamatoActive,
		YamatoForceEdgeActive,

		DuplicateRebellionAwakenedActive,
		DuplicateRebellionDormantActive,
		DuplicateCerberusActive,
		DuplicateAgniRudraActive,
		DuplicateNevanActive,
		DuplicateBeowulfActive,
		DuplicateForceEdgeActive,

		DuplicateEbonyIvoryActive,
		DuplicateShotgunActive,
		DuplicateArtemisActive,
		DuplicateSpiralActive,
		DuplicateKalinaActive,

		DuplicateYamatoActive,
		DuplicateYamatoForceEdgeActive,

		RebellionAwakenedInactive,
		RebellionDormantInactive,
		CerberusInactive,
		AgniRudraInactive,
		NevanInactive,
		BeowulfInactive,
		ForceEdgeInactive,

		EbonyIvoryInactive,
		ShotgunInactive,
		ArtemisInactive,
		SpiralInactive,
		KalinaInactive,

		YamatoInactive,
		YamatoForceEdgeInactive, 

		Size
	};

	enum class WeaponState {
		Inactive,
		Active,
		Duplicate,

		Size
	};

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
			std::vector<WeaponIDs> weapons[CHARACTER_COUNT], WheelThemes themeID = WheelThemes::Neutral, bool buttonHeld = false);
		virtual ~WeaponWheel();

		void ReloadWheel(std::vector<WeaponIDs> weapons[CHARACTER_COUNT]);
		void UpdateWeapons(std::vector<WeaponIDs> weapons[CHARACTER_COUNT]);
		void UpdateCharIndex(size_t newCharIdx);
		void SetWheelTheme(WheelThemes themeID);
		void SetActiveSlot(size_t slot);

		void OnUpdate(double ts, double tsHeldReset, double tsHeldArrow);
		void TrackButtonHeldState(bool buttonHeld);
		void TrackAlwaysShowState(bool alwaysShow);
		bool OnDraw();

		ID3D11ShaderResourceView* GetSRV();

		const std::string& GetLastRenderingErrorMsg();
		HRESULT GetLastRenderingErrorCode();

	private:
		void InitializeAnimations();
		void SetWeaponsTranslations();

		void SetInactiveOpacity(float opacity);
		void SetActiveWeaponOpacity(float opacity);

		void UpdateSlotStates();

	public:
		UINT m_Width{ 0 }, m_Height{ 0 }, m_Top{ 0 }, m_Left{ 0 };

		size_t m_CurrentActiveSlot{ 0 };
		size_t m_CurrentActiveCharIndex{ 0 };
		std::vector<WeaponIDs> m_Weapons[CHARACTER_COUNT] = {};
		std::vector<size_t> m_PanelIDs{};

		std::vector<Graphics::Sprite> m_Sprites;
		std::vector<Graphics::Sprite> m_WeaponSprites;

		WheelThemes m_ThemeID{ WheelThemes::Neutral };

		ID3D11Device* m_pD3D11Device;
		ID3D11DeviceContext* m_pD3D11DeviceContext;

		std::unique_ptr<Graphics::BatchedSprites> m_pSpriteBatch;

		std::unique_ptr<GenericAnimation> m_pWheelFadeAnimation, m_pArrowFadeAnimation, m_pArrowBrightnessAnim, m_pActiveWeaponFadeAnimation, m_pWeaponSwitchBrightnessAnimation,
			m_pWeaponSwitchScaleAnimation;

		bool m_buttonHeld = false;
		bool m_alwaysShow{ false };

		double m_SinceLatestChangeMs{ 0.0f };
		double m_SinceLatestChangeHeldResetMs{ 0.0f };
		double m_SinceLatestChangeHeldArrowMs{ 0.0f };

		static constexpr double s_FadeDelay{ 583.3 };

		static constexpr double s_WheelFadeoutDur{ 133.3 }, s_ActiveWeaponFadeoutDur{ 433.3 }, s_SwitchBrightnessAnimDur{ 215.8 },
			s_SwitchScaleAnimDur{ 166.6 }, s_ArrowFadeAnimDur{ 300.0 };

		static constexpr Transform s_MeleeInactiveSlotTransforms[5] =
		{
			{ // Slot 1
				glm::vec3(-0.142f, 0.416f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(55.120f)),
				glm::vec3(0.244f)
			},
			{ // Slot 2
				glm::vec3(0.416f, 0.287f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(-12.919f)),
				glm::vec3(0.227f)
			},
			{ // Slot 3
				glm::vec3(0.384f, -0.330f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(27.560f)),
				glm::vec3(0.178f)
			},
			{ // Slot 4
				glm::vec3(-0.086f, -0.504f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(80.366f)),
				glm::vec3(0.217f)
			},
			{ // Slot 5
				glm::vec3(-0.524f, -0.016f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
				glm::vec3(0.214f)
			}
		};

		static constexpr Transform s_RangedActiveSlotTransforms[5] =
		{
			{ // Slot 1
				glm::vec3(-0.278f, 0.486f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f)),
				glm::vec3(0.446f)
			},
			{ // Slot 2
				glm::vec3(0.522f, 0.293f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(-11.186f)),
				glm::vec3(0.420f)
			},
			{ // Slot 3
				glm::vec3(0.472f, -0.344f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.390f)),
				glm::vec3(0.388f)
			},
			{ // Slot 4
				glm::vec3(-0.122f, -0.540f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(78.486f)),
				glm::vec3(0.391f)
			},
			{ // Slot 5
				glm::vec3(-0.594f, 0.012f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
				glm::vec3(0.359f)
			}
		};

		static constexpr Transform s_MeleeActiveSlotSwitchVersTransforms[5] =
		{
			{ // Slot 1
				glm::vec3(-0.278f, 0.546f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f)),
				glm::vec3(0.446f)
			},
			{ // Slot 2
				glm::vec3(0.522f, 0.448f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(-11.186f)),
				glm::vec3(0.420f)
			},
			{ // Slot 3
				glm::vec3(0.472f, -0.424f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.390f)),
				glm::vec3(0.388f)
			},
			{ // Slot 4
				glm::vec3(-0.122f, -0.540f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(78.486f)),
				glm::vec3(0.391f)
			},
			{ // Slot 5
				glm::vec3(-0.594f, 0.012f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
				glm::vec3(0.359f)
			}
		};

		static constexpr Transform s_MeleeActiveSlotTransforms[5] =
		{
			{ // Slot 1
				glm::vec3(-0.142f, 0.416f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f)),
				glm::vec3(0.446f)
			},
			{ // Slot 2
				glm::vec3(0.416f, 0.287f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(-11.186f)),
				glm::vec3(0.420f)
			},
			{ // Slot 3
				glm::vec3(0.384f, -0.330f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.390f)),
				glm::vec3(0.388f)
			},
			{ // Slot 4
				glm::vec3(-0.086f, -0.504f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(78.486f)),
				glm::vec3(0.391f)
			},
			{ // Slot 5
				glm::vec3(-0.524f, -0.016f, 0.0f),
				glm::vec3(0.0f, 0.0f, glm::radians(23.254f)),
				glm::vec3(0.359f)
			}
		};

		static constexpr glm::vec3 s_WeaponInactiveSwitchVersRotations[(size_t)WeaponIDs::Size]{
			{ // RebellionDormant
				glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
			},
			{ // RebellionAwakened
				glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
			},
			{ // ForceEdge
				glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
			},
			{ // Cerberus
				glm::vec3(0.0f, 0.0f, glm::radians(-12.919f))
			},
			{ // AgniRudra
				glm::vec3(0.0f, 0.0f, glm::radians(27.560f))
			},
			{ // Nevan
				glm::vec3(0.0f, 0.0f, glm::radians(80.366f))
			},
			{ // Beowulf
				glm::vec3(0.0f, 0.0f, glm::radians(23.254f))
			},
			{ // Yamato
				glm::vec3(0.0f, 0.0f, glm::radians(55.120f))
			},
			{ // ForceEdgeYamato
				glm::vec3(0.0f, 0.0f, glm::radians(27.560f))
			},

			{ // EbonyIvory
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Shotgun
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Artemis
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Spiral
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // KalinaAnn
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
		};

		static constexpr glm::vec3 s_WeaponActiveSwitchVersRotations[(size_t)WeaponIDs::Size]{
			{ // RebellionDormant
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
			},
			{ // RebellionAwakened
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
			},
			{ // ForceEdge
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
			},
			{ // Cerberus
				glm::vec3(0.0f, 0.0f, glm::radians(-11.186f))
			},
			{ // AgniRudra
				glm::vec3(0.0f, 0.0f, glm::radians(23.390f))
			},
			{ // Nevan
				glm::vec3(0.0f, 0.0f, glm::radians(78.486f))
			},
			{ // Beowulf
				glm::vec3(0.0f, 0.0f, glm::radians(23.254f))
			},
			{ // Yamato
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
			},
			{ // ForceEdgeYamato
				glm::vec3(0.0f, 0.0f, glm::radians(23.390f))
			},

			{ // EbonyIvory
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Shotgun
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Artemis
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Spiral
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // KalinaAnn
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
		};

		static constexpr glm::vec3 s_WeaponNormalRotations[(size_t)WeaponIDs::Size]{
			{ // RebellionDormant
				glm::vec3(0.0f, 0.0f, glm::radians(-22.572f))
			},
			{ // RebellionAwakened
				glm::vec3(0.0f, 0.0f, glm::radians(-22.572f))
			},
			{ // ForceEdge
				glm::vec3(0.0f, 0.0f, glm::radians(-22.572f))
			},
			{ // Cerberus
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // AgniRudra
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Nevan
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Beowulf
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Yamato
				glm::vec3(0.0f, 0.0f, glm::radians(49.831f))
			},
			{ // ForceEdgeYamato
				glm::vec3(0.0f, 0.0f, glm::radians(23.390f))
			},

			{ // EbonyIvory
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Shotgun
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Artemis
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // Spiral
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
			{ // KalinaAnn
				glm::vec3(0.0f, 0.0f, glm::radians(0.0f))
			},
		};

		static constexpr glm::vec3 s_WeaponNormalInactiveScales[(size_t)WeaponIDs::Size]{
			{ // RebellionDormant
				glm::vec3(0.244f)
			},
			{ // RebellionAwakened
				glm::vec3(0.244f)
			},
			{ // ForceEdge
				glm::vec3(0.244f)
			},
			{ // Cerberus
				glm::vec3(0.227f)
			},
			{ // AgniRudra
				glm::vec3(0.178f)
			},
			{ // Nevan
				glm::vec3(0.217f)
			},
			{ // Beowulf
				glm::vec3(0.214f)
			},
			{ // Yamato
				glm::vec3(0.244f)
			},
			{ // ForceEdgeYamato
				glm::vec3(0.178f)
			},

			{ // EbonyIvory
				glm::vec3(0.214f)
			},
			{ // Shotgun
				glm::vec3(0.227f)
			},
			{ // Artemis
				glm::vec3(0.227f)
			},
			{ // Spiral
				glm::vec3(0.217f)
			},
			{ // KalinaAnn
				glm::vec3(0.214f)
			},
		};

		static constexpr glm::vec3 s_WeaponNormalActiveScales[(size_t)WeaponIDs::Size]{
			{ // RebellionDormant
				glm::vec3(0.446f)
			},
			{ // RebellionAwakened
				glm::vec3(0.446f)
			},
			{ // ForceEdge
				glm::vec3(0.446f)
			},
			{ // Cerberus
				glm::vec3(0.420f)
			},
			{ // AgniRudra
				glm::vec3(0.388f)
			},
			{ // Nevan
				glm::vec3(0.391f)
			},
			{ // Beowulf
				glm::vec3(0.359f)
			},
			{ // Yamato
				glm::vec3(0.446f)
			},
			{ // ForceEdgeYamato
				glm::vec3(0.388f)
			},

			{ // EbonyIvory
				glm::vec3(0.391f)
			},
			{ // Shotgun
				glm::vec3(0.420f)
			},
			{ // Artemis
				glm::vec3(0.420f)
			},
			{ // Spiral
				glm::vec3(0.391f)
			},
			{ // KalinaAnn
				glm::vec3(0.359f)
			},
		};

	};
}