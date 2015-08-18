/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "Durazno.h"
#include "Settings.h"
#include "Transform.h"

#include <cmath>

namespace Gamepad
{
	enum Controls
	{
		DPAD_UP = 0,
		DPAD_DOWN,
		DPAD_LEFT,
		DPAD_RIGHT,
		START,
		BACK,
		LEFT_THUMB,
		RIGHT_THUMB,
		LEFT_SHOULDER,
		RIGHT_SHOULDER,
		A,
		B,
		X,
		Y,
		LEFT_TRIGGER,
		RIGHT_TRIGGER,
		THUMB_LX_P,
		THUMB_LX_M,
		THUMB_LY_P,
		THUMB_LY_M,
		THUMB_RX_P,
		THUMB_RX_M,
		THUMB_RY_P,
		THUMB_RY_M,
		DISABLED
	};
};

f64 const analogmax = 32767.0; // 40201 real max radius

inline f64 Radius(f64 X, f64 Y)
{
	return sqrt(X*X + Y*Y);
}

inline f64 Deadzone(f64 val, f64 deadzone, f64 k)
{
	f64 value = std::abs(val);

	if (value <= deadzone)
	{
		val = 0;
	}
	else
	{
		//value = (value - deadzone) * analogmax / (analogmax - deadzone);
		value = (value - deadzone) * k;
		val = val < 0 ? -value : value;
	}

	return val;
}

inline f64 AntiDeadzone(f64 val, f64 antideadzone, f64 k)
{
	f64 value = std::abs(val);

	if (value >(1.0 / 65535.0))
	{
		//value = value * ((analogmax - antideadzone) / analogmax) + antideadzone;
		value = value * k + antideadzone;
		val = val < 0 ? -value : value;
	}

	return val;
}


inline f64 Linearity(f64 radius, f64 linearity)
{
	const f64 exp = linearity > 0 ? linearity +1 : 1.0/(-linearity+1);
	return pow(radius / 32768.0, exp) * 32768.0;
}

void __fastcall TriggerRange(u8 &trigger, SETTINGS &set)
{
	u8 range = set.triggerMax - set.triggerMin;

	if(!range) return;

	u16 tg = trigger;
	u8 dz = set.triggerMin;

	tg = tg <= dz ? 0 : ((tg - dz) * 255) / range;
	trigger = tg > 255 ? 255 : tg;
}

class tPOINT
{
	f64 X, Y;
	f64 r, rX, rY;
	f64 dzk, adzk;

	void SetPoint(f64 inX, f64 inY)
	{
		X = inX;
		Y = inY;

		r = Radius(X, Y);
		rX = X / r;
		rY = Y / r;
	}

	void SetRadius(f64 inR)
	{
		r = inR;
		X = rX * r;
		Y = rY * r;
	}

public:
	void ApplyLinearity(f64 linearity)
	{
		f64 radius = Linearity(r, linearity);
		SetRadius(radius);
	}

	void ApplyDeadzone(f64 deadzone, bool linear)
	{
		if (linear)
		{
			f64 fX = Deadzone(X, deadzone, dzk);
			f64 fY = Deadzone(Y, deadzone, dzk);
			SetPoint(fX, fY);
		}
		else
		{
			f64 radius = Deadzone(r, deadzone, dzk);
			SetRadius(radius);
		}
	}

	void ApplyAntiDeadzone(f64 antideadzone, bool linear)
	{
		if (linear)
		{
			f64 fX = AntiDeadzone(X, antideadzone, adzk);
			f64 fY = AntiDeadzone(Y, antideadzone, adzk);
			SetPoint(fX, fY);
		}
		else
		{
			f64 radius = AntiDeadzone(r, antideadzone, adzk);
			SetRadius(radius);
		}
	}

	f64 GetX() { return X; }
	f64 GetY() { return Y; }

	tPOINT(s16 inX, s16 inY, f64 dzconst, f64 adzconst):
		dzk(dzconst), adzk(adzconst)
	{
		X = (f64)inX;
		Y = (f64)inY;

		r = Radius(X, Y);
		rX = X / r;
		rY = Y / r;
	}
};

void __fastcall TransformAnalog(s16 &X, s16 &Y, SETTINGS &set, bool leftStick)
{
	// If input is dead, no need to check or do anything else
	if((X == 0) && (Y == 0)) return;

	STICK stick = leftStick ? set.stickL : set.stickR;
	tPOINT point(X, Y, stick.dzconst, stick.adzconst);

	if (stick.linearity != 0) point.ApplyLinearity(stick.linearity);
	if (stick.dzcheck  > 0) point.ApplyDeadzone(stick.dzcheck, set.linearDZ);
	if (stick.adzcheck > 0) point.ApplyAntiDeadzone(stick.adzcheck, set.linearADZ);

	f64 fX = stick.invertedX ? point.GetX() * -1.0 : point.GetX();
	f64 fY = stick.invertedY ? point.GetY() * -1.0 : point.GetY();

	X = s16(fX < -32767.0 ? -32767 : fX > 32767.0 ? 32767 : fX);
	Y = s16(fY < -32767.0 ? -32767 : fY > 32767.0 ? 32767 : fY);
}

inline WORD Clamp(f64 input)
{
	u32 result = (u32) input;
	result = result > 0xFFFF ? 0xFFFF : result;
	return (WORD)result;
}

void __fastcall DummyGetState(XINPUT_STATE* pState)
{
	pState->Gamepad.wButtons = 0;

	pState->Gamepad.bLeftTrigger = 0;
	pState->Gamepad.bRightTrigger = 0;

	pState->Gamepad.sThumbLX =0;
	pState->Gamepad.sThumbLY =0;
	pState->Gamepad.sThumbRX =0;
	pState->Gamepad.sThumbRY =0;
}

void __fastcall TransformGetState(SETTINGS &settings, XINPUT_STATE* pState)
{
	TransformAnalog(pState->Gamepad.sThumbLX, pState->Gamepad.sThumbLY, settings, true);
	TransformAnalog(pState->Gamepad.sThumbRX, pState->Gamepad.sThumbRY, settings, false);

	TriggerRange(pState->Gamepad.bLeftTrigger,  settings);
	TriggerRange(pState->Gamepad.bRightTrigger, settings);

	TransformRemap(settings.remap, pState);
}

void __fastcall TransformSetState(f64 rumble, XINPUT_VIBRATION* pVibration)
{
	pVibration->wLeftMotorSpeed = Clamp(pVibration->wLeftMotorSpeed * rumble);
	pVibration->wRightMotorSpeed = Clamp(pVibration->wRightMotorSpeed * rumble);
}

///////////////////////////////////////////////////////////////////////////////
// Remap stuff
//

s32 __fastcall RemapType(s32 value, u8 type)
{
	value = value < 0 ? -value : value;
	
	switch(type)
	{
	case RT_DIGITAL_DIGITAL:
		return value? 1 : 0;

	case RT_DIGITAL_TRIGGER:
		return value? 255 : 0;
		
	case RT_DIGITAL_ANALOG:
		return value? 32767 : 0;

	case RT_TRIGGER_DIGITAL:
		return value > 50 ? 1 : 0;

	case RT_TRIGGER_TRIGGER:
		return value;

	case RT_TRIGGER_ANALOG:
		return (int)(value * 128.5f);

	case RT_ANALOG_DIGITAL:
		return value > 16384 ? 1 : 0;

	case RT_ANALOG_TRIGGER:
		return (int)(value / 128.5f);

	case RT_ANALOG_ANALOG: 
		return value;
	}

	return 0;
}

s32 __fastcall RemapGetValue(REMAP * remap, XINPUT_GAMEPAD* gamepad)
{	
	u8 control = remap->control;
	u8 type = remap->type;

	switch(control)
	{
	case Gamepad::DPAD_UP: 
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP, type);

	case Gamepad::DPAD_DOWN:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN, type);

	case Gamepad::DPAD_LEFT:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT, type);

	case Gamepad::DPAD_RIGHT:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT, type);

	case Gamepad::START:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_START, type);

	case Gamepad::BACK:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_BACK, type);

	case Gamepad::LEFT_THUMB:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB, type);

	case Gamepad::RIGHT_THUMB:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB, type);

	case Gamepad::LEFT_SHOULDER:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER, type);

	case Gamepad::RIGHT_SHOULDER:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER, type);

	case Gamepad::A:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_A, type);

	case Gamepad::B:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_B, type);

	case Gamepad::X:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_X, type);

	case Gamepad::Y:
		return RemapType(gamepad->wButtons & XINPUT_GAMEPAD_Y, type);

	case Gamepad::LEFT_TRIGGER:
		return RemapType(gamepad->bLeftTrigger, type);

	case Gamepad::RIGHT_TRIGGER:
		return RemapType(gamepad->bRightTrigger, type);

	case Gamepad::THUMB_LX_P:
		return gamepad->sThumbLX > 0 ? RemapType(gamepad->sThumbLX, type) : 0;

	case Gamepad::THUMB_LX_M:
		return gamepad->sThumbLX < 0 ? RemapType(gamepad->sThumbLX, type) : 0;

	case Gamepad::THUMB_LY_P:
		return gamepad->sThumbLY > 0 ? RemapType(gamepad->sThumbLY, type) : 0;

	case Gamepad::THUMB_LY_M:
		return gamepad->sThumbLY < 0 ? RemapType(gamepad->sThumbLY, type) : 0;

	case Gamepad::THUMB_RX_P:
		return gamepad->sThumbRX > 0 ? RemapType(gamepad->sThumbRX, type) : 0;

	case Gamepad::THUMB_RX_M:
		return gamepad->sThumbRX < 0 ? RemapType(gamepad->sThumbRX, type) : 0;

	case Gamepad::THUMB_RY_P:
		return gamepad->sThumbRY > 0 ? RemapType(gamepad->sThumbRY, type) : 0;

	case Gamepad::THUMB_RY_M:
		return gamepad->sThumbRY < 0 ? RemapType(gamepad->sThumbRY, type) : 0;

	case Gamepad::DISABLED:
		return 0;
	}

	return 0;
}

u16 __fastcall RemapButtons(REMAP *remap, XINPUT_GAMEPAD* gamepad)
{
	u16 buttonsOut = gamepad->wButtons & 0x400;

	buttonsOut |= RemapGetValue(&remap[Gamepad::DPAD_UP], gamepad)    << 0;
	buttonsOut |= RemapGetValue(&remap[Gamepad::DPAD_DOWN], gamepad)  << 1;
	buttonsOut |= RemapGetValue(&remap[Gamepad::DPAD_LEFT], gamepad)  << 2;
	buttonsOut |= RemapGetValue(&remap[Gamepad::DPAD_RIGHT], gamepad) << 3;

	buttonsOut |= RemapGetValue(&remap[Gamepad::START], gamepad) << 4;
	buttonsOut |= RemapGetValue(&remap[Gamepad::BACK], gamepad)  << 5;

	buttonsOut |= RemapGetValue(&remap[Gamepad::LEFT_THUMB], gamepad)  << 6;
	buttonsOut |= RemapGetValue(&remap[Gamepad::RIGHT_THUMB], gamepad) << 7;

	buttonsOut |= RemapGetValue(&remap[Gamepad::LEFT_SHOULDER], gamepad)  << 8;
	buttonsOut |= RemapGetValue(&remap[Gamepad::RIGHT_SHOULDER], gamepad) << 9;

	buttonsOut |= RemapGetValue(&remap[Gamepad::A], gamepad) << 12;
	buttonsOut |= RemapGetValue(&remap[Gamepad::B], gamepad) << 13;
	buttonsOut |= RemapGetValue(&remap[Gamepad::X], gamepad) << 14;
	buttonsOut |= RemapGetValue(&remap[Gamepad::Y], gamepad) << 15;
	
	return buttonsOut;
}

void __fastcall TransformRemap(REMAP *remap, XINPUT_STATE* pState)
{
	GAMEPAD gamepad;

	gamepad.buttons = RemapButtons(remap, &pState->Gamepad);

	gamepad.triggerL = RemapGetValue(&remap[Gamepad::LEFT_TRIGGER], &pState->Gamepad);
	gamepad.triggerR = RemapGetValue(&remap[Gamepad::RIGHT_TRIGGER], &pState->Gamepad);

	gamepad.analogLX = RemapGetValue(&remap[Gamepad::THUMB_LX_P], &pState->Gamepad) - 
		               RemapGetValue(&remap[Gamepad::THUMB_LX_M], &pState->Gamepad);
	gamepad.analogLY = RemapGetValue(&remap[Gamepad::THUMB_LY_P], &pState->Gamepad) - 
		               RemapGetValue(&remap[Gamepad::THUMB_LY_M], &pState->Gamepad);
	gamepad.analogRX = RemapGetValue(&remap[Gamepad::THUMB_RX_P], &pState->Gamepad) - 
		               RemapGetValue(&remap[Gamepad::THUMB_RX_M], &pState->Gamepad);
	gamepad.analogRY = RemapGetValue(&remap[Gamepad::THUMB_RY_P], &pState->Gamepad) - 
		               RemapGetValue(&remap[Gamepad::THUMB_RY_M], &pState->Gamepad);

	pState->Gamepad.wButtons = gamepad.buttons;

	pState->Gamepad.bLeftTrigger = gamepad.triggerL;
	pState->Gamepad.bRightTrigger = gamepad.triggerR;

	pState->Gamepad.sThumbLX = gamepad.analogLX;
	pState->Gamepad.sThumbLY = gamepad.analogLY;
	pState->Gamepad.sThumbRX = gamepad.analogRX;
	pState->Gamepad.sThumbRY = gamepad.analogRY;
}

extern s32 __stdcall GetControllerInput(s32 port)
{
	for(int i = 0; i < 100; i++)
	{	
		XINPUT_STATE state;
		DWORD result = DuraznoGetState(port, &state);
	
		if(result != ERROR_SUCCESS) return -1;

		if(GetAsyncKeyState(VK_DELETE)) return Gamepad::DISABLED;
	
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)    return Gamepad::DPAD_UP;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)  return Gamepad::DPAD_DOWN;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)  return Gamepad::DPAD_LEFT;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) return Gamepad::DPAD_RIGHT;

		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_START) return Gamepad::START;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)  return Gamepad::BACK;

		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)  return Gamepad::LEFT_THUMB;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) return Gamepad::RIGHT_THUMB;

		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) return Gamepad::LEFT_SHOULDER;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) return Gamepad::RIGHT_SHOULDER;

		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_A) return Gamepad::A;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_B) return Gamepad::B;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_X) return Gamepad::X;
		if(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) return Gamepad::Y;

		s32 threshold = 100;

		if(state.Gamepad.bLeftTrigger  > threshold) return Gamepad::LEFT_TRIGGER;
		if(state.Gamepad.bRightTrigger > threshold) return Gamepad::RIGHT_TRIGGER;
	
		threshold = 16384;

		if(state.Gamepad.sThumbLX >  threshold) return Gamepad::THUMB_LX_P;
		if(state.Gamepad.sThumbLX < -threshold) return Gamepad::THUMB_LX_M;
		if(state.Gamepad.sThumbLY >  threshold) return Gamepad::THUMB_LY_P;
		if(state.Gamepad.sThumbLY < -threshold) return Gamepad::THUMB_LY_M;

		if(state.Gamepad.sThumbRX >  threshold) return Gamepad::THUMB_RX_P;
		if(state.Gamepad.sThumbRX < -threshold) return Gamepad::THUMB_RX_M;
		if(state.Gamepad.sThumbRY >  threshold) return Gamepad::THUMB_RY_P;
		if(state.Gamepad.sThumbRY < -threshold) return Gamepad::THUMB_RY_M;

		Sleep(100);
	}

	return -1;
}
