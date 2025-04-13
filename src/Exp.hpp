#pragma once

#include <initializer_list>
#include "Core/DataTypes.hpp"
#include "Vars.hpp"

namespace ExpConfig {
struct ExpData {
    uint32 styleLevels[STYLE::MAX];
    float styleExpPoints[STYLE::MAX];
    bool unlocks[64];
    bool hasPairedWithActorSystem;

    ExpData& operator=(ExpData& expData) {
        CopyMemory(this, &expData, sizeof(*this));

        return *this;
    }

    ExpData& operator=(const std::initializer_list<int>&) {
        Log("Reset");

        SetMemory(this, 0, sizeof(*this));

        return *this;
    }
};

struct ExpertiseHelper {
	new_size_t index;
	byte32 flags;
};

extern ExpertiseHelper expertiseHelpersDante[];
extern ExpertiseHelper expertiseHelpersVergil[];

extern ExpData missionExpDataDante;
extern ExpData sessionExpDataDante;
extern ExpData savedExpDataDante[SAVE_COUNT];

extern ExpData missionExpDataVergil;
extern ExpData sessionExpDataVergil;
extern ExpData savedExpDataVergil[SAVE_COUNT];

void SaveExp();
void LoadExp();
void InitExp();

void SavePlayerActorExp();

void UpdatePlayerActorExp(byte8* actorBaseAddr);

void UpdatePlayerActorExps();

void TransferUnlocksToActorSystem();

void MaintainUnlockAndExpertiseParity();

void MarkAsPairedWithActorSystem();

namespace Exp {
void InitSession();
void SceneMissionStart();
void SceneMissionResult();
void IncStyleExpPoints(byte8* actorBaseAddr);
} // namespace Exp
} // namespace ExpConfig