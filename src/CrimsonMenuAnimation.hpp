#pragma once

// Menu panels advance their animations by a fixed step every frame instead of by
// real time, so they run faster the higher the frame rate. This scales that step
// back to a 60 Hz step and applies the user's menu speed preference on top.
//
// Cockpit and staff roll are always scaled to 100%: they follow the frame rate
// correction but ignore the menu speed setting.
namespace CrimsonMenuAnimation {

void InitHooks();
void OnTick();

}
