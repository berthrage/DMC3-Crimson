#include "../ThirdParty/glm/glm.hpp"
#pragma once
#include <cstdint>
#include <functional>
#include "Vars.hpp"

namespace CrimsonEnemyAITarget {

void EnemyAIMultiplayerTargettingDetours(bool enable);
uintptr_t EnemyTargetPlayerSelection(const glm::vec3& enemyPosition, bool hasValidEnemyPosition, uintptr_t defaultPlayerAddr);
// Runs fn with the lock-on pool slot and candidate ranges seen from a secondary player's actor (plain call for player 1).
void RunInLockOnPerspective(uintptr_t actorAddr, const std::function<void()>& fn);
// Runs fn with the lock state (locked flags and lock slots on the shared enemy entries) of that actor loaded.
void RunWithActorLockState(uintptr_t actorAddr, const std::function<void()>& fn);

}