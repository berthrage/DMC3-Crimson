// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Arcade.hpp"
#include "Core/Core.hpp"
#include "Config.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"

inline bool Enable() {
    return activeConfig.Arcade.enable;
}

namespace Arcade {

void InitSession() {
    if (!Enable()) {
        return;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);


    sessionData.mission = activeConfig.Arcade.mission;

    if ((sessionData.mission >= 1) && (sessionData.mission <= 20)) {
        using namespace DIFFICULTY_MODE;

        uint32 difficultyMode     = 0;
        bool oneHitKill = false;

        difficultyMode = activeConfig.Arcade.mode;

        if (difficultyMode == HEAVEN_OR_HELL) {
            difficultyMode       = HARD;
            oneHitKill = true;
        }

        sessionData.difficultyMode       = difficultyMode;
        sessionData.oneHitKill = oneHitKill;
    }

    if (sessionData.mission == MISSION::BLOODY_PALACE) {
        sessionData.bloodyPalace = true;
    }


    sessionData.enableTutorial = true;
    sessionData.useGoldOrb     = true;


    sessionData.itemCounts[ITEM::GOLD_ORB]   = 3;
    sessionData.itemCounts[ITEM::BLUE_ORB]   = 6;
    sessionData.itemCounts[ITEM::PURPLE_ORB] = 7;

    if constexpr (debug) {
        sessionData.redOrbs = 1500000;

        sessionData.itemCounts[ITEM::BLUE_ORB]   = 0;
        sessionData.itemCounts[ITEM::PURPLE_ORB] = 0;
    }


    SetMemory(sessionData.weaponAndStyleUnlocks, 1, sizeof(sessionData.weaponAndStyleUnlocks));

	auto& activeCharacterData = GetActiveCharacterData(0, 0, 0);
	auto& queuedCharacterData = GetQueuedCharacterData(0, 0, 0);

    if (!queuedCharacterData.ignoreCostume) {
        sessionData.costume = activeCharacterData.costume;
    } else {
		sessionData.costume = activeConfig.Arcade.costume;
    }
    sessionData.unlockDevilTrigger = true;
    sessionData.hitPoints          = activeConfig.Arcade.hitPoints;
    sessionData.magicPoints        = activeConfig.Arcade.magicPoints;


    if (activeConfig.Arcade.character == CHARACTER::DANTE) {
        sessionData.style = activeConfig.Arcade.style;

        CopyMemory(sessionData.weapons, activeConfig.Arcade.weapons, sizeof(sessionData.weapons));
    }


    SetMemory(sessionData.styleLevels, 0, sizeof(sessionData.styleLevels));

    if (activeConfig.Arcade.character == CHARACTER::DANTE) {
        for_all(index, 4) {
            sessionData.styleLevels[index] = 2;
            sessionData.styleExpPoints[index] = 100000;
        }
    } else if (activeConfig.Arcade.character == CHARACTER::VERGIL) {
        sessionData.styleLevels[STYLE::DARK_SLAYER] = 2;
        sessionData.styleExpPoints[STYLE::DARK_SLAYER] = 100000;
    }

    SetMemory(sessionData.expertise, 0xFF, sizeof(sessionData.expertise));


    SetMemory(sessionData.rangedWeaponLevels, 0, sizeof(sessionData.rangedWeaponLevels));

    if (activeConfig.Arcade.character == CHARACTER::DANTE) {
        for_all(index, 5) {
            sessionData.rangedWeaponLevels[index] = 2;
        }
    }


    auto& controllerMagic = *reinterpret_cast<uint32*>(appBaseAddr + 0x553000) = 0;
}

void SetCharacter(byte8* dest) {
    if (!Enable()) {
        return;
    }

    LogFunction(dest);

    auto& character = *reinterpret_cast<uint8*>(dest + 0x4565) = activeConfig.Arcade.character;
}


void SetRoom() {
    if (!Enable()) {
        return;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_208 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_208 || !pool_208[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_208[12]);


    if ((sessionData.mission >= 1) && (sessionData.mission <= 20) && !activeConfig.BossRush.enable) {
        if (activeConfig.Arcade.enableRoomSelection) {
            nextEventData.room = static_cast<uint16>(activeConfig.Arcade.room);
        }

        if (activeConfig.Arcade.enablePositionSelection) {
            nextEventData.position = static_cast<uint16>(activeConfig.Arcade.position);
        }
    }

    if (sessionData.mission == MISSION::BLOODY_PALACE) {
        auto floor = activeConfig.Arcade.floor;
        if (floor >= countof(floorHelpers)) {
            floor = 0;
        }

        auto& floorHelper = floorHelpers[floor];

        nextEventData.room     = floorHelper.room;
        nextEventData.position = floorHelper.position;


        [&]() {
            auto pool_245 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool_245 || !pool_245[5]) {
                return;
            }
            auto name_245 = *reinterpret_cast<byte8**>(pool_245[5] + 0x10);
            if (!name_245) {
                return;
            }
            name_245 = *reinterpret_cast<byte8**>(name_245 + 8);
            if (!name_245) {
                return;
            }
            auto& bloodyPalaceData = *reinterpret_cast<BloodyPalaceData*>(name_245);


            bloodyPalaceData.lastLevel = bloodyPalaceData.level = activeConfig.Arcade.level;
        }();
    }
}


void EventCreateMainActor(byte8* baseAddr) {
    if (!Enable()) {
        return;
    }

    LogFunction(baseAddr);

    // Unlock Files
    [&]() {
        auto pool = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
        if (!pool || !pool[1] || !pool[6]) {
            return;
        }

        SetMemory((pool[1] + 0x82), 0xFF, 11);
        SetMemory((pool[1] + 0x8E), 0xFF, 1);
        SetMemory((pool[6] + 0x7E4), 0xFF, 6);
    }();

    // auto & unlock = *reinterpret_cast<byte32(*)[4]>(appBaseAddr + 0x564594);

    // unlock[0] |= 1;

    /*
    dmc3.exe+2AADD0 - 44 84 94 19 82000000 - test [rcx+rbx+00000082],r10l
    dmc3.exe+2AA003 - 41 84 84 10 8A000000 - test [r8+rdx+0000008A],al
    dmc3.exe+2AA1BF - 0FB6 99 8E000000     - movzx ebx,byte ptr [rcx+0000008E]
    dmc3.exe+2A9F7C - 41 84 84 10 E2070000 - test [r8+rdx+000007E2],al
    */
}


void Toggle(bool enable) {
    LogFunction(enable);

    static bool run = false;


    // Force New Game
    {
        auto addr                 = (appBaseAddr + 0x2433FB);
        constexpr new_size_t size = 2;
        /*
        dmc3.exe+2433FB - 74 55    - je dmc3.exe+243452
        dmc3.exe+2433FD - 80 F9 01 - cmp cl,01
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }


    // Skip Mission Select
    {
        auto addr                 = (appBaseAddr + 0x243299);
        constexpr new_size_t size = 2;
        /*
        dmc3.exe+243299 - 74 13                   - je dmc3.exe+2432AE
        dmc3.exe+24329B - C7 05 83143500 01000000 - mov [dmc3.exe+594728],00000001
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }


    // Force Mission Start
    {
        auto addr                 = (appBaseAddr + 0x2411F5);
        constexpr new_size_t size = 2;
        /*
        dmc3.exe+2411F5 - 74 4F    - je dmc3.exe+241246
        dmc3.exe+2411F7 - 83 F8 01 - cmp eax,01
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }


    // Force Costume
    {
        auto addr                 = (appBaseAddr + 0x217991);
        auto dest                 = (appBaseAddr + 0x217993);
        constexpr new_size_t size = 2;
        /*
        dmc3.exe+217991 - 74 0F         - je dmc3.exe+2179A2
        dmc3.exe+217993 - 41 0FB6 43 4C - movzx eax,byte ptr [r11+4C]
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            WriteAddress(addr, dest, size);
        } else {
            backupHelper.Restore(addr);
        }
    }

    {
        auto addr                 = (appBaseAddr + 0x21799A);
        auto dest                 = (appBaseAddr + 0x21799C);
        constexpr new_size_t size = 2;
        /*
        dmc3.exe+21799A - 74 06       - je dmc3.exe+2179A2
        dmc3.exe+21799C - 41 88 41 34 - mov [r9+34],al
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            WriteAddress(addr, dest, size);
        } else {
            backupHelper.Restore(addr);
        }
    }


    // Skip Orb Notifications
    {
        auto addr                 = (appBaseAddr + 0x1AA791);
        constexpr new_size_t size = 2;
        /*
        dmc3.exe+1AA791 - 75 18          - jne dmc3.exe+1AA7AB
        dmc3.exe+1AA793 - 41 B8 01000000 - mov r8d,00000001
        */

        if (!run) {
            backupHelper.Save(addr, size);
        }

        if (enable) {
            Write<byte8>(addr, 0xEB);
        } else {
            backupHelper.Restore(addr);
        }
    }


    run = true;
}

}; // namespace Arcade
