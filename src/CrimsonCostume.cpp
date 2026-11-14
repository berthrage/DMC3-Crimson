#include "CrimsonCostume.hpp"

#include <cstring>
#include <memory>

#include "Core/Core.hpp"
#include "CrimsonConfig.hpp"
#include "CrimsonReversedCalls.hpp"
#include "Utility/Detour.hpp"
#include "Config.hpp"
#include "File.hpp"
#include "Global.hpp"
#include "Internal.hpp"
#include "Model.hpp"

namespace CrimsonCostume {

namespace {

// The coatless costume (pl011) keeps the amulet in the coat slot of its pac.
constexpr uint32 AMULET_MODEL_FILE   = 12;
constexpr uint32 AMULET_CLOTH_FILE   = 13;
constexpr uint32 AMULET_TEXTURE_FILE = 0;

// Agni & Rudra never load a wings submodel, so its submodel, cloth and bone pool lanes stay free.
constexpr uint8 AMULET_SUBMODEL = 5;
constexpr uint8 AMULET_CLOTH    = 4;
constexpr uint8 AMULET_POOL     = 2;

constexpr uint32 LAST_MISSION = 7;

constexpr uintptr_t AMULET_LINK_TABLE_OFFSET = 0x58B054; // DantePendantLinkTable_14058B054
constexpr uintptr_t COLLIDER_VERTICES_OFFSET = 0x58B260; // 23..25 is the chest collider the amulet rests on

constexpr uintptr_t UPDATE_COAT_OFFSET = 0x212070; // UpdateCPlDanteCoat_sub_140212070
constexpr uintptr_t RESET_CLOTH_OFFSET = 0x216680; // CPlDanteResetCloth_sub_140216680
constexpr uintptr_t DRAW_COAT_OFFSET   = 0x218960; // CPlDanteDrawCoat_sub_140218960

constexpr uintptr_t VTBL_UPDATE_WITH_MATRIX = 0x190;
constexpr uintptr_t VTBL_DRAW_WITH_MATRIX   = 0x198;

constexpr uintptr_t BONE_MATRIX_OFFSET = 0x110; // in a bone's pool entry
constexpr uint32 ROOT_BONE             = 1;
constexpr uint32 CHEST_BONE            = 3;

// Squared distance the root bone may move in one update before the chain snaps to the pose instead of being dragged.
constexpr float TELEPORT_DISTANCE_SQUARED = 150.0f * 150.0f;

using WithMatrixFunc = void(__fastcall*)(ModelData* modelData, byte8* matrix);
using ActorFunc      = uintptr_t(__fastcall*)(byte8* actorBaseAddr);

std::unique_ptr<Utility::Detour_t> s_updateCoatHook;
std::unique_ptr<Utility::Detour_t> s_resetClothHook;
std::unique_ptr<Utility::Detour_t> s_drawCoatHook;

bool ShouldAttach() {
    switch (activeCrimsonConfig.Visual.danteAmulet) {
    case DANTE_AMULET::PERMANENT: {
        return true;
    }
    case DANTE_AMULET::ON: {
        auto& sessionData = *reinterpret_cast<SessionData*>(appBaseAddr + 0xC8F250);
        return (sessionData.mission >= 1) && (sessionData.mission <= LAST_MISSION);
    }
    }

    return false;
}

byte8* GetBoneMatrix(PlayerActorDataDante& actorData, uint32 bone) {
    auto metadata = actorData.newModelPhysicsMetadataPool[0][bone];
    if (!metadata) {
        return 0;
    }

    return *reinterpret_cast<byte8**>(reinterpret_cast<byte8*>(metadata) + BONE_MATRIX_OFFSET);
}

void CallWithMatrix(ModelData& modelData, uintptr_t vtableOffset, byte8* matrix) {
    auto vtable = *reinterpret_cast<byte8**>(&modelData);
    if (!vtable) {
        return;
    }

    auto func = *reinterpret_cast<WithMatrixFunc*>(vtable + vtableOffset);
    func(&modelData, matrix);
}

bool IsUsable(PlayerActorDataDante& actorData) {
    return activeConfig.Actor.enable && actorData.newAmuletActive && (actorData.activeModelIndex == 0);
}

void SnapToPose(PlayerActorDataDante& actorData, byte8* chestMatrix) {
    CrimsonReversedCalls::ClothSnapToPose_sub_1402CA0A0(
        &actorData.newDevilSubmodelPhysicsData[AMULET_CLOTH], &actorData.newSubmodelData[AMULET_SUBMODEL], chestMatrix, 0, 1);
}

bool HasTeleported(const byte8* matrix, const vec4* prevMatrix) {
    auto position = reinterpret_cast<const float*>(matrix) + 12;
    float dx      = position[0] - prevMatrix[3].x;
    float dy      = position[1] - prevMatrix[3].y;
    float dz      = position[2] - prevMatrix[3].z;

    return ((dx * dx) + (dy * dy) + (dz * dz)) > TELEPORT_DISTANCE_SQUARED;
}

// Runs the same steps the coat gets in UpdateCPlDanteCoat_sub_140212070.
void Update(PlayerActorDataDante& actorData) {
    if (!IsUsable(actorData)) {
        return;
    }

    auto rootMatrix  = GetBoneMatrix(actorData, ROOT_BONE);
    auto chestMatrix = GetBoneMatrix(actorData, CHEST_BONE);
    if (!rootMatrix || !chestMatrix) {
        return;
    }

    auto& modelData   = actorData.newSubmodelData[AMULET_SUBMODEL];
    auto& physicsData = actorData.newDevilSubmodelPhysicsData[AMULET_CLOTH];
    auto prevMatrix   = reinterpret_cast<byte8*>(actorData.newAmuletPrevMatrix);

    if (actorData.newAmuletNeedsSnap || HasTeleported(rootMatrix, actorData.newAmuletPrevMatrix)) {
        actorData.newAmuletNeedsSnap = false;
        SnapToPose(actorData, chestMatrix);
    } else {
        CrimsonReversedCalls::RigidnessCPlPlayerCoat_sub_1402C9DC0(&physicsData, &modelData, rootMatrix, prevMatrix);
    }

    CallWithMatrix(modelData, VTBL_UPDATE_WITH_MATRIX, chestMatrix);

    std::memcpy(prevMatrix, rootMatrix, sizeof(actorData.newAmuletPrevMatrix));
}

// Same as the coat in CPlDanteDrawCoat_sub_140218960. The model is hidden in Devil forms by not being drawn.
void Draw(PlayerActorDataDante& actorData) {
    if (!IsUsable(actorData)) {
        return;
    }

    auto chestMatrix = GetBoneMatrix(actorData, CHEST_BONE);
    if (!chestMatrix) {
        return;
    }

    auto& modelData = actorData.newSubmodelData[AMULET_SUBMODEL];
    auto drawBlock  = reinterpret_cast<byte8*>(&modelData) + 0x80; // CDraw_GetInternal_sub_140089DE0

    if (actorData.visibility == 2) {
        CrimsonReversedCalls::CDrawEffectBlock_SetQuicksilverMatrix_sub_140302150(drawBlock);
    } else {
        CrimsonReversedCalls::CDrawEffectBlock_ClearColorMatrix_sub_140302370(drawBlock);
    }

    CallWithMatrix(modelData, VTBL_DRAW_WITH_MATRIX, chestMatrix);
}

// Teleports, room changes and Devil Trigger exits reset the coat's cloth, so the amulet follows.
void ResetCloth(PlayerActorDataDante& actorData) {
    if (!activeConfig.Actor.enable || !actorData.newAmuletActive) {
        return;
    }

    auto rootMatrix  = GetBoneMatrix(actorData, ROOT_BONE);
    auto chestMatrix = GetBoneMatrix(actorData, CHEST_BONE);
    if ((actorData.activeModelIndex != 0) || !rootMatrix || !chestMatrix) {
        actorData.newAmuletNeedsSnap = true;
        return;
    }

    SnapToPose(actorData, chestMatrix);
    std::memcpy(actorData.newAmuletPrevMatrix, rootMatrix, sizeof(actorData.newAmuletPrevMatrix));
}

uintptr_t __fastcall UpdateCoatHook(byte8* actorBaseAddr) {
    uintptr_t trampoline = s_updateCoatHook->GetTrampoline();
    auto result          = reinterpret_cast<ActorFunc>(trampoline)(actorBaseAddr);

    if (actorBaseAddr) {
        Update(*reinterpret_cast<PlayerActorDataDante*>(actorBaseAddr));
    }

    return result;
}

uintptr_t __fastcall ResetClothHook(byte8* actorBaseAddr) {
    uintptr_t trampoline = s_resetClothHook->GetTrampoline();
    auto result          = reinterpret_cast<ActorFunc>(trampoline)(actorBaseAddr);

    if (actorBaseAddr) {
        ResetCloth(*reinterpret_cast<PlayerActorDataDante*>(actorBaseAddr));
    }

    return result;
}

uintptr_t __fastcall DrawCoatHook(byte8* actorBaseAddr) {
    uintptr_t trampoline = s_drawCoatHook->GetTrampoline();
    auto result          = reinterpret_cast<ActorFunc>(trampoline)(actorBaseAddr);

    if (actorBaseAddr) {
        Draw(*reinterpret_cast<PlayerActorDataDante*>(actorBaseAddr));
    }

    return result;
}

} // namespace

void Register(PlayerActorDataDante& actorData) {
    actorData.newAmuletActive = false;

    if (!ShouldAttach()) {
        return;
    }

    auto& file       = File_staticFiles[pl011];
    auto modelFile   = file[AMULET_MODEL_FILE];
    auto clothFile   = file[AMULET_CLOTH_FILE];
    auto textureFile = file[AMULET_TEXTURE_FILE];
    if (!modelFile || !clothFile || !textureFile) {
        return;
    }

    auto& modelData   = actorData.newSubmodelData[AMULET_SUBMODEL];
    auto& physicsData = actorData.newDevilSubmodelPhysicsData[AMULET_CLOTH];
    auto pool         = actorData.newDevilModelPhysicsMetadataPool[AMULET_POOL];

    RegisterModel(modelData, modelFile, textureFile);

    func_8A000(modelData, 0, pool);

    actorData.newSubmodelInit[AMULET_SUBMODEL] = true;

    RegisterPhysics(clothFile, &physicsData, pool);

    auto vertices      = reinterpret_cast<vec4*>(appBaseAddr + COLLIDER_VERTICES_OFFSET);
    auto& metadata     = actorData.newAmuletModelMetadata;
    metadata.count       = 4;
    metadata.vertices[0] = vertices[23];
    metadata.vertices[1] = vertices[24];
    metadata.vertices[2] = vertices[25];

    func_2CA2F0(physicsData, actorData.newModelPhysicsMetadataPool[0], (appBaseAddr + AMULET_LINK_TABLE_OFFSET), &metadata, 1);

    actorData.newAmuletNeedsSnap = true;
    actorData.newAmuletActive    = true;
}

void InitHooks() {
    using namespace Utility;

    static bool run = false;
    if (run) {
        return;
    }
    run = true;

    s_updateCoatHook = std::make_unique<Detour_t>(
        (uintptr_t)appBaseAddr + UPDATE_COAT_OFFSET, &UpdateCoatHook, 0, "UpdateCPlDanteCoat_sub_140212070");
    s_resetClothHook = std::make_unique<Detour_t>(
        (uintptr_t)appBaseAddr + RESET_CLOTH_OFFSET, &ResetClothHook, 0, "CPlDanteResetCloth_sub_140216680");
    s_drawCoatHook = std::make_unique<Detour_t>(
        (uintptr_t)appBaseAddr + DRAW_COAT_OFFSET, &DrawCoatHook, 0, "CPlDanteDrawCoat_sub_140218960");

    s_updateCoatHook->Toggle(true);
    s_resetClothHook->Toggle(true);
    s_drawCoatHook->Toggle(true);
}

} // namespace CrimsonCostume
