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
	"0 - Dante's Office",
	"1 - Dante's Office: Front",
	"2 - 66 Slum Avenue",
	"3 - Bullseye",
	"4 - Love Planet",
	"5 - 13th Avenue",
	"6 - Ice Guardian's Chamber",
	"7 - Love Planet",
	"8 - 13th Avenue: Aftermath",
	"9 - Dante's Office: Front",
	"10 - Dante's Office: Front",
	"11 - 66 Slum Avenue",
	"12 - Credits",
	"100 - Chamber of Echoes",
	"101 - Entranceway",
	"102 - Living Statue Room",
	"103 - Silence Statuary",
	"104 - Chamber of Sins",
	"105 - Cursed Skull Chamber",
	"106 - Sun & Moon Chamber",
	"107 - Ice Guardian's Chamber",
	"108 - Entranceway",
	"109 - Chamber of Sins",
	"110 - Incandescent Space",
	"111 - Giantwalker Chamber",
	"112 - Endless Infernum",
	"113 - Surge of Fortunas",
	"114 - Heavenrise Chamber",
	"115 - The Divine Library",
	"116 - Incandescent Space",
	"117 - Endless Infernum",
	"118 - Surge of Fortunas",
	"119 - High-fly Zone",
	"120 - Azure Garden",
	"121 - Firestorm Chamber",
	"122 - Mute Goddess' Chamber",
	"123 - Chamber of 3 Trials",
	"124 - Trial of the Warrior",
	"125 - Trial of Skill",
	"126 - Trial of Wisdom",
	"127 - The Dark Corridor",
	"128 - God-cube Chamber",
	"129 - Tri-sealed Antechamber",
	"130 - Trial of the Warrior",
	"131 - Trial of Skill",
	"132 - Trial of Wisdom",
	"133 - The Dark Corridor",
	"134 - Pitch-black Void",
	"135 - Skull Spire",
	"136 - Tranquil Souls Room",
	"137 - Lift Room",
	"138 - Moonlight Mile",
	"139 - Apparition Incarnate",
	"140 - Pitch-black Void",
	"141 - Skull Spire",
	"142 - Peak of Darkness",
	"143 - Dark-pact Chamber",
	"144 - Peak of Darkness",
	"145 - Astral Chamber",
	"146 - Waking Sun Chamber",
	"200 - Forbidden Land: Front",
	"201 - The Rotating Bridge",
	"202 - Provisions Storeroom",
	"203 - Subterranean Garden",
	"204 - Subground Water Vein",
	"205 - Rounded Pathway",
	"206 - Subterranean Lake",
	"207 - Rounded Pathway",
	"208 - Provisions Storeroom",
	"209 - Limestone Cavern",
	"210 - Sunken Opera House",
	"211 - Marble Throughway",
	"212 - Gears of Madness",
	"213 - Altar of Evil Pathway",
	"214 - Altar of Evil",
	"215 - Debug Room",
	"216 - Temperance Wagon",
	"217 - Torture Chamber",
	"218 - Spiral Corridor",
	"219 - Devilsprout Lift",
	"220 - Debug Room",
	"221 - Subterran Garden",
	"222 - Upper Subterran Garden",
	"223 - Debug Room",
	"224 - Rounded Pathway",
	"225 - Subterran Lake",
	"226 - Top Subterria Lack",
	"227 - Rounded Pathway",
	"228 - Underground Arena",
	"229 - Effervescence Corridor",
	"230 - Spiral Staircase",
	"231 - Top Obsidian Path",
	"232 - Obsidian Path",
	"233 - Vestibule",
	"234 - Lair of Judgement",
	"235 - Underwater Elevator",
	"236 - Hell's Highway",
	"237 - Lair of Judgement Ruins",
	"238 - Underground Arena",
	"239 - Temperance Wagon",
	"240 - Temperance Wagon",
	"241 - Lux-luminous Corridor",
	"300 - Leviathan's Stomach",
	"301 - Leviathan's Intestines",
	"302 - Leviathan's Heartcore",
	"303 - Leviathan's Intestines",
	"304 - Leviathan's Intestines",
	"305 - Leviathan's Retina",
	"306 - Leviathan's Intestines",
	"307 - Leviathan's Intestines",
	"308 - Leviathan's Stomach",
	"309 - Leviathan's Intestines",
	"310 - Leviathan's Heartcore (Debug Room)",
	"311 - Leviathan's Intestines",
	"312 - Leviathan's Intestines",
	"313 - Leviathan's Retina",
	"400 - Unsacred Hellgate",
	"401 - Damned Chess Board",
	"402 - Road to Despair",
	"403 - Lost Souls Nirvana",
	"404 - Infinity Nirvana",
	"405 - Nirvana of Illusions",
	"406 - Room of Fallen Ones",
	"407 - Debug Room",
	"408 - End of the Line",
	"409 - Forbidden Nirvana",
	"410 - No use",
	"411 - Unsacred Hellgate",
	"412 - Ice Guardian Reborn",
	"413 - Giantwalker Reborn",
	"414 - Firestorm Reborn",
	"415 - Lightning Witch Reborn",
	"416 - Lightbeast Reborn",
	"417 - Timesteed Reborn",
	"418 - Deathvoid Reborn",
	"419 - Evil God-beast Reborn",
	"420 - Demon Army Reborn",
	"421 - Forbidden Nirvana",
	"422 - Demon Clown Chamber",
	"423 - Bloody Palace",
	"424 - Bloody Palace",
	"425 - Bloody Palace",
	"426 - Bloody Palace",
	"427 - Bloody Palace",
	"428 - Bloody Palace",
	"429 - Bloody Palace",
	"430 - Bloody Palace",
	"431 - Bloody Palace",
	"432 - Bloody Palace",
	"433 - Bloody Palace Boss",
	"434 - Bloody Palace Boss",
	"435 - Bloody Palace Boss",
	"436 - Bloody Palace Boss",
	"437 - Bloody Palace Boss",
	"438 - Bloody Palace Boss",
	"439 - Bloody Palace Boss",
	"440 - Bloody Palace Boss",
	"441 - Bloody Palace Boss",
	"442 - Bloody Palace Boss",
	"443 - Bloody Palace Boss",
	"444 - Bloody Palace Boss",
	"445 - Bloody Palace Boss",
	"446 - Bloody Palace Boss",
	"447 - Bloody Palace Boss",
	"448 - Demon Clown Chamber",
	"449 - Demon Clown Chamber",
	"600 - Secret Mission",
	"601 - Secret Mission",
	"602 - Secret Mission",
	"603 - Secret Mission",
	"604 - Secret Mission",
	"605 - Secret Mission",
	"606 - Secret Mission",
	"607 - Secret Mission",
	"608 - Secret Mission",
	"609 - Secret Mission",
	"610 - Secret Mission",
	"611 - Secret Mission",
	"900 - Debug Room",
	"901 - Debug Room",
	"902 - Debug Room",
	"903 - Debug Room",
	"904 - Debug Room",
	"905 - Debug Room",
	"906 - Debug Room",
	"907 - Debug Room",
	"908 - Debug Room",
	"909 - Debug Room",
	"910 - Debug Room"
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

constexpr uint32 roomsMapu32[ROOMS_COUNT] = {
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
float g_cerbDamageValue = 1.0f;
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
