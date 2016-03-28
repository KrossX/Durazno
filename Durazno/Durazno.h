/* Copyright (c) 2012 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

#pragma once

#include <Windows.h>
#include <XInput.h>
#include <stdio.h>

#include "TypeDefs.h"

extern "C" DWORD WINAPI DuraznoGetState(DWORD dwUserIndex, XINPUT_STATE* pState);