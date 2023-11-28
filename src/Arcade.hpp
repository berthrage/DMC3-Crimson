#pragma once

#include "Core/DataTypes.hpp"

namespace Arcade {
	void InitSession();
	void SetCharacter(byte8* dest);
	void SetRoom();
	void EventCreateMainActor(byte8* baseAddr);
	void Toggle(bool enable);
}
