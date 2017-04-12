/* Copyright (c) 2017 KrossX <krossx@live.com>
 * License: http://www.opensource.org/licenses/mit-license.html  MIT License
 */
 
#define INI_FILENAME ".\\durazno.ini"
#define INI_BUFFSIZE 2048
#define INI_VERSION  4
#define INI_FFACTOR  1000.0f

char fileiobuf[INI_BUFFSIZE];

static
void load_remap(char *secbuf, struct remap *remap)
{
	char *secdata = next_str(secbuf);
	DWORD bufflen = (DWORD)(secdata - fileiobuf);
	DWORD length  = GetPrivateProfileStringA(secbuf, "Remap", "", secdata, bufflen, INI_FILENAME);
	
	if (length != 71)
	{
		for (int i = 0; i < 24; i++)
		{
			remap[i].control = i;

			if (i < 14)      remap[i].type = REMAP_DIGITAL_DIGITAL;
			else if (i < 16) remap[i].type = REMAP_TRIGGER_TRIGGER;
			else             remap[i].type = REMAP_ANALOG_ANALOG;
		}
	}
	else
	{
		for (int i = 0; i < 24; i++, secdata += 3)
		{
			int control = remap[i].control = cheap_atoi_n(secdata, 2);

			if (control < 14)
			{
				if (i < 14)      remap[i].type = REMAP_DIGITAL_DIGITAL;
				else if (i < 16) remap[i].type = REMAP_DIGITAL_TRIGGER;
				else             remap[i].type = REMAP_DIGITAL_ANALOG;
			}
			else if (control < 16)
			{
				if (i < 14)      remap[i].type = REMAP_TRIGGER_DIGITAL;
				else if (i < 16) remap[i].type = REMAP_TRIGGER_TRIGGER;
				else             remap[i].type = REMAP_TRIGGER_ANALOG;
			}
			else
			{
				if (i < 14)      remap[i].type = REMAP_ANALOG_DIGITAL;
				else if (i < 16) remap[i].type = REMAP_ANALOG_TRIGGER;
				else             remap[i].type = REMAP_ANALOG_ANALOG;
			}
		}
	}
}

static
void save_remap(char *secbuf, struct remap *remap)
{
	wsprintfA(secbuf, "Remap=");
	secbuf = next_str(secbuf) - 1;

	for (int i = 0; i < 24; i++, secbuf += 3)
	{
		wsprintfA(secbuf, "%02d ", remap[i].control);
	}

	*(secbuf-1) = '\0';
}

static
void swap_remap(struct remap *remap1, struct remap *remap2)
{
	struct remap tmp = *remap1;
	*remap1 = *remap2;
	*remap2 = tmp;
}

static
void set_stick_consts(struct stick_settings *stick)
{
	if (stick->linearity < 0) stick->exp  = 1.0f / (-stick->linearity + 1);
	else                      stick->exp  = stick->linearity + 1.0f;

	stick->deadzone_k = ANALOG_MAX / (ANALOG_MAX - stick->deadzone * ANALOG_MAX);
	stick->antideadzone_k = (ANALOG_MAX - stick->antideadzone) / ANALOG_MAX;
}

static
void set_trigger_consts(struct trigger_settings *trigger)
{
	trigger->deadzone = (float)trigger->min;
	trigger->range    = (float)(trigger->max > trigger->min ? trigger->max - trigger->min : 0xFF);
}

static
void toggle_inverted(struct settings *set)
{
	if (set->stick_l.inverted_x) swap_remap(&set->remap[CTRL_THUMB_LX_P], &set->remap[CTRL_THUMB_LX_N]);
	if (set->stick_l.inverted_y) swap_remap(&set->remap[CTRL_THUMB_LY_P], &set->remap[CTRL_THUMB_LY_N]);
	if (set->stick_r.inverted_x) swap_remap(&set->remap[CTRL_THUMB_RX_P], &set->remap[CTRL_THUMB_RX_N]);
	if (set->stick_r.inverted_y) swap_remap(&set->remap[CTRL_THUMB_RY_P], &set->remap[CTRL_THUMB_RY_N]);
}

static
void ini_load(void)
{
	if (GetPrivateProfileIntA("General", "INIVersion", 0, INI_FILENAME) != INI_VERSION)
	{
		// clear the file or something
		
	}

	GetPrivateProfileStringA("General", "LoadDLL", "", custom_dll, MAX_PATH, INI_FILENAME);

	for (int port = 0; port < 4; port++)
	{
		struct settings *set = &settings[port];
		wsprintfA(fileiobuf, "Controller%d", port);

		set->index    = GetPrivateProfileIntA(fileiobuf, "Port", port, INI_FILENAME) % 4;
		set->disabled = GetPrivateProfileIntA(fileiobuf, "Disable", 0, INI_FILENAME) == 1;
		set->dummy    = GetPrivateProfileIntA(fileiobuf, "Dummy",   0, INI_FILENAME) == 1;

		set->rumble_str = GetPrivateProfileIntA(fileiobuf, "Rumble", 1000, INI_FILENAME) / INI_FFACTOR;

		// make linear dz, adz per-stick setting
		set->stick_l.deadzone_linear     = GetPrivateProfileIntA(fileiobuf, "L_LinearDZ",     0, INI_FILENAME) == 1;
		set->stick_l.antideadzone_linear = GetPrivateProfileIntA(fileiobuf, "L_LinearADZ",    0, INI_FILENAME) == 1;
		set->stick_l.inverted_x          = GetPrivateProfileIntA(fileiobuf, "L_Inverted_X",   0, INI_FILENAME) == 1;
		set->stick_l.inverted_y          = GetPrivateProfileIntA(fileiobuf, "L_Inverted_Y",   0, INI_FILENAME) == 1;
		set->stick_l.linearity           =(GetPrivateProfileIntA(fileiobuf, "L_Linearity",  300, INI_FILENAME) - 300) / 100.0f;
		set->stick_l.deadzone            = GetPrivateProfileIntA(fileiobuf, "L_Deadzone",     0, INI_FILENAME) / INI_FFACTOR;
		set->stick_l.antideadzone        = GetPrivateProfileIntA(fileiobuf, "L_AntiDeadzone", 0, INI_FILENAME) / INI_FFACTOR;

		set->stick_r.deadzone_linear     = GetPrivateProfileIntA(fileiobuf, "R_LinearDZ",     0, INI_FILENAME) == 1;
		set->stick_r.antideadzone_linear = GetPrivateProfileIntA(fileiobuf, "R_LinearADZ",    0, INI_FILENAME) == 1;
		set->stick_r.inverted_x          = GetPrivateProfileIntA(fileiobuf, "R_Inverted_X",   0, INI_FILENAME) == 1;
		set->stick_r.inverted_y          = GetPrivateProfileIntA(fileiobuf, "R_Inverted_Y",   0, INI_FILENAME) == 1;
		set->stick_r.linearity           =(GetPrivateProfileIntA(fileiobuf, "R_Linearity",  300, INI_FILENAME) - 300) / 100.0f;
		set->stick_r.deadzone            = GetPrivateProfileIntA(fileiobuf, "R_Deadzone",     0, INI_FILENAME) / INI_FFACTOR;
		set->stick_r.antideadzone        = GetPrivateProfileIntA(fileiobuf, "R_AntiDeadzone", 0, INI_FILENAME) / INI_FFACTOR;

		set->trigger_l.min               = GetPrivateProfileIntA(fileiobuf, "L_TriggerMin",   0, INI_FILENAME) & 0xFF;
		set->trigger_l.max               = GetPrivateProfileIntA(fileiobuf, "L_TriggerMax", 255, INI_FILENAME) & 0xFF;
		set->trigger_r.min               = GetPrivateProfileIntA(fileiobuf, "R_TriggerMin",   0, INI_FILENAME) & 0xFF;
		set->trigger_r.max               = GetPrivateProfileIntA(fileiobuf, "R_TriggerMax", 255, INI_FILENAME) & 0xFF;

		set_stick_consts(&set->stick_l);
		set_stick_consts(&set->stick_r);
		
		set_trigger_consts(&set->trigger_l);
		set_trigger_consts(&set->trigger_r);

		load_remap(fileiobuf, set->remap);
		toggle_inverted(set);
	}
}

static
void ini_save(void)
{
	char *secbuf = fileiobuf;

	wsprintfA(secbuf, "INIversion=%d", INI_VERSION); secbuf = next_str(secbuf);
	wsprintfA(secbuf, "LoadDLL=%s", custom_dll);     secbuf = next_str(secbuf);
	*secbuf = '\0';

	WritePrivateProfileSectionA("General", fileiobuf, INI_FILENAME);

	for (int port = 0; port < 4; port++)
	{
		struct settings *set = &settings[port];
		
		char *section = fileiobuf;
		wsprintfA(section, "Controller%d", port); 
		char *secdata = next_str(section);
		
		secbuf = secdata;
		wsprintfA(secbuf, "Port=%d",    set->index);    secbuf = next_str(secbuf);
		wsprintfA(secbuf, "Disable=%d", set->disabled); secbuf = next_str(secbuf);
		wsprintfA(secbuf, "Dummy=%d",   set->dummy);    secbuf = next_str(secbuf);
		
		wsprintfA(secbuf, "Rumble=%d", (int)(set->rumble_str * INI_FFACTOR)); secbuf = next_str(secbuf);

		wsprintfA(secbuf, "L_LinearDZ=%d",           set->stick_l.deadzone_linear);             secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_LinearADZ=%d",          set->stick_l.antideadzone_linear);         secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_Inverted_X=%d",         set->stick_l.inverted_x);                  secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_Inverted_Y=%d",         set->stick_l.inverted_y);                  secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_Linearity=%d",    (int)(set->stick_l.linearity * 100.0f) + 300);   secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_Deadzone=%d",     (int)(set->stick_l.deadzone * INI_FFACTOR));     secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_AntiDeadzone=%d", (int)(set->stick_l.antideadzone * INI_FFACTOR)); secbuf = next_str(secbuf);

		wsprintfA(secbuf, "R_LinearDZ=%d",           set->stick_r.deadzone_linear);             secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_LinearADZ=%d",          set->stick_r.antideadzone_linear);         secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_Inverted_X=%d",         set->stick_r.inverted_x);                  secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_Inverted_Y=%d",         set->stick_r.inverted_y);                  secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_Linearity=%d",    (int)(set->stick_r.linearity * 100.0f) + 300);   secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_Deadzone=%d",     (int)(set->stick_r.deadzone * INI_FFACTOR));     secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_AntiDeadzone=%d", (int)(set->stick_r.antideadzone * INI_FFACTOR)); secbuf = next_str(secbuf);

		wsprintfA(secbuf, "L_TriggerMin=%d", set->trigger_l.min); secbuf = next_str(secbuf);
		wsprintfA(secbuf, "L_TriggerMax=%d", set->trigger_l.max); secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_TriggerMin=%d", set->trigger_r.min); secbuf = next_str(secbuf);
		wsprintfA(secbuf, "R_TriggerMax=%d", set->trigger_r.max); secbuf = next_str(secbuf);

		toggle_inverted(set);
		save_remap(secbuf, set->remap);
		toggle_inverted(set);

		WritePrivateProfileSectionA(section, secdata, INI_FILENAME);
	}
}

void INI_ReloadSettings(void)
{
	ini_load();
	ini_save();
}