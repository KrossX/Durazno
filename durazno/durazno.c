/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "durazno.h"

static
struct settings settings[4];

static
char custom_dll[MAX_PATH];

#include "custom_func.c"
#include "transform.c"
#include "file_io.c"

static
struct xinput_dll XInput;

static
BOOL  initialized;

static
XINPUT_STATE dummy_state;

static
BOOL load_xinput_dll(HINSTANCE instance)
{
	char sysdir[MAX_PATH];
	UINT sysdir_len = GetSystemDirectoryA(sysdir, MAX_PATH);
	if (sysdir_len == 0 || sysdir_len >= MAX_PATH) return FALSE;

	char modname[MAX_PATH];
	DWORD modname_len = GetModuleFileNameA(instance, modname, MAX_PATH);
	if (!modname_len) return FALSE;

	DWORD namepos = cheap_find_last_of(modname, '\\');
	char *dllpath = lstrcatA(sysdir, &modname[namepos]);
	if (!dllpath) return FALSE;

	XInput.dll = LoadLibraryA(dllpath);
	if (!XInput.dll) return FALSE;

	XInput.GetState = (t_XInputGetState)GetProcAddress(XInput.dll, "XInputGetState");
	XInput.SetState = (t_XInputSetState)GetProcAddress(XInput.dll, "XInputSetState");
	XInput.GetCapabilities = (t_XInputGetCapabilities)GetProcAddress(XInput.dll, "XInputGetCapabilities");
	XInput.Enable = (t_XInputEnable)GetProcAddress(XInput.dll, "XInputEnable");
	XInput.GetDSoundAudioDeviceGuids = (t_XInputGetDSoundAudioDeviceGuids)GetProcAddress(XInput.dll, "XInputGetDSoundAudioDeviceGuids");
	XInput.GetBatteryInformation = (t_XInputGetBatteryInformation)GetProcAddress(XInput.dll, "XInputGetBatteryInformation");
	XInput.GetKeystroke = (t_XInputGetKeystroke)GetProcAddress(XInput.dll, "XInputGetKeystroke");
	XInput.GetAudioDeviceIds = (t_XInputGetAudioDeviceIds)GetProcAddress(XInput.dll, "XInputGetAudioDeviceIds");

	XInput.GetStateEx = (t_XInputGetStateEx)GetProcAddress(XInput.dll, (LPCSTR)100);
	XInput.WaitForGuideButton = (t_XInputWaitForGuideButton)GetProcAddress(XInput.dll, (LPCSTR)101);
	XInput.CancelGuideButtonWait = (t_XInputCancelGuideButtonWait)GetProcAddress(XInput.dll, (LPCSTR)102);
	XInput.PowerOffController = (t_XInputPowerOffController)GetProcAddress(XInput.dll, (LPCSTR)103);
	XInput.GetBaseBusInformation = (t_XInputGetBaseBusInformation)GetProcAddress(XInput.dll, (LPCSTR)104);
	XInput.GetCapabilitiesEx = (t_XInputGetCapabilitiesEx)GetProcAddress(XInput.dll, (LPCSTR)108);

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	// This shouldn't be done on DllMain
	if (!initialized)
	{
		initialized = TRUE;

		ini_load();
		ini_save();

		if (!load_xinput_dll(hinstDLL))
			return FALSE;
	}


	return TRUE;
}

DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	struct settings *set = &settings[dwUserIndex];

	if(set->disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	DWORD retval = XInput.GetState(set->index, pState);

	if (retval == ERROR_SUCCESS)
	{
		if (set->dummy)
		{
			*pState = dummy_state;
		}
		else
		{
			transform_get_state(set, pState);
		}
	}

	return retval;
}

DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	struct settings *set = &settings[dwUserIndex];

	if (set->disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	if (set->dummy)
	{
		return ERROR_SUCCESS;
	}
	else
	{
		transform_set_state(set, pVibration);
		return XInput.SetState(set->index, pVibration);
	}
}

DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetCapabilities(settings[dwUserIndex].index, dwFlags, pCapabilities);
}

VOID WINAPI XInputEnable(BOOL enable)
{
	XInput.Enable(enable);
}

DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetDSoundAudioDeviceGuids(settings[dwUserIndex].index, pDSoundRenderGuid, pDSoundCaptureGuid);
}

DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetBatteryInformation(settings[dwUserIndex].index, devType, pBatteryInformation);
}

DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetKeystroke(settings[dwUserIndex].index, dwReserved, pKeystroke);
}

DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetAudioDeviceIds(settings[dwUserIndex].index, pRenderDeviceId, pRenderCount, pCaptureDeviceId, pCaptureCount);
}

// UNDOCUMENTED

DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE *pState)
{
	struct settings *set = &settings[dwUserIndex];

	if (set->disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	DWORD retval = XInput.GetStateEx(set->index, pState);

	if (retval == ERROR_SUCCESS)
	{
		if (set->dummy)
		{
			*pState = dummy_state;
		}
		else
		{
			transform_get_state(set, pState);
		}
	}

	return retval;
}

DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.WaitForGuideButton(settings[dwUserIndex].index, dwFlag, pVoid);
}

DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.CancelGuideButtonWait(settings[dwUserIndex].index);
}

DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.PowerOffController(settings[dwUserIndex].index);
}

DWORD WINAPI XInputGetBaseBusInformation(DWORD dwUserIndex, XINPUT_BUSINFO* pBusinfo)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetBaseBusInformation(settings[dwUserIndex].index, pBusinfo);
}

DWORD WINAPI XInputGetCapabilitiesEx(DWORD dwUnk, DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIESEX* pCapabilitiesEx)
{
	if (settings[dwUserIndex].disabled)
		return ERROR_DEVICE_NOT_CONNECTED;

	return XInput.GetCapabilitiesEx(dwUnk, settings[dwUserIndex].index, dwFlags, pCapabilitiesEx);
}

// GUI helper functions

DWORD WINAPI DuraznoGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	return XInput.GetState(dwUserIndex, pState);
}

DWORD WINAPI DuraznoGetStateEx(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	return XInput.GetStateEx(dwUserIndex, pState);
}

DWORD WINAPI GetControllerInput(DWORD port)
{
	XINPUT_STATE state;

	for (int i = 0; i < 100; i++)
	{
		if (DuraznoGetState(port, &state) != ERROR_SUCCESS)
			return (DWORD)-1;

		if (GetAsyncKeyState(VK_DELETE)) return CTRL_DISABLED;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)    return CTRL_DPAD_UP;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)  return CTRL_DPAD_DOWN;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)  return CTRL_DPAD_LEFT;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) return CTRL_DPAD_RIGHT;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) return CTRL_START;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)  return CTRL_BACK;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)  return CTRL_LEFT_THUMB;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) return CTRL_RIGHT_THUMB;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)  return CTRL_LEFT_SHOULDER;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) return CTRL_RIGHT_SHOULDER;

		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) return CTRL_A;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) return CTRL_B;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) return CTRL_X;
		if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) return CTRL_Y;

		if (state.Gamepad.bLeftTrigger  > 100) return CTRL_LEFT_TRIGGER;
		if (state.Gamepad.bRightTrigger > 100) return CTRL_RIGHT_TRIGGER;

		if (state.Gamepad.sThumbLX >  16000) return CTRL_THUMB_LX_P;
		if (state.Gamepad.sThumbLX < -16000) return CTRL_THUMB_LX_N;
		if (state.Gamepad.sThumbLY >  16000) return CTRL_THUMB_LY_P;
		if (state.Gamepad.sThumbLY < -16000) return CTRL_THUMB_LY_N;

		if (state.Gamepad.sThumbRX >  16000) return CTRL_THUMB_RX_P;
		if (state.Gamepad.sThumbRX < -16000) return CTRL_THUMB_RX_N;
		if (state.Gamepad.sThumbRY >  16000) return CTRL_THUMB_RY_P;
		if (state.Gamepad.sThumbRY < -16000) return CTRL_THUMB_RY_N;

		Sleep(100);
	}

	return (DWORD)-1;
}


