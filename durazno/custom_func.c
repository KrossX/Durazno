/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */

// cheap strings

static
DWORD cheap_find_last_of(char *string, char c)
{
	DWORD charpos = 0;

	for (char *strpos = string; *strpos; strpos++)
	{
		if (*strpos == c)
			charpos = (DWORD)(strpos - string);
	}

	return charpos;
}

static
DWORD cheap_strlen(char *string)
{
	DWORD count = 0;

	while (*string++)
		count++;

	return count;
}

static
int cheap_atoi_n(char *str, int n)
{
	// no negative
	int num = 0;

	for (char *p = str; *p && n; p++, n--)
	{
		if (*p >= '0' && *p <= '9')
			num = num * 10 + (*p - '0');
	}

	return num;
}

static
char* next_str(char *str)
{
	return str + cheap_strlen(str) + 1;
}

// REDDITNESS?
static
double fastPow(double a, double b)
{
	union {
		double d;
		int x[2];
	} u;

	u.d = a;
	u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
	u.x[0] = 0;
	return u.d;
}

// QUAKENESS!
static
float Q_rsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;            // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);  // what the fuck?
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y));   // 1st iteration
										   //y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}