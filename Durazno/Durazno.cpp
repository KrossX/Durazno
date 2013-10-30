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
#include "Wrapper.h"
#include "Transform.h"
#include "FileIO.h"
#include "Settings.h"
#include <string>

CRITICAL_SECTION cs;
HINSTANCE g_hinstDLL = NULL;
HINSTANCE realXInput = NULL;

FARPROC XInput[XInputTotal];

_Settings settings[4];
s32 INIversion = 2; // INI version stuff
std::wstring customDLL; // Custom DLL to load first

static bool LoadCustomDLL()
{
	realXInput = LoadLibrary(customDLL.c_str());
	return realXInput != NULL;
}

static BOOL LoadSystemXInputDLL()
{
	if(!LoadCustomDLL())
	{
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

	if(!realXInput) return FALSE;

	XInput[GetState]				= GetProcAddress(realXInput, "XInputGetState");
	XInput[SetState]				= GetProcAddress(realXInput, "XInputSetState");
	XInput[GetCapabilities]			= GetProcAddress(realXInput, "XInputGetCapabilities");
	XInput[Enable]					= GetProcAddress(realXInput, "XInputEnable");
	XInput[GetDSoundAudioDeviceGuids] = GetProcAddress(realXInput, "XInputGetDSoundAudioDeviceGuids");
	XInput[GetBatteryInformation]	= GetProcAddress(realXInput, "XInputGetBatteryInformation");
	XInput[GetKeystroke]			= GetProcAddress(realXInput, "XInputGetKeystroke");

	XInput[GetStateEx]				= GetProcAddress(realXInput, (LPCSTR) 100);
	XInput[WaitForGuideButton]		= GetProcAddress(realXInput, (LPCSTR) 101);
	XInput[CancelGuideButtonWait]	= GetProcAddress(realXInput, (LPCSTR) 102);
	XInput[PowerOffController]		= GetProcAddress(realXInput, (LPCSTR) 103);

	//for(int i = 0; i < XInputTotal; i++)
	//	if(!XInput[i]) return FALSE;

	return TRUE;
}

extern "C" BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
	UNREFERENCED_PARAMETER(lpReserved);
	BOOL result = TRUE;

	switch( fdwReason )
	{
	case DLL_PROCESS_ATTACH:
		{
			InitializeCriticalSection(&cs);
			EnterCriticalSection(&cs);

			INI_LoadSettings();
			INI_SaveSettings();
			
			g_hinstDLL =  hinstDLL;

			if(!realXInput)
				result = LoadSystemXInputDLL();

			if(!result)
				MessageBeep(MB_ICONERROR);

			LeaveCriticalSection(&cs);
		}
		break;

	case DLL_PROCESS_DETACH:
		{
			EnterCriticalSection(&cs);

			if(realXInput)
			{
				FreeLibrary(realXInput);
				realXInput = NULL;
			}

			LeaveCriticalSection(&cs);
			DeleteCriticalSection(&cs);
		}
		break;
	}

	return result;
}

extern "C" DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	
	u8 index = (u8)dwUserIndex;
	dwUserIndex = settings[index].isDummy ? 0 : settings[index].port;

	DWORD ret = ((t_XInputGetState)XInput[GetState])(dwUserIndex, pState);

	if(ret == ERROR_SUCCESS)
	{
		if(settings[index].isDummy)
			DummyGetState(pState);
		else
			TransformGetState(dwUserIndex, pState);
	}

	return ret;
}

DWORD DuraznoGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	return ((t_XInputGetState)XInput[GetState])(dwUserIndex, pState);
}

extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;

	u8 index = (u8)dwUserIndex;
	dwUserIndex = settings[index].isDummy ? 0 : settings[index].port;
	
	DWORD ret;

	if(settings[index].isDummy)
	{
		ret = ERROR_SUCCESS;
	}
	else
	{
		TransformSetState(dwUserIndex, pVibration);
		ret = ((t_XInputSetState)XInput[SetState])(dwUserIndex, pVibration);
	}
	
	return ret;
}

extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputGetCapabilities)XInput[GetCapabilities])(dwUserIndex, dwFlags, pCapabilities);
}

extern "C" VOID WINAPI XInputEnable(BOOL enable)
{
	((t_XInputEnable)XInput[Enable])(enable);
}

extern "C" DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputGetDSoundAudioDeviceGuids)XInput[GetDSoundAudioDeviceGuids])(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputGetBatteryInformation)XInput[GetBatteryInformation])(dwUserIndex, devType, pBatteryInformation);
}

extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputGetKeystroke)XInput[GetKeystroke])(dwUserIndex, dwReserved, pKeystroke);
}

// UNDOCUMENTED

extern "C" DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE *pState)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;

	u8 index = (u8)dwUserIndex;
	dwUserIndex = settings[index].isDummy ? 0 : settings[index].port;

	DWORD ret = ((t_XInputGetStateEx)XInput[GetStateEx])(dwUserIndex, pState);

	if(ret == ERROR_SUCCESS)
	{
		if(settings[index].isDummy)
			DummyGetState(pState);
		else
			TransformGetState(dwUserIndex, pState);
	}

	return ret;
}

extern "C" DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputWaitForGuideButton)XInput[WaitForGuideButton])(dwUserIndex, dwFlag, pVoid);
}

extern "C" DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputCancelGuideButtonWait)XInput[CancelGuideButtonWait])(dwUserIndex);
}

extern "C" DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex)
{
	if(settings[dwUserIndex].isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = settings[dwUserIndex].isDummy ? 0 : settings[dwUserIndex].port;
	
	return ((t_XInputPowerOffController)XInput[PowerOffController])(dwUserIndex);
}