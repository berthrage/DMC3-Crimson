#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>

#include "BatchedSprites.hpp"
#include "GenericAnimation.hpp"
#include "Texture2DArrayD3D11.hpp"

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
        WeaponWheel(
            ID3D11Device* pD3D11Device,
            ID3D11DeviceContext* pD3D11DeviceContext,
            UINT width,
            UINT height,
            std::vector<WeaponIDs> weapons,
            WheelThemes themeID = WheelThemes::Neutral);
        virtual ~WeaponWheel();

        /// <summary>
        /// Changes the weapons shown in the wheel
        /// </summary>
        /// <param name="weapons">A vector that contains the weapon id's for the weapons to be shown in the order</param>
        void SetWeapons(std::vector<WeaponIDs> weapons);

        /// <summary>
        /// Changes the theme of the wheel 
        /// </summary>
        /// <param name="themeID">Id of the theme to change the wheel into</param>
        void SetWheelTheme(WheelThemes themeID);

        /// <summary>
        /// Change the active weapon slot
        /// </summary>
        /// <param name="slot">Index of the slot (0-4) to activate</param>
        void SetActiveSlot(size_t slot);

        /// <summary>
        /// Configres the wheel to either hide after being idle for a while or to always be visible
        /// </summary>
        /// <param name="hide">If passed true, the wheel will hide after being idle, if false the wheel always stays visible
        /// if not passed anything, toggles it</param>
        void ToggleNeverHide(std::optional<bool> hide);

        /// <summary>
        /// Sets the wheel into the analog switching mode, which keeps the wheel visible and draws
        /// extra arrows for every slot
        /// </summary>
        /// <param name="state">If given true, turns on the analog switchign mode,
        /// if false turns it off, if not passed anything toggles it</param>
        void ToggleAnalogSwitchingUI(std::optional<bool> state = {});

        /// <summary>
        /// Checks if the wheel is currently visible
        /// </summary>
        /// <returns>If visible, true, if hidden, false</returns>
        bool IsVisible();

        /// <summary>
        /// Updates the wheel appearance
        /// </summary>
        /// <param name="ts">The timestep to advance the update with</param>
        void OnUpdate(double ts);

        /// <summary>
        /// Draws the wheel
        /// </summary>
        /// <returns>Whteher the drawing was successful</returns>
        bool OnDraw();

        /// <summary>
        /// Get the d3d11 shader resource view for the texture the wheel is drawn on
        /// </summary>
        /// <returns>The pointer to the ID3D11ShaderResourceView of the drawn wheel texture</returns>
        ID3D11ShaderResourceView* GetSRV();

        /// <summary>
        /// Gets the latest error message occuring in the rendering of the wheel
        /// </summary>
        /// <returns>The error message string</returns>
        const std::string& GetLastRenderingErrorMsg();

        /// <summary>
        /// Gets the latest error code occuring in the rendering of the wheel
        /// </summary>
        /// <returns>The error HRESULT code</returns>
        HRESULT GetLastRenderingErrorCode();

        /// <summary>
        /// Checks if the never hiding setting is turned on
        /// </summary>
        /// <returns>Returns true if the wheel never hides, false if it does</returns>
        const auto& GetNeverHide() { return m_NeverHide; }

        /// <summary>
        /// Checksi if the wheel is in the analog switching mode
        /// </summary>
        /// <returns>Returns trie of the wheel is in analaog switching mode, false if not</returns>
        const auto& GetAnalogSwitchingMode() { return m_AnalogSwitching; }

    private: // Types

        // This object manages the IDs for the sprites used in the wheel
        class SpriteIndices {
        public:
            SpriteIndices() = default;
            ~SpriteIndices() = default;

            void UpdateSprites(WheelThemes themeId, const std::vector<WeaponIDs>& weaponIds);

            inline const auto& GetSpriteIds() { return m_SpriteIds; }
            inline const auto& GetCenterPieceIdx() { return m_CenterPieceIdx; }
            inline const auto& GetArrowIdx(size_t slot) { return m_ArrowsIdx[slot]; }
            inline const auto& GetInactivePanelIdx(size_t slot) { return m_InactivePanelsIdx[slot]; }
            inline const auto& GetActivePanelIdx(size_t slot) { return m_ActivePanelsIdx[slot]; }
            inline const auto& GetInactiveWeaponIdx(size_t slot) { return m_InactiveWeaponsIdx[slot]; }
            inline const auto& GetActiveWeaponIdx(size_t slot) { return m_ActiveWeaponsIdx[slot]; }
            inline const auto& GetActiveWeaponAnimationDupeIdx(size_t slot) { return m_ActiveWeaponAnimationDupesIdx[slot]; }

        private:
            std::vector<size_t> m_SpriteIds;

            size_t m_CenterPieceIdx;
            std::vector<size_t> m_ArrowsIdx;
            std::vector<size_t> m_InactivePanelsIdx;
            std::vector<size_t> m_ActivePanelsIdx;
            std::vector<size_t> m_InactiveWeaponsIdx;
            std::vector<size_t> m_ActiveWeaponsIdx;
            std::vector<size_t> m_ActiveWeaponAnimationDupesIdx;
        };

        struct AnimState {
            // The animation itself
            std::unique_ptr<GenericAnimation> pAnimation;

            // Whether the animation should run or not
            bool RunFlag;

            // If it's already been triggered once, will be used to manage the state of the animation when updating
            bool AlreadyTriggeredFlag;
        };

        enum class SpriteUpdateModes {
            //KeepVisibility = 0,
            FullyVisible = 0,
            FullyHide,

            size
        };

    private: // Private methods
        void InitializeAnimations();
        void SetWeaponsTranslations();

        void SetInactiveOpacity(float opacity);
        void SetActiveWeaponOpacity(float opacity);

        void UpdateCurrentWheelVisibility(SpriteUpdateModes mode);

    private: // Members
        UINT m_Width{ 0 }, m_Height{ 0 }, m_Top{ 0 }, m_Left{ 0 };

        size_t m_CurrentActiveSlot{ 0 }, m_PreviousActiveSlot{ m_CurrentActiveSlot };
        std::vector<WeaponIDs> m_Weapons{}, m_PreviousWeapons{ m_Weapons };
        std::vector<size_t> m_PanelIDs{};

        WheelThemes m_ThemeID{ WheelThemes::Neutral };

        SpriteIndices m_SpriteIndices;

        ID3D11Device* m_pD3D11Device;
        ID3D11DeviceContext* m_pD3D11DeviceContext;

        std::unique_ptr<Graphics::BatchedSprites> m_pSpriteBatch;

        bool m_AnalogSwitching{ false };
        bool m_NeverHide{ false };

        struct {
            AnimState WheelFadeOut,
                ArrowFadeOut,
                ArrowBrightness,
                ActiveWeaponFadeOut,
                WeaponSwitchBrightness,
                WeaponSwitchScaleUp,
                AnalogArrowsFadeIn,
                AnalogArrowsFadeOut;

            double HideTimerMs{ 0.0 };
            double SwitchingTimerMs{ 0.0 };
            double AnalogUnhidingTimerMs{ 0.0 };
            double AnalogHidingTimerMs{ 0.0 };

            // Resets the animation states and the timers, and will allow the animations to be run again
            inline void ResetHidingAnimations(bool allowRun = true) {
                WheelFadeOut.pAnimation->OnReset();
                ArrowFadeOut.pAnimation->OnReset();
                ActiveWeaponFadeOut.pAnimation->OnReset();

                WheelFadeOut.AlreadyTriggeredFlag = !allowRun;
                ArrowFadeOut.AlreadyTriggeredFlag = false;
                ActiveWeaponFadeOut.AlreadyTriggeredFlag = !allowRun;

                HideTimerMs = 0.0;
            }

            inline void ResetSwitchingAnimations(bool allowRun = true) {
                WeaponSwitchBrightness.pAnimation->OnReset();
                WeaponSwitchScaleUp.pAnimation->OnReset();
                ArrowBrightness.pAnimation->OnReset();

                ArrowBrightness.AlreadyTriggeredFlag = !allowRun;
                WeaponSwitchScaleUp.AlreadyTriggeredFlag = !allowRun;
                WeaponSwitchBrightness.AlreadyTriggeredFlag = !allowRun;

                SwitchingTimerMs = 0.0;
            }

            // Analog switching animations
            inline void ResetASUnhidingAnimations(bool allowRun = true) {
                AnalogArrowsFadeIn.pAnimation->OnReset();

                AnalogArrowsFadeIn.AlreadyTriggeredFlag = !allowRun;

                AnalogUnhidingTimerMs = 0.0;
            }

            inline void ResetASHidingAnimations(bool allowRun = true) {
                AnalogArrowsFadeOut.pAnimation->OnReset();

                AnalogArrowsFadeOut.AlreadyTriggeredFlag = !allowRun;

                AnalogHidingTimerMs = 0.0;
            }

            inline void AdvanceTimers(double ts)
            {
                HideTimerMs += ts;
                SwitchingTimerMs += ts;
                AnalogUnhidingTimerMs += ts;
                AnalogHidingTimerMs += ts;
            }
        } m_AnimData;

    private: // Statics
        static std::shared_ptr<Graphics::Texture2DArrayD3D11> s_pTextureArray;

        static constexpr double s_FadeDelay{ 583.3 };

        static constexpr double s_WheelFadeoutDur{ 133.3 },
            s_ActiveWeaponFadeoutDur{ 433.3 },
            s_SwitchBrightnessAnimDur{ 215.8 },
            s_SwitchScaleAnimDur{ 166.6 },
            s_ArrowFadeAnimDur{ 300.0 },
            s_AnalogArrowFadInAnimDur{ 700.0 };
    };
}
