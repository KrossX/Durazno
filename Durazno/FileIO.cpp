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

#include "Settings.h"

extern HINSTANCE g_hinstDLL;
extern _Settings settings[4];

bool SaveEntry(wchar_t * section, int sectionNumber, wchar_t * key, int value, wchar_t * filename)
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

int ReadEntry(wchar_t * section, int sectionNumber, wchar_t * key, wchar_t * filename)
{	
	wchar_t controller[512] = {0};

	if(sectionNumber < 0)
		swprintf(controller, 512, L"%s", section);
	else
		swprintf(controller, 512, L"%s%d", section, sectionNumber);
	
	int returnInteger = -1;
	wchar_t returnvalue[512] = {0};
	int nSize = GetPrivateProfileString(controller, key, L"-1", returnvalue, 512, filename);

	 if(nSize < 256) returnInteger = _wtoi(returnvalue);	 

	return returnInteger;
}

void INI_SaveSettings()
{
	wchar_t filename[] = L".\\Durazno.ini";

	for(int port = 0; port < 4; port++)
	{
		SaveEntry(L"Controller", port, L"Disable", settings[port].isDisabled?1:0, filename);
		
		SaveEntry(L"Controller", port, L"AxisInvertedLX", settings[port].axisInverted[GP_AXIS_LX]?1:0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedLY", settings[port].axisInverted[GP_AXIS_LY]?1:0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedRX", settings[port].axisInverted[GP_AXIS_RX]?1:0, filename);
		SaveEntry(L"Controller", port, L"AxisInvertedRY", settings[port].axisInverted[GP_AXIS_RY]?1:0, filename);

		SaveEntry(L"Controller", port, L"Linearity", (int)(settings[port].linearity*10) +30, filename);
		SaveEntry(L"Controller", port, L"Deadzone", (int)(settings[port].deadzone * 100), filename);
		SaveEntry(L"Controller", port, L"AntiDeadzone", (int)(settings[port].antiDeadzone * 100), filename);
		SaveEntry(L"Controller", port, L"Rumble", (int)(settings[port].rumble * 100), filename);
	}		
}

void INI_LoadSettings()
{
	wchar_t filename[] = L".\\Durazno.ini";

	for(int port = 0; port < 4; port++)
	{
		int result;
		
		settings[port].isDisabled = ReadEntry(L"Controller", port, L"Disable", filename) == 1? true : false;

		settings[port].axisInverted[GP_AXIS_LX] = ReadEntry(L"Controller", port, L"AxisInvertedLX", filename) == 1? true : false;
		settings[port].axisInverted[GP_AXIS_LY] = ReadEntry(L"Controller", port, L"AxisInvertedLY", filename) == 1? true : false;
		settings[port].axisInverted[GP_AXIS_RX] = ReadEntry(L"Controller", port, L"AxisInvertedRX", filename) == 1? true : false;
		settings[port].axisInverted[GP_AXIS_RY] = ReadEntry(L"Controller", port, L"AxisInvertedRY", filename) == 1? true : false;

		result = ReadEntry(L"Controller", port, L"Linearity",  filename);
		if(result != -1) settings[port].linearity = result/10.0 - 3.0;

		result = ReadEntry(L"Controller", port, L"Deadzone",  filename);
		if(result != -1) settings[port].deadzone = result / 100.0f;

		result = ReadEntry(L"Controller", port, L"AntiDeadzone",  filename);
		if(result != -1) settings[port].antiDeadzone = result / 100.0f;

		result = ReadEntry(L"Controller", port, L"Rumble", filename);
		if(result != -1) settings[port].rumble = result / 100.0f;
	}
}