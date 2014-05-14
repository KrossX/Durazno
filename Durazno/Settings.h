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

#pragma once

enum REMAP_TYPE
{
	RT_DIGITAL_DIGITAL,
	RT_DIGITAL_TRIGGER,
	RT_DIGITAL_ANALOG,

	RT_TRIGGER_DIGITAL,
	RT_TRIGGER_TRIGGER,
	RT_TRIGGER_ANALOG,

	RT_ANALOG_DIGITAL,
	RT_ANALOG_TRIGGER,
	RT_ANALOG_ANALOG
};

struct GAMEPAD
{
	u16 buttons;
	u8 triggerL, triggerR;
	s32 analogLX, analogLY;
	s32 analogRX, analogRY;
};

struct REMAP
{
	u8 type;
	u8 control;
};

struct STICK
{
	bool invertedX, invertedY;
	f64 linearity, deadzone, antiDeadzone;

	STICK();
};

class SETTINGS
{
public:
	SETTINGS();

public:
	u8 port;
	bool linearDZ, linearADZ;
	bool isDisabled, isDummy;
	
	u8 triggerMin, triggerMax;
	
	f64 rumble;
	STICK stickL, stickR;
	
	REMAP remap[24];
	
private:
	SETTINGS(const SETTINGS &);
	SETTINGS& operator=(const SETTINGS &);
};