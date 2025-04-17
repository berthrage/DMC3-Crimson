#include "CrimsonLDK.hpp"
#include "Utility/Detour.hpp"
#include "../Core/Core.hpp"
#include <memory>
#include <array>
#include "imgui.h"
#include <cassert>

static constexpr auto SPAWN_GUYS_PROC_OFFSET() { return 0x1A4680; }

static int  s_dudeMultiplier {3};
static int  s_bossMultiplier {1};

static bool s_ldkBosses {false};
static bool s_ldkEnable {true};

static std::unique_ptr<Utility::Detour_t> s_cEmsetCtrlSpawnGuyHook;

// listing vtables here
static constexpr std::array buggedEnemies {
	uintptr_t(0xFEFEFEFE), // Giga-Pete  | CEm023 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Clown      | CEm037 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Horse      | CEm029 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Clone      | CEm031 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Blob       | CEm032 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Dog        | CEm025 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : IComAction : IComActionState
};

// nullptr dereferences in collision related logic for enemies below 
// or some other weird shit that crashes the game eventually
static constexpr std::array weirdEnemies {
	uintptr_t(0xFEFEFEFE), // Birds      | CEm014 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // ChessPiece | CEm017 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // ChessKing? | CEm021 : CEm017 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
};

static constexpr std::array bossEnemies {
	uintptr_t(0xFEFEFEFE), // Nevan      | CEm028 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Beowulf    | CEm028 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // A&R        | CEm026 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : CComAction : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Vergil     | CEm035 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : IComAction : IComActionState
	uintptr_t(0xFEFEFEFE), // Lady       | CEm034 : CNonPlayer : CActor : CWork : IActor : ICollisionHandle : INonPlayer : CCom : ICom : IComAction : IComActionState
};

static uintptr_t __fastcall cEnemySetCtrl_spawnGuy_sub_1401A4680(uintptr_t pthis, float* a2) {
	typedef uintptr_t (__fastcall *spawGuyTrampoline)(uintptr_t, float*);

	uintptr_t trampoline_raw = s_cEmsetCtrlSpawnGuyHook->GetTrampoline();
	spawGuyTrampoline trampoline = (spawGuyTrampoline)trampoline_raw;

	uintptr_t res = trampoline(pthis, a2);
	if (!res || !s_ldkEnable) {
		return res;
	}
	uintptr_t vtable_pointer = *(uintptr_t*)res; // to check vtable of whatever we just spawned
	int multiplier = s_dudeMultiplier;

	// dont mess with bugged enemies
	for (uintptr_t enemy : buggedEnemies) {
		if (vtable_pointer == enemy) {
			return res;
		}
	}

	// only double weird enemies
	for (uintptr_t enemy : weirdEnemies) {
		if ((vtable_pointer == enemy) && (s_dudeMultiplier > 1)) {
			res = trampoline(pthis, a2);
			return res;
		}
	}

	// boss fights mode
	for (uintptr_t boss : bossEnemies) {
		if ((vtable_pointer == boss) && s_ldkBosses) {
			multiplier = s_bossMultiplier;
		}
	}

	for (int i = 0; i < multiplier - 1; i++) {
		res = trampoline(pthis, a2);
		if (!res) {
			MessageBoxA(NULL, "Failed to spawn a dude", "CrimsonLDK", MB_ICONERROR);
			break;
		}
	}
	return res;
}

void LdkInitDetour() {
	s_cEmsetCtrlSpawnGuyHook =
		std::make_unique<Utility::Detour_t>(
			(uintptr_t)appBaseAddr + SPAWN_GUYS_PROC_OFFSET(),
			(uintptr_t)&cEnemySetCtrl_spawnGuy_sub_1401A4680,
			NULL, "spawn_a_guy_detour");
	bool res = s_cEmsetCtrlSpawnGuyHook->Toggle();
	assert(res);
}

void LdkDrawImGuiWidget() {
	ImGui::Checkbox("Enable LDK", &s_ldkEnable);
	ImGui::Checkbox("Bosses LDK", &s_ldkBosses);
	ImGui::InputInt("Enemy multiplier", &s_dudeMultiplier);
	if(s_ldkBosses) {
		ImGui::InputInt("Bosses multipler", &s_bossMultiplier);
	}
}
