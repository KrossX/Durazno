/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

enum XINPUT_FUNCTIONS
{
	GetState,
	SetState,
	GetCapabilities,
	Enable, 
	GetDSoundAudioDeviceGuids,
	GetBatteryInformation,
	GetKeystroke,

	GetStateEx,
	WaitForGuideButton,
	CancelGuideButtonWait,
	PowerOffController,

	XInputTotal
};

// I don't wanna install DirectX SDK...
#ifndef XINPUT_BATTERY_INFORMATION
#define XINPUT_BATTERY_INFORMATION void
#endif

#ifndef XINPUT_KEYSTROKE
#define XINPUT_KEYSTROKE void
#endif

typedef DWORD (WINAPI* t_XInputGetState)(DWORD dwUserIndex, XINPUT_STATE* pState);
typedef DWORD (WINAPI* t_XInputSetState)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
typedef DWORD (WINAPI* t_XInputGetCapabilities)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
typedef DWORD (WINAPI* t_XInputEnable)(BOOL enable);
typedef DWORD (WINAPI* t_XInputGetDSoundAudioDeviceGuids)(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
typedef DWORD (WINAPI* t_XInputGetBatteryInformation)(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
typedef DWORD (WINAPI* t_XInputGetKeystroke)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);

typedef DWORD (WINAPI* t_XInputGetStateEx)(DWORD dwUserIndex, XINPUT_STATE *pState);
typedef DWORD (WINAPI* t_XInputWaitForGuideButton)(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid);
typedef DWORD (WINAPI* t_XInputCancelGuideButtonWait)(DWORD dwUserIndex);
typedef DWORD (WINAPI* t_XInputPowerOffController)(DWORD dwUserIndex);
