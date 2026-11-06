#include "CrimsonStageInteraction.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <memory>

#include "Core/Core.hpp"
#include "Utility/Detour.hpp"
#include "Global.hpp"
#include "Config.hpp"
#include "DMC3Input.hpp"
#include "Vars.hpp"

// The room zone updaters test pool_10222[3] (player 1) against every zone, so only player 1 can walk through a
// door or trigger an event. For the duration of an updater the zone test checks every player instead and
// pool_10222[3] is pointed at the player that is inside (and pressing, for interact zones), so everything the
// updater then does to "the player" lands on that player. Latches and flags stay shared, which makes a zone
// enter once when the first player gets in and exit once when the last one leaves.

namespace CrimsonStageInteraction {

namespace {

constexpr uintptr_t PLAYER_POOL_OFFSET = 0xC90E28;
constexpr size_t POOL_MAIN_ACTOR       = 3;    // pool_10222[3]
constexpr size_t POOL_LOCK_FLAG        = 0x10; // byte, 1 while the player is locked out (cutscene)
constexpr uint16 INTERACT_BUTTON       = 0x20;
constexpr uint32 STATE_GROUNDED        = 1;

constexpr uintptr_t UPDATE_DOOR_ZONES_OFFSET  = 0x1A5EB0;
constexpr uintptr_t UPDATE_EVENT_ZONES_OFFSET = 0x1A9420;
constexpr uintptr_t CATAPULT_UPDATE_OFFSET    = 0x24D810;
constexpr uintptr_t ZONE_TEST_OFFSET          = 0x2CEB30;
constexpr uintptr_t INTERACT_CHECK_OFFSET     = 0x1FA750;
constexpr uintptr_t INTERACT_LOCK_OFFSET      = 0x1FD100;
constexpr uintptr_t SCRIPT_CONDITION_OFFSET   = 0x1A8320;
constexpr uintptr_t ITEM_KEY_UPDATE_OFFSET    = 0x1B5720;
constexpr uintptr_t ADJUDICATOR_HIT_OFFSET    = 0x24FCD0;

// The attacker handed to a collision callback is the collision interface embedded in its actor.
constexpr size_t COLLISION_INTERFACE_OFFSET = 0x60;

constexpr size_t ZONE_MGR_BUSY_OFFSET = 0xAA8; // CRoomZoneMgr_t.transitionBusy

// EventZone_t: flag id +0, type +4, zone +0x20. Type 0 raises a room flag while a player is inside.
constexpr size_t EVENT_ZONE_TYPE_OFFSET = 4;
constexpr size_t EVENT_ZONE_ZONE_OFFSET = 0x20;
constexpr uint8_t EVENT_ZONE_FLAG_TYPE  = 0;

// CItemKey: world position (x, y, z) and the range the pickup is tested in.
constexpr size_t ITEM_KEY_POSITION_OFFSET = 0xF0;
constexpr float ITEM_KEY_RANGE            = 200.0f;

using ZoneUpdateFn   = void(__fastcall*)(void*);
using EventZonesFn   = __int64(__fastcall*)(void*);
using ZoneTestFn     = bool(__fastcall*)(void*, const void*, const void*);
using InteractTestFn = bool(__fastcall*)(void*);
using InteractLockFn = void(__fastcall*)(void*);
using ScriptFn       = __int64(__fastcall*)(void*);
using HitCallbackFn  = void(__fastcall*)(void*, void*, void*, void*);

enum class Zone { None, Door, Event, Catapult, Script, Item };

struct State {
	Zone zone = Zone::None;
	uintptr_t* pool = nullptr;
	uintptr_t mainActor = 0; // pool[3] as the game had it
	int playerCount = 0;
	// Door zones test the interact button first and the zone second: the players that pressed it
	// are the only ones the next zone test may pick.
	uint8_t pressedMask = 0;
	bool pressedPending = false;
	// Event zones test the zone first and the button second.
	uint8_t insideMask = 0;
	// Players inside a flag zone this tick. Room scripts raise on those flags and then wait for the interact button.
	uint8_t flagZoneMask = 0;
	// Players standing in a flag zone: the game blocks doors while the one it checks is in one (transitionBusy).
	uint8_t busyMask = 0;
	unsigned rotate = 0;
};
State s;

std::unique_ptr<Utility::Detour_t> s_doorZonesHook;
std::unique_ptr<Utility::Detour_t> s_eventZonesHook;
std::unique_ptr<Utility::Detour_t> s_catapultHook;
std::unique_ptr<Utility::Detour_t> s_zoneTestHook;
std::unique_ptr<Utility::Detour_t> s_interactTestHook;
std::unique_ptr<Utility::Detour_t> s_interactLockHook;
std::unique_ptr<Utility::Detour_t> s_scriptHook;
std::unique_ptr<Utility::Detour_t> s_itemKeyHook;
std::unique_ptr<Utility::Detour_t> s_adjudicatorHitHook;

PlayerActorData* PlayerActor(int player) {
	if (player == 0) {
		return reinterpret_cast<PlayerActorData*>(s.mainActor);
	}
	auto& playerData = GetPlayerData(static_cast<uint8>(player));
	auto& newActorData = GetNewActorData(static_cast<uint8>(player), playerData.activeCharacterIndex, ENTITY::MAIN);
	return reinterpret_cast<PlayerActorData*>(newActorData.baseAddr);
}

PlayerActorData* LivingPlayerActor(int player) {
	auto* actor = PlayerActor(player);
	return (actor && !actor->dead) ? actor : nullptr;
}

int PlayerIndexOf(const void* actor) {
	for (int player = 0; player < s.playerCount; player++) {
		if (PlayerActor(player) == actor) {
			return player;
		}
	}
	return -1;
}

bool Begin(Zone zone) {
	if (!activeConfig.Actor.enable || activeConfig.Actor.playerCount < 2) {
		return false;
	}
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);
	if (!pool || !pool[POOL_MAIN_ACTOR]) {
		return false;
	}
	s.zone = zone;
	s.pool = pool;
	s.mainActor = pool[POOL_MAIN_ACTOR];
	s.playerCount = (std::min)(static_cast<int>(activeConfig.Actor.playerCount), static_cast<int>(PLAYER_COUNT));
	s.pressedMask = 0;
	s.pressedPending = false;
	s.insideMask = 0;
	s.busyMask = 0;
	if (zone == Zone::Event) {
		s.flagZoneMask = 0;
	}
	return true;
}

void End() {
	s.pool[POOL_MAIN_ACTOR] = s.mainActor;
	s.zone = Zone::None;
}

void SetMainActor(int player) {
	s.pool[POOL_MAIN_ACTOR] = reinterpret_cast<uintptr_t>(PlayerActor(player));
}

bool Pressing(int player, PlayerActorData& actor) {
	if (player == 0) {
		return s_interactTestHook->GetTrampoline<InteractTestFn>()(&actor);
	}
	// Same conditions as IsActorPressingInteract_sub_1401FA750. The pad is read directly: actor.buttons is only
	// refreshed when the actor updates, after the zones, which would let the press through to the style action for a tick.
	// The game tests the pressed edge (buttons[2]), not the held state.
	const bool locked = *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(s.pool) + POOL_LOCK_FLAG) == 1;
	const bool held = (GetGamepad(actor.newGamepad).buttons[2] & actor.newButtonMask & INTERACT_BUTTON) != 0;
	const bool grounded = (actor.state & STATE_GROUNDED) != 0;
	if (held && (locked || actor.dead || !grounded)) {
		static ULONGLONG lastLog = 0;
		const ULONGLONG now = GetTickCount64();
		if (now - lastLog > 1000) {
			lastLog = now;
			Log("Stage interaction: player %d holds interact but is blocked (pool lock %d, dead %d, state %x)", player, locked ? 1 : 0,
				actor.dead ? 1 : 0, static_cast<uint32>(actor.state));
		}
	}
	return !locked && !actor.dead && held && grounded;
}

// IsPointInZone_sub_1402CEB30
bool __fastcall ZoneTestHook(void* context, const void* position, const void* zone) {
	const auto original = s_zoneTestHook->GetTrampoline<ZoneTestFn>();
	if (s.zone == Zone::None || s.zone == Zone::Script || s.zone == Zone::Item) {
		return original(context, position, zone);
	}

	uint8_t candidates = static_cast<uint8_t>((1u << s.playerCount) - 1) & ~s.busyMask;
	if (s.pressedPending) {
		candidates &= s.pressedMask;
	}
	s.pressedPending = false;
	s.pressedMask = 0;
	s.insideMask = 0;

	// Catapults only affect the player they find, so take turns.
	const int first = (s.zone == Zone::Catapult) ? static_cast<int>(s.rotate++ % s.playerCount) : 0;
	PlayerActorData* chosen = nullptr;
	for (int i = 0; i < s.playerCount; i++) {
		const int player = (first + i) % s.playerCount;
		if (!(candidates & (1u << player))) {
			continue;
		}
		auto* actor = LivingPlayerActor(player);
		if (!actor || !original(context, &actor->position, zone)) {
			continue;
		}
		s.insideMask |= static_cast<uint8_t>(1u << player);
		if (!chosen) {
			chosen = actor;
		}
	}

	if (s.zone == Zone::Event && s.insideMask) {
		const auto entry = reinterpret_cast<const uint8_t*>(zone) - EVENT_ZONE_ZONE_OFFSET;
		if (entry[EVENT_ZONE_TYPE_OFFSET] == EVENT_ZONE_FLAG_TYPE) {
			s.flagZoneMask |= s.insideMask;
		}
	}

	s.pool[POOL_MAIN_ACTOR] = chosen ? reinterpret_cast<uintptr_t>(chosen) : s.mainActor;
	return chosen != nullptr;
}

// IsActorPressingInteract_sub_1401FA750
bool __fastcall InteractTestHook(void* actorAddr) {
	const auto original = s_interactTestHook->GetTrampoline<InteractTestFn>();
	if (s.zone == Zone::None || s.zone == Zone::Catapult) {
		return original(actorAddr);
	}
	if (s.zone == Zone::Item) {
		// pool_10222[3] already points at the player this item picked.
		const int player = PlayerIndexOf(actorAddr);
		return player <= 0 ? original(actorAddr) : Pressing(player, *reinterpret_cast<PlayerActorData*>(actorAddr));
	}

	uint8_t pressed = 0;
	if (s.zone == Zone::Script && !s.flagZoneMask) {
		return original(actorAddr);
	}
	for (int player = 0; player < s.playerCount; player++) {
		if (s.zone == Zone::Event && !(s.insideMask & (1u << player))) {
			continue;
		}
		if (s.zone == Zone::Script && !(s.flagZoneMask & (1u << player))) {
			continue;
		}
		if (s.zone == Zone::Door && (s.busyMask & (1u << player))) {
			continue;
		}
		auto* actor = LivingPlayerActor(player);
		if (actor && Pressing(player, *actor)) {
			pressed |= static_cast<uint8_t>(1u << player);
		}
	}

	if (s.zone == Zone::Door) {
		s.pressedMask = pressed;
		s.pressedPending = pressed != 0;
		return pressed != 0;
	}
	if (!pressed) {
		return false;
	}
	for (int player = 0; player < s.playerCount; player++) {
		if (pressed & (1u << player)) {
			SetMainActor(player);
			break;
		}
	}
	return true;
}

// SetActorInteractLock_sub_1401FD100: the script condition locks the player that pressed, then goes back to player 1.
void __fastcall InteractLockHook(void* actorAddr) {
	s_interactLockHook->GetTrampoline<InteractLockFn>()(actorAddr);
	if (s.zone == Zone::Script) {
		s.pool[POOL_MAIN_ACTOR] = s.mainActor;
	}
}

// UpdateDoorTriggerZones_sub_1401A5EB0: doors and teleports.
void __fastcall UpdateDoorZonesHook(void* zones) {
	const auto original = s_doorZonesHook->GetTrampoline<ZoneUpdateFn>();
	if (!Begin(Zone::Door)) {
		return original(zones);
	}
	// transitionBusy is raised while the player the game checks (player 1) is in a flag zone. Make it per player:
	// clear it and keep the players that are in a flag zone out of the door tests instead.
	auto& busy = *(reinterpret_cast<uint8_t*>(zones) + ZONE_MGR_BUSY_OFFSET);
	const uint8_t savedBusy = busy;
	if (busy && s.flagZoneMask) {
		s.busyMask = s.flagZoneMask;
		busy = 0;
	}
	original(zones);
	if (s.busyMask) {
		busy = savedBusy;
	}
	End();
}

// UpdateEventTriggerZones_sub_1401A9420: event triggers, press-to-interact zones, fade zones.
__int64 __fastcall UpdateEventZonesHook(void* zones) {
	const auto original = s_eventZonesHook->GetTrampoline<EventZonesFn>();
	if (!Begin(Zone::Event)) {
		return original(zones);
	}
	const __int64 result = original(zones);
	End();
	return result;
}

// CStageSetCatapult_Update_sub_14024D810
void __fastcall CatapultUpdateHook(void* catapult) {
	const auto original = s_catapultHook->GetTrampoline<ZoneUpdateFn>();
	if (!Begin(Zone::Catapult)) {
		return original(catapult);
	}
	original(catapult);
	End();
}

// sub_1401A8320: room script conditions, including "interact button pressed" (the stage objects with messages).
__int64 __fastcall ScriptConditionHook(void* mission) {
	const auto original = s_scriptHook->GetTrampoline<ScriptFn>();
	const uint8_t flagZoneMask = s.flagZoneMask;
	if (!Begin(Zone::Script)) {
		return original(mission);
	}
	s.flagZoneMask = flagZoneMask;
	const __int64 result = original(mission);
	End();
	return result;
}

// sub_1401B5720: CItemKey pickup. Tested against player 1 only; use the player in range, preferring the one pressing.
void __fastcall ItemKeyUpdateHook(void* item) {
	const auto original = s_itemKeyHook->GetTrampoline<ZoneUpdateFn>();
	if (!Begin(Zone::Item)) {
		return original(item);
	}

	const auto position = reinterpret_cast<const float*>(reinterpret_cast<uintptr_t>(item) + ITEM_KEY_POSITION_OFFSET);
	int chosen = -1;
	float chosenDistance = FLT_MAX;
	bool chosenPressing = false;
	for (int player = 0; player < s.playerCount; player++) {
		auto* actor = LivingPlayerActor(player);
		if (!actor) {
			continue;
		}
		const float dx = actor->position.x - position[0];
		const float dz = actor->position.z - position[2];
		const float distance = sqrtf(dx * dx + dz * dz);
		if (distance >= ITEM_KEY_RANGE || fabsf(actor->position.y - position[1]) >= ITEM_KEY_RANGE) {
			continue;
		}
		const bool pressing = Pressing(player, *actor);
		if ((pressing && !chosenPressing) || (pressing == chosenPressing && distance < chosenDistance)) {
			chosen = player;
			chosenDistance = distance;
			chosenPressing = pressing;
		}
	}
	if (chosen > 0) {
		SetMainActor(chosen);
	}
	original(item);
	End();
}

// CStageSetCombo_OnHit_sub_14024FCD0: Combat Adjudicator hit callback. The weapon test
// (CStageSetCombo_IsWeaponMatch_sub_14024F920) reads the equipped weapon of pool_10222[3], so give it the attacker.
void __fastcall AdjudicatorHitHook(void* statueInterface, void* hitInfo, void* attacker, void* position) {
	const auto original = s_adjudicatorHitHook->GetTrampoline<HitCallbackFn>();
	if (!attacker || !activeConfig.Actor.enable || activeConfig.Actor.playerCount < 2) {
		return original(statueInterface, hitInfo, attacker, position);
	}
	auto* pool = *reinterpret_cast<uintptr_t**>(appBaseAddr + PLAYER_POOL_OFFSET);
	if (!pool || !pool[POOL_MAIN_ACTOR]) {
		return original(statueInterface, hitInfo, attacker, position);
	}

	const auto attackerActor = reinterpret_cast<uintptr_t>(attacker) - COLLISION_INTERFACE_OFFSET;
	const int playerCount    = (std::min)(static_cast<int>(activeConfig.Actor.playerCount), static_cast<int>(PLAYER_COUNT));
	bool isOtherPlayer       = false;
	for (int player = 1; player < playerCount; player++) {
		if (reinterpret_cast<uintptr_t>(PlayerActor(player)) == attackerActor) {
			isOtherPlayer = true;
			break;
		}
	}
	if (!isOtherPlayer) {
		return original(statueInterface, hitInfo, attacker, position);
	}

	const uintptr_t mainActor = pool[POOL_MAIN_ACTOR];
	pool[POOL_MAIN_ACTOR]     = attackerActor;
	original(statueInterface, hitInfo, attacker, position);
	pool[POOL_MAIN_ACTOR] = mainActor;
}

} // namespace

void InitHooks() {
	using namespace Utility;

	static bool run = false;
	if (run) {
		return;
	}
	run = true;

	s_doorZonesHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_DOOR_ZONES_OFFSET, &UpdateDoorZonesHook, 0, "UpdateDoorTriggerZones_sub_1401A5EB0");
	s_eventZonesHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + UPDATE_EVENT_ZONES_OFFSET, &UpdateEventZonesHook, 0, "UpdateEventTriggerZones_sub_1401A9420");
	s_catapultHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + CATAPULT_UPDATE_OFFSET, &CatapultUpdateHook, 0, "CStageSetCatapult_Update_sub_14024D810");
	s_zoneTestHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + ZONE_TEST_OFFSET, &ZoneTestHook, 0, "IsPointInZone_sub_1402CEB30");
	s_interactTestHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + INTERACT_CHECK_OFFSET, &InteractTestHook, 0, "IsActorPressingInteract_sub_1401FA750");
	s_interactLockHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + INTERACT_LOCK_OFFSET, &InteractLockHook, 0, "SetActorInteractLock_sub_1401FD100");
	s_scriptHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + SCRIPT_CONDITION_OFFSET, &ScriptConditionHook, 0, "EvalCEventMissionCondition_sub_1401A8320");
	s_itemKeyHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + ITEM_KEY_UPDATE_OFFSET, &ItemKeyUpdateHook, 0, "CItemKey_UpdateIdle_sub_1401B5720");
	s_adjudicatorHitHook = std::make_unique<Detour_t>(
		(uintptr_t)appBaseAddr + ADJUDICATOR_HIT_OFFSET, &AdjudicatorHitHook, 0, "CStageSetCombo_OnHit_sub_14024FCD0");

	s_doorZonesHook->Toggle(true);
	s_eventZonesHook->Toggle(true);
	s_catapultHook->Toggle(true);
	s_zoneTestHook->Toggle(true);
	s_interactTestHook->Toggle(true);
	s_interactLockHook->Toggle(true);
	s_scriptHook->Toggle(true);
	s_itemKeyHook->Toggle(true);
	s_adjudicatorHitHook->Toggle(true);
}

} // namespace CrimsonStageInteraction
