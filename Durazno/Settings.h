/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
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

struct TRIGGER
{
	u8 min = 0;
	u8 max = 255;
};

struct STICK
{
	bool invertedX = false;
	bool invertedY = false;
	
	f64 linearity    = 1.0;
	f64 deadzone     = 0.0;
	f64 antiDeadzone = 0.0;

	f64 dzcheck = 0.0;
	f64 adzcheck = 0.0;

	f64 dzconst, adzconst;
};

class SETTINGS
{
public:
	SETTINGS();

public:
	u8 port = 0;

	bool linearDZ   = false;
	bool linearADZ  = false;
	bool isDisabled = false;
	bool isDummy    = false;
	
	f64 rumble = 1.0;

	TRIGGER triggerL, triggerR;
	STICK stickL, stickR;
	REMAP remap[24];
	
private:
	SETTINGS(const SETTINGS &);
	SETTINGS& operator=(const SETTINGS &);
};

void SetStickConsts(STICK &s);