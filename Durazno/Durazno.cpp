/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "Durazno.h"
#include "Wrapper.h"
#include "Settings.h"
#include "Transform.h"
#include "FileIO.h"
#include <string>

CRITICAL_SECTION cs;
HINSTANCE g_hinstDLL = NULL;
HINSTANCE realXInput = NULL;

FARPROC XInput[XInputTotal];

SETTINGS settings[4];
s32 INIversion = 3; // INI version stuff
std::wstring customDLL; // Custom DLL to load first

bool g_initialized = false;

static bool LoadCustomDLL()
{
	realXInput = LoadLibrary(customDLL.c_str());
	return realXInput != NULL;
}

static void durazno_shutdown()
{
	EnterCriticalSection(&cs);

	if (realXInput)
	{
		FreeLibrary(realXInput);
		realXInput = NULL;
	}

	LeaveCriticalSection(&cs);
	DeleteCriticalSection(&cs);
}

static void durazno_init()
{
	if (g_initialized) return;
	else g_initialized = true;

	INI_LoadSettings(settings);
	INI_SaveSettings(settings);

	if(!LoadCustomDLL())
	{
		WCHAR sysdir[MAX_PATH];
		WCHAR module[MAX_PATH];

		GetSystemDirectory(sysdir, MAX_PATH);
		GetModuleFileName(g_hinstDLL, module, MAX_PATH);

		std::wstring fullpath(sysdir);

		if(GetLastError() == ERROR_SUCCESS)
		{
			std::wstring filename(module);
			filename = filename.substr(filename.find_last_of(L"\\/")+1);
			fullpath.append(L"\\").append(filename);
		}
		else
			fullpath.append(L"\\").append(L"xinput1_3.dll");

		realXInput = LoadLibrary(fullpath.c_str());
	}

	if (realXInput)
	{
		XInput[GetState] = GetProcAddress(realXInput, "XInputGetState");
		XInput[SetState] = GetProcAddress(realXInput, "XInputSetState");
		XInput[GetCapabilities] = GetProcAddress(realXInput, "XInputGetCapabilities");
		XInput[Enable] = GetProcAddress(realXInput, "XInputEnable");
		XInput[GetDSoundAudioDeviceGuids] = GetProcAddress(realXInput, "XInputGetDSoundAudioDeviceGuids");
		XInput[GetBatteryInformation] = GetProcAddress(realXInput, "XInputGetBatteryInformation");
		XInput[GetKeystroke] = GetProcAddress(realXInput, "XInputGetKeystroke");

		XInput[GetStateEx] = GetProcAddress(realXInput, (LPCSTR)100);
		XInput[WaitForGuideButton] = GetProcAddress(realXInput, (LPCSTR)101);
		XInput[CancelGuideButtonWait] = GetProcAddress(realXInput, (LPCSTR)102);
		XInput[PowerOffController] = GetProcAddress(realXInput, (LPCSTR)103);
	}
	else
	{
		MessageBoxA(NULL, "Could not load XInput DLL", "Durazno Error!", MB_OK | MB_ICONERROR);

		settings[0].isDisabled = true;
		settings[1].isDisabled = true;
		settings[2].isDisabled = true;
		settings[3].isDisabled = true;
	}

	//for(int i = 0; i < XInputTotal; i++)
	//	if(!XInput[i]) return FALSE;

	std::atexit(durazno_shutdown);
}

extern "C" BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		InitializeCriticalSection(&cs);
		g_hinstDLL =  hinstDLL;
	}

	return TRUE;
}

inline void check_durazno_init()
{
	if (!g_initialized)
	{
		EnterCriticalSection(&cs);
		durazno_init();
		LeaveCriticalSection(&cs);
	}
}

extern "C" DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;

	dwUserIndex = set.isDummy ? 0 : set.port;

	DWORD ret = ((t_XInputGetState)XInput[GetState])(dwUserIndex, pState);

	if(ret == ERROR_SUCCESS)
	{
		if(set.isDummy)
			DummyGetState(pState);
		else
			TransformGetState(set, pState);
	}

	return ret;
}

DWORD DuraznoGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	check_durazno_init();
	
	if (!realXInput) return ERROR_DEVICE_NOT_CONNECTED;
	else return ((t_XInputGetState)XInput[GetState])(dwUserIndex, pState);
}

DWORD DuraznoGetStateEx(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	check_durazno_init();

	if (!realXInput) return ERROR_DEVICE_NOT_CONNECTED;
	else return ((t_XInputGetStateEx)XInput[GetStateEx])(dwUserIndex, pState);
}

extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	DWORD ret;

	if(set.isDummy)
	{
		ret = ERROR_SUCCESS;
	}
	else
	{
		TransformSetState(set.rumble, pVibration);
		ret = ((t_XInputSetState)XInput[SetState])(dwUserIndex, pVibration);
	}
	
	return ret;
}

extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputGetCapabilities)XInput[GetCapabilities])(dwUserIndex, dwFlags, pCapabilities);
}

extern "C" VOID WINAPI XInputEnable(BOOL enable)
{
	check_durazno_init();

	if (realXInput)
		((t_XInputEnable)XInput[Enable])(enable);
}

extern "C" DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputGetDSoundAudioDeviceGuids)XInput[GetDSoundAudioDeviceGuids])(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputGetBatteryInformation)XInput[GetBatteryInformation])(dwUserIndex, devType, pBatteryInformation);
}

extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputGetKeystroke)XInput[GetKeystroke])(dwUserIndex, dwReserved, pKeystroke);
}

// UNDOCUMENTED

extern "C" DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE *pState)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	DWORD ret = ((t_XInputGetStateEx)XInput[GetStateEx])(dwUserIndex, pState);

	if(ret == ERROR_SUCCESS)
	{
		if(set.isDummy)
			DummyGetState(pState);
		else
			TransformGetState(set, pState);
	}

	return ret;
}

extern "C" DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputWaitForGuideButton)XInput[WaitForGuideButton])(dwUserIndex, dwFlag, pVoid);
}

extern "C" DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputCancelGuideButtonWait)XInput[CancelGuideButtonWait])(dwUserIndex);
}

extern "C" DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex)
{
	check_durazno_init();

	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;
	
	return ((t_XInputPowerOffController)XInput[PowerOffController])(dwUserIndex);
}