#pragma once

#include "Core/DataTypes.hpp"

typedef byte8* (__fastcall* GetPlayerActorBaseAddressByEffectData_t)(byte8* effectDataAddr);

extern GetPlayerActorBaseAddressByEffectData_t GetPlayerActorBaseAddressByEffectData;

namespace Actor {
	void ToggleBase(bool enable);
}