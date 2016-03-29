/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#include <Windows.h>
#include <stdio.h>

#include "TypeDefs.h"
#include "Settings.h"
#include "FileIO.h"

#include <string>

extern HINSTANCE g_hinstDLL;
extern s32 INIversion;
extern std::wstring customDLL;

const f64 FACTOR = 1000.0;

bool SaveEntry(wchar_t * section, s32 sectionNumber, wchar_t * key, s32 value, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);

	wchar_t valuestring[512] = {0};
	swprintf(valuestring, 512, L"%d", value);

	return WritePrivateProfileString(controller,  key, valuestring, filename) ? true : false;
}

bool SaveString(wchar_t * section, s32 sectionNumber, wchar_t * key, std::wstring value, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);

	return WritePrivateProfileString(controller,  key, value.c_str(), filename) ? true : false;
}

bool SaveRemap(u8 port, wchar_t * filename, SETTINGS settings[4])
{	
	wchar_t controller[512] = {0};
	wchar_t valuestring[512] = {0};

	swprintf(controller, 512, L"Controller%d", port);
		
	for (int i = 0; i < 24; i++)
	{
		wchar_t temp[512];
		memcpy_s(temp, 512 * sizeof(wchar_t), valuestring, 512 * sizeof(wchar_t));

		swprintf(valuestring, 512, i == 0 ? L"%s%02d" : L"%s %02d", temp, settings[port].remap[i].control);
	}

	return WritePrivateProfileString(controller,  L"Remap", valuestring, filename) ? true : false;
}

void ReadRemap(u8 port, wchar_t * filename, SETTINGS settings[4])
{	
	wchar_t controller[512] = {0};
	swprintf(controller, 512, L"Controller%d", port);
	
	wchar_t returnvalue[512] = {0};
	s32 nSize = GetPrivateProfileString(controller, L"Remap", L"-1", returnvalue, 512, filename);
	std::wstring value(returnvalue);
	
	if(nSize < 70) return;
	
	for (int i = 0; i < 24; i++)
	{
		std::wstring val = value.substr(i*3, 2);
		u8 control = settings[port].remap[i].control = _wtoi(val.c_str());

		if(control < 14) 
		{
			if(i < 14) settings[port].remap[i].type = RT_DIGITAL_DIGITAL;
			else if(i < 16) settings[port].remap[i].type = RT_DIGITAL_TRIGGER;
			else settings[port].remap[i].type = RT_DIGITAL_ANALOG;
		}
		else if(control < 16)
		{
			if(i < 14) settings[port].remap[i].type = RT_TRIGGER_DIGITAL;
			else if(i < 16) settings[port].remap[i].type = RT_TRIGGER_TRIGGER;
			else settings[port].remap[i].type = RT_TRIGGER_ANALOG;
		}
		else
		{
			if(i < 14) settings[port].remap[i].type = RT_ANALOG_DIGITAL;
			else if(i < 16) settings[port].remap[i].type = RT_ANALOG_TRIGGER;
			else settings[port].remap[i].type = RT_ANALOG_ANALOG;
		}
	}
}

s32 ReadEntry(wchar_t * section, s32 sectionNumber, wchar_t * key, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);
	
	s32 returnInteger = -1;
	wchar_t returnvalue[512] = {0};
	s32 nSize = GetPrivateProfileString(controller, key, L"-1", returnvalue, 512, filename);

	 if(nSize < 256) returnInteger = _wtoi(returnvalue);	 

	return returnInteger;
}

std::wstring ReadString(wchar_t * section, s32 sectionNumber, wchar_t * key, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);
	
	wchar_t returnvalue[512] = {0};
	s32 nSize = GetPrivateProfileString(controller, key, L"", returnvalue, 512, filename);

	std::wstring returnString(returnvalue);
	return returnString;
}

void INI_SaveSettings(SETTINGS *settings)
{
	wchar_t filename[] = L".\\Durazno.ini";

	SaveEntry(L"General", -1, L"INIversion", INIversion, filename);
	SaveString(L"General", -1, L"LoadDLL", customDLL, filename);

	for(s32 port = 0; port < 4; port++)
	{
		SETTINGS &set = settings[port];

		SaveEntry(L"Controller", port, L"Port", (s32)(set.port), filename);
		SaveEntry(L"Controller", port, L"Dummy", set.isDummy ? 1 : 0, filename);
		SaveEntry(L"Controller", port, L"Disable", set.isDisabled ? 1 : 0, filename);
		SaveEntry(L"Controller", port, L"LinearDZ", set.linearDZ ? 1 : 0, filename);
		SaveEntry(L"Controller", port, L"LinearADZ", set.linearADZ ? 1 : 0, filename);

		SaveEntry(L"Controller", port, L"AxisInvertedLX", set.stickL.invertedX ? 1 : 0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedLY", set.stickL.invertedY ? 1 : 0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedRX", set.stickR.invertedX ? 1 : 0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedRY", set.stickR.invertedY ? 1 : 0, filename);

		SaveEntry(L"Controller", port, L"TriggerMin", set.triggerMin, filename);
		SaveEntry(L"Controller", port, L"TriggerMax", set.triggerMax, filename);

		SaveEntry(L"Controller", port, L"Rumble", (s32)(set.rumble * FACTOR), filename);

		SaveEntry(L"Controller", port, L"L_Linearity", (s32)(set.stickL.linearity * 100.0) + 300, filename);
		SaveEntry(L"Controller", port, L"L_Deadzone", (s32)(set.stickL.deadzone * FACTOR), filename);
		SaveEntry(L"Controller", port, L"L_AntiDeadzone", (s32)(set.stickL.antiDeadzone * FACTOR), filename);

		SaveEntry(L"Controller", port, L"R_Linearity", (s32)(set.stickR.linearity * 100.0) + 300, filename);
		SaveEntry(L"Controller", port, L"R_Deadzone", (s32)(set.stickR.deadzone * FACTOR), filename);
		SaveEntry(L"Controller", port, L"R_AntiDeadzone", (s32)(set.stickR.antiDeadzone * FACTOR), filename);

		SaveRemap(port, filename, settings);
	}
}

void INI_LoadSettings(SETTINGS *settings)
{
	wchar_t filename[] = L".\\Durazno.ini";

	settings[0].port = 0;
	settings[1].port = 1;
	settings[2].port = 2;
	settings[3].port = 3;

	if(ReadEntry(L"General", -1, L"INIversion", filename) != INIversion) return;
	customDLL = ReadString(L"General", -1, L"LoadDLL", filename);
	
	for(s32 port = 0; port < 4; port++)
	{
		SETTINGS &set = settings[port];
		s32 result;
		
		result = ReadEntry(L"Controller", port, L"Port", filename);
		if (result != -1) set.port = result % 4;

		set.isDisabled = ReadEntry(L"Controller", port, L"Disable", filename) == 1 ? true : false;
		set.isDummy = ReadEntry(L"Controller", port, L"Dummy", filename) == 1 ? true : false;
		set.linearDZ = ReadEntry(L"Controller", port, L"LinearDZ", filename) == 1 ? true : false;
		set.linearADZ = ReadEntry(L"Controller", port, L"LinearADZ", filename) == 1 ? true : false;

		set.stickL.invertedX = ReadEntry(L"Controller", port, L"AxisInvertedLX", filename) == 1 ? true : false;
		set.stickL.invertedY = ReadEntry(L"Controller", port, L"AxisInvertedLY", filename) == 1 ? true : false;
		set.stickR.invertedX = ReadEntry(L"Controller", port, L"AxisInvertedRX", filename) == 1 ? true : false;
		set.stickR.invertedY = ReadEntry(L"Controller", port, L"AxisInvertedRY", filename) == 1 ? true : false;

		result = ReadEntry(L"Controller", port, L"TriggerMin", filename);
		if (result != -1) set.triggerMin = result & 0xFF;

		result = ReadEntry(L"Controller", port, L"TriggerMax", filename);
		if (result != -1) set.triggerMax = result & 0xFF;

		result = ReadEntry(L"Controller", port, L"Rumble", filename);
		if (result != -1) set.rumble = result / FACTOR;

		result = ReadEntry(L"Controller", port, L"L_Linearity", filename);
		if (result != -1) set.stickL.linearity = (result - 300.0) / 100.0;

		result = ReadEntry(L"Controller", port, L"L_Deadzone", filename);
		if (result != -1) set.stickL.deadzone = result / FACTOR;

		result = ReadEntry(L"Controller", port, L"L_AntiDeadzone", filename);
		if (result != -1) set.stickL.antiDeadzone = result / FACTOR;

		result = ReadEntry(L"Controller", port, L"R_Linearity", filename);
		if (result != -1) set.stickR.linearity = (result - 300.0) / 100.0;

		result = ReadEntry(L"Controller", port, L"R_Deadzone", filename);
		if (result != -1) set.stickR.deadzone = result / FACTOR;

		result = ReadEntry(L"Controller", port, L"R_AntiDeadzone", filename);
		if (result != -1) set.stickR.antiDeadzone = result / FACTOR;

		set.stickL.SetConsts();
		set.stickR.SetConsts();

		ReadRemap(port, filename, settings);
	}
}

void INI_ReloadSettings()
{
	extern SETTINGS settings[4];
	INI_LoadSettings(settings);
}