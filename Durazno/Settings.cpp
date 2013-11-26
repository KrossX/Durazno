/*  Durazno - XInput to XInput Wrapper
 *  Copyright (C) 2012 KrossX
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TypeDefs.h"
#include "Settings.h"

_Settings::_Settings()
{
	port = 0;
	isDisabled = false;
	isDummy = false;
	deadzone = 0.0;
	antiDeadzone = 0.0;
	rumble = 1.0;
	linearity = 0;
	triggerMin = 0;
	triggerMax = 255;
	
	for(s16 i = 0; i < 4; i++)
	{
		axisInverted[i] = false;
	}

	for (int i = 0; i < 24; i++)
	{
		remap[i].control = i;

		if(i < 14) remap[i].type = RT_DIGITAL_DIGITAL;
		else if(i < 16) remap[i].type = RT_TRIGGER_TRIGGER;
		else remap[i].type = RT_ANALOG_ANALOG;
	}
}