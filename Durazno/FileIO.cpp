/*  Durazno - XInput to XInput Wrapper
 *  Copyright (C) 2012 KrossX
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Windows.h>
#include <stdio.h>

#include "TypeDefs.h"
#include "Settings.h"

#include <string>

extern HINSTANCE g_hinstDLL;
extern _Settings settings[4];
extern s32 INIversion;
extern std::wstring customDLL;

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

bool SaveRemap(u8 port, wchar_t * filename)
{	
	wchar_t controller[512] = {0};
	wchar_t valuestring[512] = {0};

	swprintf(controller, 512, L"Controller%d", port);
		
	for (int i = 0; i < 24; i++)
		swprintf(valuestring, 512, i == 0? L"%s%02d" : L"%s %02d", valuestring, settings[port].remap[i].control);

	return WritePrivateProfileString(controller,  L"Remap", valuestring, filename) ? true : false;
}

void ReadRemap(u8 port, wchar_t * filename)
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

void INI_SaveSettings()
{
	wchar_t filename[] = L".\\Durazno.ini";

	SaveEntry(L"General", -1, L"INIversion", INIversion, filename);
	SaveString(L"General", -1, L"LoadDLL", customDLL, filename);

	for(s32 port = 0; port < 4; port++)
	{
		SaveEntry(L"Controller", port, L"Disable", settings[port].isDisabled?1:0, filename);
		SaveEntry(L"Controller", port, L"Dummy", settings[port].isDummy?1:0, filename);
		
		SaveEntry(L"Controller", port, L"AxisInvertedLX", settings[port].axisInverted[GP_AXIS_LX]?1:0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedLY", settings[port].axisInverted[GP_AXIS_LY]?1:0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedRX", settings[port].axisInverted[GP_AXIS_RX]?1:0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedRY", settings[port].axisInverted[GP_AXIS_RY]?1:0, filename);

		SaveEntry(L"Controller", port, L"TriggerDeadzone", settings[port].triggerDeadzone, filename);
		SaveEntry(L"Controller", port, L"Linearity", (s32)(settings[port].linearity*10) +30, filename);
		SaveEntry(L"Controller", port, L"Deadzone", (s32)(settings[port].deadzone * 100), filename);
		SaveEntry(L"Controller", port, L"AntiDeadzone", (s32)(settings[port].antiDeadzone * 100), filename);
		SaveEntry(L"Controller", port, L"Rumble", (s32)(settings[port].rumble * 100), filename);
		SaveEntry(L"Controller", port, L"Port", (s32)(settings[port].port), filename);

		SaveRemap(port, filename);
	}
}

void INI_LoadSettings()
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
		s32 result;
		
		settings[port].isDisabled = ReadEntry(L"Controller", port, L"Disable", filename) == 1? true : false;
		settings[port].isDummy = ReadEntry(L"Controller", port, L"Dummy", filename) == 1? true : false;

		settings[port].axisInverted[GP_AXIS_LX] = ReadEntry(L"Controller", port, L"AxisInvertedLX", filename) == 1? true : false;
		settings[port].axisInverted[GP_AXIS_LY] = ReadEntry(L"Controller", port, L"AxisInvertedLY", filename) == 1? true : false;
		settings[port].axisInverted[GP_AXIS_RX] = ReadEntry(L"Controller", port, L"AxisInvertedRX", filename) == 1? true : false;
		settings[port].axisInverted[GP_AXIS_RY] = ReadEntry(L"Controller", port, L"AxisInvertedRY", filename) == 1? true : false;

		result = ReadEntry(L"Controller", port, L"TriggerDeadzone",  filename);
		if(result != -1) settings[port].triggerDeadzone = result & 0xFF;

		result = ReadEntry(L"Controller", port, L"Linearity",  filename);
		if(result != -1) settings[port].linearity = result/10.0 - 3.0;

		result = ReadEntry(L"Controller", port, L"Deadzone",  filename);
		if(result != -1) settings[port].deadzone = result / 100.0f;

		result = ReadEntry(L"Controller", port, L"AntiDeadzone",  filename);
		if(result != -1) settings[port].antiDeadzone = result / 100.0f;

		result = ReadEntry(L"Controller", port, L"Rumble", filename);
		if(result != -1) settings[port].rumble = result / 100.0f;

		result = ReadEntry(L"Controller", port, L"Port", filename);
		if(result != -1) settings[port].port = result % 4;

		ReadRemap(port, filename);
	}
}