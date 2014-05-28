/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "TypeDefs.h"
#include "Settings.h"

STICK::STICK():
	invertedX(false),
	invertedY(false),
	deadzone(0.0),
	antiDeadzone(0.0),
	linearity(0.0)
{
}

void STICK::SetConsts()
{
	f64 const analogmax = 32767.0; // 40201 real max radius

	dzcheck = deadzone * analogmax;
	adzcheck = antiDeadzone * analogmax;

	dzconst = analogmax / (analogmax - dzcheck);
	adzconst = (analogmax - adzcheck) / analogmax;
}

SETTINGS::SETTINGS():
	port(0),
	isDisabled(false),
	isDummy(false),
	linearDZ(false),
	linearADZ(false),
	rumble(0.0),
	triggerMin(0),
	triggerMax(255)
{
	for (int i = 0; i < 24; i++)
	{
		remap[i].control = i;

		if(i < 14) remap[i].type = RT_DIGITAL_DIGITAL;
		else if(i < 16) remap[i].type = RT_TRIGGER_TRIGGER;
		else remap[i].type = RT_ANALOG_ANALOG;
	}
}