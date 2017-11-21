/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#define ANALOG_MAX 32767.0f

int _fltused; // GRRRRRRR!!!

static
float deadzone(float value, float deadzone, float k)
{
	if (value < -deadzone)
	{
		value = (value + deadzone) * k;
	}
	else if(value > deadzone)
	{
		value = (value - deadzone) * k;
	}
	else
	{
		value = 0;
	}

	return value;
}

static
float antideadzone(float value, float antideadzone, float k)
{
	float f = 1.0f / 65535.0f;

	if (value > f) // check for deadzoned value
	{
		value = value * k + antideadzone;
	}
	else if (value < -f)
	{
		value = value * k - antideadzone;
	}

	return value;
}

static
void transform_analog(struct stick_settings *set, SHORT *X, SHORT *Y)
{
	float x, y;
	
	if (*X == 0 && *Y == 0) return;
	
	x = (float)*X;
	y = (float)*Y;

	if (set->linearity != 0)
	{
		float length = Q_rsqrt(x*x + y*y);

		x *= length;
		y *= length;

		length = 1.0f / length / ANALOG_MAX;
		length = (float)fastPow(length, set->exp);

		x *= length * ANALOG_MAX;
		y *= length * ANALOG_MAX;
	}

	if (set->deadzone > 0)
	{
		if (set->deadzone_linear)
		{
			x = deadzone(x, set->deadzone_check, set->deadzone_k);
			y = deadzone(y, set->deadzone_check, set->deadzone_k);
		}
		else
		{
			float length = Q_rsqrt(x*x + y*y);

			x *= length;
			y *= length;

			length = deadzone(1.0f / length, set->deadzone_check, set->deadzone_k);

			x *= length;
			y *= length;
		}
	}

	if (set->antideadzone > 0)
	{
		if (set->antideadzone_linear)
		{
			x = antideadzone(x, set->antideadzone_check, set->antideadzone_k);
			y = antideadzone(y, set->antideadzone_check, set->antideadzone_k);
		}
		else
		{
			float length = Q_rsqrt(x*x + y*y);

			x *= length;
			y *= length;

			length = antideadzone(1.0f / length, set->antideadzone_check, set->antideadzone_k);

			x *= length;
			y *= length;
		}
	}

	*X = (SHORT)(x < -ANALOG_MAX ? -ANALOG_MAX : x > ANALOG_MAX ? ANALOG_MAX : x);
	*Y = (SHORT)(y < -ANALOG_MAX ? -ANALOG_MAX : y > ANALOG_MAX ? ANALOG_MAX : y);
}

static
void transform_trigger(struct trigger_settings *set, BYTE *trigger)
{
	if (set->range > 0 )
	{
		float tg  = *trigger;

		tg = tg < set->deadzone ? 0 : ((tg - set->deadzone) * 255.0f) / set->range;
		*trigger = tg > 255.0f ? 255 : (BYTE)tg;
	}
}

static
int remap_type(int value, int type)
{
	value = value < 0 ? -value : value;

	switch (type)
	{
	case REMAP_DIGITAL_DIGITAL:
		return value ? 1 : 0;

	case REMAP_DIGITAL_TRIGGER:
		return value ? 255 : 0;

	case REMAP_DIGITAL_ANALOG:
		return value ? 32767 : 0;

	case REMAP_TRIGGER_DIGITAL:
		return value > 50 ? 1 : 0;

	case REMAP_TRIGGER_TRIGGER:
		return value;

	case REMAP_TRIGGER_ANALOG:
		return (int)(value * 128.5f);

	case REMAP_ANALOG_DIGITAL:
		return value > 16384 ? 1 : 0;

	case REMAP_ANALOG_TRIGGER:
		return (int)(value / 128.5f);

	case REMAP_ANALOG_ANALOG:
		return value;
	}

	return 0;
}

static
int remap_get_value(struct remap *remap, XINPUT_GAMEPAD* gamepad)
{
	switch (remap->control)
	{
	case CTRL_DPAD_UP:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP, remap->type);

	case CTRL_DPAD_DOWN:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN, remap->type);

	case CTRL_DPAD_LEFT:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT, remap->type);

	case CTRL_DPAD_RIGHT:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT, remap->type);

	case CTRL_START:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_START, remap->type);

	case CTRL_BACK:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_BACK, remap->type);

	case CTRL_LEFT_THUMB:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_LEFT_THUMB, remap->type);

	case CTRL_RIGHT_THUMB:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_THUMB, remap->type);

	case CTRL_LEFT_SHOULDER:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER, remap->type);

	case CTRL_RIGHT_SHOULDER:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER, remap->type);

	case CTRL_A:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_A, remap->type);

	case CTRL_B:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_B, remap->type);

	case CTRL_X:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_X, remap->type);

	case CTRL_Y:
		return remap_type(gamepad->wButtons & XINPUT_GAMEPAD_Y, remap->type);

	case CTRL_LEFT_TRIGGER:
		return remap_type(gamepad->bLeftTrigger, remap->type);

	case CTRL_RIGHT_TRIGGER:
		return remap_type(gamepad->bRightTrigger, remap->type);

	case CTRL_THUMB_LX_P:
		return gamepad->sThumbLX > 0 ? remap_type(gamepad->sThumbLX, remap->type) : 0;

	case CTRL_THUMB_LX_N:
		return gamepad->sThumbLX < 0 ? remap_type(gamepad->sThumbLX, remap->type) : 0;

	case CTRL_THUMB_LY_P:
		return gamepad->sThumbLY > 0 ? remap_type(gamepad->sThumbLY, remap->type) : 0;

	case CTRL_THUMB_LY_N:
		return gamepad->sThumbLY < 0 ? remap_type(gamepad->sThumbLY, remap->type) : 0;

	case CTRL_THUMB_RX_P:
		return gamepad->sThumbRX > 0 ? remap_type(gamepad->sThumbRX, remap->type) : 0;

	case CTRL_THUMB_RX_N:
		return gamepad->sThumbRX < 0 ? remap_type(gamepad->sThumbRX, remap->type) : 0;

	case CTRL_THUMB_RY_P:
		return gamepad->sThumbRY > 0 ? remap_type(gamepad->sThumbRY, remap->type) : 0;

	case CTRL_THUMB_RY_N:
		return gamepad->sThumbRY < 0 ? remap_type(gamepad->sThumbRY, remap->type) : 0;

	case CTRL_DISABLED:
		return 0;
	}

	return 0;
}

static
WORD remap_buttons(struct remap *remap, XINPUT_GAMEPAD* gamepad)
{
	WORD buttonsOut = gamepad->wButtons & 0x400;

	buttonsOut |= remap_get_value(&remap[CTRL_DPAD_UP], gamepad)    << 0;
	buttonsOut |= remap_get_value(&remap[CTRL_DPAD_DOWN], gamepad)  << 1;
	buttonsOut |= remap_get_value(&remap[CTRL_DPAD_LEFT], gamepad)  << 2;
	buttonsOut |= remap_get_value(&remap[CTRL_DPAD_RIGHT], gamepad) << 3;

	buttonsOut |= remap_get_value(&remap[CTRL_START], gamepad) << 4;
	buttonsOut |= remap_get_value(&remap[CTRL_BACK], gamepad)  << 5;

	buttonsOut |= remap_get_value(&remap[CTRL_LEFT_THUMB], gamepad)  << 6;
	buttonsOut |= remap_get_value(&remap[CTRL_RIGHT_THUMB], gamepad) << 7;

	buttonsOut |= remap_get_value(&remap[CTRL_LEFT_SHOULDER], gamepad)  << 8;
	buttonsOut |= remap_get_value(&remap[CTRL_RIGHT_SHOULDER], gamepad) << 9;

	buttonsOut |= remap_get_value(&remap[CTRL_A], gamepad) << 12;
	buttonsOut |= remap_get_value(&remap[CTRL_B], gamepad) << 13;
	buttonsOut |= remap_get_value(&remap[CTRL_X], gamepad) << 14;
	buttonsOut |= remap_get_value(&remap[CTRL_Y], gamepad) << 15;

	return buttonsOut;
}

static
void transform_remap(struct remap *remap, XINPUT_STATE* pState)
{
	WORD  buttons  =  remap_buttons(remap, &pState->Gamepad);

	BYTE  triggerL = (BYTE)remap_get_value(&remap[CTRL_LEFT_TRIGGER], &pState->Gamepad);
	BYTE  triggerR = (BYTE)remap_get_value(&remap[CTRL_RIGHT_TRIGGER], &pState->Gamepad);

	SHORT analogLX = (SHORT)remap_get_value(&remap[CTRL_THUMB_LX_P], &pState->Gamepad)
		           - (SHORT)remap_get_value(&remap[CTRL_THUMB_LX_N], &pState->Gamepad);
	SHORT analogLY = (SHORT)remap_get_value(&remap[CTRL_THUMB_LY_P], &pState->Gamepad)
		           - (SHORT)remap_get_value(&remap[CTRL_THUMB_LY_N], &pState->Gamepad);
	SHORT analogRX = (SHORT)remap_get_value(&remap[CTRL_THUMB_RX_P], &pState->Gamepad)
		           - (SHORT)remap_get_value(&remap[CTRL_THUMB_RX_N], &pState->Gamepad);
	SHORT analogRY = (SHORT)remap_get_value(&remap[CTRL_THUMB_RY_P], &pState->Gamepad)
		           - (SHORT)remap_get_value(&remap[CTRL_THUMB_RY_N], &pState->Gamepad);

	pState->Gamepad.wButtons      = buttons;
	pState->Gamepad.bLeftTrigger  = triggerL;
	pState->Gamepad.bRightTrigger = triggerR;
	pState->Gamepad.sThumbLX      = analogLX;
	pState->Gamepad.sThumbLY      = analogLY;
	pState->Gamepad.sThumbRX      = analogRX;
	pState->Gamepad.sThumbRY      = analogRY;
}

static
void transform_get_state(struct settings *set, XINPUT_STATE *state)
{
	transform_analog(&set->stick_l, &state->Gamepad.sThumbLX, &state->Gamepad.sThumbLY);
	transform_analog(&set->stick_r, &state->Gamepad.sThumbRX, &state->Gamepad.sThumbRY);

	transform_trigger(&set->trigger_l, &state->Gamepad.bLeftTrigger);
	transform_trigger(&set->trigger_r, &state->Gamepad.bRightTrigger);

	transform_remap(set->remap, state);
}

static
void transform_set_state(struct settings *set, XINPUT_VIBRATION *vib)
{
	float motor_l = vib->wLeftMotorSpeed  * set->rumble_str;
	float motor_r = vib->wRightMotorSpeed * set->rumble_str;

	vib->wLeftMotorSpeed  = motor_l > 65535.0f ? 0xFFFF : (WORD)motor_l;
	vib->wRightMotorSpeed = motor_r > 65535.0f ? 0xFFFF : (WORD)motor_r;
}
