/*  Durazno - XInput to XInput Wrapper
 *  - 2012 KrossX
 *	
 *	Content of this file is based on code from 
 *	x360ce http://code.google.com/p/x360ce/
 *
 *  Licenced under GNU Lesser GPL licence.
 *  http://www.gnu.org/licenses/lgpl.html
 */

#include "Durazno.h"
#include "Transform.h"
#include "FileIO.h"
#include "Settings.h"
#include <string>

CRITICAL_SECTION cs;
HINSTANCE g_hinstDLL = NULL;
HINSTANCE realXInput = NULL;

_Settings settings[4];
s32 INIversion = 2; // INI version stuff
std::wstring customDLL; // Custom DLL to load first

bool LoadCustomDLL()
{
	realXInput = LoadLibrary(customDLL.c_str());
	return realXInput != NULL;
}

void LoadSystemXInputDLL()
{
	if(LoadCustomDLL()) return;

	WCHAR sysdir[MAX_PATH];
	WCHAR buffer[MAX_PATH];
	WCHAR module[MAX_PATH];

	GetSystemDirectory(sysdir, MAX_PATH);
	GetModuleFileName(g_hinstDLL, module, MAX_PATH);

	if(GetLastError() == ERROR_SUCCESS)
	{
		std::wstring filename(module);
		filename = filename.substr(filename.find_last_of(L"\\/")+1);
		swprintf_s(buffer,L"%s\\%s",sysdir,filename);
	}
	else
		swprintf_s(buffer,L"%s\\%s",sysdir,L"xinput1_3.dll");

	realXInput = LoadLibrary(buffer);
}

extern "C" BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch( fdwReason )
	{
	case DLL_PROCESS_ATTACH:
		{
			InitializeCriticalSection(&cs);
			EnterCriticalSection(&cs);

			INI_LoadSettings();
			INI_SaveSettings();
			
			g_hinstDLL =  hinstDLL;
			LoadSystemXInputDLL();

			LeaveCriticalSection(&cs);
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			EnterCriticalSection(&cs);

			if(realXInput) FreeLibrary(realXInput);

			LeaveCriticalSection(&cs);
			DeleteCriticalSection(&cs);
		}
		break;
	}

	return TRUE;
}

extern "C" DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	
	u8 index = (u8)dwUserIndex;
	dwUserIndex = settings[index].isDummy ? 0 : settings[index].port;

	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetState_t)(DWORD dwUserIndex, XINPUT_STATE* pState);
	XInputGetState_t realXInputGetState = (XInputGetState_t) GetProcAddress(realXInput, "XInputGetState");

	DWORD ret = realXInputGetState(dwUserIndex,pState);

	if(ret == ERROR_SUCCESS)
	{
		if(settings[index].isDummy)
			DummyGetState(pState);
		else
			TransformGetState(dwUserIndex, pState);
	}

	return ret;
}

extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;

	u8 index = (u8)dwUserIndex;
	dwUserIndex = settings[index].isDummy ? 0 : settings[index].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputSetState_t)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
	XInputSetState_t realXInputSetState = (XInputSetState_t) GetProcAddress(realXInput, "XInputSetState");
	

	DWORD ret;

	if(settings[index].isDummy)
	{
		ret = ERROR_SUCCESS;
	}
	else
	{
		TransformSetState(dwUserIndex, pVibration);
		ret = realXInputSetState(dwUserIndex,pVibration);
	}
	
	return ret;
}

extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetCapabilities_t)(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities);
	XInputGetCapabilities_t realXInputGetCapabilities = (XInputGetCapabilities_t) GetProcAddress(realXInput, "XInputGetCapabilities");
	return realXInputGetCapabilities(dwUserIndex,dwFlags,pCapabilities);
}

extern "C" VOID WINAPI XInputEnable(BOOL enable)
{
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputEnable_t)(BOOL enable);
	XInputEnable_t realXInputEnable = (XInputEnable_t) GetProcAddress(realXInput, "XInputEnable");
	realXInputEnable(enable);
}

extern "C" DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetDSoundAudioDeviceGuids_t)(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
	XInputGetDSoundAudioDeviceGuids_t realXInputGetDSoundAudioDeviceGuids = (XInputGetDSoundAudioDeviceGuids_t) GetProcAddress(realXInput, "XInputGetDSoundAudioDeviceGuids");
	return realXInputGetDSoundAudioDeviceGuids(dwUserIndex,pDSoundRenderGuid,pDSoundCaptureGuid);
}

extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetBatteryInformation_t)(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
	XInputGetBatteryInformation_t realXInputGetBatteryInformation = (XInputGetBatteryInformation_t) GetProcAddress(realXInput, "XInputGetBatteryInformation");
	return realXInputGetBatteryInformation(dwUserIndex,devType,pBatteryInformation);
}

extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetKeystroke_t)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
	XInputGetKeystroke_t realXInputGetKeystroke = (XInputGetKeystroke_t) GetProcAddress(realXInput, "XInputGetKeystroke");
	return realXInputGetKeystroke(dwUserIndex,dwReserved,pKeystroke);
}

// UNDOCUMENTED

extern "C" DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetStateEx_t)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
	XInputGetStateEx_t realXInputGetStateEx = (XInputGetStateEx_t) GetProcAddress(realXInput, (LPCSTR) 100);
	return realXInputGetStateEx(dwUserIndex,dwReserved,pKeystroke);
}

extern "C" DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputWaitForGuideButton_t)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
	XInputWaitForGuideButton_t realXInputWaitForGuideButton = (XInputWaitForGuideButton_t) GetProcAddress(realXInput, (LPCSTR) 101);
	return realXInputWaitForGuideButton(dwUserIndex,dwReserved,pKeystroke);
}

extern "C" DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputCancelGuideButtonWait_t)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
	XInputCancelGuideButtonWait_t realXInputCancelGuideButtonWait = (XInputCancelGuideButtonWait_t) GetProcAddress(realXInput, (LPCSTR) 102);
	return realXInputCancelGuideButtonWait(dwUserIndex,dwReserved,pKeystroke);
}

extern "C" DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputPowerOffController_t)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
	XInputPowerOffController_t realXInputPowerOffController = (XInputPowerOffController_t) GetProcAddress(realXInput, (LPCSTR) 103);
	return realXInputPowerOffController(dwUserIndex,dwReserved,pKeystroke);
}