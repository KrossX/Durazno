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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace DuraznoGUI
{
	class INIsettings
	{
		[DllImport("kernel32")]
		private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);
		[DllImport("kernel32")]
		private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal,	int size, string filePath);

		MainWindow mainWindow;
		const string INIfilename = ".\\Durazno.INI";
		int INIversion;

		public INIsettings(MainWindow mWin, int version)
		{
			mainWindow = mWin;
			INIversion = version;
		}

		bool SaveRemapString(int port)
		{
			string valueString = "";

			for (int i = 0; i < 24; i++)
				valueString += string.Format("{0:00}", mainWindow.RemapSet[port, i]) + (i == 23 ? "" : " ");
			
			return WritePrivateProfileString("Controller" + port, "Remap", valueString, INIfilename) != 0 ? true : false;
		}

		bool SaveEntry(string section, string key, int value)
		{
			string valueString = value.ToString();
			return WritePrivateProfileString(section, key, valueString, INIfilename) != 0 ? true : false;
		}

		void ReadRemapString(int port)
		{
			StringBuilder strBuilder = new StringBuilder(512);

			int nSize = GetPrivateProfileString("Controller" + port, "Remap", "-1", strBuilder, 512, INIfilename);

			if (nSize > 70) for (int i = 0; i < 24; i++)
			{
				int value = int.Parse(strBuilder.ToString().Substring(i * 3, 2));
				mainWindow.RemapSet[port, i] = value;
			}
		}

		int ReadEntry(string section, string key)
		{
			int returnValue = -1;

			StringBuilder strBuilder = new StringBuilder(512);

			int nSize = GetPrivateProfileString(section, key, "-1", strBuilder, 512, INIfilename);

			returnValue = int.Parse(strBuilder.ToString());

			return returnValue;
		}

		public void LoadSettings()
		{
			if (ReadEntry("General", "INIversion") != INIversion) return;
			
			for (int port = 0; port < 4; port++)
			{
				mainWindow.isEnabled[port] = ReadEntry("Controller" + port, "Disable") == 1 ? false : true;

				mainWindow.invertedAxis[port, 0] = ReadEntry("Controller" + port, "AxisInvertedLX") == 1 ? true : false;
				mainWindow.invertedAxis[port, 1] = ReadEntry("Controller" + port, "AxisInvertedLY") == 1 ? true : false;
				mainWindow.invertedAxis[port, 2] = ReadEntry("Controller" + port, "AxisInvertedRX") == 1 ? true : false;
				mainWindow.invertedAxis[port, 3] = ReadEntry("Controller" + port, "AxisInvertedRY") == 1 ? true : false;

				mainWindow.linearity[port] = ReadEntry("Controller" + port, "Linearity");
				mainWindow.linearity[port] = mainWindow.linearity[port] < 0 ? 0 : mainWindow.linearity[port] / 10.0 - 3.0;

				mainWindow.deadzone[port] = ReadEntry("Controller" + port, "Deadzone");
				mainWindow.antiDeadzone[port] = ReadEntry("Controller" + port, "AntiDeadzone");
				mainWindow.rumble[port] = ReadEntry("Controller" + port, "Rumble");

				if (mainWindow.deadzone[port] < 0) mainWindow.deadzone[port] = 0.0;
				if (mainWindow.antiDeadzone[port] < 0) mainWindow.antiDeadzone[port] = 0.0;
				if (mainWindow.rumble[port] < 0) mainWindow.rumble[port] = 100.0;

				ReadRemapString(port);
			}
		}

		public void SaveSettings()
		{
			SaveEntry("General", "INIversion", INIversion);
			
			for (int port = 0; port < 4; port++)
			{
				SaveEntry("Controller" + port, "Disable", mainWindow.isEnabled[port] ? 0 : 1);

				SaveEntry("Controller" + port, "AxisInvertedLX", mainWindow.invertedAxis[port, 0] ? 1 : 0);
				SaveEntry("Controller" + port, "AxisInvertedLY", mainWindow.invertedAxis[port, 1] ? 1 : 0);
				SaveEntry("Controller" + port, "AxisInvertedRX", mainWindow.invertedAxis[port, 2] ? 1 : 0);
				SaveEntry("Controller" + port, "AxisInvertedRY", mainWindow.invertedAxis[port, 3] ? 1 : 0);

				SaveEntry("Controller" + port, "Linearity", (int)(mainWindow.linearity[port]*10 + 30));
				SaveEntry("Controller" + port, "Deadzone", (int)mainWindow.deadzone[port]);
				SaveEntry("Controller" + port, "AntiDeadzone", (int)mainWindow.antiDeadzone[port]);
				SaveEntry("Controller" + port, "Rumble", (int)mainWindow.rumble[port]);

				SaveRemapString(port);
			}
		}

	}
}
