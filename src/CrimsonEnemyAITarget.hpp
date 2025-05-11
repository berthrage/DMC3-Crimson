#include "../ThirdParty/glm/glm.hpp"
#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace CrimsonEnemyAITarget {

void EnemyAIMultiplayerTargettingDetours(bool enable);
uintptr_t EnemyTargetPlayerSelection(const glm::vec3& enemyPosition, bool hasValidEnemyPosition, uintptr_t defaultPlayerAddr);

}