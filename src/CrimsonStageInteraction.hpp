#pragma once

namespace CrimsonStageInteraction {

// Lets every player trigger room zones (doors, teleports, event triggers, catapults); the game only checks player 1.
// Also makes the Combat Adjudicators test the weapon of the player that hit them instead of player 1's.
void InitHooks();

}
