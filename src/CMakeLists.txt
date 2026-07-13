set(MASM_SRC
    masm/CommonMacros.inc
    masm/SampleMod.asm
    masm/GuardGravity.asm
    masm/AddToMirageGauge.asm
    masm/AirRisingDragonDetours.asm
    masm/CameraHittingWall.asm
    masm/CameraSensitivity.asm
    masm/ConfirmSetAction.asm
    masm/CreateEffect.asm
    masm/HoldToCrazyComboAsm.asm
    masm/DisableStaggerRoyalguard.asm
    masm/InterceptCollisions.asm
    masm/JudgementCutDetours.asm
    masm/TurnRateFix.asm
    masm/GuardBreakToNormalBlock.asm
    masm/FixBallsHangHitSpeed.asm
    masm/FixBlendingEffects.asm
    masm/FixBossCam.asm
    masm/FixClothPhysics.asm
    masm/FixCrashArkhamPt2Doppel.asm
    masm/FixCrashArkhamPt2Grab.asm
    masm/FixCrashCerberus.asm
    masm/CerbDamageFix.asm
    masm/FixCrashM5.asm
    masm/FixCrashVergilM3.asm
    masm/FixCStageSetGateSpawn.asm
    masm/FixSecretMissionTimerFPS.asm
    masm/FixStaggerGravity.asm
    masm/HideStyleRankHUD.asm
    masm/HudHPSeparation.asm
    masm/HudStyleBarPosAsm.asm
    masm/PortalsHideAndMute.asm
    masm/PlaytimeOnTick.asm
    masm/DisableDriveHoldAsm.asm
    masm/CustomCameraPositioning.asm
    masm/FreeformSoftLockHelper.asm
    masm/GreenOrbsMPRegen.asm
    masm/DMC4LockOnDirection.asm
    masm/DTMustStyleArmor.asm
    masm/EnemyAITarget.asm
    masm/ShotgunShlSpawnAnglePointBlank.asm
    masm/SetAirTauntAsm.asm
    masm/SkyLaunchAirTaunt.asm
    masm/StyleLevellingCCSFix.asm
    masm/RerouteRedOrbsCounterAlpha.asm
    masm/ToggleTakeDamage.asm
    masm/TrickAlterationsDante.asm
    masm/VergilTricksAsm.asm
    masm/StyleRankHudNoFadeout.asm
    masm/ArtemisInstantFullCharge.asm
    masm/CheckScreenBreak.asm
    masm/CheckMissionResultScreen.asm
    masm/CheckMissionResultBPScreen.asm
    masm/NoAirLunarPhaseLift.asm
    masm/FixEnemyAttackCooldowns.asm
    masm/FixBloodgoyleStormFormTime.asm
    masm/FixCStaffRollCredits.asm
    masm/FixCStageSetSeal.asm
    masm/CheckTotalResultsScreen.asm
    masm/FixSummonedSwordsInitialTravel.asm
    masm/ChargeMechanicsCPlayer.asm
    masm/CItemOrbPickupAllPlayers.asm
    masm/FasterSummonedSwords.asm
    masm/SummonedSwordsFormationShortcuts.asm
    masm/StormSwordsDownedEnemyFix.asm
    masm/FixMPXinputVibration.asm
    masm/AdjustAirWhirlwindLaunchHeight.asm
)

set (ASM_LIBS)
set (ASM_FLAGS)

set(CXX_LIBS
    d3d11.lib
    d3dcompiler.lib
    dinput8.lib
    #xinput.lib
    Xinput9_1_0.lib
    dxguid.lib

    ImGui
    libzip
    zlib
    Detours

    EffekseerRendererDX11
    Effekseer

    debug ${CMAKE_SOURCE_DIR}/ThirdParty/curl/lib/x64-debug/libcurl_a_debug.lib optimized ${CMAKE_SOURCE_DIR}/ThirdParty/curl/lib/x64-release/libcurl_a.lib
    debug ${CMAKE_SOURCE_DIR}/ThirdParty/openssl/lib/x64-debug/libcrypto_static.lib optimized ${CMAKE_SOURCE_DIR}/ThirdParty/openssl/lib/x64-release/libcrypto_static.lib
    debug ${CMAKE_SOURCE_DIR}/ThirdParty/openssl/lib/x64-debug/libssl_static.lib optimized ${CMAKE_SOURCE_DIR}/ThirdParty/openssl/lib/x64-release/libssl_static.lib
)

set(CXX_FLAGS
    /nologo
    /c
    #/std:c++latest
    #/modulesearch
    /permissive-
    #/experimental:module
    /Zc:forScope
    /Zc:inline
    /Zc:preprocessor
    /Zc:wchar_t
    /Zc:char8_t
    /utf-8
    /Zc:externC-
    /W3
    /wd4005 # macro redefinition
    /wd4996 # was declared deprecated
    /wd5105 # macro expansion producing 'defined' has undefined behavior
    #/wd5202 # a global module fragment can only contain preprocessor directives
    /wd4244 # float to int conversion
    #/O2
    /Oi
    /Gy
    /Zi # make pdb file
    /EHsc
    /GS-
    /GR-
)

project(Crimson LANGUAGES CXX C ASM_MASM)

# Generate PatreonData.hpp from patrons.json at build time (not configure time)
# so that editing patrons.json and rebuilding picks up changes automatically.
set(PATREONDATA_HPP "${CMAKE_CURRENT_BINARY_DIR}/PatreonData.hpp")
add_custom_command(
    OUTPUT  "${PATREONDATA_HPP}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/patrons.json"
    COMMAND ${CMAKE_COMMAND}
        "-DSRC_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
        "-DOUTPUT_FILE=${PATREONDATA_HPP}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/embed_patrons.cmake"
    COMMENT "Generating PatreonData.hpp from patrons.json"
)

file(GLOB FONTS_HDR 
                    Fonts/Benguiat.hpp
                    Fonts/Messenger.hpp
                    Fonts/RobotoMedium.hpp
                    Fonts/RussoOneRegular.hpp
		    Fonts/RedOrbRusso.hpp
		    Fonts/RedOrbRussoBackdrop.hpp
)

file(GLOB GLOBAL_SRC 
    Global/ConfigFunctions.h
    Global/GlobalBase.cpp
    Global/GlobalBase.hpp
    Global/GUIBase.cpp
    Global/GUIBase.hpp
    Global/HooksBase.cpp
    Global/HooksBase.hpp
    Global/SwapChainWrapper.cpp
    Global/SwapChainWrapper.hpp
)

file(GLOB MAIN_SRC
    Actor.cpp
    Actor.hpp
    ActorBase.cpp
    ActorBase.hpp
    ActorRelocations.cpp
    ActorRelocations.hpp
    Arcade.cpp
    Arcade.hpp
    BossRush.cpp
    BossRush.hpp
    Camera.cpp
    Camera.hpp
    Config.cpp
    Config.hpp
    Crimson.cpp 
    CrimsonConfig.cpp
    CrimsonConfig.hpp
    CrimsonConfigGameplay.cpp
    CrimsonConfigGameplay.hpp
    CrimsonConfigGameplayMask.hpp
    CrimsonConfigHandling.h
    CrimsonInput.cpp
    CrimsonInput.hpp
    CrimsonBetterArkham2.cpp
    CrimsonBetterArkham2.hpp
    CrimsonClock.cpp
    CrimsonClock.hpp
    CrimsonCameraController.cpp
    CrimsonCameraController.hpp
    CrimsonDetours.cpp
    CrimsonDetours.hpp
    CrimsonEnemyAITarget.cpp
    CrimsonEnemyAITarget.hpp
    CrimsonFastcallDetours.cpp
    CrimsonFastcallDetours.hpp
    CrimsonFileHandling.cpp
    CrimsonFileHandling.hpp	
    CrimsonFX.cpp
    CrimsonFX.hpp
    CrimsonGameModes.cpp
    CrimsonGameModes.hpp
    CrimsonGameplay.cpp
    CrimsonGameplay.hpp
    CrimsonGUI.cpp
    CrimsonGUI.hpp
    CrimsonLicense.hpp
    CrimsonOnboarding.cpp
    CrimsonOnboarding.hpp
    Patreon.hpp
    embed_patrons.cmake
    patrons.json
    parse_patreon.py
    CrimsonHighFPSFixes.cpp
    CrimsonHighFPSFixes.hpp
    CrimsonHUD.cpp
    CrimsonHUD.hpp
    CrimsonLDK.cpp
    CrimsonLDK.hpp
    CrimsonUtil.cpp
    CrimsonUtil.hpp
    CrimsonVisualStyle.cpp
    CrimsonVisualStyle.hpp
    CrimsonOnTick.cpp
    CrimsonOnTick.hpp
    CrimsonPatches.cpp
    CrimsonPatches.hpp
    CrimsonReversedCalls.cpp
    CrimsonReversedCalls.hpp
    CrimsonSDL.cpp
    CrimsonSDL.hpp
    CrimsonTimers.cpp	
    CrimsonTimers.hpp	
    DebugDrawDX11.cpp
    DebugDrawDX11.hpp
    DMC3Input.cpp
    DMC3Input.hpp
    Event.cpp
    Event.hpp
    Exp.hpp
    File.cpp
    File.hpp
    FMOD.cpp
    FMOD.hpp
    Global.cpp
    Global.hpp
    Graphics.cpp
    Graphics.hpp
    Hooks.cpp
    Hooks.hpp
    HUD.cpp
    HUD.hpp
    ImGuiExtra.cpp
    ImGuiExtra.hpp
    Internal.cpp
    Internal.hpp
    Memory.cpp
    Memory.hpp
    Model.cpp
    Model.hpp
    Scene.cpp
    Scene.hpp
    SDL.cpp
    SDL.hpp
    Sound.cpp
    Sound.hpp
    SoundRelocations.cpp
    SoundRelocations.hpp
    Speed.cpp Speed.hpp Training.cpp
    Training.hpp
    Vars.cpp
    Vars.hpp
    WebAPICalls.cpp
    WebAPICalls.hpp
    Window.cpp
    Window.hpp
    StyleSwitchFX.cpp
    StyleSwitchFX.hpp
    CrimsonEfk.hpp
    CrimsonEfk.cpp
    CrimsonEfkPreload.cpp
    CrimsonEfkPreload.hpp
    NvApiReflex.cpp
    NvApiReflex.hpp
)

file(GLOB CORE_SRC 
    Core/DebugSwitch.hpp
    Core/Core.cpp
    Core/Core.hpp
    Core/Core_ImGui.cpp
    Core/Core_ImGui.hpp
    Core/DataTypes.hpp
    Core/GUI.cpp
    Core/GUI.hpp
    Core/Input.cpp
    Core/Input.hpp
    Core/JSON.h
    Core/Macros.h
    Core/RapidJSON.h
)

set(UTILITY_SRC
    Utility/Address.cpp
    Utility/Address.hpp
    Utility/Detour.cpp
    Utility/Detour.hpp
)

set(UI_SRC 
    UI/BatchedSprites.cpp
    UI/BatchedSprites.hpp
    UI/EmbeddedImages.cpp
    UI/EmbeddedImages.hpp
    UI/GenericAnimation.cpp
    UI/GenericAnimation.hpp
    UI/GraphicsBase.cpp
    UI/GraphicsBase.hpp
    UI/Sprite.cpp
    UI/Sprite.hpp
    UI/Texture2DArrayD3D11.cpp
    UI/Texture2DArrayD3D11.hpp
    UI/Texture2DD3D11.cpp
    UI/Texture2DD3D11.hpp
    UI/WeaponWheel.cpp
    UI/WeaponWheel.hpp
)

source_group("Global" FILES ${GLOBAL_SRC})
source_group("Crimson" FILES ${MAIN_SRC} "${PATREONDATA_HPP}")
source_group("Core" FILES ${CORE_SRC})
source_group("Utility" FILES ${UTILITY_SRC})
source_group("ASM" FILES ${MASM_SRC})
source_group("Fonts" FILES ${FONTS_HDR})
source_group("UI" FILES ${UI_SRC})

add_library(${PROJECT_NAME} SHARED
            ${MAIN_SRC}
            ${UTILITY_SRC}
            ${MASM_SRC}
            ${FONTS_HDR}
            ${UI_SRC}

            ${GLOBAL_SRC}
            ${CORE_SRC}
            "${PATREONDATA_HPP}"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
)

# Precompiled header — the single biggest compile-time win.
# Collects Windows SDK, D3D, ImGui, SDL, RapidJSON, and other heavy
# system/third-party headers so they are parsed once instead of ~75 times.
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.hpp)

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME Crimson)

project(dinput8_injector)

set(DI_SOURCE
    dinput8.cpp
    CrashHandler.hpp
    CrashHandler.cpp
)

set(DI_CORE_SOURCE
    Core/DebugSwitch.hpp
    Core/Core.cpp
    Core/Core.hpp
    Core/DataTypes.hpp
)

set(DI_CORE_HEADER
    Core/Macros.h
)

source_group("Core" FILES ${DI_CORE_SOURCE} ${DI_CORE_HEADER})
source_group("Source" FILES ${DI_SOURCE})

add_library(${PROJECT_NAME} SHARED
            ${DI_SOURCE}
            ${DI_CORE_SOURCE}
            ${DI_CORE_HEADER}
)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${CXX_FLAGS}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE dinput8_pch.hpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME dinput8_injector)

