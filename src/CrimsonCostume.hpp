#pragma once

#include "Vars.hpp"

namespace CrimsonCostume {

// Dante's amulet is the cloth submodel the coatless costume keeps in its coat slot, so a coat costume can't hold both.
// This loads it as a second submodel (and cloth chain) next to the coat and keeps it updated and drawn with the actor.

// Called by UpdateModel for Dante costumes that wear a coat, after the costume's own models are registered.
void Register(PlayerActorDataDante& actorData);

void InitHooks();

} // namespace CrimsonCostume
