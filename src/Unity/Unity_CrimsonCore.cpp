// Unity build — batches related Crimson core translation units.
// Each #included .cpp contributes its code to this single TU,
// eliminating redundant header parsing across them.

#include "../Crimson.cpp"
#include "../CrimsonDetours.cpp"
#include "../CrimsonPatches.cpp"
#include "../CrimsonUtil.cpp"
