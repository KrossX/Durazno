/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
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
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Windows.Threading;


namespace DuraznoGUI
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	/// 

	public struct STICK
	{
		public double linearity, deadzone, antiDeadzone;
	}

	public class SETTINGS
	{
		public int port;
		public bool linearDZ, linearADZ, isDisabled, isDummy, perStick;
		public bool[] invertedAxis;
		public int triggerMin, triggerMax;
		public double rumble;

		public STICK stickL, stickR;
		public int[] remap;

		public SETTINGS()
		{
			port = 0;
			isDisabled = false;
			isDummy = false;
			linearDZ = false;
			linearADZ = false;

			perStick = false;

			rumble = 100.0;

			stickL.deadzone = 0.0;
			stickL.antiDeadzone = 0.0;
			stickL.linearity = 0.0;

			stickR.deadzone = 0.0;
			stickR.antiDeadzone = 0.0;
			stickR.linearity = 0.0;

			triggerMin = 0;
			triggerMax = 255;

			invertedAxis = new bool[4];
			remap = new int[24];

			for (int i = 0; i < 4; i++)
				invertedAxis[i] = false;

			for (int i = 0; i < 24; i++)
				remap[i] = i;
		}
	}

	[StructLayout(LayoutKind.Explicit)]
	public struct XInputState
	{
		[FieldOffset(0)]
		public int PacketNumber;

		[FieldOffset(4)]
		public XInputGamepad Gamepad;
	}

	[StructLayout(LayoutKind.Explicit)]
	public struct XInputGamepad
	{
		[MarshalAs(UnmanagedType.I2)]
		[FieldOffset(0)]
		public UInt16 wButtons;

		[MarshalAs(UnmanagedType.I1)]
		[FieldOffset(2)]
		public byte bLeftTrigger;

		[MarshalAs(UnmanagedType.I1)]
		[FieldOffset(3)]
		public byte bRightTrigger;

		[MarshalAs(UnmanagedType.I2)]
		[FieldOffset(4)]
		public short sThumbLX;

		[MarshalAs(UnmanagedType.I2)]
		[FieldOffset(6)]
		public short sThumbLY;

		[MarshalAs(UnmanagedType.I2)]
		[FieldOffset(8)]
		public short sThumbRX;

		[MarshalAs(UnmanagedType.I2)]
		[FieldOffset(10)]
		public short sThumbRY;
	}

	enum XInputButtons
	{
		DPAD_UP          = 0x0001,
		DPAD_DOWN        = 0x0002,
		DPAD_LEFT        = 0x0004,
		DPAD_RIGHT       = 0x0008,
		START            = 0x0010,
		BACK             = 0x0020,
		LEFT_THUMB       = 0x0040,
		RIGHT_THUMB      = 0x0080,
		LEFT_SHOULDER    = 0x0100,
		RIGHT_SHOULDER   = 0x0200,
		GUIDE            = 0x0400,
		A                = 0x1000,
		B                = 0x2000,
		X                = 0x4000,
		Y                = 0x8000
	}

	public partial class MainWindow : Window
	{
		[DllImport("xinput1_3.dll")]
		private static extern void INI_ReloadSettings();

		[DllImport("xinput1_3.dll", CallingConvention = CallingConvention.Winapi)]
		private static extern uint DuraznoGetStateEx(int port, ref XInputState state);

		[DllImport("xinput1_3.dll", CallingConvention = CallingConvention.Winapi, EntryPoint = "#100")]
		private static extern uint XInputGetStateEx(int port, ref XInputState state);

		public List<SETTINGS> settings;
		List<Remap_Sel_Button> RemapControl;

		int curPad = 0;
		SETTINGS curSet = new SETTINGS();

		[DllImportAttribute("user32.dll")]
		public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
		[DllImportAttribute("user32.dll")]
		public static extern bool ReleaseCapture();

		INIsettings INIstuff;
		
		public MainWindow()
		{
			settings = new List<SETTINGS> { new SETTINGS(), new SETTINGS(), new SETTINGS(), new SETTINGS()};
			curSet = settings[curPad];

			InitializeComponent();
			GridStickR_Hide();
			BtnSettings.BorderBrush = new SolidColorBrush(Colors.White);

			if (Version_Label != null) Version_Label.Content = "v0.6 ";

			INIstuff = new INIsettings(this, 3); // Set INI version here
			INIstuff.LoadSettings();
			INIstuff.SaveSettings();

			RemapControl = new List<Remap_Sel_Button>
			{
				R_Dpad_Up, R_Dpad_Down, R_Dpad_Left, R_Dpad_Right, R_Button_Start, R_Button_Back,
				R_LeftStick_Button, R_RightStick_Button, R_Shoulder_Left, R_Shoulder_Right,
				R_Button_A, R_Button_B, R_Button_X, R_Button_Y, R_Trigger_Left, R_Trigger_Right,
				R_LeftStick_XP, R_LeftStick_XM, R_LeftStick_YP, R_LeftStick_YM,
				R_RightStick_XP, R_RightStick_XM, R_RightStick_YP, R_RightStick_YM
			};

			for (int i = 0; i < 24; i++)
				RemapControl[i].SetNewInput(curSet.remap[i]);

			UpdateControls();

			System.ComponentModel.BackgroundWorker worker1 = new System.ComponentModel.BackgroundWorker();
			worker1.WorkerReportsProgress = true;
			worker1.DoWork += worker1_DoWork;
			worker1.ProgressChanged += worker1_ProgressChanged;
			worker1.RunWorkerAsync();
		}

		private void worker1_DoWork(object sender, EventArgs e)
		{
			System.ComponentModel.BackgroundWorker worker = sender as System.ComponentModel.BackgroundWorker;

			while(true)
			{
				worker.ReportProgress(50);
				System.Threading.Thread.Sleep(41);
			}
		}

		private void worker1_ProgressChanged(object sender, EventArgs e)
		{
			TestUpdate();
		}

		private void TestUpdate()
		{
			XInputState state = new XInputState();

			try { DuraznoGetStateEx(curSet.port, ref state); }
			catch { }

			double LX = (state.Gamepad.sThumbLX + 32767) / 65535.0 * 110.0;
			double LY = (32768 - state.Gamepad.sThumbLY) / 65535.0 * 110.0;

			double RX = (state.Gamepad.sThumbRX + 32767) / 65535.0 * 110.0;
			double RY = (32768 - state.Gamepad.sThumbRY) / 65535.0 * 110.0;

			double LT = state.Gamepad.bLeftTrigger / 255.0 * 120.0;
			double RT = state.Gamepad.bRightTrigger / 255.0 * 120.0;

			StickL.Margin = new Thickness(LX, LY, 0, 0);
			StickR.Margin = new Thickness(RX, RY, 0, 0);

			TriggerL.Height = LT;
			TriggerR.Height = RT;

			SolidColorBrush cOff = new SolidColorBrush(Color.FromArgb(0x19, 0xFF, 0xFF, 0xFF));
			SolidColorBrush cOn = new SolidColorBrush(Color.FromArgb(0xCC, 0xFF, 0xFF, 0xFF));

			UInt16 buttons = state.Gamepad.wButtons;

			Button0.Fill = (buttons & (UInt16)XInputButtons.DPAD_UP) > 0 ? cOn : cOff;
			Button1.Fill = (buttons & (UInt16)XInputButtons.DPAD_DOWN) > 0 ? cOn : cOff;
			Button2.Fill = (buttons & (UInt16)XInputButtons.DPAD_LEFT) > 0 ? cOn : cOff;
			Button3.Fill = (buttons & (UInt16)XInputButtons.DPAD_RIGHT) > 0 ? cOn : cOff;
			Button4.Fill = (buttons & (UInt16)XInputButtons.START) > 0 ? cOn : cOff;
			Button5.Fill = (buttons & (UInt16)XInputButtons.BACK) > 0 ? cOn : cOff;
			Button6.Fill = (buttons & (UInt16)XInputButtons.LEFT_THUMB) > 0 ? cOn : cOff;
			Button7.Fill = (buttons & (UInt16)XInputButtons.RIGHT_THUMB) > 0 ? cOn : cOff;
			Button8.Fill = (buttons & (UInt16)XInputButtons.LEFT_SHOULDER) > 0 ? cOn : cOff;
			Button9.Fill = (buttons & (UInt16)XInputButtons.RIGHT_SHOULDER) > 0 ? cOn : cOff;
			Button10.Fill = (buttons & (UInt16)XInputButtons.GUIDE) > 0 ? cOn : cOff;
			Button11.Fill = (buttons & (UInt16)XInputButtons.A) > 0 ? cOn : cOff;
			Button12.Fill = (buttons & (UInt16)XInputButtons.B) > 0 ? cOn : cOff;
			Button13.Fill = (buttons & (UInt16)XInputButtons.X) > 0 ? cOn : cOff;
			Button14.Fill = (buttons & (UInt16)XInputButtons.Y) > 0 ? cOn : cOff;

			TextDebug.Text = state.Gamepad.sThumbLX.ToString() + "\n" + state.Gamepad.sThumbLY.ToString() + "\n" +
							 state.Gamepad.sThumbRX.ToString() + "\n" + state.Gamepad.sThumbRY.ToString() + "\n" +
							 state.Gamepad.bLeftTrigger.ToString() + "\n" + state.Gamepad.bRightTrigger.ToString() + "\n" +
							 state.Gamepad.wButtons.ToString("X4");

			try { XInputGetStateEx(curSet.port, ref state); }
			catch { }

			LX = (state.Gamepad.sThumbLX + 32767) / 65535.0 * 110.0;
			LY = (32768 - state.Gamepad.sThumbLY) / 65535.0 * 110.0;

			RX = (state.Gamepad.sThumbRX + 32767) / 65535.0 * 110.0;
			RY = (32768 - state.Gamepad.sThumbRY) / 65535.0 * 110.0;

			LT = state.Gamepad.bLeftTrigger / 255.0 * 120.0;
			RT = state.Gamepad.bRightTrigger / 255.0 * 120.0;

			t_StickL.Margin = new Thickness(LX, LY, 0, 0);
			t_StickR.Margin = new Thickness(RX, RY, 0, 0);

			t_TriggerL.Height = LT;
			t_TriggerR.Height = RT;

			buttons = state.Gamepad.wButtons;

			t_Button0.Fill = (buttons & (UInt16)XInputButtons.DPAD_UP) > 0 ? cOn : cOff;
			t_Button1.Fill = (buttons & (UInt16)XInputButtons.DPAD_DOWN) > 0 ? cOn : cOff;
			t_Button2.Fill = (buttons & (UInt16)XInputButtons.DPAD_LEFT) > 0 ? cOn : cOff;
			t_Button3.Fill = (buttons & (UInt16)XInputButtons.DPAD_RIGHT) > 0 ? cOn : cOff;
			t_Button4.Fill = (buttons & (UInt16)XInputButtons.START) > 0 ? cOn : cOff;
			t_Button5.Fill = (buttons & (UInt16)XInputButtons.BACK) > 0 ? cOn : cOff;
			t_Button6.Fill = (buttons & (UInt16)XInputButtons.LEFT_THUMB) > 0 ? cOn : cOff;
			t_Button7.Fill = (buttons & (UInt16)XInputButtons.RIGHT_THUMB) > 0 ? cOn : cOff;
			t_Button8.Fill = (buttons & (UInt16)XInputButtons.LEFT_SHOULDER) > 0 ? cOn : cOff;
			t_Button9.Fill = (buttons & (UInt16)XInputButtons.RIGHT_SHOULDER) > 0 ? cOn : cOff;
			t_Button10.Fill = (buttons & (UInt16)XInputButtons.GUIDE) > 0 ? cOn : cOff;
			t_Button11.Fill = (buttons & (UInt16)XInputButtons.A) > 0 ? cOn : cOff;
			t_Button12.Fill = (buttons & (UInt16)XInputButtons.B) > 0 ? cOn : cOff;
			t_Button13.Fill = (buttons & (UInt16)XInputButtons.X) > 0 ? cOn : cOff;
			t_Button14.Fill = (buttons & (UInt16)XInputButtons.Y) > 0 ? cOn : cOff;

			t_TextDebug.Text = state.Gamepad.sThumbLX.ToString() + "\n" + state.Gamepad.sThumbLY.ToString() + "\n" +
							   state.Gamepad.sThumbRX.ToString() + "\n" + state.Gamepad.sThumbRY.ToString() + "\n" +
							   state.Gamepad.bLeftTrigger.ToString() + "\n" + state.Gamepad.bRightTrigger.ToString() + "\n" +
							   state.Gamepad.wButtons.ToString("X4");
		}

		private void UpdateControls()
		{
			if (ControllerEnabled != null)
			{
				ControllerEnabled.IsChecked = !curSet.isDisabled;
				
				if (curSet.isDisabled)
				{
					Settings.IsEnabled = false;
					Settings.Opacity = 0.5;
				}
			}

			if (InvertLX != null) InvertLX.IsChecked = curSet.invertedAxis[0];
			if (InvertLY != null) InvertLY.IsChecked = curSet.invertedAxis[1];
			if (InvertRX != null) InvertRX.IsChecked = curSet.invertedAxis[2];
			if (InvertRY != null) InvertRY.IsChecked = curSet.invertedAxis[3];

			if (LinearDZ != null) { LinearDZ.IsChecked = curSet.linearDZ; }
			if (LinearADZ != null) { LinearADZ.IsChecked = curSet.linearADZ; }
			if (SeparateSticks != null) { SeparateSticks.IsChecked = curSet.perStick; }

			if (Rumble_Slider != null) Rumble_Slider.Value = curSet.rumble;
			if (Rumble_Value != null) Rumble_Value.Content = ((int)Rumble_Slider.Value).ToString() + "%";

			if (Deadzone_Slider != null) Deadzone_Slider.Value = curSet.stickL.deadzone;
			if (Deadzone_Value != null) Deadzone_Value.Content = ((int)Deadzone_Slider.Value).ToString() + "%";

			if (AntiDeadzone_Slider != null) AntiDeadzone_Slider.Value = curSet.stickL.antiDeadzone;
			if (AntiDeadzone_Value != null) AntiDeadzone_Value.Content = ((int)AntiDeadzone_Slider.Value).ToString() + "%";

			if (Linearity_Slider != null) Linearity_Slider.Value = curSet.stickL.linearity;

			if (Linearity_Value != null)
			{
				double dLinearity = Linearity_Slider.Value;
				dLinearity = dLinearity > 0 ? dLinearity + 1 : dLinearity < 0 ? dLinearity - 1 : dLinearity;
				Linearity_Value.Content = dLinearity.ToString("F2");
			}

			if (Deadzone_Slider2 != null) Deadzone_Slider2.Value = curSet.stickR.deadzone;
			if (Deadzone_Value2 != null) Deadzone_Value2.Content = ((int)Deadzone_Slider2.Value).ToString() + "%";

			if (AntiDeadzone_Slider2 != null) AntiDeadzone_Slider2.Value = curSet.stickR.antiDeadzone;
			if (AntiDeadzone_Value2 != null) AntiDeadzone_Value2.Content = ((int)AntiDeadzone_Slider2.Value).ToString() + "%";

			if (Linearity_Slider2 != null) Linearity_Slider2.Value = curSet.stickR.linearity;

			if (Linearity_Value2 != null)
			{
				double dLinearity = Linearity_Slider2.Value;
				dLinearity = dLinearity > 0 ? dLinearity + 1 : dLinearity < 0 ? dLinearity - 1 : dLinearity;
				Linearity_Value2.Content = dLinearity.ToString("F2");
			}

			for (int i = 0; i < 24; i++)
				RemapControl[i].SetNewInput(curSet.remap[i]);
		}

		private void SaveRemap()
		{
			for (int i = 0; i < 24; i++)
				curSet.remap[i] = RemapControl[i].GetInputValue();
		}

		private void Close_Label_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			SaveRemap();
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
			ControllerEnabled.Content = "Controller: Enabled";

			RemapControls.IsEnabled = true;
			RemapControls.Opacity = 1.0;

			Settings.IsEnabled = true;
			Settings.Opacity = 1.0;

			curSet.isDummy = false;
			curSet.isDisabled = false;
		}

		private void ControllerEnabled_Unchecked(object sender, System.Windows.RoutedEventArgs e)
		{
			ControllerEnabled.Content = "Controller: Disabled";

			RemapControls.IsEnabled = false;
			RemapControls.Opacity = 0.5;

			Settings.IsEnabled = false;
			Settings.Opacity = 0.5;

			curSet.isDummy = false;
			curSet.isDisabled = true;
		}

		private void ControllerEnabled_Indeterminate(object sender, System.Windows.RoutedEventArgs e)
		{
			ControllerEnabled.Content = "Controller: Dummy";

			RemapControls.IsEnabled = false;
			RemapControls.Opacity = 0.5;

			Settings.IsEnabled = false;
			Settings.Opacity = 0.5;

			curSet.isDummy = true;
			curSet.isDisabled = false;
		}

		private void Rumble_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if(Rumble_Value != null)
			Rumble_Value.Content = ((int)Rumble_Slider.Value).ToString() + "%";

			curSet.rumble = Rumble_Slider.Value;
		}

		private void Deadzone_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if (Deadzone_Value != null)
				Deadzone_Value.Content = ((int)Deadzone_Slider.Value).ToString() + "%";

			curSet.stickL.deadzone = Deadzone_Slider.Value;
		}
		
		private void AntiDeadzone_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if (AntiDeadzone_Value != null)
				AntiDeadzone_Value.Content = ((int)AntiDeadzone_Slider.Value).ToString() + "%";

			curSet.stickL.antiDeadzone = AntiDeadzone_Slider.Value;
		}

		private void Linearity_Slider_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			double dLinearity = (int)(Linearity_Slider.Value * 10.0)/10.0;

			curSet.stickL.linearity = dLinearity;

			dLinearity = dLinearity > 0 ? dLinearity + 1 : dLinearity < 0 ? dLinearity - 1 : dLinearity;
			
			if (Linearity_Value != null)
				Linearity_Value.Content = dLinearity.ToString("F2");
		}

		private void Deadzone_Slider2_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if (Deadzone_Value2 != null)
				Deadzone_Value2.Content = ((int)Deadzone_Slider2.Value).ToString() + "%";

			curSet.stickR.deadzone = Deadzone_Slider2.Value;
		}

		private void AntiDeadzone_Slider2_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			if (AntiDeadzone_Value2 != null)
				AntiDeadzone_Value2.Content = ((int)AntiDeadzone_Slider2.Value).ToString() + "%";

			curSet.stickR.antiDeadzone = AntiDeadzone_Slider2.Value;
		}

		private void Linearity_Slider2_ValueChanged(object sender, System.Windows.RoutedPropertyChangedEventArgs<double> e)
		{
			double dLinearity = (int)(Linearity_Slider2.Value * 10.0) / 10.0;

			curSet.stickR.linearity = dLinearity;

			dLinearity = dLinearity > 0 ? dLinearity + 1 : dLinearity < 0 ? dLinearity - 1 : dLinearity;

			if (Linearity_Value2 != null)
				Linearity_Value2.Content = dLinearity.ToString("F2");
		}

		private void PadSelection_MouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
		{
			if (PadSelection != null)
			{
				SaveRemap();

				curPad++;
				if(curPad > 3) curPad = 0;
				curSet = settings[curPad];

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

		private void InvertLX_Checked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[0] = true; }
		private void InvertLX_Unchecked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[0] = false; }

		private void InvertLY_Checked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[1] = true; }
		private void InvertLY_Unchecked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[1] = false; }

		private void InvertRX_Checked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[2] = true; }
		private void InvertRX_Unchecked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[2] = false; }

		private void InvertRY_Checked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[3] = true; }
		private void InvertRY_Unchecked(object sender, System.Windows.RoutedEventArgs e) { curSet.invertedAxis[3] = false; }

		private void LinearDZ_Checked(object sender, RoutedEventArgs e) { curSet.linearDZ = true; }
		private void LinearDZ_Unchecked(object sender, RoutedEventArgs e) { curSet.linearDZ = false; }

		private void LinearADZ_Checked(object sender, RoutedEventArgs e) { curSet.linearADZ = true; }
		private void LinearADZ_Unchecked(object sender, RoutedEventArgs e) { curSet.linearADZ = false; }

		private void GridStickR_Show()
		{
			Settings.RowDefinitions[2].Height = new GridLength(40, GridUnitType.Star);
			Settings.RowDefinitions[3].Height = new GridLength(30, GridUnitType.Star);

			SolidColorBrush green = new SolidColorBrush(Color.FromArgb(0xFF, 0xB6, 0xC8, 0xB6));

			Deadzone_Label.Foreground = green;
			Deadzone_Value.Foreground = green;
			AntiDeadzone_Label.Foreground = green;
			AntiDeadzone_Value.Foreground = green;
			Linearity_Label.Foreground = green;
			Linearity_Value.Foreground = green;
		}

		private void GridStickR_Hide()
		{
			Settings.RowDefinitions[2].Height = new GridLength(70, GridUnitType.Star);
			Settings.RowDefinitions[3].Height = new GridLength(0, GridUnitType.Star);

			SolidColorBrush gray = new SolidColorBrush(Color.FromArgb(0xFF, 0xB6, 0xB6, 0xB6));

			Deadzone_Label.Foreground = gray;
			Deadzone_Value.Foreground = gray;
			AntiDeadzone_Label.Foreground = gray;
			AntiDeadzone_Value.Foreground = gray;
			Linearity_Label.Foreground = gray;
			Linearity_Value.Foreground = gray;
		}

		private void SeparateSticks_Checked(object sender, RoutedEventArgs e)
		{
			Storyboard story = FindResource("Sticks_Off") as Storyboard;
			
			story.Completed += new EventHandler(delegate (Object o, EventArgs a) 
			{
				GridStickR_Show();
			
				story = FindResource("Sticks_On") as Storyboard;
				story.Begin();
			});

			story.Begin();

			curSet.perStick = true;
		}

		private void SeparateSticks_Unchecked(object sender, RoutedEventArgs e)
		{
			Storyboard story = FindResource("Sticks_Off") as Storyboard;

			story.Completed += new EventHandler(delegate(Object o, EventArgs a)
			{
				GridStickR_Hide();

				story = FindResource("Sticks_On") as Storyboard;
				story.Begin();
			});

			story.Begin();

			curSet.perStick = false;
		}

		private void BtnSettings_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			Settings.Visibility = Visibility.Visible;
			RemapControls.Visibility = Visibility.Collapsed;
			TestControls.Visibility = Visibility.Collapsed;

			BtnSettings.BorderBrush = new SolidColorBrush(Colors.White);
			BtnRemap.BorderBrush = new SolidColorBrush(Colors.Black);
			BtnTest.BorderBrush = new SolidColorBrush(Colors.Black);
		}

		private void BtnRemap_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			RemapControls.Visibility = Visibility.Visible;
			Settings.Visibility = Visibility.Collapsed;
			TestControls.Visibility = Visibility.Collapsed;

			BtnSettings.BorderBrush = new SolidColorBrush(Colors.Black);
			BtnRemap.BorderBrush = new SolidColorBrush(Colors.White);
			BtnTest.BorderBrush = new SolidColorBrush(Colors.Black);
		}

		private void BtnTest_MouseLeftButtonUp(object sender, MouseButtonEventArgs e)
		{
			TestControls.Visibility = Visibility.Visible;
			Settings.Visibility = Visibility.Collapsed;
			RemapControls.Visibility = Visibility.Collapsed;

			BtnSettings.BorderBrush = new SolidColorBrush(Colors.Black);
			BtnRemap.BorderBrush = new SolidColorBrush(Colors.Black);
			BtnTest.BorderBrush = new SolidColorBrush(Colors.White);

			SaveRemap();
			INIstuff.SaveSettings();

			try { INI_ReloadSettings(); }
			catch { }
		}


	}
}

