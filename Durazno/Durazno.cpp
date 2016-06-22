/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include "Durazno.h"
#include "Wrapper.h"
#include "Settings.h"
#include "Transform.h"
#include "FileIO.h"

#include <string>
//#include <mutex>

HINSTANCE hInstance;
XInputStruct XInput;

SETTINGS settings[4];
s32 INIversion = 3; // INI version stuff
std::wstring customDLL;

struct logger
{
	FILE *file;

	void printl(const char* func, const char* msg)
	{
		fprintf(file, "%s: %s\n", func, msg);
		fflush(file);
	}

	void printl(const wchar_t* func, const wchar_t* msg)
	{
		fwprintf(file, L"%s: %s\n", func, msg);
		fflush(file);
	}
	
	logger()
	{
		fopen_s(&file, ".\\Durazno.log", "w");
		fprintf_s(file, "= Durazno Log Opened =\n");
		fflush(file);
	}

	~logger()
	{
		fclose(file);
	}

} logfile;

extern "C" BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		logfile.printl(__FUNCTION__, "DLL_PROCESS_ATTACH");

		hInstance = hinstDLL;

		INI_LoadSettings(settings);
		INI_SaveSettings(settings);
	}

	return TRUE;
}

void XInputLoadLibrary()
{
	if (customDLL.empty())
	{
		WCHAR sysdir[MAX_PATH];
		WCHAR module[MAX_PATH];

		GetSystemDirectory(sysdir, MAX_PATH);
		GetModuleFileName(hInstance, module, MAX_PATH);

		customDLL = std::wstring(sysdir);

		if (GetLastError() == ERROR_SUCCESS)
		{
			std::wstring filename(module);
			filename = filename.substr(filename.find_last_of(L"\\/") + 1);
			customDLL.append(L"\\").append(filename);
		}
		else
			customDLL.append(L"\\").append(L"xinput1_3.dll");
	}

	XInput.dll = LoadLibraryW(customDLL.c_str());
	logfile.printl(__FUNCTIONW__, customDLL.c_str());

	if (XInput.dll == nullptr)
	{
		//MessageBoxW(NULL, customDLL.c_str(), L"XInput Load Failed!", MB_OK);
		logfile.printl(__FUNCTION__, "XInput Load Failed!");
	}
}

FARPROC XInputGetProc(LPCSTR name, const char* alt = nullptr)
{
	//static std::mutex load_lock;
	//load_lock.lock();

	if (XInput.dll == nullptr) XInputLoadLibrary();
	FARPROC func = GetProcAddress(XInput.dll, name);

	logfile.printl(__FUNCTION__, alt == nullptr ? name : alt);

	if (func == nullptr)
	{
		//MessageBoxA(NULL, name, "XInput Proc Failed!", MB_OK);
		logfile.printl(__FUNCTION__, "XInput Proc Failed!");
	}

	//load_lock.unlock();
	return func;
}

extern "C" DWORD WINAPI DuraznoGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if (XInput.GetState == nullptr)
		XInput.GetState = (t_XInputGetState)XInputGetProc("XInputGetState");

	return XInput.GetState(dwUserIndex, pState);
}

extern "C" DWORD WINAPI DuraznoGetStateEx(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if (XInput.GetStateEx == nullptr)
		XInput.GetStateEx = (t_XInputGetStateEx)XInputGetProc((LPCSTR)100, "XInputGetStateEx");

	return XInput.GetStateEx(dwUserIndex, pState);
}

extern "C" DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;

	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetState == nullptr)
		XInput.GetState = (t_XInputGetState)XInputGetProc("XInputGetState");

	DWORD ret = XInput.GetState(dwUserIndex, pState);

	if(ret == ERROR_SUCCESS)
	{
		if(set.isDummy)
			DummyGetState(pState);
		else
			TransformGetState(set, pState);
	}

	return ret;
}

extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
{
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
		if (XInput.SetState == nullptr)
			XInput.SetState = (t_XInputSetState)XInputGetProc("XInputSetState");

		TransformSetState(set.rumble, pVibration);
		ret = XInput.SetState(dwUserIndex, pVibration);
	}
	
	return ret;
}

extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetCapabilities == nullptr)
		XInput.GetCapabilities = (t_XInputGetCapabilities)XInputGetProc("XInputGetCapabilities");
	
	return XInput.GetCapabilities(dwUserIndex, dwFlags, pCapabilities);
}

extern "C" VOID WINAPI XInputEnable(BOOL enable)
{
	if (XInput.Enable == nullptr)
		XInput.Enable = (t_XInputEnable)XInputGetProc("XInputEnable");

	XInput.Enable(enable);
}

extern "C" DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetDSoundAudioDeviceGuids == nullptr)
		XInput.GetDSoundAudioDeviceGuids = (t_XInputGetDSoundAudioDeviceGuids)XInputGetProc("XInputGetDSoundAudioDeviceGuids");
	
	return XInput.GetDSoundAudioDeviceGuids(dwUserIndex, pDSoundRenderGuid, pDSoundCaptureGuid);
}

extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetBatteryInformation == nullptr)
		XInput.GetBatteryInformation = (t_XInputGetBatteryInformation)XInputGetProc("XInputGetBatteryInformation");
	
	return XInput.GetBatteryInformation(dwUserIndex, devType, pBatteryInformation);
}

extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetKeystroke == nullptr)
		XInput.GetKeystroke = (t_XInputGetKeystroke)XInputGetProc("XInputGetKeystroke");

	return XInput.GetKeystroke(dwUserIndex, dwReserved, pKeystroke);
}

extern "C" DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetAudioDeviceIds == nullptr)
		XInput.GetAudioDeviceIds = (t_XInputGetAudioDeviceIds)XInputGetProc("XInputGetAudioDeviceIds");

	return XInput.GetAudioDeviceIds(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId, pCaptureCount);
}

// UNDOCUMENTED

extern "C" DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE *pState)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetStateEx == nullptr)
		XInput.GetStateEx = (t_XInputGetStateEx)XInputGetProc((LPCSTR)100, __FUNCTION__);

	DWORD ret = XInput.GetStateEx(dwUserIndex, pState);

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
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.WaitForGuideButton == nullptr)
		XInput.WaitForGuideButton = (t_XInputWaitForGuideButton)XInputGetProc((LPCSTR)101, __FUNCTION__);
	
	return XInput.WaitForGuideButton(dwUserIndex, dwFlag, pVoid);
}

extern "C" DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.CancelGuideButtonWait == nullptr)
		XInput.CancelGuideButtonWait = (t_XInputCancelGuideButtonWait)XInputGetProc((LPCSTR)102, __FUNCTION__);
	
	return XInput.CancelGuideButtonWait(dwUserIndex);
}

extern "C" DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.PowerOffController == nullptr)
		XInput.PowerOffController = (t_XInputPowerOffController)XInputGetProc((LPCSTR)103, __FUNCTION__);
	
	return XInput.PowerOffController(dwUserIndex);
}

extern "C" DWORD WINAPI XInputGetBaseBusInformation(DWORD dwUserIndex, XINPUT_BUSINFO* pBusinfo)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetBaseBusInformation == nullptr)
		XInput.GetBaseBusInformation = (t_XInputGetBaseBusInformation)XInputGetProc((LPCSTR)104, __FUNCTION__);

	return XInput.GetBaseBusInformation(dwUserIndex, pBusinfo);
}

extern "C" DWORD WINAPI XInputGetCapabilitiesEx(DWORD dwUnk, DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIESEX* pCapabilitiesEx)
{
	SETTINGS &set = settings[dwUserIndex];
	if (set.isDisabled) return ERROR_DEVICE_NOT_CONNECTED;
	dwUserIndex = set.isDummy ? 0 : set.port;

	if (XInput.GetCapabilitiesEx == nullptr)
		XInput.GetCapabilitiesEx = (t_XInputGetCapabilitiesEx)XInputGetProc((LPCSTR)108, __FUNCTION__);

	return XInput.GetCapabilitiesEx(dwUnk, dwUserIndex, dwFlags, pCapabilitiesEx);
}