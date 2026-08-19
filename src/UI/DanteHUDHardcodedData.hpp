#pragma once

#include "DanteHUD.hpp"

#include <vector>

namespace HUD::Dante::HC
{
namespace Sprites 
{
struct SpriteInfo_t
{
	Graphics::Rect RegionRect;
};

enum class SpriteID_t {
	Frame_Compass = 0,
	Panel_T,
	Panel_S,
	Panel_DS,
	Panel_G,
	Panel_DG,
	BG_DS,
	Panel_R,
	Panel_QS,
	BG_G,
	BG_T,
	BG_DG,
	BG_QS,
	Frame_DT_123,
	BG_S,
	BG_R,
	Colored_DG,
	Colored_DS,
	Colored_G,
	Colored_QS,
	Colored_R,
	Colored_S,
	Colored_T,
	Allorbs_DT,
	Complete_S,
	Shadow_S,
	Complete_DS,
	Shadow_DS,
	Complete_G,
	Shadow_G,
	Complete_T,
	Shadow_T,
	Complete_DG,
	Shadow_DG,
	Complete_QS,
	Complete_R,
	Shadow_QS,
	Shadow_R,
	Fore_S,
	Runes_DTE,
	Fore_DS,
	Fore_T,
	Fore_DG,
	Fore_QS,
	Fore_G,
	Fore_R,
	Frame_Exp,
	Written_Colored_DG,
	Written_Crimson_DG,
	Blurred_Colored_R,
	Blurred_Crimson_R,
	Written_Colored_R,
	Written_Crimson_R,
	Written_Colored_G,
	Written_Crimson_G,
	Blurred_Colored_DG,
	Blurred_Colored_G,
	Blurred_Crimson_DG,
	Blurred_Crimson_G,
	Frame_DT_4,
	Blurred_Colored_DS,
	Blurred_Crimson_DS,
	Written_Colored_DS,
	Written_Crimson_DS,
	Frame_DT_5,
	Blurred_Colored_S,
	Blurred_Crimson_S,
	Written_Colored_S,
	Written_Crimson_S,
	Frame_RG_3,
	Frame_DT_8,
	Frame_DT_9,
	Frame_DT_6,
	Frame_DT_7,
	Blurred_Colored_QS,
	Blurred_Crimson_QS,
	Written_Colored_QS,
	Written_Crimson_QS,
	Frame_DT_10,
	Orb_DT,
	Blurred_Colored_T,
	Blurred_Crimson_T,
	Written_Colored_T,
	Written_Crimson_T,
	Fill_RG_3,
	Frame_Exp_Lv1,
	Frame_Exp_Lv3,
	Frame_Exp_Lv2,
	Frame_RG_2,
	Frame_HP_End,
	Frame_HP_Middle,
	Frame_HP_Single,
	Frame_HP_Start,
	Glow_HP,
	Dmg_HP,
	BG_HP,
	Vital_HP,
	Fill_RG_2,
	Frame_RG_1,
	Fill_RG_1,
	Exp_Crimson,
	Exp_DG,
	Exp_DS,
	Exp_G,
	Exp_QS,
	Exp_RG,
	Exp_S,
	Exp_T,

	SIZE
};

enum class StyleNameInitialsVarient
{
	Colored = 0,
	Shadowed,
	Shadowless,
	ShadowOnly,

	SIZE
};

enum class HPBarFrameVarient_t {
	Single = 0,
	Head,
	Mid,
	Tail,

	SIZE
};

constexpr SpriteInfo_t SPRITES_INFO[] =
{
	{ // Frame_Compass
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 736.0f / 4096.0f,
			.B = 542.0f / 4096.0f,
		}
	},
	{ // Panel_T
		.RegionRect =
		{
			.L = 753.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 1228.0f / 4096.0f,
			.B = 489.0f / 4096.0f,
		}
	},
	{ // Panel_S
		.RegionRect =
		{
			.L = 1245.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 1709.0f / 4096.0f,
			.B = 488.0f / 4096.0f,
		}
	},
	{ // Panel_DS
		.RegionRect =
		{
			.L = 1726.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 2154.0f / 4096.0f,
			.B = 485.0f / 4096.0f,
		}
	},
	{ // Panel_G
		.RegionRect =
		{
			.L = 2171.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 2553.0f / 4096.0f,
			.B = 484.0f / 4096.0f,
		}
	},
	{ // Panel_DG
		.RegionRect =
		{
			.L = 2570.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 2976.0f / 4096.0f,
			.B = 481.0f / 4096.0f,
		}
	},
	{ // BG_DS
		.RegionRect =
		{
			.L = 2993.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 3471.0f / 4096.0f,
			.B = 476.0f / 4096.0f,
		}
	},
	{ // Panel_R
		.RegionRect =
		{
			.L = 3488.0f / 4096.0f,
			.T = 24.0f / 4096.0f,
			.R = 3963.0f / 4096.0f,
			.B = 447.0f / 4096.0f,
		}
	},
	{ // Panel_QS
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 497.0f / 4096.0f,
			.B = 977.0f / 4096.0f,
		}
	},
	{ // BG_G
		.RegionRect =
		{
			.L = 514.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 821.0f / 4096.0f,
			.B = 976.0f / 4096.0f,
		}
	},
	{ // BG_T
		.RegionRect =
		{
			.L = 838.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 1109.0f / 4096.0f,
			.B = 967.0f / 4096.0f,
		}
	},
	{ // BG_DG
		.RegionRect =
		{
			.L = 1126.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 1432.0f / 4096.0f,
			.B = 956.0f / 4096.0f,
		}
	},
	{ // BG_QS
		.RegionRect =
		{
			.L = 1449.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 1926.0f / 4096.0f,
			.B = 934.0f / 4096.0f,
		}
	},
	{ // Frame_DT_123
		.RegionRect =
		{
			.L = 1943.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 2507.0f / 4096.0f,
			.B = 924.0f / 4096.0f,
		}
	},
	{ // BG_S
		.RegionRect =
		{
			.L = 2524.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 2815.0f / 4096.0f,
			.B = 897.0f / 4096.0f,
		}
	},
	{ // BG_R
		.RegionRect =
		{
			.L = 2832.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 3143.0f / 4096.0f,
			.B = 896.0f / 4096.0f,
		}
	},
	{ // Colored_DG
		.RegionRect =
		{
			.L = 3160.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 3440.0f / 4096.0f,
			.B = 875.0f / 4096.0f,
		}
	},
	{ // Colored_DS
		.RegionRect =
		{
			.L = 3457.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 3752.0f / 4096.0f,
			.B = 875.0f / 4096.0f,
		}
	},
	{ // Colored_G
		.RegionRect =
		{
			.L = 3769.0f / 4096.0f,
			.T = 559.0f / 4096.0f,
			.R = 4017.0f / 4096.0f,
			.B = 875.0f / 4096.0f,
		}
	},
	{ // Colored_QS
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 306.0f / 4096.0f,
			.B = 1310.0f / 4096.0f,
		}
	},
	{ // Colored_R
		.RegionRect =
		{
			.L = 323.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 564.0f / 4096.0f,
			.B = 1310.0f / 4096.0f,
		}
	},
	{ // Colored_S
		.RegionRect =
		{
			.L = 581.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 877.0f / 4096.0f,
			.B = 1310.0f / 4096.0f,
		}
	},
	{ // Colored_T
		.RegionRect =
		{
			.L = 894.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 1172.0f / 4096.0f,
			.B = 1310.0f / 4096.0f,
		}
	},
	{ // Allorbs_DT
		.RegionRect =
		{
			.L = 1189.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 3094.0f / 4096.0f,
			.B = 1270.0f / 4096.0f,
		}
	},
	{ // Complete_S
		.RegionRect =
		{
			.L = 3111.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 3278.0f / 4096.0f,
			.B = 1268.0f / 4096.0f,
		}
	},
	{ // Shadow_S
		.RegionRect =
		{
			.L = 3295.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 3462.0f / 4096.0f,
			.B = 1261.0f / 4096.0f,
		}
	},
	{ // Complete_DS
		.RegionRect =
		{
			.L = 3479.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 3706.0f / 4096.0f,
			.B = 1259.0f / 4096.0f,
		}
	},
	{ // Shadow_DS
		.RegionRect =
		{
			.L = 3723.0f / 4096.0f,
			.T = 994.0f / 4096.0f,
			.R = 3950.0f / 4096.0f,
			.B = 1259.0f / 4096.0f,
		}
	},
	{ // Complete_G
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 289.0f / 4096.0f,
			.B = 1590.0f / 4096.0f,
		}
	},
	{ // Shadow_G
		.RegionRect =
		{
			.L = 306.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 571.0f / 4096.0f,
			.B = 1590.0f / 4096.0f,
		}
	},
	{ // Complete_T
		.RegionRect =
		{
			.L = 588.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 814.0f / 4096.0f,
			.B = 1589.0f / 4096.0f,
		}
	},
	{ // Shadow_T
		.RegionRect =
		{
			.L = 831.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 1057.0f / 4096.0f,
			.B = 1589.0f / 4096.0f,
		}
	},
	{ // Complete_DG
		.RegionRect =
		{
			.L = 1074.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 1298.0f / 4096.0f,
			.B = 1587.0f / 4096.0f,
		}
	},
	{ // Shadow_DG
		.RegionRect =
		{
			.L = 1315.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 1539.0f / 4096.0f,
			.B = 1587.0f / 4096.0f,
		}
	},
	{ // Complete_QS
		.RegionRect =
		{
			.L = 1556.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 1791.0f / 4096.0f,
			.B = 1586.0f / 4096.0f,
		}
	},
	{ // Complete_R
		.RegionRect =
		{
			.L = 1808.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 1988.0f / 4096.0f,
			.B = 1586.0f / 4096.0f,
		}
	},
	{ // Shadow_QS
		.RegionRect =
		{
			.L = 2005.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 2240.0f / 4096.0f,
			.B = 1586.0f / 4096.0f,
		}
	},
	{ // Shadow_R
		.RegionRect =
		{
			.L = 2257.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 2437.0f / 4096.0f,
			.B = 1586.0f / 4096.0f,
		}
	},
	{ // Fore_S
		.RegionRect =
		{
			.L = 2454.0f / 4096.0f,
			.T = 1327.0f / 4096.0f,
			.R = 2594.0f / 4096.0f,
			.B = 1577.0f / 4096.0f,
		}
	},
	{ // Runes_Dte
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 2174.0f / 4096.0f,
			.B = 1852.0f / 4096.0f,
		}
	},
	{ // Fore_DS
		.RegionRect =
		{
			.L = 2191.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 2387.0f / 4096.0f,
			.B = 1848.0f / 4096.0f,
		}
	},
	{ // Fore_T
		.RegionRect =
		{
			.L = 2404.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 2606.0f / 4096.0f,
			.B = 1847.0f / 4096.0f,
		}
	},
	{ // Fore_DG
		.RegionRect =
		{
			.L = 2623.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 2819.0f / 4096.0f,
			.B = 1845.0f / 4096.0f,
		}
	},
	{ // Fore_QS
		.RegionRect =
		{
			.L = 2836.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 3029.0f / 4096.0f,
			.B = 1842.0f / 4096.0f,
		}
	},
	{ // Fore_G
		.RegionRect =
		{
			.L = 3046.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 3176.0f / 4096.0f,
			.B = 1841.0f / 4096.0f,
		}
	},
	{ // Fore_R
		.RegionRect =
		{
			.L = 3193.0f / 4096.0f,
			.T = 1607.0f / 4096.0f,
			.R = 3308.0f / 4096.0f,
			.B = 1839.0f / 4096.0f,
		}
	},
	{ // Frame_Exp
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 1869.0f / 4096.0f,
			.R = 1283.0f / 4096.0f,
			.B = 2080.0f / 4096.0f,
		}
	},
	{ // Written_Colored_DG
		.RegionRect =
		{
			.L = 1300.0f / 4096.0f,
			.T = 1869.0f / 4096.0f,
			.R = 2253.0f / 4096.0f,
			.B = 2059.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_DG
		.RegionRect =
		{
			.L = 2270.0f / 4096.0f,
			.T = 1869.0f / 4096.0f,
			.R = 3223.0f / 4096.0f,
			.B = 2059.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_R
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 2097.0f / 4096.0f,
			.R = 926.0f / 4096.0f,
			.B = 2284.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_R
		.RegionRect =
		{
			.L = 943.0f / 4096.0f,
			.T = 2097.0f / 4096.0f,
			.R = 1845.0f / 4096.0f,
			.B = 2284.0f / 4096.0f,
		}
	},
	{ // Written_Colored_R
		.RegionRect =
		{
			.L = 1862.0f / 4096.0f,
			.T = 2097.0f / 4096.0f,
			.R = 2668.0f / 4096.0f,
			.B = 2284.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_R
		.RegionRect =
		{
			.L = 2685.0f / 4096.0f,
			.T = 2097.0f / 4096.0f,
			.R = 3491.0f / 4096.0f,
			.B = 2284.0f / 4096.0f,
		}
	},
	{ // Written_Colored_G
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 2301.0f / 4096.0f,
			.R = 709.0f / 4096.0f,
			.B = 2473.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_G
		.RegionRect =
		{
			.L = 726.0f / 4096.0f,
			.T = 2301.0f / 4096.0f,
			.R = 1411.0f / 4096.0f,
			.B = 2473.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_DG
		.RegionRect =
		{
			.L = 1428.0f / 4096.0f,
			.T = 2301.0f / 4096.0f,
			.R = 2477.0f / 4096.0f,
			.B = 2472.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_G
		.RegionRect =
		{
			.L = 2494.0f / 4096.0f,
			.T = 2301.0f / 4096.0f,
			.R = 3275.0f / 4096.0f,
			.B = 2472.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_DG
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 2490.0f / 4096.0f,
			.R = 1073.0f / 4096.0f,
			.B = 2661.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_G
		.RegionRect =
		{
			.L = 1090.0f / 4096.0f,
			.T = 2490.0f / 4096.0f,
			.R = 1871.0f / 4096.0f,
			.B = 2661.0f / 4096.0f,
		}
	},
	{ // Frame_DT_4
		.RegionRect =
		{
			.L = 1888.0f / 4096.0f,
			.T = 2490.0f / 4096.0f,
			.R = 2120.0f / 4096.0f,
			.B = 2654.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_DS
		.RegionRect =
		{
			.L = 2137.0f / 4096.0f,
			.T = 2490.0f / 4096.0f,
			.R = 2952.0f / 4096.0f,
			.B = 2653.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_DS
		.RegionRect =
		{
			.L = 2969.0f / 4096.0f,
			.T = 2490.0f / 4096.0f,
			.R = 3784.0f / 4096.0f,
			.B = 2653.0f / 4096.0f,
		}
	},
	{ // Written_Colored_DS
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 2678.0f / 4096.0f,
			.R = 743.0f / 4096.0f,
			.B = 2841.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_DS
		.RegionRect =
		{
			.L = 760.0f / 4096.0f,
			.T = 2678.0f / 4096.0f,
			.R = 1479.0f / 4096.0f,
			.B = 2841.0f / 4096.0f,
		}
	},
	{ // Frame_DT_5
		.RegionRect =
		{
			.L = 1496.0f / 4096.0f,
			.T = 2678.0f / 4096.0f,
			.R = 1718.0f / 4096.0f,
			.B = 2840.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_S
		.RegionRect =
		{
			.L = 1735.0f / 4096.0f,
			.T = 2678.0f / 4096.0f,
			.R = 2658.0f / 4096.0f,
			.B = 2833.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_S
		.RegionRect =
		{
			.L = 2675.0f / 4096.0f,
			.T = 2678.0f / 4096.0f,
			.R = 3598.0f / 4096.0f,
			.B = 2833.0f / 4096.0f,
		}
	},
	{ // Written_Colored_S
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 851.0f / 4096.0f,
			.B = 3013.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_S
		.RegionRect =
		{
			.L = 868.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 1695.0f / 4096.0f,
			.B = 3013.0f / 4096.0f,
		}
	},
	{ // Frame_RG_3
		.RegionRect =
		{
			.L = 1712.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 2314.0f / 4096.0f,
			.B = 3012.0f / 4096.0f,
		}
	},
	{ // Frame_DT_8
		.RegionRect =
		{
			.L = 2331.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 2570.0f / 4096.0f,
			.B = 3010.0f / 4096.0f,
		}
	},
	{ // Frame_DT_9
		.RegionRect =
		{
			.L = 2587.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 2828.0f / 4096.0f,
			.B = 3010.0f / 4096.0f,
		}
	},
	{ // Frame_DT_6
		.RegionRect =
		{
			.L = 2845.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 3089.0f / 4096.0f,
			.B = 3009.0f / 4096.0f,
		}
	},
	{ // Frame_DT_7
		.RegionRect =
		{
			.L = 3106.0f / 4096.0f,
			.T = 2858.0f / 4096.0f,
			.R = 3347.0f / 4096.0f,
			.B = 3009.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_QS
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3030.0f / 4096.0f,
			.R = 880.0f / 4096.0f,
			.B = 3180.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_QS
		.RegionRect =
		{
			.L = 897.0f / 4096.0f,
			.T = 3030.0f / 4096.0f,
			.R = 1753.0f / 4096.0f,
			.B = 3180.0f / 4096.0f,
		}
	},
	{ // Written_Colored_QS
		.RegionRect =
		{
			.L = 1770.0f / 4096.0f,
			.T = 3030.0f / 4096.0f,
			.R = 2530.0f / 4096.0f,
			.B = 3180.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_QS
		.RegionRect =
		{
			.L = 2547.0f / 4096.0f,
			.T = 3030.0f / 4096.0f,
			.R = 3307.0f / 4096.0f,
			.B = 3180.0f / 4096.0f,
		}
	},
	{ // Frame_DT_10
		.RegionRect =
		{
			.L = 3324.0f / 4096.0f,
			.T = 3030.0f / 4096.0f,
			.R = 3545.0f / 4096.0f,
			.B = 3170.0f / 4096.0f,
		}
	},
	{ // Orb_DT
		.RegionRect =
		{
			.L = 3562.0f / 4096.0f,
			.T = 3030.0f / 4096.0f,
			.R = 3704.0f / 4096.0f,
			.B = 3169.0f / 4096.0f,
		}
	},
	{ // Blurred_Colored_T
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 625.0f / 4096.0f,
			.B = 3336.0f / 4096.0f,
		}
	},
	{ // Blurred_Crimson_T
		.RegionRect =
		{
			.L = 642.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 1243.0f / 4096.0f,
			.B = 3336.0f / 4096.0f,
		}
	},
	{ // Written_Colored_T
		.RegionRect =
		{
			.L = 1260.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 1765.0f / 4096.0f,
			.B = 3336.0f / 4096.0f,
		}
	},
	{ // Written_Crimson_T
		.RegionRect =
		{
			.L = 1782.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 2287.0f / 4096.0f,
			.B = 3336.0f / 4096.0f,
		}
	},
	{ // Fill_RG_3
		.RegionRect =
		{
			.L = 2304.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 2860.0f / 4096.0f,
			.B = 3323.0f / 4096.0f,
		}
	},
	{ // Frame_Exp_Lv1
		.RegionRect =
		{
			.L = 2877.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 3084.0f / 4096.0f,
			.B = 3321.0f / 4096.0f,
		}
	},
	{ // Frame_Exp_Lv3
		.RegionRect =
		{
			.L = 3101.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 3341.0f / 4096.0f,
			.B = 3317.0f / 4096.0f,
		}
	},
	{ // Frame_Exp_Lv2
		.RegionRect =
		{
			.L = 3358.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 3590.0f / 4096.0f,
			.B = 3310.0f / 4096.0f,
		}
	},
	{ // Frame_RG_2
		.RegionRect =
		{
			.L = 3607.0f / 4096.0f,
			.T = 3197.0f / 4096.0f,
			.R = 4025.0f / 4096.0f,
			.B = 3309.0f / 4096.0f,
		}
	},
	{ // Frame_HP_End
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3353.0f / 4096.0f,
			.R = 403.0f / 4096.0f,
			.B = 3463.0f / 4096.0f,
		}
	},
	{ // Frame_HP_Middle
		.RegionRect =
		{
			.L = 420.0f / 4096.0f,
			.T = 3353.0f / 4096.0f,
			.R = 765.0f / 4096.0f,
			.B = 3463.0f / 4096.0f,
		}
	},
	{ // Frame_HP_Single
		.RegionRect =
		{
			.L = 782.0f / 4096.0f,
			.T = 3353.0f / 4096.0f,
			.R = 1181.0f / 4096.0f,
			.B = 3463.0f / 4096.0f,
		}
	},
	{ // Frame_HP_Start
		.RegionRect =
		{
			.L = 1198.0f / 4096.0f,
			.T = 3353.0f / 4096.0f,
			.R = 1563.0f / 4096.0f,
			.B = 3463.0f / 4096.0f,
		}
	},
	{ // Glow_HP
		.RegionRect =
		{
			.L = 1580.0f / 4096.0f,
			.T = 3353.0f / 4096.0f,
			.R = 1843.0f / 4096.0f,
			.B = 3439.0f / 4096.0f,
		}
	},
	{ // Dmg_HP
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3480.0f / 4096.0f,
			.R = 2783.0f / 4096.0f,
			.B = 3561.0f / 4096.0f,
		}
	},
	{ // BG_HP
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3578.0f / 4096.0f,
			.R = 2783.0f / 4096.0f,
			.B = 3658.0f / 4096.0f,
		}
	},
	{ // Vital_HP
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3675.0f / 4096.0f,
			.R = 2783.0f / 4096.0f,
			.B = 3755.0f / 4096.0f,
		}
	},
	{ // Fill_RG_2
		.RegionRect =
		{
			.L = 2800.0f / 4096.0f,
			.T = 3675.0f / 4096.0f,
			.R = 3196.0f / 4096.0f,
			.B = 3743.0f / 4096.0f,
		}
	},
	{ // Frame_RG_1
		.RegionRect =
		{
			.L = 3213.0f / 4096.0f,
			.T = 3675.0f / 4096.0f,
			.R = 3444.0f / 4096.0f,
			.B = 3742.0f / 4096.0f,
		}
	},
	{ // Fill_RG_1
		.RegionRect =
		{
			.L = 3461.0f / 4096.0f,
			.T = 3675.0f / 4096.0f,
			.R = 3668.0f / 4096.0f,
			.B = 3706.0f / 4096.0f,
		}
	},
	{ // Exp_Crimson
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3772.0f / 4096.0f,
			.R = 1130.0f / 4096.0f,
			.B = 3788.0f / 4096.0f,
		}
	},
	{ // Exp_DG
		.RegionRect =
		{
			.L = 1147.0f / 4096.0f,
			.T = 3772.0f / 4096.0f,
			.R = 2253.0f / 4096.0f,
			.B = 3788.0f / 4096.0f,
		}
	},
	{ // Exp_DS
		.RegionRect =
		{
			.L = 2270.0f / 4096.0f,
			.T = 3772.0f / 4096.0f,
			.R = 3376.0f / 4096.0f,
			.B = 3788.0f / 4096.0f,
		}
	},
	{ // Exp_G
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3805.0f / 4096.0f,
			.R = 1130.0f / 4096.0f,
			.B = 3821.0f / 4096.0f,
		}
	},
	{ // Exp_QS
		.RegionRect =
		{
			.L = 1147.0f / 4096.0f,
			.T = 3805.0f / 4096.0f,
			.R = 2253.0f / 4096.0f,
			.B = 3821.0f / 4096.0f,
		}
	},
	{ // Exp_RG
		.RegionRect =
		{
			.L = 2270.0f / 4096.0f,
			.T = 3805.0f / 4096.0f,
			.R = 3376.0f / 4096.0f,
			.B = 3821.0f / 4096.0f,
		}
	},
	{ // Exp_S
		.RegionRect =
		{
			.L = 24.0f / 4096.0f,
			.T = 3838.0f / 4096.0f,
			.R = 1130.0f / 4096.0f,
			.B = 3854.0f / 4096.0f,
		}
	},
	{ // Exp_T
		.RegionRect =
		{
			.L = 1147.0f / 4096.0f,
			.T = 3838.0f / 4096.0f,
			.R = 2253.0f / 4096.0f,
			.B = 3854.0f / 4096.0f,
		}
	},
};

static_assert(sizeof(SPRITES_INFO) / sizeof(SpriteInfo_t) == (size_t)SpriteID_t::SIZE);

constexpr auto& GetSpriteInfo(SpriteID_t textureId)
{
	return SPRITES_INFO[(size_t)textureId];
}

constexpr auto GetCompassStyleCenterPieceSpriteID(Theme_t theme, Style_t style)
{
	// Currently the theme is unused for this
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::Panel_T;

	case Style_t::Swordmaster:
		return SpriteID_t::Panel_S;

	case Style_t::Royalguard:
		return SpriteID_t::Panel_R;

	case Style_t::Gunslinger:
		return SpriteID_t::Panel_G;

	case Style_t::Darkslayer:
		return SpriteID_t::Panel_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::Panel_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::Panel_DG;
	}
}

constexpr auto GetCompassStyleShatteredBGSpriteID(Theme_t theme, Style_t style)
{
	// Currently the theme is unused for this
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::BG_T;

	case Style_t::Swordmaster:
		return SpriteID_t::BG_S;

	case Style_t::Royalguard:
		return SpriteID_t::BG_R;

	case Style_t::Gunslinger:
		return SpriteID_t::BG_G;

	case Style_t::Darkslayer:
		return SpriteID_t::BG_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::BG_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::BG_DG;
	}
}

constexpr auto GetStyleNameInitialsColoredSpriteID(Theme_t theme, Style_t style)
{
	// Currently the theme is unused for this
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::Colored_T;

	case Style_t::Swordmaster:
		return SpriteID_t::Colored_S;

	case Style_t::Royalguard:
		return SpriteID_t::Colored_R;

	case Style_t::Gunslinger:
		return SpriteID_t::Colored_G;

	case Style_t::Darkslayer:
		return SpriteID_t::Colored_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::Colored_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::Colored_DG;
	}
}

constexpr auto GetStyleNameInitialsShadowedSpriteID(Theme_t theme, Style_t style)
{
	// Currently the theme is unused for this
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::Complete_T;

	case Style_t::Swordmaster:
		return SpriteID_t::Complete_S;

	case Style_t::Royalguard:
		return SpriteID_t::Complete_R;

	case Style_t::Gunslinger:
		return SpriteID_t::Complete_G;

	case Style_t::Darkslayer:
		return SpriteID_t::Complete_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::Complete_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::Complete_DG;
	}
}

constexpr auto GetStyleNameInitialsShadowlessSpriteID(Theme_t theme, Style_t style)
{
	// Currently the theme is unused for this
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::Fore_T;

	case Style_t::Swordmaster:
		return SpriteID_t::Fore_S;

	case Style_t::Royalguard:
		return SpriteID_t::Fore_R;

	case Style_t::Gunslinger:
		return SpriteID_t::Fore_G;

	case Style_t::Darkslayer:
		return SpriteID_t::Fore_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::Fore_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::Fore_DG;
	}
}

constexpr auto GetStyleNameInitialsShadowOnlySpriteID(Theme_t theme, Style_t style)
{
	// Currently the theme is unused for this
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::Shadow_T;

	case Style_t::Swordmaster:
		return SpriteID_t::Shadow_S;

	case Style_t::Royalguard:
		return SpriteID_t::Shadow_R;

	case Style_t::Gunslinger:
		return SpriteID_t::Shadow_G;

	case Style_t::Darkslayer:
		return SpriteID_t::Shadow_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::Shadow_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::Shadow_DG;
	}
}

constexpr auto GetStyleNameInitialsSpriteID(Theme_t theme, Style_t style, StyleNameInitialsVarient varient)
{
	switch (varient)
	{
	case StyleNameInitialsVarient::Shadowed:
		return GetStyleNameInitialsShadowedSpriteID(theme, style);

	case StyleNameInitialsVarient::Shadowless:
		return GetStyleNameInitialsShadowlessSpriteID(theme, style);

	case StyleNameInitialsVarient::ShadowOnly:
		return GetStyleNameInitialsShadowOnlySpriteID(theme, style);

	default:
	case StyleNameInitialsVarient::Colored:
		return GetStyleNameInitialsColoredSpriteID(theme, style);
	}
}

constexpr auto GetCompassFrameSpriteID(Theme_t theme)
{
	// Currently the theme is unused for this
	return SpriteID_t::Frame_Compass;
}

constexpr auto GetRoyalguardGuageFrameSpriteID(Theme_t theme, size_t level)
{
	// Currently the theme is unused for this
	switch (level)
	{
	default:
	case 1:
		return SpriteID_t::Frame_RG_1;

	case 2:
		return SpriteID_t::Frame_RG_2;

	case 3:
		return SpriteID_t::Frame_RG_3;
	}
}

constexpr auto GetRoyalguardGuageSpriteID(Theme_t theme, size_t level)
{
	// Currently the theme is unused for this
	switch (level)
	{
	default:
	case 1:
		return SpriteID_t::Fill_RG_1;

	case 2:
		return SpriteID_t::Fill_RG_2;

	case 3:
		return SpriteID_t::Fill_RG_3;
	}
}

constexpr auto GetStyleExpBarNameColoredSpriteID(Style_t style, bool blurred)
{
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return blurred ? SpriteID_t::Blurred_Colored_T : SpriteID_t::Written_Colored_T;

	case Style_t::Swordmaster:
		return blurred ? SpriteID_t::Blurred_Colored_S : SpriteID_t::Written_Colored_S;

	case Style_t::Royalguard:
		return blurred ? SpriteID_t::Blurred_Colored_R : SpriteID_t::Written_Colored_R;

	case Style_t::Gunslinger:
		return blurred ? SpriteID_t::Blurred_Colored_G : SpriteID_t::Written_Colored_G;

	case Style_t::Darkslayer:
		return blurred ? SpriteID_t::Blurred_Colored_DS : SpriteID_t::Written_Colored_DS;

	case Style_t::Quicksilver:
		return blurred ? SpriteID_t::Blurred_Colored_QS : SpriteID_t::Written_Colored_QS;

	case Style_t::Doppleganger:
		return blurred ? SpriteID_t::Blurred_Colored_DG : SpriteID_t::Written_Colored_DG;
	}
}

constexpr auto GetStyleExpBarNameCrimsonSpriteID(Style_t style, bool blurred)
{
	switch (style)
	{
	default:
	case Style_t::Trickster:
		return blurred ? SpriteID_t::Blurred_Crimson_T : SpriteID_t::Written_Crimson_T;

	case Style_t::Swordmaster:
		return blurred ? SpriteID_t::Blurred_Crimson_S : SpriteID_t::Written_Crimson_S;

	case Style_t::Royalguard:
		return blurred ? SpriteID_t::Blurred_Crimson_R : SpriteID_t::Written_Crimson_R;

	case Style_t::Gunslinger:
		return blurred ? SpriteID_t::Blurred_Crimson_G : SpriteID_t::Written_Crimson_G;

	case Style_t::Darkslayer:
		return blurred ? SpriteID_t::Blurred_Crimson_DS : SpriteID_t::Written_Crimson_DS;

	case Style_t::Quicksilver:
		return blurred ? SpriteID_t::Blurred_Crimson_QS : SpriteID_t::Written_Crimson_QS;

	case Style_t::Doppleganger:
		return blurred ? SpriteID_t::Blurred_Crimson_DG : SpriteID_t::Written_Crimson_DG;
	}
}

constexpr auto GetStyleExpBarNameSpriteID(Theme_t theme, Style_t style, bool blurred)
{
	switch (theme)
	{
	case Theme_t::Crimson:
		return GetStyleExpBarNameCrimsonSpriteID(style, blurred);

	default:
	case Theme_t::Colored:
		return GetStyleExpBarNameColoredSpriteID(style, blurred);
	}
}

constexpr auto GetStyleExpBarSpriteID(Theme_t theme, Style_t style)
{
	if (theme == Theme_t::Crimson)
		return SpriteID_t::Exp_Crimson;

	switch (style)
	{
	default:
	case Style_t::Trickster:
		return SpriteID_t::Exp_T;

	case Style_t::Swordmaster:
		return SpriteID_t::Exp_S;

	case Style_t::Royalguard:
		return SpriteID_t::Exp_RG;

	case Style_t::Gunslinger:
		return SpriteID_t::Exp_G;

	case Style_t::Darkslayer:
		return SpriteID_t::Exp_DS;

	case Style_t::Quicksilver:
		return SpriteID_t::Exp_QS;

	case Style_t::Doppleganger:
		return SpriteID_t::Exp_DG;
	}
}

constexpr auto GetStyleExpBarFrameSpriteID(Theme_t theme)
{
	return SpriteID_t::Frame_Exp;
}

constexpr auto GetStyleExpBarLevelNumberSpriteID(Theme_t theme, size_t level)
{
	// Currently the theme is unused for this
	switch (level)
	{
	default:
	case 1:
		return SpriteID_t::Frame_Exp_Lv1;

	case 2:
		return SpriteID_t::Frame_Exp_Lv2;

	case 3:
		return SpriteID_t::Frame_Exp_Lv3;
	}
}

constexpr auto GetDTBarFrameSpriteID(Theme_t theme, size_t level)
{
	// Currently the theme is unused for this
	switch (level)
	{
	default:
	case 1:
		return SpriteID_t::Frame_DT_123;

	case 2:
		return SpriteID_t::Frame_DT_4;

	case 3:
		return SpriteID_t::Frame_DT_5;

	case 4:
		return SpriteID_t::Frame_DT_6;

	case 5:
		return SpriteID_t::Frame_DT_7;

	case 6:
		return SpriteID_t::Frame_DT_8;

	case 7:
		return SpriteID_t::Frame_DT_9;

	case 8:
		return SpriteID_t::Frame_DT_10;
	}
}

constexpr auto GetDTBarOrbSpriteID(Theme_t theme, bool allOrbs)
{
	// Currently the theme is unused for this
	return allOrbs ? SpriteID_t::Allorbs_DT : SpriteID_t::Orb_DT;
}

constexpr auto GetHPBarFrameSpriteID(Theme_t theme, HPBarFrameVarient_t varient)
{
	// Currently the theme is unused for this
	switch (varient)
	{
	case HPBarFrameVarient_t::Single:
		return SpriteID_t::Frame_HP_Single;

	case HPBarFrameVarient_t::Head:
		return SpriteID_t::Frame_HP_Start;

	default:
	case HPBarFrameVarient_t::Mid:
		return SpriteID_t::Frame_HP_Middle;

	case HPBarFrameVarient_t::Tail:
		return SpriteID_t::Frame_HP_End;
	}
}

constexpr auto GetHPBarBGSpriteID(Theme_t theme)
{
	// Currently the theme is unused for this
	return SpriteID_t::BG_HP;
}

constexpr auto GetHPBarVitalitySpriteID(Theme_t theme)
{
	// Currently the theme is unused for this
	return SpriteID_t::Vital_HP;
}

constexpr auto GetHPBarDamageSpriteID(Theme_t theme)
{
	// Currently the theme is unused for this
	return SpriteID_t::Dmg_HP;
}

constexpr auto GetHPBarVitalityTailGlowSpriteID(Theme_t theme)
{
	// Currently the theme is unused for this
	return SpriteID_t::Glow_HP;
}

constexpr const char* GetTextureNameFromSpriteID(SpriteID_t texID)
{
	switch (texID)
	{
	case SpriteID_t::Frame_Compass: return "Frame_Compass";
	case SpriteID_t::Panel_T: return "Panel_T";
	case SpriteID_t::Panel_S: return "Panel_S";
	case SpriteID_t::Panel_DS: return "Panel_DS";
	case SpriteID_t::Panel_G: return "Panel_G";
	case SpriteID_t::Panel_DG: return "Panel_DG";
	case SpriteID_t::BG_DS: return "BG_DS";
	case SpriteID_t::Panel_R: return "Panel_R";
	case SpriteID_t::Panel_QS: return "Panel_QS";
	case SpriteID_t::BG_G: return "BG_G";
	case SpriteID_t::BG_T: return "BG_T";
	case SpriteID_t::BG_DG: return "BG_DG";
	case SpriteID_t::BG_QS: return "BG_QS";
	case SpriteID_t::Frame_DT_123: return "Frame_DT_123";
	case SpriteID_t::BG_S: return "BG_S";
	case SpriteID_t::BG_R: return "BG_R";
	case SpriteID_t::Colored_DG: return "Colored_DG";
	case SpriteID_t::Colored_DS: return "Colored_DS";
	case SpriteID_t::Colored_G: return "Colored_G";
	case SpriteID_t::Colored_QS: return "Colored_QS";
	case SpriteID_t::Colored_R: return "Colored_R";
	case SpriteID_t::Colored_S: return "Colored_S";
	case SpriteID_t::Colored_T: return "Colored_T";
	case SpriteID_t::Allorbs_DT: return "Allorbs_DT";
	case SpriteID_t::Complete_S: return "Complete_S";
	case SpriteID_t::Shadow_S: return "Shadow_S";
	case SpriteID_t::Complete_DS: return "Complete_DS";
	case SpriteID_t::Shadow_DS: return "Shadow_DS";
	case SpriteID_t::Complete_G: return "Complete_G";
	case SpriteID_t::Shadow_G: return "Shadow_G";
	case SpriteID_t::Complete_T: return "Complete_T";
	case SpriteID_t::Shadow_T: return "Shadow_T";
	case SpriteID_t::Complete_DG: return "Complete_DG";
	case SpriteID_t::Shadow_DG: return "Shadow_DG";
	case SpriteID_t::Complete_QS: return "Complete_QS";
	case SpriteID_t::Complete_R: return "Complete_R";
	case SpriteID_t::Shadow_QS: return "Shadow_QS";
	case SpriteID_t::Shadow_R: return "Shadow_R";
	case SpriteID_t::Fore_S: return "Fore_S";
	case SpriteID_t::Runes_DTE: return "Runes_DTE";
	case SpriteID_t::Fore_DS: return "Fore_DS";
	case SpriteID_t::Fore_T: return "Fore_T";
	case SpriteID_t::Fore_DG: return "Fore_DG";
	case SpriteID_t::Fore_QS: return "Fore_QS";
	case SpriteID_t::Fore_G: return "Fore_G";
	case SpriteID_t::Fore_R: return "Fore_R";
	case SpriteID_t::Frame_Exp: return "Frame_Exp";
	case SpriteID_t::Written_Colored_DG: return "Written_Colored_DG";
	case SpriteID_t::Written_Crimson_DG: return "Written_Crimson_DG";
	case SpriteID_t::Blurred_Colored_R: return "Blurred_Colored_R";
	case SpriteID_t::Blurred_Crimson_R: return "Blurred_Crimson_R";
	case SpriteID_t::Written_Colored_R: return "Written_Colored_R";
	case SpriteID_t::Written_Crimson_R: return "Written_Crimson_R";
	case SpriteID_t::Written_Colored_G: return "Written_Colored_G";
	case SpriteID_t::Written_Crimson_G: return "Written_Crimson_G";
	case SpriteID_t::Blurred_Colored_DG: return "Blurred_Colored_DG";
	case SpriteID_t::Blurred_Colored_G: return "Blurred_Colored_G";
	case SpriteID_t::Blurred_Crimson_DG: return "Blurred_Crimson_DG";
	case SpriteID_t::Blurred_Crimson_G: return "Blurred_Crimson_G";
	case SpriteID_t::Frame_DT_4: return "Frame_DT_4";
	case SpriteID_t::Blurred_Colored_DS: return "Blurred_Colored_DS";
	case SpriteID_t::Blurred_Crimson_DS: return "Blurred_Crimson_DS";
	case SpriteID_t::Written_Colored_DS: return "Written_Colored_DS";
	case SpriteID_t::Written_Crimson_DS: return "Written_Crimson_DS";
	case SpriteID_t::Frame_DT_5: return "Frame_DT_5";
	case SpriteID_t::Blurred_Colored_S: return "Blurred_Colored_S";
	case SpriteID_t::Blurred_Crimson_S: return "Blurred_Crimson_S";
	case SpriteID_t::Written_Colored_S: return "Written_Colored_S";
	case SpriteID_t::Written_Crimson_S: return "Written_Crimson_S";
	case SpriteID_t::Frame_RG_3: return "Frame_RG_3";
	case SpriteID_t::Frame_DT_8: return "Frame_DT_8";
	case SpriteID_t::Frame_DT_9: return "Frame_DT_9";
	case SpriteID_t::Frame_DT_6: return "Frame_DT_6";
	case SpriteID_t::Frame_DT_7: return "Frame_DT_7";
	case SpriteID_t::Blurred_Colored_QS: return "Blurred_Colored_QS";
	case SpriteID_t::Blurred_Crimson_QS: return "Blurred_Crimson_QS";
	case SpriteID_t::Written_Colored_QS: return "Written_Colored_QS";
	case SpriteID_t::Written_Crimson_QS: return "Written_Crimson_QS";
	case SpriteID_t::Frame_DT_10: return "Frame_DT_10";
	case SpriteID_t::Orb_DT: return "Orb_DT";
	case SpriteID_t::Blurred_Colored_T: return "Blurred_Colored_T";
	case SpriteID_t::Blurred_Crimson_T: return "Blurred_Crimson_T";
	case SpriteID_t::Written_Colored_T: return "Written_Colored_T";
	case SpriteID_t::Written_Crimson_T: return "Written_Crimson_T";
	case SpriteID_t::Fill_RG_1: return "Fill_RG_1";
	case SpriteID_t::Frame_Exp_Lv1: return "Frame_Exp_Lv1";
	case SpriteID_t::Frame_Exp_Lv3: return "Frame_Exp_Lv3";
	case SpriteID_t::Frame_Exp_Lv2: return "Frame_Exp_Lv2";
	case SpriteID_t::Frame_RG_2: return "Frame_RG_2";
	case SpriteID_t::Frame_HP_End: return "Frame_HP_End";
	case SpriteID_t::Frame_HP_Middle: return "Frame_HP_Middle";
	case SpriteID_t::Frame_HP_Single: return "Frame_HP_Single";
	case SpriteID_t::Frame_HP_Start: return "Frame_HP_Start";
	case SpriteID_t::Glow_HP: return "Glow_HP";
	case SpriteID_t::Dmg_HP: return "Dmg_HP";
	case SpriteID_t::BG_HP: return "BG_HP";
	case SpriteID_t::Vital_HP: return "Vital_HP";
	case SpriteID_t::Fill_RG_2: return "Fill_RG_2";
	case SpriteID_t::Frame_RG_1: return "Frame_RG_1";
	case SpriteID_t::Fill_RG_3: return "Fill_RG_3";
	case SpriteID_t::Exp_Crimson: return "Exp_Crimson";
	case SpriteID_t::Exp_DG: return "Exp_DG";
	case SpriteID_t::Exp_DS: return "Exp_DS";
	case SpriteID_t::Exp_G: return "Exp_G";
	case SpriteID_t::Exp_QS: return "Exp_QS";
	case SpriteID_t::Exp_RG: return "Exp_RG";
	case SpriteID_t::Exp_S: return "Exp_S";
	case SpriteID_t::Exp_T: return "Exp_T";
	}
}

void LoadSpriteDescs(std::vector<Graphics::SpriteDesc>& spriteDescs, const std::string& path)
{
	spriteDescs.reserve((size_t)SpriteID_t::SIZE);

	for (size_t i = 0; i < (size_t)SpriteID_t::SIZE; i++)
	{
		spriteDescs.emplace_back(
			Graphics::SpriteDesc(
				path,
				0,
				SPRITES_INFO[i].RegionRect
			)
		);
	}
};
}

namespace Transforms
{
struct SpriteTransform
{
	Sprites::SpriteID_t  SpriteID_t;
	float				Scale;
	glm::f32vec2		Position;
};

enum class GlowPos_t
{
	Head = 0,
	Tail,

	SIZE
};

enum class StyleInitialsVarient_t
{
	Colored = 0,
	Shadowed,
	Shadowless,
	ShadowOnly,

	SIZE
};

enum class CompassSlotID_t
{
	Top = 0,
	Right,
	Bottom,
	Left,
	RightSecondary,
	BottomSecondary,
	LeftSecondary,

	SIZE
};

enum class BarPositionID_t
{
	Top = 0,
	Bottom,

	SIZE
};

enum class DTBarFrameID_t
{
	F123 = 0,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,

	SIZE
};

enum class HPBarFrameVarient_t
{
	Single = 0,
	Head,
	Middle,
	Tail,

	SIZE
};

enum class HPBarVarient_t
{
	Background = 0,
	Damage,
	Vitality,

	SIZE
};

enum class TransformID_t
{
	Compass_Frame = 0,
	
	Compass_Center_Piece_DG,
	Compass_Center_Piece_DS,
	Compass_Center_Piece_G,
	Compass_Center_Piece_QS,
	Compass_Center_Piece_R,
	Compass_Center_Piece_S,
	Compass_Center_Piece_T,
	
	Compass_Slots_Bottom_Colored_QS,
	Compass_Slots_Bottom_Colored_R,
	Compass_Slots_Bottom_Shadowed_QS,
	Compass_Slots_Bottom_Shadowed_R,
	Compass_Slots_Bottom_Sec_Shadowed_QS,
	Compass_Slots_Bottom_Sec_Shadowed_R,
	
	Compass_Slots_Left_Colored_DG,
	Compass_Slots_Left_Colored_G,
	Compass_Slots_Left_Shadowed_G,
	Compass_Slots_Left_Sec_Shadowed_DG,
	Compass_Slots_Left_Sec_Shadowed_G,
	
	Compass_Slots_Right_Colored_DS,
	Compass_Slots_Right_Colored_S,
	Compass_Slots_Right_Shadowed_S,
	Compass_Slots_Right_Sec_Shadowed_DS,
	Compass_Slots_Right_Sec_Shadowed_S,
	
	Compass_Slots_Top_Colored_T,
	Compass_Slots_Top_Shadowed_T,
	
	DT_Bar_Frames_Bottom_123,
	DT_Bar_Frames_Bottom_4,
	DT_Bar_Frames_Bottom_5,
	DT_Bar_Frames_Bottom_6,
	DT_Bar_Frames_Bottom_7,
	DT_Bar_Frames_Bottom_8,
	DT_Bar_Frames_Bottom_9,
	DT_Bar_Frames_Bottom_10,
	
	DT_Bar_Frames_Top_123,
	DT_Bar_Frames_Top_4,
	DT_Bar_Frames_Top_5,
	DT_Bar_Frames_Top_6,
	DT_Bar_Frames_Top_7,
	DT_Bar_Frames_Top_8,
	DT_Bar_Frames_Top_9,
	DT_Bar_Frames_Top_10,
	
	DT_Bar_Orbs_Bottom_0,
	DT_Bar_Orbs_Bottom_1,
	DT_Bar_Orbs_Bottom_2,
	DT_Bar_Orbs_Bottom_3,
	DT_Bar_Orbs_Bottom_4,
	DT_Bar_Orbs_Bottom_5,
	DT_Bar_Orbs_Bottom_6,
	DT_Bar_Orbs_Bottom_7,
	DT_Bar_Orbs_Bottom_8,
	DT_Bar_Orbs_Bottom_9,
	
	DT_Bar_Orbs_Top_0,
	DT_Bar_Orbs_Top_1,
	DT_Bar_Orbs_Top_2,
	DT_Bar_Orbs_Top_3,
	DT_Bar_Orbs_Top_4,
	DT_Bar_Orbs_Top_5,
	DT_Bar_Orbs_Top_6,
	DT_Bar_Orbs_Top_7,
	DT_Bar_Orbs_Top_8,
	DT_Bar_Orbs_Top_9,
	
	HP_Bar_BG_Top,
	HP_Bar_BG_Bottom,
	
	HP_Bar_Dmg_Top,
	HP_Bar_Dmg_Bottom,
	
	HP_Bar_Frame_0_Single,
	HP_Bar_Frame_0_Start,
	HP_Bar_Frame_1_End,
	HP_Bar_Frame_1_Middle,
	HP_Bar_Frame_2_End,
	HP_Bar_Frame_2_Middle,
	HP_Bar_Frame_3_End,
	HP_Bar_Frame_3_Middle,
	HP_Bar_Frame_4_End,
	HP_Bar_Frame_4_Middle,
	HP_Bar_Frame_5_End,
	HP_Bar_Frame_5_Middle,
	HP_Bar_Frame_6_End,
	HP_Bar_Frame_6_Middle,
	HP_Bar_Frame_7_End,
	HP_Bar_Frame_7_Middle,
	HP_Bar_Frame_8_End,
	HP_Bar_Frame_8_Middle,
	HP_Bar_Frame_9_End,
	HP_Bar_Frame_10_Single,
	HP_Bar_Frame_10_Start,
	HP_Bar_Frame_11_End,
	HP_Bar_Frame_11_Middle,
	HP_Bar_Frame_12_End,
	HP_Bar_Frame_12_Middle,
	HP_Bar_Frame_13_End,
	HP_Bar_Frame_13_Middle,
	HP_Bar_Frame_14_End,
	HP_Bar_Frame_14_Middle,
	HP_Bar_Frame_15_End,
	HP_Bar_Frame_15_Middle,
	HP_Bar_Frame_16_End,
	HP_Bar_Frame_16_Middle,
	HP_Bar_Frame_17_End,
	HP_Bar_Frame_17_Middle,
	HP_Bar_Frame_18_End,
	HP_Bar_Frame_18_Middle,
	HP_Bar_Frame_19_End,
	
	HP_Bar_Glow_Top_Head,
	HP_Bar_Glow_Top_Tail,
	HP_Bar_Glow_Bottom_Head,
	HP_Bar_Glow_Bottom_Tail,
	
	HP_Bar_Vit_Top,
	HP_Bar_Vit_Bottom,
	
	RG_Guage_Fill_Lv1,
	RG_Guage_Fill_Lv2,
	RG_Guage_Fill_Lv3,
	
	RG_Guage_Frame_Lv1,
	RG_Guage_Frame_Lv2,
	RG_Guage_Frame_Lv3,
	
	Shattered_BG_DG,
	Shattered_BG_DS,
	Shattered_BG_G,
	Shattered_BG_QS,
	Shattered_BG_R,
	Shattered_BG_S,
	Shattered_BG_T,
	
	Style_Exp_Bar_Crimson,
	
	Style_Exp_Bar_DG,
	Style_Exp_Bar_DS,
	Style_Exp_Bar_G,
	Style_Exp_Bar_QS,
	Style_Exp_Bar_RG,
	Style_Exp_Bar_S,
	Style_Exp_Bar_T,
	
	Style_Exp_Bar_Frame,
	
	Style_Exp_Bar_Level_Digit_Lv1,
	Style_Exp_Bar_Level_Digit_Lv2,
	Style_Exp_Bar_Level_Digit_Lv3,
	
	Style_Exp_Bar_Name_Written_Colored_DG,
	Style_Exp_Bar_Name_Written_Colored_DS,
	Style_Exp_Bar_Name_Written_Colored_G,
	Style_Exp_Bar_Name_Written_Colored_QS,
	Style_Exp_Bar_Name_Written_Colored_R,
	Style_Exp_Bar_Name_Written_Colored_S,
	Style_Exp_Bar_Name_Written_Colored_T,
	Style_Exp_Bar_Name_Written_Crimson_DG,
	Style_Exp_Bar_Name_Written_Crimson_DS,
	Style_Exp_Bar_Name_Written_Crimson_G,
	Style_Exp_Bar_Name_Written_Crimson_QS,
	Style_Exp_Bar_Name_Written_Crimson_R,
	Style_Exp_Bar_Name_Written_Crimson_S,
	Style_Exp_Bar_Name_Written_Crimson_T,
	
	SIZE
};

constexpr SpriteTransform SPRITE_TRANSFORMS[] =
{
	SpriteTransform // Compass_Frame
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_Compass,
		.Scale = 0.48100000619888306f,
		.Position = { -0.7319999933242798f, 0.18299999833106995f },
	},
	SpriteTransform // Compass_Center_Piece_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_DG,
		.Scale = 0.4099999964237213f,
		.Position = { -0.7639999985694885f, 0.19900000095367432f },
	},
	SpriteTransform // Compass_Center_Piece_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_DS,
		.Scale = 0.42100000381469727f,
		.Position = { -0.7900000214576721f, 0.20100000500679016f },
	},
	SpriteTransform // Compass_Center_Piece_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_G,
		.Scale = 0.4059999883174896f,
		.Position = { -0.7570000290870667f, 0.19300000369548798f },
	},
	SpriteTransform // Compass_Center_Piece_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_QS,
		.Scale = 0.3869999945163727f,
		.Position = { -0.781000018119812f, 0.22599999606609344f },
	},
	SpriteTransform // Compass_Center_Piece_R
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_R,
		.Scale = 0.38999998569488525f,
		.Position = { -0.7820000052452087f, 0.2290000021457672f },
	},
	SpriteTransform // Compass_Center_Piece_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_S,
		.Scale = 0.42100000381469727f,
		.Position = { -0.777999997138977f, 0.1899999976158142f },
	},
	SpriteTransform // Compass_Center_Piece_T
	{
		.SpriteID_t = Sprites::SpriteID_t::Panel_T,
		.Scale = 0.421999990940094f,
		.Position = { -0.7770000100135803f, 0.1899999976158142f },
	},
	SpriteTransform // Compass_Slots_Bottom_Colored_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_QS,
		.Scale = 0.28999999165534973f,
		.Position = { -0.7720000147819519f, -0.1720000058412552f },
	},
	SpriteTransform // Compass_Slots_Bottom_Colored_R
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_R,
		.Scale = 0.28999999165534973f,
		.Position = { -0.7720000147819519f, -0.14900000393390656f },
	},
	SpriteTransform // Compass_Slots_Bottom_Shadowed_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_QS,
		.Scale = 0.21899999678134918f,
		.Position = { -0.777999997138977f, -0.44699999690055847f },
	},
	SpriteTransform // Compass_Slots_Bottom_Shadowed_R
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_R,
		.Scale = 0.257999986410141f,
		.Position = { -0.7689999938011169f, -0.18199999630451202f },
	},
	SpriteTransform // Compass_Slots_Bottom_Sec_Shadowed_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_QS,
		.Scale = 0.21899999678134918f,
		.Position = { -0.777999997138977f, -0.44699999690055847f },
	},
	SpriteTransform // Compass_Slots_Bottom_Sec_Shadowed_R
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_R,
		.Scale = 0.21199999749660492f,
		.Position = { -0.7680000066757202f, -0.4690000116825104f },
	},
	SpriteTransform // Compass_Slots_Left_Colored_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_DG,
		.Scale = 0.28999999165534973f,
		.Position = { -0.9120000004768372f, 0.18199999630451202f },
	},
	SpriteTransform // Compass_Slots_Left_Colored_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_G,
		.Scale = 0.28999999165534973f,
		.Position = { -0.8970000147819519f, 0.1809999942779541f },
	},
	SpriteTransform // Compass_Slots_Left_Shadowed_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_G,
		.Scale = 0.26600000262260437f,
		.Position = { -0.8619999885559082f, 0.164000004529953f },
	},
	SpriteTransform // Compass_Slots_Left_Sec_Shadowed_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_DG,
		.Scale = 0.20000000298023224f,
		.Position = { -0.9269999861717224f, -0.12099999934434891f },
	},
	SpriteTransform // Compass_Slots_Left_Sec_Shadowed_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_G,
		.Scale = 0.20000000298023224f,
		.Position = { -0.9269999861717224f, -0.12099999934434891f },
	},
	SpriteTransform // Compass_Slots_Right_Colored_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_DS,
		.Scale = 0.28999999165534973f,
		.Position = { -0.6449999809265137f, 0.19200000166893005f },
	},
	SpriteTransform // Compass_Slots_Right_Colored_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_S,
		.Scale = 0.28999999165534973f,
		.Position = { -0.6779999732971191f, 0.1809999942779541f },
	},
	SpriteTransform // Compass_Slots_Right_Shadowed_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_S,
		.Scale = 0.2770000100135803f,
		.Position = { -0.6779999732971191f, 0.17499999701976776f },
	},
	SpriteTransform // Compass_Slots_Right_Sec_Shadowed_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_DS,
		.Scale = 0.2070000022649765f,
		.Position = { -0.6129999756813049f, -0.12300000339746475f },
	},
	SpriteTransform // Compass_Slots_Right_Sec_Shadowed_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_S,
		.Scale = 0.20000000298023224f,
		.Position = { -0.6259999871253967f, -0.12099999934434891f },
	},
	SpriteTransform // Compass_Slots_Top_Colored_T
	{
		.SpriteID_t = Sprites::SpriteID_t::Colored_T,
		.Scale = 0.29100000858306885f,
		.Position = { -0.7770000100135803f, 0.4950000047683716f },
	},
	SpriteTransform // Compass_Slots_Top_Shadowed_T
	{
		.SpriteID_t = Sprites::SpriteID_t::Complete_T,
		.Scale = 0.2639999985694885f,
		.Position = { -0.7839999794960022f, 0.48399999737739563f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_123
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_123,
		.Scale = 0.33399999141693115f,
		.Position = { -0.3630000054836273f, -0.2669999897480011f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_4
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_4,
		.Scale = 0.1550000011920929f,
		.Position = { -0.16099999845027924f, -0.29899999499320984f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_5
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_5,
		.Scale = 0.14900000393390656f,
		.Position = { -0.05900000035762787f, -0.16500000655651093f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_6
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_6,
		.Scale = 0.13699999451637268f,
		.Position = { 0.03999999910593033f, -0.23399999737739563f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_7
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_7,
		.Scale = 0.14300000667572021f,
		.Position = { 0.1459999978542328f, -0.17399999499320984f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_8
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_8,
		.Scale = 0.13699999451637268f,
		.Position = { 0.2529999911785126f, -0.2240000069141388f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_9
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_9,
		.Scale = 0.13699999451637268f,
		.Position = { 0.3580000102519989f, -0.19599999487400055f },
	},
	SpriteTransform // DT_Bar_Frames_Bottom_10
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_10,
		.Scale = 0.13099999725818634f,
		.Position = { 0.4560000002384186f, -0.19699999690055847f },
	},
	SpriteTransform // DT_Bar_Frames_Top_123
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_123,
		.Scale = 0.33399999141693115f,
		.Position = { -0.40299999713897705f, -0.04499999061226845f },
	},
	SpriteTransform // DT_Bar_Frames_Top_4
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_4,
		.Scale = 0.1550000011920929f,
		.Position = { -0.20100000500679016f, -0.07699999213218689f },
	},
	SpriteTransform // DT_Bar_Frames_Top_5
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_5,
		.Scale = 0.14900000393390656f,
		.Position = { -0.0989999994635582f, 0.056999992579221725f },
	},
	SpriteTransform // DT_Bar_Frames_Top_3_Frame_DT_6
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_6,
		.Scale = 0.13699999451637268f,
		.Position = { -8.940696516468449e-10f, -0.011999997310340405f },
	},
	SpriteTransform // DT_Bar_Frames_Top_7
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_7,
		.Scale = 0.14300000667572021f,
		.Position = { 0.10599999874830246f, 0.04800000414252281f },
	},
	SpriteTransform // DT_Bar_Frames_Top_8
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_8,
		.Scale = 0.13699999451637268f,
		.Position = { 0.21299998462200165f, -0.0020000068470835686f },
	},
	SpriteTransform // DT_Bar_Frames_Top_9
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_9,
		.Scale = 0.13699999451637268f,
		.Position = { 0.31800001859664917f, 0.0260000042617321f },
	},
	SpriteTransform // DT_Bar_Frames_Top_10
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_DT_10,
		.Scale = 0.13099999725818634f,
		.Position = { 0.41600000858306885f, 0.02500000223517418f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_0
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.12399999797344208f,
		.Position = { -0.4560000002384186f, -0.1379999965429306f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_1
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.12399999797344208f,
		.Position = { -0.35499998927116394f, -0.38499999046325684f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_2
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.12399999797344208f,
		.Position = { -0.25999999046325684f, -0.15800000727176666f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_3
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { -0.1420000046491623f, -0.27799999713897705f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_4
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { -0.04100000113248825f, -0.15299999713897705f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_5
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.05999999865889549f, -0.22100000083446503f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_6
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.16899999976158142f, -0.16300000250339508f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_7
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.27300000190734863f, -0.210999995470047f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_8
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.37700000405311584f, -0.17100000381469727f },
	},
	SpriteTransform // DT_Bar_Orbs_Bottom_9
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.4790000021457672f, -0.19099999964237213f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_0
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.12399999797344208f,
		.Position = { -0.4959999918937683f, 0.08400000631809235f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_1
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.12399999797344208f,
		.Position = { -0.39499998092651367f, -0.1629999876022339f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_2
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.12399999797344208f,
		.Position = { -0.29999998211860657f, 0.06399999558925629f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_3
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { -0.18200001120567322f, -0.0559999980032444f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_4
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { -0.08100000023841858f, 0.0690000057220459f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_5
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.019999999552965164f, 0.0009999991161748767f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_6
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.1289999932050705f, 0.05899999663233757f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_7
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.2329999953508377f, 0.011000004597008228f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_8
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.3370000123977661f, 0.050999995321035385f },
	},
	SpriteTransform // DT_Bar_Orbs_Top_9
	{
		.SpriteID_t = Sprites::SpriteID_t::Orb_DT,
		.Scale = 0.0989999994635582f,
		.Position = { 0.4390000104904175f, 0.03099999949336052f },
	},
	SpriteTransform // HP_Bar_BG_Top
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_HP,
		.Scale = 0.7210000157356262f,
		.Position = { 0.1850000023841858f, 0.37700000405311584f },
	},
	SpriteTransform // HP_Bar_BG_Bottom
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_HP,
		.Scale = 0.7210000157356262f,
		.Position = { 0.23399999737739563f, 0.15600000321865082f },
	},
	SpriteTransform // HP_Bar_Dmg_Top
	{
		.SpriteID_t = Sprites::SpriteID_t::Dmg_HP,
		.Scale = 0.7210000157356262f,
		.Position = { 0.1850000023841858f, 0.37700000405311584f },
	},
	SpriteTransform // HP_Bar_Dmg_Bottom
	{
		.SpriteID_t = Sprites::SpriteID_t::Dmg_HP,
		.Scale = 0.7210000157356262f,
		.Position = { 0.23399999737739563f, 0.15600000321865082f },
	},
	SpriteTransform // HP_Bar_Frame_0_Single
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Single,
		.Scale = 0.10499999672174454f,
		.Position = { -0.45399999618530273f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_0_Start
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Start,
		.Scale = 0.10100000351667404f,
		.Position = { -0.46299999952316284f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_1_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { -0.30799999833106995f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_1_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { -0.31700000166893005f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_2_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { -0.16599999368190765f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_2_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { -0.17499999701976776f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_3_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { -0.02500000037252903f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_3_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { -0.03400000184774399f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_4_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.11699999868869781f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_4_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.1080000028014183f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_5_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.2590000033378601f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_5_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.25f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_6_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.4000000059604645f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_6_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.39100000262260437f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_7_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.5419999957084656f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_7_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.5329999923706055f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_8_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.6830000281333923f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_8_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.6740000247955322f, 0.367000013589859f },
	},
	SpriteTransform // HP_Bar_Frame_9_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.8240000009536743f, 0.36800000071525574f },
	},
	SpriteTransform // HP_Bar_Frame_10_Single
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Single,
		.Scale = 0.10499999672174454f,
		.Position = { -0.40400001406669617f, 0.14900000393390656f },
	},
	SpriteTransform // HP_Bar_Frame_10_Start
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Start,
		.Scale = 0.10100000351667404f,
		.Position = { -0.4129999876022339f, 0.14900000393390656f },
	},
	SpriteTransform // HP_Bar_Frame_11_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { -0.257999986410141f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_11_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { -0.2680000066757202f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_12_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { -0.11699999868869781f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_12_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { -0.12600000202655792f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_13_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.024000000208616257f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_13_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.014999999664723873f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_14_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.16599999368190765f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_14_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.15700000524520874f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_15_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.3070000112056732f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_15_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.296999990940094f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_16_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.44699999690055847f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_16_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.43799999356269836f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_17_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.5879999995231628f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_17_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.5789999961853027f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_18_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.7289999723434448f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_18_Middle
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_Middle,
		.Scale = 0.10199999809265137f,
		.Position = { 0.7200000286102295f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Frame_19_End
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_HP_End,
		.Scale = 0.10199999809265137f,
		.Position = { 0.8709999918937683f, 0.14800000190734863f },
	},
	SpriteTransform // HP_Bar_Glow_Top_Head
	{
		.SpriteID_t = Sprites::SpriteID_t::Glow_HP,
		.Scale = 0.0729999989271164f,
		.Position = { -0.57f, 0.37700000405311584f },
	},
	SpriteTransform // HP_Bar_Glow_Top_Tail
	{
		.SpriteID_t = Sprites::SpriteID_t::Glow_HP,
		.Scale = 0.0729999989271164f,
		.Position = { 0.843999981880188f, 0.37700000405311584f },
	},
	SpriteTransform // HP_Bar_Glow_Bottom_Head
	{
		.SpriteID_t = Sprites::SpriteID_t::Glow_HP,
		.Scale = 0.0729999989271164f,
		.Position = { -0.521f, 0.1550000011920929f },
	},
	SpriteTransform // HP_Bar_Glow_Bottom_Tail
	{
		.SpriteID_t = Sprites::SpriteID_t::Glow_HP,
		.Scale = 0.0729999989271164f,
		.Position = { 0.8930000066757202f, 0.1550000011920929f },
	},
	SpriteTransform // HP_Bar_Vit_Top
	{
		.SpriteID_t = Sprites::SpriteID_t::Vital_HP,
		.Scale = 0.7210000157356262f,
		.Position = { 0.1850000023841858f, 0.37700000405311584f },
	},
	SpriteTransform // HP_Bar_Vit_Bottom
	{
		.SpriteID_t = Sprites::SpriteID_t::Vital_HP,
		.Scale = 0.7210000157356262f,
		.Position = { 0.23399999737739563f, 0.15600000321865082f },
	},
	SpriteTransform // RG_Guage_Fill_Lv1
	{
		.SpriteID_t = Sprites::SpriteID_t::Fill_RG_1,
		.Scale = 0.05400000140070915f,
		.Position = { -0.7799999713897705f, -0.7760000228881836f },
	},
	SpriteTransform // RG_Guage_Fill_Lv2
	{
		.SpriteID_t = Sprites::SpriteID_t::Fill_RG_2,
		.Scale = 0.10000000149011612f,
		.Position = { -0.777999997138977f, -0.7620000243186951f },
	},
	SpriteTransform // RG_Guage_Fill_Lv3
	{
		.SpriteID_t = Sprites::SpriteID_t::Fill_RG_3,
		.Scale = 0.14499999582767487f,
		.Position = { -0.781000018119812f, -0.699999988079071f },
	},
	SpriteTransform // RG_Guage_Frame_Lv1
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_RG_1,
		.Scale = 0.061000000685453415f,
		.Position = { -0.7820000052452087f, -0.7559999823570251f },
	},
	SpriteTransform // RG_Guage_Frame_Lv2
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_RG_2,
		.Scale = 0.10999999940395355f,
		.Position = { -0.777999997138977f, -0.7490000128746033f },
	},
	SpriteTransform // RG_Guage_Frame_Lv3
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_RG_3,
		.Scale = 0.15800000727176666f,
		.Position = { -0.781000018119812f, -0.6830000281333923f },
	},
	SpriteTransform // Shattered_BG_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_DG,
		.Scale = 0.3449999988079071f,
		.Position = { -0.8550000190734863f, 0.1420000046491623f },
	},
	SpriteTransform // Shattered_BG_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_DS,
		.Scale = 0.421999990940094f,
		.Position = { -0.7559999823570251f, 0.1340000033378601f },
	},
	SpriteTransform // Shattered_BG_G
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_G,
		.Scale = 0.39899998903274536f,
		.Position = { -0.8519999980926514f, 0.19200000166893005f },
	},
	SpriteTransform // Shattered_BG_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_QS,
		.Scale = 0.33799999952316284f,
		.Position = { -0.7749999761581421f, -0.05299999937415123f },
	},
	SpriteTransform // Shattered_BG_R
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_R,
		.Scale = 0.3100000023841858f,
		.Position = { -0.7829999923706055f, -0.07800000160932541f },
	},
	SpriteTransform // Shattered_BG_S
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_S,
		.Scale = 0.31200000643730164f,
		.Position = { -0.6970000267028809f, 0.20100000500679016f },
	},
	SpriteTransform // Shattered_BG_T
	{
		.SpriteID_t = Sprites::SpriteID_t::BG_T,
		.Scale = 0.37400001287460327f,
		.Position = { -0.7739999890327454f, 0.4300000071525574f },
	},
	SpriteTransform // Style_Exp_Bar_Crimson
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_Crimson,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_DG,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_DS,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_G,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_QS,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_RG
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_RG,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_S,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_T
	{
		.SpriteID_t = Sprites::SpriteID_t::Exp_T,
		.Scale = 0.29100000858306885f,
		.Position = { -0.2150000035762787f, 0.5199999809265137f },
	},
	SpriteTransform // Style_Exp_Bar_Frame
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_Exp,
		.Scale = 0.3310000002384186f,
		.Position = { -0.25f, 0.5370000004768372f },
	},
	SpriteTransform // Style_Exp_Bar_Level_Digit_Lv1
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_Exp_Lv1,
		.Scale = 0.10999999940395355f,
		.Position = { 0.10300000011920929f, 0.5950000286102295f },
	},
	SpriteTransform // Style_Exp_Bar_Level_Digit_Lv2
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_Exp_Lv2,
		.Scale = 0.10999999940395355f,
		.Position = { 0.13500000536441803f, 0.5960000157356262f },
	},
	SpriteTransform // Style_Exp_Bar_Level_Digit_Lv3
	{
		.SpriteID_t = Sprites::SpriteID_t::Frame_Exp_Lv3,
		.Scale = 0.10499999672174454f,
		.Position = { 0.13099999725818634f, 0.5799999833106995f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_DG,
		.Scale = 0.25699999928474426f,
		.Position = { -0.18700000643730164f, 0.7310000061988831f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_DS,
		.Scale = 0.21199999749660492f,
		.Position = { -0.23100000619888306f, 0.7519999742507935f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_G,
		.Scale = 0.21199999749660492f,
		.Position = { -0.23199999332427979f, 0.7739999890327454f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_QS,
		.Scale = 0.19900000095367432f,
		.Position = { -0.23800000548362732f, 0.7570000290870667f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_R
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_R,
		.Scale = 0.21199999749660492f,
		.Position = { -0.2329999953508377f, 0.7519999742507935f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_S,
		.Scale = 0.2160000056028366f,
		.Position = { -0.2199999988079071f, 0.7739999890327454f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Colored_T
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Colored_T,
		.Scale = 0.13300000131130219f,
		.Position = { -0.296999990940094f, 0.7670000195503235f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_DG
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_DG,
		.Scale = 0.25699999928474426f,
		.Position = { -0.18700000643730164f, 0.7310000061988831f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_DS
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_DS,
		.Scale = 0.21199999749660492f,
		.Position = { -0.23100000619888306f, 0.7519999742507935f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_G
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_G,
		.Scale = 0.21199999749660492f,
		.Position = { -0.23199999332427979f, 0.7739999890327454f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_QS
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_QS,
		.Scale = 0.19900000095367432f,
		.Position = { -0.23800000548362732f, 0.7570000290870667f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_R
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_R,
		.Scale = 0.21199999749660492f,
		.Position = { -0.2329999953508377f, 0.7519999742507935f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_S
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_S,
		.Scale = 0.2160000056028366f,
		.Position = { -0.2199999988079071f, 0.7739999890327454f },
	},
	SpriteTransform // Style_Exp_Bar_Name_Written_Crimson_T
	{
		.SpriteID_t = Sprites::SpriteID_t::Written_Crimson_T,
		.Scale = 0.13300000131130219f,
		.Position = { -0.296999990940094f, 0.7670000195503235f },
	},
};

static_assert(sizeof(SPRITE_TRANSFORMS) / sizeof(SpriteTransform) == (size_t)TransformID_t::SIZE);

constexpr const auto& GetSpriteTransform(TransformID_t spriteTransformID)
{
	return SPRITE_TRANSFORMS[(size_t)spriteTransformID];
}

constexpr const auto& GetCompassFrame(Theme_t theme)
{
	return GetSpriteTransform(TransformID_t::Compass_Frame);
}

constexpr const auto& GetCompassCenterPiece(Theme_t theme, Style_t style)
{
	switch (style)
	{
	default:
	case Style_t::Trickster:    return GetSpriteTransform(TransformID_t::Compass_Center_Piece_T);
	case Style_t::Swordmaster:  return GetSpriteTransform(TransformID_t::Compass_Center_Piece_S);
	case Style_t::Royalguard:   return GetSpriteTransform(TransformID_t::Compass_Center_Piece_R);
	case Style_t::Gunslinger:   return GetSpriteTransform(TransformID_t::Compass_Center_Piece_G);
	case Style_t::Darkslayer:   return GetSpriteTransform(TransformID_t::Compass_Center_Piece_DS);
	case Style_t::Quicksilver:  return GetSpriteTransform(TransformID_t::Compass_Center_Piece_QS);
	case Style_t::Doppleganger: return GetSpriteTransform(TransformID_t::Compass_Center_Piece_DG);
	}
}

constexpr const auto& GetCompassStyleShatteredBackground(Theme_t theme, Style_t style)
{
	switch (style)
	{
	default:
	case Style_t::Trickster:    return GetSpriteTransform(TransformID_t::Shattered_BG_T);
	case Style_t::Swordmaster:  return GetSpriteTransform(TransformID_t::Shattered_BG_S);
	case Style_t::Royalguard:   return GetSpriteTransform(TransformID_t::Shattered_BG_R);
	case Style_t::Gunslinger:   return GetSpriteTransform(TransformID_t::Shattered_BG_G);
	case Style_t::Darkslayer:   return GetSpriteTransform(TransformID_t::Shattered_BG_DS);
	case Style_t::Quicksilver:  return GetSpriteTransform(TransformID_t::Shattered_BG_QS);
	case Style_t::Doppleganger: return GetSpriteTransform(TransformID_t::Shattered_BG_DG);
	}
}

constexpr const auto& GetCompassTopSlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Trickster:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Top_Shadowed_T);
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Top_Colored_T);
		}
	} break;
	}
}

constexpr const auto& GetCompassRightSlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Swordmaster:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Right_Shadowed_S);
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Right_Colored_S);
		}
	} break;

	case Style_t::Darkslayer:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Right_Colored_DS);
		}
	} break;
	}
}

constexpr const auto& GetCompassBottomSlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Royalguard:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Bottom_Shadowed_R);
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Bottom_Colored_R);
		}
	} break;

	case Style_t::Quicksilver:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Bottom_Colored_QS);
		}
	} break;
	}
}

constexpr const auto& GetCompassLeftSlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Gunslinger:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Left_Shadowed_G);
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Left_Colored_G);
		}
	} break;

	case Style_t::Doppleganger:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Colored:  return GetSpriteTransform(TransformID_t::Compass_Slots_Left_Colored_DG);
		}
	} break;
	}
}

constexpr const auto& GetCompassRightSecondarySlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Swordmaster:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Right_Sec_Shadowed_S);
		}
	} break;

	case Style_t::Darkslayer:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Right_Sec_Shadowed_DS);
		}
	} break;
	}
}

constexpr const auto& GetCompassBottomSecondarySlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Royalguard:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Bottom_Sec_Shadowed_R);
		}
	} break;

	case Style_t::Quicksilver:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Bottom_Sec_Shadowed_QS);
		}
	} break;
	}
}

constexpr const auto& GetCompassLeftSecondarySlotInitials(Theme_t theme, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (style)
	{
	default:
	case Style_t::Gunslinger:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Left_Sec_Shadowed_G);
		}
	} break;

	case Style_t::Doppleganger:
	{
		switch (initialsVarient)
		{
		default:
		case StyleInitialsVarient_t::Shadowed: return GetSpriteTransform(TransformID_t::Compass_Slots_Left_Sec_Shadowed_DG);
		}
	} break;
	}
}

constexpr const auto& GetCompassStyleInitials(Theme_t theme, CompassSlotID_t slot, Style_t style, StyleInitialsVarient_t initialsVarient)
{
	switch (slot)
	{
	default:
	case CompassSlotID_t::Top:			   return GetCompassTopSlotInitials(theme, style, initialsVarient);
	case CompassSlotID_t::Right:		   return GetCompassRightSlotInitials(theme, style, initialsVarient);
	case CompassSlotID_t::Bottom:		   return GetCompassBottomSlotInitials(theme, style, initialsVarient);
	case CompassSlotID_t::Left:			   return GetCompassLeftSlotInitials(theme, style, initialsVarient);
	case CompassSlotID_t::RightSecondary:  return GetCompassRightSecondarySlotInitials(theme, style, initialsVarient);
	case CompassSlotID_t::BottomSecondary: return GetCompassBottomSecondarySlotInitials(theme, style, initialsVarient);
	case CompassSlotID_t::LeftSecondary:   return GetCompassLeftSecondarySlotInitials(theme, style, initialsVarient);
	}
}

constexpr const auto& GetTopDTBarFrame(Theme_t theme, DTBarFrameID_t dtBarFrame)
{
	switch (dtBarFrame)
	{
	default:
	case DTBarFrameID_t::F123: return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_123);
	case DTBarFrameID_t::F4:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_4);
	case DTBarFrameID_t::F5:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_5);
	case DTBarFrameID_t::F6:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_6);
	case DTBarFrameID_t::F7:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_7);
	case DTBarFrameID_t::F8:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_8);
	case DTBarFrameID_t::F9:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_9);
	case DTBarFrameID_t::F10:  return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Top_10);
	}
}

constexpr const auto& GetBottomDTBarFrame(Theme_t theme, DTBarFrameID_t dtBarFrame)
{
	switch (dtBarFrame)
	{
	default:
	case DTBarFrameID_t::F123: return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_123);
	case DTBarFrameID_t::F4:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_4);
	case DTBarFrameID_t::F5:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_5);
	case DTBarFrameID_t::F6:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_6);
	case DTBarFrameID_t::F7:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_7);
	case DTBarFrameID_t::F8:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_8);
	case DTBarFrameID_t::F9:   return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_9);
	case DTBarFrameID_t::F10:  return GetSpriteTransform(TransformID_t::DT_Bar_Frames_Bottom_10);
	}
}

constexpr const auto& GetDTBarFrame(Theme_t theme, BarPositionID_t barPosition, DTBarFrameID_t dtFrame)
{
	switch (barPosition)
	{
	default:
	case BarPositionID_t::Top:	  return GetTopDTBarFrame(theme, dtFrame);
	case BarPositionID_t::Bottom: return GetBottomDTBarFrame(theme, dtFrame);
	}
}

constexpr const auto& GetTopDTBarOrb(Theme_t theme, size_t level)
{
	switch (level)
	{
	default:
	case 1:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_0);
	case 2:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_1);
	case 3:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_2);
	case 4:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_3);
	case 5:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_4);
	case 6:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_5);
	case 7:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_6);
	case 8:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_7);
	case 9:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_8);
	case 10: return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Top_9);
	}
}

constexpr const auto& GetBottomDTBarOrb(Theme_t theme, size_t level)
{
	switch (level)
	{
	default:
	case 1:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_0);
	case 2:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_1);
	case 3:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_2);
	case 4:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_3);
	case 5:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_4);
	case 6:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_5);
	case 7:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_6);
	case 8:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_7);
	case 9:  return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_8);
	case 10: return GetSpriteTransform(TransformID_t::DT_Bar_Orbs_Bottom_9);
	}
}

constexpr const auto& GetDTBarOrb(Theme_t theme, BarPositionID_t barPosition, size_t level)
{
	switch (barPosition)
	{
	default:
	case BarPositionID_t::Top:	  return GetTopDTBarOrb(theme, level);
	case BarPositionID_t::Bottom: return GetBottomDTBarOrb(theme, level);
	}
}

constexpr const auto& GetTopHPBar(Theme_t theme, HPBarVarient_t hpBarVarient)
{
	switch (hpBarVarient)
	{
	case HPBarVarient_t::Background: return GetSpriteTransform(TransformID_t::HP_Bar_BG_Top);
	case HPBarVarient_t::Damage:	 return GetSpriteTransform(TransformID_t::HP_Bar_Dmg_Top);
	case HPBarVarient_t::Vitality:	 return GetSpriteTransform(TransformID_t::HP_Bar_Vit_Top);
	}
}

constexpr const auto& GetBottomHPBar(Theme_t theme, HPBarVarient_t hpBarVarient)
{
	switch (hpBarVarient)
	{
	case HPBarVarient_t::Background: return GetSpriteTransform(TransformID_t::HP_Bar_BG_Bottom);
	case HPBarVarient_t::Damage:	 return GetSpriteTransform(TransformID_t::HP_Bar_Dmg_Bottom);
	case HPBarVarient_t::Vitality:	 return GetSpriteTransform(TransformID_t::HP_Bar_Vit_Bottom);
	}
}

constexpr const auto& GetHPBar(Theme_t theme, BarPositionID_t barPosition, HPBarVarient_t hpBarVarient)
{
	switch (barPosition)
	{
	case BarPositionID_t::Top:	  return GetTopHPBar(theme, hpBarVarient);
	case BarPositionID_t::Bottom: return GetBottomHPBar(theme, hpBarVarient);
	}
}

constexpr const auto& GetHPBarFrame(Theme_t theme, size_t level, HPBarFrameVarient_t frameVarient)
{
	switch (frameVarient)
	{
	default:
	case HPBarFrameVarient_t::Single:
	{
		switch (level)
		{
		default:
		case 1:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_0_Single);
		case 11: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_10_Single);
		}
	} break;

	case HPBarFrameVarient_t::Head:
	{
		switch (level)
		{
		default:
		case 1:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_0_Start);
		case 11: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_10_Start);
		}
	} break;

	case HPBarFrameVarient_t::Middle:
	{
		switch (level)
		{
		default:
		case 2:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_1_Middle);
		case 3:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_2_Middle);
		case 4:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_3_Middle);
		case 5:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_4_Middle);
		case 6:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_5_Middle);
		case 7:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_6_Middle);
		case 8:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_7_Middle);
		case 9:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_8_Middle);
		case 12: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_11_Middle);
		case 13: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_12_Middle);
		case 14: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_13_Middle);
		case 15: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_14_Middle);
		case 16: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_15_Middle);
		case 17: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_16_Middle);
		case 18: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_17_Middle);
		case 19: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_18_Middle);
		}
	} break;

	case HPBarFrameVarient_t::Tail:
	{
		switch (level)
		{
		default:
		case 2:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_1_End);
		case 3:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_2_End);
		case 4:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_3_End);
		case 5:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_4_End);
		case 6:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_5_End);
		case 7:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_6_End);
		case 8:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_7_End);
		case 9:  return GetSpriteTransform(TransformID_t::HP_Bar_Frame_8_End);
		case 10: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_9_End);
		case 12: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_11_End);
		case 13: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_12_End);
		case 14: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_13_End);
		case 15: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_14_End);
		case 16: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_15_End);
		case 17: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_16_End);
		case 18: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_17_End);
		case 19: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_18_End);
		case 20: return GetSpriteTransform(TransformID_t::HP_Bar_Frame_19_End);
		}
	} break;
	}
}

constexpr const auto& GetVitalityBarGlow(Theme_t theme, BarPositionID_t barPosition, GlowPos_t glowPosition)
{
	switch (barPosition)
	{
	default:
	case BarPositionID_t::Top:
	{
		switch (glowPosition)
		{
		case GlowPos_t::Head: return GetSpriteTransform(TransformID_t::HP_Bar_Glow_Top_Head);
		case GlowPos_t::Tail: return GetSpriteTransform(TransformID_t::HP_Bar_Glow_Top_Tail);
		}
	} break;

	case BarPositionID_t::Bottom:
	{
		switch (glowPosition)
		{
		case GlowPos_t::Head: return GetSpriteTransform(TransformID_t::HP_Bar_Glow_Bottom_Head);
		case GlowPos_t::Tail: return GetSpriteTransform(TransformID_t::HP_Bar_Glow_Bottom_Tail);
		}
	} break;
	}
}

constexpr const auto& GetRGGuageFrame(Theme_t theme, size_t level)
{
	switch (level)
	{
	default:
	case 1: return GetSpriteTransform(TransformID_t::RG_Guage_Frame_Lv1);
	case 2: return GetSpriteTransform(TransformID_t::RG_Guage_Frame_Lv2);
	case 3: return GetSpriteTransform(TransformID_t::RG_Guage_Frame_Lv3);
	}
}

constexpr const auto& GetRGGuage(Theme_t theme, size_t level)
{
	switch (level)
	{
	default:
	case 1: return GetSpriteTransform(TransformID_t::RG_Guage_Fill_Lv1);
	case 2: return GetSpriteTransform(TransformID_t::RG_Guage_Fill_Lv2);
	case 3: return GetSpriteTransform(TransformID_t::RG_Guage_Fill_Lv3);
	}
}

constexpr const auto& GetStyleExpBarFrame(Theme_t theme)
{
	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Frame);
}

constexpr const auto& GetStyleExpBar(Theme_t theme, Style_t style)
{
	if (theme == Theme_t::Crimson) return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Crimson);

	switch (style)
	{
	default:
	case Style_t::Trickster:	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	case Style_t::Swordmaster:	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	case Style_t::Royalguard:	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	case Style_t::Gunslinger:	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	case Style_t::Darkslayer:	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	case Style_t::Quicksilver:	return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	case Style_t::Doppleganger: return GetSpriteTransform(TransformID_t::Style_Exp_Bar_T);
	}
}

constexpr const auto& GetStyleExpBarLevelDigit(Theme_t theme, size_t level)
{
	switch (level)
	{
	default:
	case 1: return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Level_Digit_Lv1);
	case 2: return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Level_Digit_Lv2);
	case 3: return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Level_Digit_Lv3);
	}
}

constexpr const auto& GetStyleExpBarName(Theme_t theme, Style_t style)
{
	switch (theme)
	{
	default:
	case Theme_t::Colored:
	{
		switch (style)
		{
		default:
		case Style_t::Trickster:    return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_T);
		case Style_t::Swordmaster:  return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_S);
		case Style_t::Royalguard:   return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_R);
		case Style_t::Gunslinger:   return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_G);
		case Style_t::Darkslayer:   return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_DS);
		case Style_t::Quicksilver:  return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_QS);
		case Style_t::Doppleganger: return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Colored_DG);
		}
	} break;

	case Theme_t::Crimson:
	{
		switch (style)
		{
		default:
		case Style_t::Trickster:    return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_T);
		case Style_t::Swordmaster:  return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_S);
		case Style_t::Royalguard:   return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_R);
		case Style_t::Gunslinger:   return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_G);
		case Style_t::Darkslayer:   return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_DS);
		case Style_t::Quicksilver:  return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_QS);
		case Style_t::Doppleganger: return GetSpriteTransform(TransformID_t::Style_Exp_Bar_Name_Written_Crimson_DG);
		}
	} break;
	}
}

auto GetVitalityGlowX(BarPositionID_t barPosition, size_t level, float fraction)
{
	Theme_t theme			    = Theme_t::Colored;
	auto    headPos			    = GetVitalityBarGlow(theme, barPosition, GlowPos_t::Head).Position.x;
	auto    tailMaxPos		    = GetVitalityBarGlow(theme, barPosition, GlowPos_t::Tail).Position.x;
	auto    currentLevelTailPos = glm::mix(headPos, tailMaxPos, level / 10.0f);

	return glm::mix(headPos, currentLevelTailPos, fraction);
}
}

namespace Masks
{
enum class MaskInfoID_t
{
	HPBarVitalitySlantedHead = 0,
	HPBarVitalitySlantedTail,
	HPBarDamageSlantedHead,
	HPBarDamageSlantedTail,
	HPBarBackgroundSlantedHead,
	HPBarBackgroundSlantedTail,
	RGGuageBarLV1,
	RGGuageBarLV2,
	RGGuageBarLV3,
	StyleExpBarSlantedTail,
	HPBarTailGlow,

	SIZE
};

constexpr Graphics::BatchedSprites::MaskInfo MASKS_INFO[] =
{
	{ // HPBarVitalitySlantedHead
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 0.0f, 0.0f },
			.LineNormalDeg = 325.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // HPBarVitalitySlantedTail
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 33.8f, 0.0f },
			.LineNormalDeg = 145.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // HPBarDamageSlantedHead
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 0.0f, 0.0f },
			.LineNormalDeg = 325.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // HPBarDamageSlantedTail
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin = { 33.385f, 0.0f },
			.LineNormalDeg = 145.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // HPBarBackgroundSlantedHead
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 0.0f, 0.0f },
			.LineNormalDeg = 325.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // HPBarBackgroundSlantedTail
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 33.8f, 0.0f },
			.LineNormalDeg = 145.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // RGGuageBarLV1
		.Type = Graphics::BatchedSprites::MaskType_t::RadialMask,
		.Radial =
		{
			.Origin		  = { 2.912f, 13.113f },
			.Radius		  = 15.0f,
			.StartDeg	  = 257.2f,
			.EndDeg		  = 285.45f,
			.KeepPositive = true,
			.FlipMask	  = false
		}
	},
	{ // RGGuageBarLV2
		.Type = Graphics::BatchedSprites::MaskType_t::RadialMask,
		.Radial =
		{
			.Origin		  = { 2.912f, 5.831f },
			.Radius		  = 6.0f,
			.StartDeg	  = 240.3f,
			.EndDeg		  = 299.7f,
			.KeepPositive = true,
			.FlipMask	  = false
		}
	},
	{ // RGGuageBarLV3
		.Type = Graphics::BatchedSprites::MaskType_t::RadialMask,
		.Radial =
		{
			.Origin		  = { 2.206f, 3.225f },
			.Radius		  = 4.0f,
			.StartDeg	  = 226.45f,
			.EndDeg		  = 313.3f,
			.KeepPositive = true,
			.FlipMask	  = false
		}
	},
	{ // StyleExpBarSlantedTail
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 68.415f, 0.0f },
			.LineNormalDeg = 180.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	},
	{ // HPBarTailGlow
		.Type = Graphics::BatchedSprites::MaskType_t::SlantedMask,
		.Slanted =
		{
			.Origin		   = { 0.0f, 0.0f },
			.LineNormalDeg = 325.0f,
			.SoftnessRange = { 0.0f, 0.0f }
		}
	}
};

static_assert(sizeof(MASKS_INFO) / sizeof(Graphics::BatchedSprites::MaskInfo) == (size_t)MaskInfoID_t::SIZE);

constexpr const auto& GetMaskInfo(MaskInfoID_t maskKey)
{
	return MASKS_INFO[(size_t)maskKey];
}

std::vector<Graphics::BatchedSprites::MaskInfo> GetHPBarBackgroundMasks(float fraction = 1.0f)
{
	constexpr auto headMaskInfo = GetMaskInfo(MaskInfoID_t::HPBarBackgroundSlantedHead);
	auto		   tailMaskInfo = GetMaskInfo(MaskInfoID_t::HPBarBackgroundSlantedTail);

	tailMaskInfo.Slanted.Origin.x = glm::mix(headMaskInfo.Slanted.Origin.x, tailMaskInfo.Slanted.Origin.x, fraction);

	return { headMaskInfo, tailMaskInfo };
}

std::vector<Graphics::BatchedSprites::MaskInfo> GetHPBarDamageMasks(float fraction = 1.0f)
{
	constexpr auto headMaskInfo = GetMaskInfo(MaskInfoID_t::HPBarDamageSlantedHead);
	auto		   tailMaskInfo = GetMaskInfo(MaskInfoID_t::HPBarDamageSlantedTail);

	tailMaskInfo.Slanted.Origin.x = glm::mix(headMaskInfo.Slanted.Origin.x, tailMaskInfo.Slanted.Origin.x, fraction);

	return { headMaskInfo, tailMaskInfo };
}

std::vector<Graphics::BatchedSprites::MaskInfo> GetHPBarVitalityMasks(float fraction = 1.0f)
{
	constexpr auto headMaskInfo = GetMaskInfo(MaskInfoID_t::HPBarVitalitySlantedHead);
	auto		   tailMaskInfo = GetMaskInfo(MaskInfoID_t::HPBarVitalitySlantedTail);

	tailMaskInfo.Slanted.Origin.x = glm::mix(headMaskInfo.Slanted.Origin.x, tailMaskInfo.Slanted.Origin.x, fraction);

	return { headMaskInfo, tailMaskInfo };
}

constexpr std::vector<Graphics::BatchedSprites::MaskInfo> GetStyleExpBarMasks()
{
	return { GetMaskInfo(MaskInfoID_t::StyleExpBarSlantedTail) };
}

constexpr std::vector<Graphics::BatchedSprites::MaskInfo> GetRGGuageBarMasks(size_t level)
{
	switch (level)
	{
	default:
	case 1:
		return { GetMaskInfo(MaskInfoID_t::RGGuageBarLV1) };

	case 2:
		return { GetMaskInfo(MaskInfoID_t::RGGuageBarLV2) };

	case 3:
		return { GetMaskInfo(MaskInfoID_t::RGGuageBarLV3) };
	}
}

constexpr std::vector<Graphics::BatchedSprites::MaskInfo> GetHPBarVitalityEndGlowMasks()
{
	return { GetMaskInfo(MaskInfoID_t::HPBarTailGlow) };
}

}

namespace Processed
{
float GetGlowMaskRangeX()
{
	using namespace Sprites;
	using namespace Masks;

	constexpr auto& maskInfo	= GetMaskInfo(MaskInfoID_t::HPBarTailGlow);
	constexpr auto  angleRad	= glm::radians(maskInfo.Slanted.LineNormalDeg);
	auto			extra		= glm::abs(glm::tan(angleRad));
	constexpr auto  spriteWidth = GetSpriteInfo(GetHPBarVitalityTailGlowSpriteID(Theme_t::Colored)).RegionRect.GetAspectRatio();

	return spriteWidth - extra;
}
}
}