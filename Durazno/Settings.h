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

struct STICK
{
	bool invertedX, invertedY;
	f64 linearity, deadzone, antiDeadzone;
	f64 dzconst, adzconst;
	f64 dzcheck, adzcheck;

	void SetConsts();

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