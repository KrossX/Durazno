/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

#pragma warning(push, 0)
#define WINVER         0x0400
#define _WIN32_WINNT   0x0400
#define _WIN32_WINDOWS 0x0400
#define _WIN32_IE      0x0400

#include <windows.h>
#include "xinput.h"
#pragma warning(pop)

struct UNKNOWN;
typedef struct UNKNOWN XINPUT_BUSINFO;
typedef struct UNKNOWN XINPUT_CAPABILITIESEX;
typedef struct UNKNOWN XINPUT_BATTERY_INFORMATION;
typedef struct UNKNOWN XINPUT_KEYSTROKE;

typedef DWORD(WINAPI* t_XInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);
typedef DWORD(WINAPI* t_XInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
typedef DWORD(WINAPI* t_XInputGetCapabilities)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
typedef DWORD(WINAPI* t_XInputEnable)(BOOL enable);
typedef DWORD(WINAPI* t_XInputGetDSoundAudioDeviceGuids)(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
typedef DWORD(WINAPI* t_XInputGetBatteryInformation)(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
typedef DWORD(WINAPI* t_XInputGetKeystroke)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
typedef DWORD(WINAPI* t_XInputGetAudioDeviceIds)(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount);

typedef DWORD(WINAPI* t_XInputGetStateEx)(DWORD dwUserIndex, XINPUT_STATE *pState);
typedef DWORD(WINAPI* t_XInputWaitForGuideButton)(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid);
typedef DWORD(WINAPI* t_XInputCancelGuideButtonWait)(DWORD dwUserIndex);
typedef DWORD(WINAPI* t_XInputPowerOffController)(DWORD dwUserIndex);
typedef DWORD(WINAPI* t_XInputGetBaseBusInformation)(DWORD dwUserIndex, XINPUT_BUSINFO* pBusinfo);
typedef DWORD(WINAPI* t_XInputGetCapabilitiesEx)(DWORD dwUnk, DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIESEX* pCapabilitiesEx);

struct xinput_dll
{
	HINSTANCE dll;

	t_XInputGetState                  GetState;
	t_XInputSetState                  SetState;
	t_XInputGetCapabilities           GetCapabilities;
	t_XInputEnable                    Enable;
	t_XInputGetDSoundAudioDeviceGuids GetDSoundAudioDeviceGuids;
	t_XInputGetBatteryInformation     GetBatteryInformation;
	t_XInputGetKeystroke              GetKeystroke;
	t_XInputGetAudioDeviceIds         GetAudioDeviceIds;

	t_XInputGetStateEx            GetStateEx;
	t_XInputWaitForGuideButton    WaitForGuideButton;
	t_XInputCancelGuideButtonWait CancelGuideButtonWait;
	t_XInputPowerOffController    PowerOffController;
	t_XInputGetBaseBusInformation GetBaseBusInformation;
	t_XInputGetCapabilitiesEx     GetCapabilitiesEx;
};

enum
{
	CTRL_DPAD_UP = 0,
	CTRL_DPAD_DOWN,
	CTRL_DPAD_LEFT,
	CTRL_DPAD_RIGHT,
	CTRL_START,
	CTRL_BACK,
	CTRL_LEFT_THUMB,
	CTRL_RIGHT_THUMB,
	CTRL_LEFT_SHOULDER,
	CTRL_RIGHT_SHOULDER,
	CTRL_A,
	CTRL_B,
	CTRL_X,
	CTRL_Y,
	CTRL_LEFT_TRIGGER,
	CTRL_RIGHT_TRIGGER,
	CTRL_THUMB_LX_P,
	CTRL_THUMB_LX_N,
	CTRL_THUMB_LY_P,
	CTRL_THUMB_LY_N,
	CTRL_THUMB_RX_P,
	CTRL_THUMB_RX_N,
	CTRL_THUMB_RY_P,
	CTRL_THUMB_RY_N,
	CTRL_DISABLED
};

enum 
{
	REMAP_DIGITAL_DIGITAL,
	REMAP_DIGITAL_TRIGGER,
	REMAP_DIGITAL_ANALOG,

	REMAP_TRIGGER_DIGITAL,
	REMAP_TRIGGER_TRIGGER,
	REMAP_TRIGGER_ANALOG,

	REMAP_ANALOG_DIGITAL,
	REMAP_ANALOG_TRIGGER,
	REMAP_ANALOG_ANALOG
};

struct stick_settings
{
	float linearity;
	float deadzone;
	float antideadzone;

	float exp;
	float deadzone_k;
	float antideadzone_k;
	float deadzone_check;
	float antideadzone_check;
	
	BOOL deadzone_linear;
	BOOL antideadzone_linear;
	BOOL inverted_x;
	BOOL inverted_y;
};

struct trigger_settings
{
	int min;
	int max;

	float deadzone;
	float range;
};

struct remap
{
	int control;
	int type;
};

struct settings
{
	struct remap remap[24];

	struct stick_settings stick_l, stick_r;
	struct trigger_settings trigger_l, trigger_r;

	BOOL disabled;
	BOOL dummy;

	DWORD index;
	float rumble_str;
};
