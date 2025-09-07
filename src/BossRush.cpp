#include <string.h>

// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "BossRush.hpp"
#include "Core/Core.hpp"
#include "Internal.hpp"
#include "Config.hpp"
#include "Vars.hpp"

#include "Core/Macros.h"

#include "Core/DebugSwitch.hpp"

namespace BossRush {

void SetRoom() {
    if (!activeConfig.BossRush.enable) {
        return;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_44 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_44 || !pool_44[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_44[8]);

    auto pool_55 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_55 || !pool_55[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_55[12]);

    auto pool_66 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
    if (!pool_66 || !pool_66[1]) {
        return;
    }
    auto eventFlags = reinterpret_cast<byte32*>(pool_66[1]);


    auto SetNextEventData = [&](uint8 boss) {
        nextEventData.room     = bossHelpers[boss].room;
        nextEventData.position = bossHelpers[boss].position;
    };

    if (nextEventData.room == missionStartHelpers[sessionData.mission].room) {
        switch (sessionData.mission) {
        case 3: {
            SetNextEventData(BOSS::CERBERUS);
            eventFlags[20] = 1;
            break;
        }
        case 4: {
            SetNextEventData(BOSS::GIGAPEDE);
            break;
        }
        case 5: {
            if (!activeConfig.BossRush.Mission5.skipJester) {
                SetNextEventData(BOSS::JESTER_1);
            } else {
                SetNextEventData(BOSS::AGNI_RUDRA);
                eventFlags[20] = 1;
            }
            break;
        }
        case 7: {
            SetNextEventData(BOSS::VERGIL_1);
            break;
        }
        case 8: {
            SetNextEventData(BOSS::LEVIATHAN);
            auto pool1 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
            if (!pool1) {
                break;
            }
            if (!pool1[12]) {
                break;
            }
            auto pool2 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
            if (!pool2) {
                break;
            }
            *reinterpret_cast<uint8*>(reinterpret_cast<byte8*>(pool2) + 0x6A) = 1;
            *reinterpret_cast<byte8*>(pool1[12] + 0x7DF)                      = 0x40;
            break;
        }
        case 9: {
            SetNextEventData(BOSS::NEVAN);
            eventFlags[20] = 1;
            break;
        }
        case 11: {
            SetNextEventData(BOSS::BEOWULF);
            eventFlags[20] = 1;
            break;
        }
        case 12: {
            if (!activeConfig.BossRush.Mission12.skipJester) {
                SetNextEventData(BOSS::JESTER_2);
            } else if (!activeConfig.BossRush.Mission12.skipGeryonPart1) {
                SetNextEventData(BOSS::GERYON_PART_1);
                eventFlags[20] = 1;
            } else {
                SetNextEventData(BOSS::GERYON_PART_2);
                eventFlags[20] = 2;
            }
            break;
        }
        case 13: {
            SetNextEventData(BOSS::VERGIL_2);
            break;
        }
        case 16: {
            SetNextEventData(BOSS::LADY);
            eventFlags[20] = 1;
            break;
        }
        case 17: {
            if (!activeConfig.BossRush.Mission17.skipJester) {
                SetNextEventData(BOSS::JESTER_3);
            } else {
                SetNextEventData(BOSS::DOPPELGANGER);
            }
            break;
        }
        case 18: {
            SetNextEventData(BOSS::TAIZAI_REBORN);
            break;
        }
        case 19: {
            if (!activeConfig.BossRush.Mission19.skipArkhamPart1) {
                SetNextEventData(BOSS::ARKHAM_PART_1);
            } else {
                SetNextEventData(BOSS::ARKHAM_PART_2);
                eventFlags[20] = 1;
            }
            break;
        }
        }
    }
}

void SetNextRoom() {
    if (!activeConfig.BossRush.enable) {
        return;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_194 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_194 || !pool_194[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_194[8]);

    auto pool_215 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_215 || !pool_215[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_215[12]);

    auto pool_236 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
    if (!pool_236 || !pool_236[1]) {
        return;
    }
    auto eventFlags = reinterpret_cast<byte32*>(pool_236[1]);


    auto SetNextEventData = [&](uint8 boss) {
        nextEventData.room     = bossHelpers[boss].room;
        nextEventData.position = bossHelpers[boss].position;
    };

    switch (sessionData.mission) {
    case 5: {
        if (eventData.room == bossHelpers[BOSS::JESTER_1].room) {
            SetNextEventData(BOSS::AGNI_RUDRA);
            eventFlags[20] = 1;
        }
        break;
    }
    case 12: {
        if (eventData.room == bossHelpers[BOSS::JESTER_2].room) {
            if (!activeConfig.BossRush.Mission12.skipGeryonPart1) {
                SetNextEventData(BOSS::GERYON_PART_1);
                eventFlags[20] = 1;
            } else {
                SetNextEventData(BOSS::GERYON_PART_2);
                eventFlags[20] = 2;
            }
        }
        break;
    }
    case 17: {
        if (eventData.room == bossHelpers[BOSS::JESTER_3].room) {
            SetNextEventData(BOSS::DOPPELGANGER);
        }
        break;
    }
    case 18: {
        switch (static_cast<uint16>(eventData.room)) {
        case bossHelpers[BOSS::TAIZAI_REBORN].room: {
            SetNextEventData(BOSS::CERBERUS_REBORN);
            break;
        }
        case bossHelpers[BOSS::CERBERUS_REBORN].room: {
            SetNextEventData(BOSS::GIGAPEDE_REBORN);
            break;
        }
        case bossHelpers[BOSS::GIGAPEDE_REBORN].room: {
            SetNextEventData(BOSS::AGNI_RUDRA_REBORN);
            break;
        }
        case bossHelpers[BOSS::AGNI_RUDRA_REBORN].room: {
            SetNextEventData(BOSS::LEVIATHAN_REBORN);
            break;
        }
        case bossHelpers[BOSS::LEVIATHAN_REBORN].room: {
            SetNextEventData(BOSS::NEVAN_REBORN);
            break;
        }
        case bossHelpers[BOSS::NEVAN_REBORN].room: {
            SetNextEventData(BOSS::BEOWULF_REBORN);
            break;
        }
        case bossHelpers[BOSS::BEOWULF_REBORN].room: {
            SetNextEventData(BOSS::GERYON_REBORN);
            break;
        }
        case bossHelpers[BOSS::GERYON_REBORN].room: {
            SetNextEventData(BOSS::DOPPELGANGER_REBORN);
            break;
        }
        case bossHelpers[BOSS::DOPPELGANGER_REBORN].room: {
            nextEventData.room     = 403;
            nextEventData.position = 2;
            eventFlags[14]         = 0x3FE;
            eventFlags[15]         = 0x1FF;
            break;
        }
        }
        break;
    }
    }
}

void SetContinueRoom() {
    if (!activeConfig.BossRush.enable) {
        return;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_309 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_309 || !pool_309[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_309[8]);

    auto pool_340 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_340 || !pool_340[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_340[12]);

    auto pool_371 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
    if (!pool_371 || !pool_371[1]) {
        return;
    }
    auto eventFlags = reinterpret_cast<byte32*>(pool_371[1]);


    nextEventData.room     = static_cast<uint16>(eventData.room);
    nextEventData.position = static_cast<uint16>(eventData.position);
}

bool SetTrack(const char* filename) {
    if (!activeConfig.BossRush.enable) {
        return true;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_328 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_328 || !pool_328[8]) {
        return true;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_328[8]);

    auto pool_369 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_369 || !pool_369[12]) {
        return true;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_369[12]);

    auto pool_410 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
    if (!pool_410 || !pool_410[1]) {
        return true;
    }
    auto eventFlags = reinterpret_cast<byte32*>(pool_410[1]);


    if ((sessionData.mission == 5) && (strcmp(filename, "afs/sound/Battle_03.adx") == 0)) {
        return false;
    }

    if ((sessionData.mission == 5) && (eventData.room == bossHelpers[BOSS::AGNI_RUDRA].room) &&
        (strcmp(filename, bossHelpers[BOSS::JESTER_1].track) == 0)) {
        return false;
    }


    //arkham 2 audio skip
    
    if ((sessionData.mission == 19) && activeConfig.BossRush.Mission19.skipArkhamPart2 && (nextEventData.room == bossHelpers[BOSS::ARKHAM_PART_2].room) &&
        (strcmp(filename, bossHelpers[BOSS::ARKHAM_PART_2].track) == 0)) {
        return false;
    }

    return true;
}

void EventMain() {
    if (!activeConfig.BossRush.enable) {
        return;
    }

    LogFunction();

    auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);

    auto pool_365 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_365 || !pool_365[8]) {
        return;
    }
    auto& eventData = *reinterpret_cast<EventData*>(pool_365[8]);

    auto pool_416 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E10);
    if (!pool_416 || !pool_416[12]) {
        return;
    }
    auto& nextEventData = *reinterpret_cast<NextEventData*>(pool_416[12]);

    auto pool_467 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E30);
    if (!pool_467 || !pool_467[1]) {
        return;
    }
    auto eventFlags = reinterpret_cast<byte32*>(pool_467[1]);


    auto PlayTrack = [](const char* filename) {
        func_32BE20((appBaseAddr + 0xCF3700));
        func_32BA90((appBaseAddr + 0xCF3708), filename, 0, 0);
    };

    switch (sessionData.mission) {
    case 3: {
        if ((eventData.room == bossHelpers[BOSS::CERBERUS].room) && (eventFlags[20] == 1)) {
            PlayTrack(bossHelpers[BOSS::CERBERUS].track);
        }
        break;
    }
    case 5: {
        if ((eventData.room == bossHelpers[BOSS::AGNI_RUDRA].room) && (eventFlags[20] == 1)) {
            PlayTrack(bossHelpers[BOSS::AGNI_RUDRA].track);
        }
        break;
    }
    case 7: {
        if (eventData.room == bossHelpers[BOSS::VERGIL_1].room) {
            PlayTrack(bossHelpers[BOSS::VERGIL_1].track);
        }
        break;
    }
    case 9: {
        if ((eventData.room == bossHelpers[BOSS::NEVAN].room) && (eventFlags[20] == 1)) {
            PlayTrack(bossHelpers[BOSS::NEVAN].track);
        }
        break;
    }
    case 11: {
        if ((eventData.room == bossHelpers[BOSS::BEOWULF].room) && (eventFlags[20] == 1)) {
            PlayTrack(bossHelpers[BOSS::BEOWULF].track);
        }
        break;
    }
    case 12: {
        if ((eventData.room == bossHelpers[BOSS::GERYON_PART_1].room) && (eventFlags[20] == 1)) {
            PlayTrack(bossHelpers[BOSS::GERYON_PART_1].track);
        } else if ((eventData.room == bossHelpers[BOSS::GERYON_PART_2].room) && (eventFlags[20] == 2)) {
            PlayTrack(bossHelpers[BOSS::GERYON_PART_2].track);
        }
        break;
    }
    case 13: {
        if (eventData.room == bossHelpers[BOSS::VERGIL_2].room) {
            PlayTrack(bossHelpers[BOSS::VERGIL_2].track);
        }
        break;
    }
    case 16: {
        if (eventData.room == bossHelpers[BOSS::LADY].room) {
            PlayTrack(bossHelpers[BOSS::LADY].track);
        }
        break;
    }
    case 17: {
        if ((eventData.room == bossHelpers[BOSS::DOPPELGANGER].room) && (eventFlags[20] == 0)) {
            PlayTrack(bossHelpers[BOSS::DOPPELGANGER].track);
        }
        break;
    }
    case 19: {
        if ((eventData.room == bossHelpers[BOSS::ARKHAM_PART_1].room) && (eventFlags[20] == 0)) {
            PlayTrack(bossHelpers[BOSS::ARKHAM_PART_1].track);
        } else if ((eventData.room == bossHelpers[BOSS::ARKHAM_PART_2].room) && (eventFlags[20] == 1)) {
            //stop playing the music jackass
            if (!activeConfig.BossRush.Mission19.skipArkhamPart2)
                PlayTrack(bossHelpers[BOSS::ARKHAM_PART_2].track);
        }
        break;
    }
    }
}

}; // namespace BossRush
