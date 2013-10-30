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

void LoadSystemXInputDLL()
{
	WCHAR sysdir[MAX_PATH] = {0};
	WCHAR buffer[MAX_PATH] = {0};
	WCHAR module[MAX_PATH] = {0};

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

			g_hinstDLL =  hinstDLL;
			LoadSystemXInputDLL();

			INI_LoadSettings();
			INI_SaveSettings();

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

	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetState_t)(DWORD dwUserIndex, XINPUT_STATE* pState);
	XInputGetState_t realXInputGetState = (XInputGetState_t) GetProcAddress(realXInput, "XInputGetState");

	DWORD ret = realXInputGetState(dwUserIndex,pState);
	if(ret == ERROR_SUCCESS) TransformGetState(dwUserIndex, pState);
	return ret;
}

extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputSetState_t)(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);
	XInputSetState_t realXInputSetState = (XInputSetState_t) GetProcAddress(realXInput, "XInputSetState");
	
	DWORD ret = realXInputSetState(dwUserIndex,pVibration);
	if(ret == ERROR_SUCCESS) TransformSetState(dwUserIndex, pVibration);
	return ret;
}

extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	
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
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetDSoundAudioDeviceGuids_t)(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid);
	XInputGetDSoundAudioDeviceGuids_t realXInputGetDSoundAudioDeviceGuids = (XInputGetDSoundAudioDeviceGuids_t) GetProcAddress(realXInput, "XInputGetDSoundAudioDeviceGuids");
	return realXInputGetDSoundAudioDeviceGuids(dwUserIndex,pDSoundRenderGuid,pDSoundCaptureGuid);
}

extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetBatteryInformation_t)(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
	XInputGetBatteryInformation_t realXInputGetBatteryInformation = (XInputGetBatteryInformation_t) GetProcAddress(realXInput, "XInputGetBatteryInformation");
	return realXInputGetBatteryInformation(dwUserIndex,devType,pBatteryInformation);
}

extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	
	if(!realXInput) LoadSystemXInputDLL();
	typedef DWORD (WINAPI* XInputGetKeystroke_t)(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke);
	XInputGetKeystroke_t realXInputGetKeystroke = (XInputGetKeystroke_t) GetProcAddress(realXInput, "XInputGetKeystroke");
	return realXInputGetKeystroke(dwUserIndex,dwReserved,pKeystroke);
}

