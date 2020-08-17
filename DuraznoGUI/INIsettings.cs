/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
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

		const double FACTOR = 10.0;

		public INIsettings(MainWindow mWin, int version)
		{
			mainWindow = mWin;
			INIversion = version;
		}

		bool SaveRemapString(int port)
		{
			string valueString = "";

			for (int i = 0; i < 24; i++)
				valueString += string.Format("{0:00}", mainWindow.settings[port].remap[i]) + (i == 23 ? "" : " ");
			
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

			if (nSize == 71) for (int i = 0; i < 24; i++)
			{
				int value = int.Parse(strBuilder.ToString().Substring(i * 3, 2));
				mainWindow.settings[port].remap[i] = value;
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
			mainWindow.settings[0].port = 0;
			mainWindow.settings[1].port = 1;
			mainWindow.settings[2].port = 2;
			mainWindow.settings[3].port = 3;

			if (ReadEntry("General", "INIversion") != INIversion) return;

			for (int port = 0; port < 4; port++)
			{
				SETTINGS set = mainWindow.settings[port];

				int l_linearity, l_deadzone, l_antideadzone;
				int r_linearity, r_deadzone, r_antideadzone;

				set.port       = ReadEntry("Controller" + port, "Port") % 4;
				set.isDisabled = ReadEntry("Controller" + port, "Disable") == 1;
				set.isDummy    = ReadEntry("Controller" + port, "Dummy") == 1;

				set.rumble = ReadEntry("Controller" + port, "Rumble") / FACTOR;

				set.linearDZ        = ReadEntry("Controller" + port, "L_LinearDZ") == 1;
				set.linearADZ       = ReadEntry("Controller" + port, "L_LinearADZ") == 1;
				set.invertedAxis[0] = ReadEntry("Controller" + port, "L_Inverted_X") == 1;
				set.invertedAxis[1] = ReadEntry("Controller" + port, "L_Inverted_Y") == 1;
				l_linearity         = ReadEntry("Controller" + port, "L_Linearity");
				l_deadzone          = ReadEntry("Controller" + port, "L_Deadzone");
				l_antideadzone      = ReadEntry("Controller" + port, "L_AntiDeadzone");

				set.linearDZ        = ReadEntry("Controller" + port, "R_LinearDZ") == 1;
				set.linearADZ       = ReadEntry("Controller" + port, "R_LinearADZ") == 1;
				set.invertedAxis[2] = ReadEntry("Controller" + port, "R_Inverted_X") == 1;
				set.invertedAxis[3] = ReadEntry("Controller" + port, "R_Inverted_Y") == 1;
				r_linearity         = ReadEntry("Controller" + port, "R_Linearity");
				r_deadzone          = ReadEntry("Controller" + port, "R_Deadzone");
				r_antideadzone      = ReadEntry("Controller" + port, "R_AntiDeadzone");

				set.triggerL.min = ReadEntry("Controller" + port, "L_TriggerMin") & 0xFF;
				set.triggerL.max = ReadEntry("Controller" + port, "L_TriggerMax") & 0xFF;
				set.triggerR.min = ReadEntry("Controller" + port, "R_TriggerMin") & 0xFF;
				set.triggerR.max = ReadEntry("Controller" + port, "R_TriggerMax") & 0xFF;

				set.stickL.linearity    = l_linearity < 0 ? 0 : (l_linearity - 300) / 100.0;
				set.stickL.deadzone     = l_deadzone / FACTOR;
				set.stickL.antiDeadzone = l_antideadzone / FACTOR;

				set.stickR.linearity    = r_linearity < 0 ? 0 : (r_linearity - 300) / 100.0;
				set.stickR.deadzone     = r_deadzone / FACTOR;
				set.stickR.antiDeadzone = r_antideadzone / FACTOR;

				if (l_linearity == r_linearity &&
					l_deadzone == r_deadzone &&
					l_antideadzone == r_antideadzone)
					set.perStick = false;
				else
					set.perStick = true;

				if (set.rumble < 0) set.rumble = 100.0;
				if (set.stickL.deadzone < 0) set.stickL.deadzone = 0.0;
				if (set.stickL.antiDeadzone < 0) set.stickL.antiDeadzone = 0.0;
				if (set.stickR.deadzone < 0) set.stickR.deadzone = 0.0;
				if (set.stickR.antiDeadzone < 0) set.stickR.antiDeadzone = 0.0;

				ReadRemapString(port);
			}
		}

		public void SaveSettings()
		{
			SaveEntry("General", "INIversion", INIversion);
			
			for (int port = 0; port < 4; port++)
			{
				SETTINGS set = mainWindow.settings[port];

				SaveEntry("Controller" + port, "Port",    set.port);
				SaveEntry("Controller" + port, "Disable", set.isDisabled ? 1 : 0);
				SaveEntry("Controller" + port, "Dummy",   set.isDummy ? 1 : 0);

				SaveEntry("Controller" + port, "Rumble", (int)(set.rumble * FACTOR));

				SaveEntry("Controller" + port, "L_LinearDZ",           set.linearDZ ? 1 : 0);
				SaveEntry("Controller" + port, "L_LinearADZ",          set.linearADZ ? 1 : 0);
				SaveEntry("Controller" + port, "L_Inverted_X",         set.invertedAxis[0] ? 1 : 0);
				SaveEntry("Controller" + port, "L_Inverted_Y",         set.invertedAxis[1] ? 1 : 0);
				SaveEntry("Controller" + port, "L_Linearity",    (int)(set.stickL.linearity * 100.0) + 300);
				SaveEntry("Controller" + port, "L_Deadzone",     (int)(set.stickL.deadzone * FACTOR));
				SaveEntry("Controller" + port, "L_AntiDeadzone", (int)(set.stickL.antiDeadzone * FACTOR));

				if (!set.perStick)
					set.stickR = set.stickL;

				SaveEntry("Controller" + port, "R_LinearDZ",           set.linearDZ ? 1 : 0);
				SaveEntry("Controller" + port, "R_LinearADZ",          set.linearADZ ? 1 : 0);
				SaveEntry("Controller" + port, "R_Inverted_X",         set.invertedAxis[2] ? 1 : 0);
				SaveEntry("Controller" + port, "R_Inverted_Y",         set.invertedAxis[3] ? 1 : 0);
				SaveEntry("Controller" + port, "R_Linearity",    (int)(set.stickR.linearity * 100.0) + 300);
				SaveEntry("Controller" + port, "R_Deadzone",     (int)(set.stickR.deadzone * FACTOR));
				SaveEntry("Controller" + port, "R_AntiDeadzone", (int)(set.stickR.antiDeadzone * FACTOR));

				SaveEntry("Controller" + port, "L_TriggerMin", set.triggerL.min);
				SaveEntry("Controller" + port, "L_TriggerMax", set.triggerL.max);
				SaveEntry("Controller" + port, "R_TriggerMin", set.triggerR.min);
				SaveEntry("Controller" + port, "R_TriggerMax", set.triggerR.max);

				SaveRemapString(port);
			}
		}

	}
}
