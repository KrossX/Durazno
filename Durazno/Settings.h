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

enum GAMEPAD_AXES
{
	GP_AXIS_LX,
	GP_AXIS_LY,
	GP_AXIS_RX,
	GP_AXIS_RY
};

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

struct _Gamepad
{
	u16 buttons;
	u8 triggerL, triggerR;
	s32 analogLX, analogLY;
	s32 analogRX, analogRY;
};

struct _Remap
{
	u8 type;
	u8 control;	
};

class _Settings
{
public:
	_Settings();

public:
	u8 port;
	bool isDisabled, isDummy;
	f64 deadzone, antiDeadzone, rumble;
	bool axisInverted[4];
	f64 linearity;
	u8 triggerMin, triggerMax;
	
	_Remap remap[24];
	
private:
	_Settings(const _Settings &);
	_Settings& operator=(const _Settings &);
};