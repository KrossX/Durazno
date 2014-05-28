/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

void __fastcall DummyGetState(XINPUT_STATE* pState);
void __fastcall TransformGetState(SETTINGS &settings, XINPUT_STATE* pState);
void __fastcall TransformRemap(REMAP *remap, XINPUT_STATE* pState);
void __fastcall TransformSetState(f64 rumble, XINPUT_VIBRATION* pVibration);