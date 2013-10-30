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
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Runtime.InteropServices;


namespace DuraznoGUI
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	/// 
	public partial class MainWindow : Window
	{
		public bool[,] invertedAxis = new bool[4,4];
		public bool[] isEnabled = new bool[4];
		public double[] deadzone = new double[4];
		public double[] rumble = new double[4];
		public int[] linearity = new int[4];
			
		int curPad = 0;
		
		//
		// From FreewareFire's info (GPLv3)
		// http://www.codeproject.com/Articles/11114/Move-window-form-without-Titlebar-in-C
		//
		[DllImportAttribute("user32.dll")]
		public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
		[DllImportAttribute("user32.dll")]
		public static extern bool ReleaseCapture();

		INIsettings INIstuff;
		
		public MainWindow()
		{
			InitializeComponent();

			if (Version_Label != null) Version_Label.Content = "0.5 r" + SvnRevision.SVN_REV;

			INIstuff = new INIsettings(this);

			for (curPad = 0; curPad < 4; curPad++) SetDefaults();
			curPad = 0;

			INIstuff.LoadSettings();
			INIstuff.SaveSettings();

			UpdateControls();
		}

		private void SetDefaults()
		{
			for (int i = 0; i < 4; i++)
				invertedAxis[curPad, i] = false;

			isEnabled[curPad] = true;
			deadzone[curPad] = 0;
			rumble[curPad] = 100;
			linearity[curPad] = 0;
		}

		private void UpdateControls()
		{
			if (ControllerEnabled != null)
			{
				ControllerEnabled.IsChecked = isEnabled[curPad];
				if (!isEnabled[curPad])
				{
					Settings.IsEnabled = false;
					Settings.Opacity = 0.5;
				}
			}

			if (InvertLX != null) InvertLX.IsChecked = invertedAxis[curPad, 0];
			if (InvertLY != null) InvertLY.IsChecked = invertedAxis[curPad, 1];
			if (InvertRX != null) InvertRX.IsChecked = invertedAxis[curPad, 2];
			if (InvertRY != null) InvertRY.IsChecked = invertedAxis[curPad, 3];

			if (Rumble_Slider != null) Rumble_Slider.Value = rumble[curPad];
			if (Rumble_Value != null) Rumble_Value.Content = ((int)Rumble_Slider.Value).ToString() + "%";

			if (Deadzone_Slider != null) Deadzone_Slider.Value = deadzone[curPad];
			if (Deadzone_Value != null) Deadzone_Value.Content = ((int)Deadzone_Slider.Value).ToString() + "%";

			if (Linearity_Slider != null) Linearity_Slider.Value = linearity[curPad];

			if (Linearity_Value != null)
			{
				double dLinearity = Linearity_Slider.Value;
				int tmpLinearity = (int)(dLinearity + 0.5 * (dLinearity < 0 ? -1 : 1));
				tmpLinearity = tmpLinearity > 0 ? tmpLinearity + 1 : tmpLinearity < 0 ? tmpLinearity - 1 : tmpLinearity;

				Linearity_Value.Content = tmpLinearity.ToString();
			}
		}

		private void Close_Label_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			INIstuff.SaveSettings();
			Close();
		}

		private void DragMe_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			const int WM_NCLBUTTONDOWN = 0xA1;
			const int HT_CAPTION = 0x2;
			
			IntPtr hWnd = new WindowInteropHelper(this).Handle;

			ReleaseCapture();
        	SendMessage(hWnd, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
		}

		private void ControllerEnabled_Checked(object sender, System.Windows.RoutedEventArgs e)
		{
			Settings.IsEnabled = true;
			Settings.Opacity = 1.0;
			
			isEnabled[curPad] = true;
		}

		private void ControllerEnabled_Unchecked(object sender, System.Windows.RoutedEventArgs e)
		{
			Settings.IsEnabled = false;
			Settings.Opacity = 0.5;

			isEnabled[curPad] = false;
		}

		private void Rumble_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if(Rumble_Value != null)
			Rumble_Value.Content = ((int)Rumble_Slider.Value).ToString() + "%";

			rumble[curPad] = Rumble_Slider.Value;
		}

		private void Deadzone_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if (Deadzone_Value != null)
				Deadzone_Value.Content = ((int)Deadzone_Slider.Value).ToString() + "%";

			deadzone[curPad] = Deadzone_Slider.Value;
		}

		private void Linearity_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			double dLinearity = Linearity_Slider.Value;
			
			int _linearity = (int)(dLinearity + 0.5 * (dLinearity < 0? -1 : 1));

			linearity[curPad] = _linearity;

			_linearity = _linearity > 0 ? _linearity + 1 : _linearity < 0 ? _linearity - 1 : _linearity;
			
			if (Linearity_Value != null)
				Linearity_Value.Content = _linearity.ToString();
		}

		private void PadSelection_MouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			if (PadSelection != null)
			{
				curPad++;
				if(curPad > 3) curPad = 0;
				
				RotateTransform Wheee = new RotateTransform();

				switch (curPad)
				{
				case 0: Wheee.Angle = 0; break;
				case 1: Wheee.Angle = 90; break;
				case 2: Wheee.Angle = 270; break;
				case 3: Wheee.Angle = 180; break;
				}
				
				PadSelection.RenderTransform = Wheee;

				UpdateControls();
			}
		}

		private void InvertLX_Checked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 0] = true; }
		private void InvertLX_Unchecked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 0] = false; }

		private void InvertLY_Checked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 1] = true; }
		private void InvertLY_Unchecked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 1] = false; }

		private void InvertRX_Checked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 2] = true; }
		private void InvertRX_Unchecked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 2] = false; }

		private void InvertRY_Checked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 3] = true; }
		private void InvertRY_Unchecked(object sender, System.Windows.RoutedEventArgs e) { invertedAxis[curPad, 3] = false; }
	}
}
