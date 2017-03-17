/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "TypeDefs.h"
#include "Settings.h"

void SetStickConsts(STICK &s)
{
	f64 const analogmax = 32767.0; // 40201 real max radius

	s.dzcheck = s.deadzone * analogmax;
	s.adzcheck = s.antiDeadzone * analogmax;

	s.dzconst = analogmax / (analogmax - s.dzcheck);
	s.adzconst = (analogmax - s.adzcheck) / analogmax;
}

SETTINGS::SETTINGS()
{
	SetStickConsts(stickL);
	SetStickConsts(stickR);

	for (int i = 0; i < 24; i++)
	{
		remap[i].control = i;

		if(i < 14) remap[i].type = RT_DIGITAL_DIGITAL;
		else if(i < 16) remap[i].type = RT_TRIGGER_TRIGGER;
		else remap[i].type = RT_ANALOG_ANALOG;
	}
}