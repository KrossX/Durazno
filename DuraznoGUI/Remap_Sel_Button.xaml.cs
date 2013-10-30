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
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;
using System.Runtime.InteropServices;


namespace DuraznoGUI
{
	/// <summary>
	/// Interaction logic for Remap_Sel_Button.xaml
	/// </summary>
	public partial class Remap_Sel_Button : UserControl
	{
		[DllImport("xinput1_3")]
		private static extern int GetControllerInput(int port);
		
		List<String> ButtonText;
		Storyboard ending, starting;
		int port, input;

		public Remap_Sel_Button()
		{
			ButtonText = new List<string>()
			{
				"Up", "Down", "Left", "Right", "Start", "Back",
				"LS", "RS", "LB", "RB", "A", "B", "X", "Y", "LT", "RT",
				"LX+", "LX-", "LY+", "LY-", "RX+", "RX-","RY+", "RY-"
			};			
						
			this.InitializeComponent();

			ending = FindResource("Waiting_Off") as Storyboard;
			starting = FindResource("Waiting_On") as Storyboard;

			port = 0;
		}

		public void SetPort(int newport)
		{
			port = newport;
		}

		public int GetInputValue()
		{
			return input;
		}
		
		private void StoryEnd(Storyboard story)
		{
			story.Begin();
		}

		public void SetNewInput(int newInput)
		{
			if (newInput >= 0)
			{
				Text.Text = ButtonText[newInput];
				input = newInput;
			}
		}

		private void Worker()
		{
			try { input = GetControllerInput(port); }
			catch {};
			
			Dispatcher.Invoke(new Action<int>(SetNewInput), input);
			Dispatcher.Invoke(new Action<Storyboard>(StoryEnd), ending);
		}

		private void UserControl_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			starting.Begin();
			new Thread(new ThreadStart(Worker)).Start();			
		}

		
	}
}
