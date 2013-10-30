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

#pragma once

void __fastcall DummyGetState(XINPUT_STATE* pState);
void __fastcall TransformGetState(DWORD dwUserIndex, XINPUT_STATE* pState);
void __fastcall TransformRemap(DWORD dwUserIndex, XINPUT_STATE* pState);
void __fastcall TransformSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration);