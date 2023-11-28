set(MASM_SRC
    masm/SampleMod.asm
    masm/GuardGravity.asm
    masm/CreateEffect.asm
    masm/HoldToCrazyComboAsm.asm
    masm/HudHPSeparation.asm
    masm/HudStyleBarPosAsm.asm
    masm/DisableDriveHoldAsm.asm
    masm/SetAirTauntAsm.asm
    masm/VergilTricksAsm.asm
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

file(GLOB FONTS_HDR Fonts/RobotoMedium.hpp
                    Fonts/RussoOneRegular.hpp
)

file(GLOB GLOBAL_SRC 
    Global/ConfigFunctions.h
    Global/GlobalBase.cpp
    Global/GlobalBase.hpp
    Global/GUIBase.cpp
    Global/GUIBase.hpp
    Global/HooksBase.cpp
    Global/HooksBase.hpp
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
    CrimsonGameplay.cpp
    CrimsonGameplay.hpp
    CrimsonGUI.cpp
    CrimsonGUI.hpp
    CrimsonUtil.cpp
    CrimsonUtil.hpp
    DebugDrawDX11.cpp
    DebugDrawDX11.hpp
    DetourFunctions.cpp
    DetourFunctions.hpp
    DMC3Input.cpp
    DMC3Input.hpp
    Event.cpp
    Event.hpp
    Exp.hpp
    ExtraSound.cpp
    ExtraSound.hpp
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
    Internal.cpp
    Internal.hpp
    Memory.cpp
    Memory.hpp
    Model.cpp
    Model.hpp
    PatchFunctions.cpp
    PatchFunctions.hpp
    Scene.cpp
    Scene.hpp
    SDL.cpp
    SDL.hpp
    Sound.cpp
    Sound.hpp
    SoundRelocations.cpp
    SoundRelocations.hpp
    Speed.cpp
    Speed.hpp
    Timers.cpp
    Timers.hpp
    Training.cpp
    Training.hpp
    Vars.cpp
    Vars.hpp
    Window.cpp
    Window.hpp
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
    UI/EmbdeddedImages.hpp
    UI/Texture2DD3D11.cpp
    UI/Texture2DD3D11.hpp
    UI/UI.cpp
    UI/UI.hpp
)

source_group("Global" FILES ${GLOBAL_SRC})
source_group("Crimson" FILES ${MAIN_SRC})
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
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
    $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
)

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME Crimson)

project(dinput8_injector)

set(DI_SOURCE
    dinput8.cpp  
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

target_link_libraries(${PROJECT_NAME} PUBLIC ${CXX_LIBS}) # Dependencies

set_target_properties(${PROJECT_NAME} PROPERTIES 
OUTPUT_NAME dinput8_injector)