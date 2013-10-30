/*  From some old project of mine
 *  Copyright (C) 2011 KrossX
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

#ifndef KROSSX_TYPEDEFS
#define KROSSX_TYPEDEFS

#include <stdint.h>

typedef int8_t    s8;
typedef uint8_t   u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef uint32_t u32;
typedef int64_t  s64;
typedef uint64_t u64;

typedef	intptr_t sPointer;
typedef uintptr_t uPointer;

typedef float  f32;
typedef double f64;

union Word
{
	f32 float32;
	u32 bits32;
	u16 bits16[2];
	u8  bits8[4];
};

union DWord
{
	u64 bits64;
	f64 float64;
	f32 float32[2];
	u32 bits32[2];
	u16 bits16[4];
	u8  bits8[8];

	Word word[2];
};

union QWord
{
	u64 bits64[2];
	f64 float64[2];
	f32 float32[4];
	u32 bits32[4];
	u16 bits16[8];
	u8  bits8[16];

	Word word[4];
	DWord dword[2];
};
#endif // KROSSX_TYPEDEFS
