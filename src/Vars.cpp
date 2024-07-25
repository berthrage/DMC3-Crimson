// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "Vars.hpp"
#undef VOID
#undef IGNORE

#include <string>
#include <vector>

#pragma region constexpr

const char* itemNames[ITEM::COUNT] = {
    "",
    "",
    "",
    "",
    "",
    "Gold Orb",
    "Yellow Orb",
    "Blue Orb",
    "Purple Orb",
    "Blue Orb Fragment",
    "",
    "",
    "",
    "",
    "",
    "",
    "Vital Star Large",
    "Vital Star Small",
    "Devil Star",
    "Holy Water",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "Astronomical Board",
    "Vajura",
    "",
    "Soul of Steel",
    "Essence of Fighting",
    "Essence of Technique",
    "Essence of Intelligence",
    "Orihalcon Fragment",
    "Siren's Shriek",
    "Crystal Skull",
    "Ignis Fatuus",
    "Ambrosia",
    "Stone Mask",
    "Neo-Generator",
    "Haywire Neo-Generator",
    "Orihalcon",
    "Orihalcon Fragment Right",
    "Orihalcon Fragment Bottom",
    "Orihalcon Fragment Left",
    "Golden Sun",
    "Onyx Moonshard",
    "Samsara",
    "",
    "",
    "",
    "",
};

const char* buyNames[BUY::COUNT] = {
    "Vital Star Small",
    "Vital Star Large",
    "Devil Star",
    "Holy Water",
    "Blue Orb",
    "Purple Orb",
    "Gold Orb",
    "Yellow Orb",
};

const char* enemyNames[ENEMY::COUNT] = {
    "Pride 1",
    "Pride 2",
    "Pride 3",
    "Pride 4",
    "Gluttony 1",
    "Gluttony 2",
    "Gluttony 3",
    "Gluttony 4",
    "Lust 1",
    "Lust 2",
    "Lust 3",
    "Lust 4",
    "Sloth 1",
    "Sloth 2",
    "Sloth 3",
    "Sloth 4",
    "Wrath 1",
    "Wrath 2",
    "Wrath 3",
    "Wrath 4",
    "Greed 1",
    "Greed 2",
    "Greed 3",
    "Greed 4",
    "Abyss",
    "Envy",
    "Hell Vanguard",
    "",
    "Arachne",
    "The Fallen",
    "Dullahan",
    "Enigma",
    "Blood-Goyle",
    "",
    "Soul Eater",
    "Damned Chessmen Pawn",
    "Damned Chessmen Knight",
    "Damned Chessmen Bishop",
    "Damned Chessmen Rook",
    "Damned Chessmen Queen",
    "Damned Chessmen King",
    "Gigapede",
    "",
    "Cerberus",
    "Agni & Rudra All",
    "Agni & Rudra",
    "Agni & Rudra Black",
    "Agni & Rudra Red",
    "Agni & Rudra Blue",
    "Nevan",
    "Geryon",
    "Beowulf",
    "Doppelganger",
    "Arkham",
    "",
    "Lady",
    "",
    "",
    "Vergil",
    "",
    "Leviathan Heart",
    "Leviathan Other",
    "Jester",
    "",
};

Toggle toggle;

#pragma endregion

#pragma region Helpers
WeaponWheelTiming meleeWeaponWheelTiming;
WeaponWheelTiming rangedWeaponWheelTiming;

bool ebonyIvoryCancel = true;

bool triggerVFX = false;

int styleVFXCount    = 0;
int styleVFXCountMax = 1;

float siytimer = 0;

float rainstormMomentum = 0;


float highTimeRotation = 0;

DoubleTap quickDoubleTap;
DoubleTap doppDoubleTap;

bool inGunShoot;
bool gunShootInverted   = false;
bool gunShootNormalized = false;

float distanceToEnemy;

float storedSkyLaunchPosX              = 0;
float storedSkyLaunchPosY              = 0;
float storedSkyLaunchPosZ              = 0;
float storedSkyLaunchRank              = 0;
bool beginSkyLaunch                    = false;
bool skyLaunchSetVolume                = false;
bool skyLaunchForceJustFrameToggledOff = false;
bool skyLaunchTrackerRunning           = false;
bool executingSkyLaunch                = false;
bool appliedSkyLaunchProperties        = false;
bool skyLaunchSetJustFrameTrue         = false;
bool skyLaunchSetJustFrameGround       = false;

bool executingRoyalRelease        = false;
bool royalReleaseTrackerRunning   = false;
bool forcingJustFrameRoyalRelease = false;
bool royalReleaseExecuted         = false;
bool royalReleaseJustFrameCheck   = false;

float storedRisingSunTauntPosY      = 0;
float storedRisingSunTauntPosYClone = 0;
float storedLunarPhasePosY          = 0;

int airStingerEndTimeInt = 0;

AirStingerEnd airStingerEnd;

RankAnnouncer rankAnnouncer[7];

uint16 relativeTiltController = 0;

float doppSeconds           = 0;
float doppSecondsDT         = 0;
bool doppTimeTrackerRunning = false;
float currentDTDoppOn       = 0;
float currentDTDoppDTOn     = 0;
float doppDuration          = 8000;
float doppDurationDT        = 5000;

int storedTrickUpCount         = 0;
int storedSkyStarCount         = 0;
int storedAirHikeCount         = 0;
bool royalCancelTrackerRunning = false;

FasterDarkslayer fasterDarkslayer;

FasterRapidSlash fasterRapidSlash;

bool inRapidSlash;

/*export struct Sounds {

} sounds;*/

bool styleChanged[6] = {false, false, false, false, false, false};

bool g_inCombat = false;
bool g_inBossfight = false;
int g_bossQuantity = 0;
bool g_inGame = false;

bool exceptionShot = false;

SprintVFX sprintVFX;

int notHoldingMelee = 0;

bool crazyComboHold = false;

TrickUpCancel trickUpCancel;

GunsCancel gunsCancel;

RainstormCancel rainstormCancel;

bool inertiaFixesEnabled = false;

bool inRoyalBlock;
bool inGuardfly;
float rainstormPull;

CrimsonPlayerData crimsonPlayer[20];

bool devilTriggerReadyPlayed = false;

bool missionClearSongPlayed = false;


GuiPause guiPause;

DelayedComboFX delayedComboFX;

bool changedFrameRateCorrection = false;
bool gamepadCombinationMainRelease = false;

std::vector<std::string> HUDdirectories;

float frameRateSpeedMultiplier = 0;

#pragma endregion
