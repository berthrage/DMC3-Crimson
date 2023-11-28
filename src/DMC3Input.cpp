// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "Global.hpp"

#include "Core/Macros.h"

void ToggleCursor()
{
	if
	(
		g_showMain ||
		g_showShop
	)
	{
		Windows_ToggleCursor(true);
	}
	else
	{
		Windows_ToggleCursor(!activeConfig.hideMouseCursor);
	}
}