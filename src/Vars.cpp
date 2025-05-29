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

const char* roomNames[ROOMS_COUNT] = {
	"Dante's Office",
	"Dante's Office: Front",
	"66 Slum Avenue",
	"Bullseye",
	"Love Planet",
	"13th Avenue",
	"Ice Guardian's Chamber",
	"Love Planet",
	"13th Avenue: Aftermath",
	"Dante's Office: Front",
	"Dante's Office: Front",
	"66 Slum Avenue",
	"Credits",
	"Chamber of Echoes",
	"Entranceway",
	"Living Statue Room",
	"Silence Statuary",
	"Chamber of Sins",
	"Cursed Skull Chamber",
	"Sun & Moon Chamber",
	"Ice Guardian's Chamber",
	"Entranceway",
	"Chamber of Sins",
	"Incandescent Space",
	"Giantwalker Chamber",
	"Endless Infernum",
	"Surge of Fortunas",
	"Heavenrise Chamber",
	"The Divine Library",
	"Incandescent Space",
	"Endless Infernum",
	"Surge of Fortunas",
	"High-fly Zone",
	"Azure Garden",
	"Firestorm Chamber",
	"Mute Goddess' Chamber",
	"Chamber of 3 Trials",
	"Trial of the Warrior",
	"Trial of Skill",
	"Trial of Wisdom",
	"The Dark Corridor",
	"God-cube Chamber",
	"Tri-sealed Antechamber",
	"Trial of the Warrior",
	"Trial of Skill",
	"Trial of Wisdom",
	"The Dark Corridor",
	"Pitch-black Void",
	"Skull Spire",
	"Tranquil Souls Room",
	"Lift Room",
	"Moonlight Mile",
	"Apparition Incarnate",
	"Pitch-black Void",
	"Skull Spire",
	"Peak of Darkness",
	"Dark-pact Chamber",
	"Peak of Darkness",
	"Astral Chamber",
	"Waking Sun Chamber",
	"Forbidden Land: Front",
	"The Rotating Bridge",
	"Provisions Storeroom",
	"Subterranean Garden",
	"Subground Water Vein",
	"Rounded Pathway",
	"Subterranean Lake",
	"Rounded Pathway",
	"Provisions Storeroom",
	"Limestone Cavern",
	"Sunken Opera House",
	"Marble Throughway",
	"Gears of Madness",
	"Altar of Evil Pathway",
	"Altar of Evil",
	"Debug Room",
	"Temperance Wagon",
	"Torture Chamber",
	"Spiral Corridor",
	"Devilsprout Lift",
	"Debug Room",
	"Subterran Garden",
	"Upper Subterran Garden",
	"Debug Room",
	"Rounded Pathway",
	"Subterran Lake",
	"Top Subterria Lack",
	"Rounded Pathway",
	"Underground Arena",
	"Effervescence Corridor",
	"Spiral Staircase",
	"Top Obsidian Path",
	"Obsidian Path",
	"Vestibule",
	"Lair of Judgement",
	"Underwater Elevator",
	"Hell's Highway",
	"Lair of Judgement Ruins",
	"Underground Arena",
	"Temperance Wagon",
	"Temperance Wagon",
	"Lux-luminous Corridor",
	"Leviathan's Stomach",
	"Leviathan's Intestines",
	"Leviathan's Heartcore",
	"Leviathan's Intestines",
	"Leviathan's Intestines",
	"Leviathan's Retina",
	"Leviathan's Intestines",
	"Leviathan's Intestines",
	"Leviathan's Stomach",
	"Leviathan's Intestines",
	"Leviathan's Heartcore (Debug Room)",
	"Leviathan's Intestines",
	"Leviathan's Intestines",
	"Leviathan's Retina",
	"Unsacred Hellgate",
	"Damned Chess Board",
	"Road to Despair",
	"Lost Souls Nirvana",
	"Infinity Nirvana",
	"Nirvana of Illusions",
	"Room of Fallen Ones",
	"Debug Room",
	"End of the Line",
	"Forbidden Nirvana",
	"No use",
	"Unsacred Hellgate",
	"Ice Guardian Reborn",
	"Giantwalker Reborn",
	"Firestorm Reborn",
	"Lightning Witch Reborn",
	"Lightbeast Reborn",
	"Timesteed Reborn",
	"Deathvoid Reborn",
	"Evil God-beast Reborn",
	"Demon Army Reborn",
	"Forbidden Nirvana",
	"Demon Clown Chamber",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Bloody Palace Boss",
	"Demon Clown Chamber",
	"Demon Clown Chamber",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Secret Mission",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room",
	"Debug Room"
};

constexpr uint16_t roomsMap[ROOMS_COUNT] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	100,
	101,
	102,
	103,
	104,
	105,
	106,
	107,
	108,
	109,
	110,
	111,
	112,
	113,
	114,
	115,
	116,
	117,
	118,
	119,
	120,
	121,
	122,
	123,
	124,
	125,
	126,
	127,
	128,
	129,
	130,
	131,
	132,
	133,
	134,
	135,
	136,
	137,
	138,
	139,
	140,
	141,
	142,
	143,
	144,
	145,
	146,
	200,
	201,
	202,
	203,
	204,
	205,
	206,
	207,
	208,
	209,
	210,
	211,
	212,
	213,
	214,
	215,
	216,
	217,
	218,
	219,
	220,
	221,
	222,
	223,
	224,
	225,
	226,
	227,
	228,
	229,
	230,
	231,
	232,
	233,
	234,
	235,
	236,
	237,
	238,
	239,
	240,
	241,
	300,
	301,
	302,
	303,
	304,
	305,
	306,
	307,
	308,
	309,
	310,
	311,
	312,
	313,
	400,
	401,
	402,
	403,
	404,
	405,
	406,
	407,
	408,
	409,
	410,
	411,
	412,
	413,
	414,
	415,
	416,
	417,
	418,
	419,
	420,
	421,
	422,
	423,
	424,
	425,
	426,
	427,
	428,
	429,
	430,
	431,
	432,
	433,
	434,
	435,
	436,
	437,
	438,
	439,
	440,
	441,
	442,
	443,
	444,
	445,
	446,
	447,
	448,
	449,
	600,
	601,
	602,
	603,
	604,
	605,
	606,
	607,
	608,
	609,
	610,
	611,
	900,
	901,
	902,
	903,
	904,
	905,
	906,
	907,
	908,
	909,
	910
};

ColorPresets colorPresets;

Toggle toggle;

bool g_disableRightStickCenterCamera;

#pragma endregion

#pragma region Helpers

bool ebonyIvoryCancel = true;

bool triggerVFX = false;

int styleVFXCount    = 0;
int styleVFXCountMax = 1;

float siytimer = 0;

DoubleTap quickDoubleTap[PLAYER_COUNT];
DoubleTap doppDoubleTap[PLAYER_COUNT];

bool inGunShoot;
bool gunShootInverted   = false;
bool gunShootNormalized = false;

float distanceToEnemy;

float storedRisingSunTauntPosY      = 0;
float storedRisingSunTauntPosYClone = 0;
float storedLunarPhasePosY          = 0;

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

float g_FrameRate = 0;
float g_FrameRateTimeMultiplier = 0;
float g_FrameRateTimeMultiplierRounded = 0;
bool g_inCombat = false;
bool g_inBossfight = false;
bool g_inCredits = false;
int g_bossQuantity = 0;
bool g_inGameDelayed = false;
bool g_inGameCutscene = false;
bool g_inMainMenu = false;
bool g_allActorsSpawned = false;
std::string g_gameTrackPlaying;

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

float storedHP = 0;
float storedDT = 0;

CrimsonHudData crimsonHud;

CheatsPopUpData cheatsPopUp;

WeaponProgressionData weaponProgression;

GameModeData gameModeData;

CrimsonPlayerData crimsonPlayer[20];

HeldStyleExpData heldStyleExpDataDante;
HeldStyleExpData heldStyleExpDataVergil;

bool devilTriggerReadyPlayed = false;

bool missionClearSongPlayed = false;


GuiPause guiPause;

DelayedComboFX delayedComboFX;

bool changedFrameRateCorrection = false;

std::vector<std::string> HUDdirectories;

float frameRateSpeedMultiplier = 0;

// 0 - 1P, 1 - 1P clone, 2 - 2P, 3 - 2P clone, 4 - 3P, 5 - 3P clone, 6 - 4P, 7 - 4P clone
std::array<float, PLAYER_COUNT * 2> g_plEntityTo1PDistances;
std::array<float, 50> g_enemiesTo1PDistances;
std::array<float, PLAYER_COUNT * 2> g_plEntityCameraDistances;
std::array<SimpleVec3, PLAYER_COUNT * 2> g_plEntityScreenPositions;
std::array<bool, PLAYER_COUNT * 2> g_plEntityOutOfView;
std::array<SimpleVec3, PLAYER_COUNT * 2> g_plEntityPositions;
int g_activeClonesCount;
int g_activePlayableEntitiesCount;
int g_activeAllEntitiesCount;
bool g_isMPCamActive = false;
bool g_isParanoramicCamActive = false;

float g_rotationDifference = 0;
float g_rotationBeforeCalculation = 0;
float g_rotationCalculated = 0;
__declspec(align(16)) float g_customCameraPos[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
__declspec(align(16)) float g_customCameraSensitivity = 0.0f;
bool g_cameraHittingWall = true;

#pragma endregion
