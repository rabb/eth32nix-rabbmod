// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"

CSettings Settings;

void CSettings::Load(const char *filename)
{
	int numSettings = sizeof(settingsDefs) / sizeof(settingsDefs[0]);
	const settingdef_t *setting;
	char value[1024];

	for (int i=0 ; i<numSettings ; i++)
	{
		setting = &settingsDefs[i];

		Tools.GetPrivateProfileString("Main Settings", setting->name, setting->defaultValue, value, sizeof(value), (char *)filename);

#ifdef ETH32_DEBUG
		Debug.Log("Load Setting: %s ( Value: %s )", setting->name, value);
#endif

		switch (setting->type)
		{
		case SETTING_INT:
			*(int*)(setting->target) = atoi(value);
			break;
		case SETTING_FLOAT:
			*(float*)(setting->target) = atof(value);
			break;
		case SETTING_BOOL:
			*(bool*)(setting->target) = (strcmp(value, "true") == 0);
			break;
		case SETTING_VEC3:
		{
			float *v = (float*)setting->target;
			sscanf(value, "%f %f %f", &v[0], &v[1], &v[2]);
			break;
		}
		case SETTING_VEC4:
		{
			float *v = (float*)setting->target;
			sscanf(value, "%f %f %f %f", &v[0], &v[1], &v[2], &v[3]);
			break;
		}
		case SETTING_BYTE3:
		{
			uchar *b = (uchar*)setting->target;
			int b0, b1, b2;
			sscanf(value, "%i %i %i", &b0, &b1, &b2);
			b[0] = (uchar)b0;
			b[1] = (uchar)b1;
			b[2] = (uchar)b2;
			break;
		}
		case SETTING_STRING:
		{
			strncpy((char *)setting->target, value, MAX_STRING_CHARS);
			break;
		}
		default:
#ifdef ETH32_DEBUG
			Debug.Log("Unable to load setting \"%s\", type %i unknown", setting->name, setting->type);
#endif
			continue;
		}
	}

}

void CSettings::Save(const char *filename)
{
	int numSettings = sizeof(settingsDefs) / sizeof(settingsDefs[0]);
	const settingdef_t *setting;
	char value[MAX_STRING_CHARS];

	//for (int i=0 ; i<numSettings ; i++)
	for (int i=numSettings-1 ; i>=0 ; i--) // Inverted the order so the settings.ini values appear in the same order as the settings.h file.
	{
		setting = &settingsDefs[i];

		switch (setting->type)
		{
		case SETTING_INT:
			sprintf(value, "%i", *(int*)setting->target);
			break;
		case SETTING_FLOAT:
			sprintf(value, "%.3f", *(float*)setting->target);
			break;
		case SETTING_BOOL:
			sprintf(value, "%s", *(bool*)setting->target ? "true" : "false");
			break;
		case SETTING_VEC3:
		{
			float *v = (float*)setting->target;
			sprintf(value, "%.3f %.3f %.3f", v[0], v[1], v[2]);
			break;
		}
		case SETTING_VEC4:
		{
			float *v = (float*)setting->target;
			sprintf(value, "%.3f %.3f %.3f %.3f", v[0], v[1], v[2], v[3]);
			break;
		}
		case SETTING_BYTE3:
		{
			uchar *b = (uchar*)setting->target;
			sprintf(value, "%i %i %i", b[0], b[1], b[2]);
			break;
		}
		case SETTING_STRING: // using pointer to pointer
		{
			strncpy(value, (char *)setting->target, MAX_STRING_CHARS);
			break;
		}
		default:
#ifdef ETH32_DEBUG
			Debug.Log("Unable to save setting \"%s\", type %i unknown.", setting->name, setting->type);
#endif
			continue;
		}

		Tools.WritePrivateProfileString("Main Settings", setting->name, value, (char *)filename);
	}
}

bool CSettings::symbolicDiscription(char *buf, const settingdef_t *setting)
{
	if (!setting->values[0].text)
		return false;

	for(int j=0; j<MAX_SETTING_VALUES; j++) {
		if (setting->values[j].text){
			switch (setting->type){
				case SETTING_INT:
					if (*(int*)setting->target == (int)setting->values[j].value){
						sprintf(buf, "%s", setting->values[j].text);
						return true;
					}
					break;
				case SETTING_FLOAT:
					if (*(float*)setting->target == setting->values[j].value){
						sprintf(buf, "%s", setting->values[j].text);
						return true;
					}
					break;
				default:
					return false;
			}
		} else
			break;
	}
	return false;
}

// saves user settings from settings_t in format that can be loaded in a cfg
bool CSettings::SaveUserSettings(const char *filename)
{
	int numSettings = sizeof(settingsDefs) / sizeof(settingsDefs[0]);
	const settingdef_t *setting;
	char value[MAX_STRING_CHARS];

	FILE *fp = fopen(filename, "w");
	if (!fp)
		return false;

	for (int i=0 ; i<numSettings ; i++)
	{
		setting = &settingsDefs[i];

		switch (setting->type)
		{
		case SETTING_INT:
			if (!symbolicDiscription(value, setting))
				sprintf(value, "%i", *(int*)setting->target);
			break;
		case SETTING_FLOAT:
			if (!symbolicDiscription(value, setting))
				sprintf(value, "%.3f", *(float*)setting->target);
			break;
		case SETTING_BOOL:
			sprintf(value, "%s", *(bool*)setting->target ? "on" : "off");
			break;
		case SETTING_VEC3:
		{
			float *v = (float*)setting->target;
			sprintf(value, "%.3f %.3f %.3f", v[0], v[1], v[2]);
			break;
		}
		case SETTING_VEC4:
		{
			float *v = (float*)setting->target;
			sprintf(value, "%.3f %.3f %.3f %.3f", v[0], v[1], v[2], v[3]);
			break;
		}
		case SETTING_BYTE3:
		{
			uchar *b = (uchar*)setting->target;
			sprintf(value, "%i %i %i", b[0], b[1], b[2]);
			break;
		}
		case SETTING_STRING: // using pointer to pointer
		{
			sprintf(value, "\"%s\"", (char *)setting->target);
			break;
		}
		default:
#ifdef ETH32_DEBUG
			Debug.Log("Unable to save setting \"%s\", type %i unknown.", setting->name, setting->type);
#endif
			continue;
		}

		fprintf(fp, "eth_set %s %s\n", setting->name, value);
	}
	fclose(fp);
	return true;
}

void CSettings::ConsoleCommand(void)
{
	int numSettings = sizeof(settingsDefs) / sizeof(settingsDefs[0]);
	const settingdef_t *setting;

	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^dSyntax:  ^o/eth_set ^2[^9setting^9] ^2[^9value^2]\n\n");
		PrintAll();
		return;
	}

	const char *cv = Syscall.UI_Argv(1);

	if (Syscall.UI_Argc() == 2){
		for (int i=0 ; i<numSettings ; i++){
			setting = &settingsDefs[i];
			if (stristr(setting->name, cv)){
				PrintVarDescription(setting);
			}
		}
	} else if (Syscall.UI_Argc() > 2) {
		int found = 0;
		const settingdef_t *fs;
		for (int i=0 ; i<numSettings ; i++){
			setting = &settingsDefs[i];
			if (!strcmp(setting->name, cv)){
				fs = setting;
				found = 1;
				break;
			} else if (stristr(setting->name, cv)){
				fs = setting;
				found++;
			}
		}

		if (found == 1) {
			setting = fs;
			char value[1024];

			strcpy(value, Syscall.UI_Argv(2));
			int argnum = 3;
			while (argnum < Syscall.UI_Argc()) {
				strcat(value, " ");
				strcat(value, Syscall.UI_Argv(argnum));
				argnum++;
			}

			switch (setting->type) {
				case SETTING_INT:
				{
					if (setting->values[0].text) {
						for (int i=0 ; i<MAX_SETTING_VALUES && setting->values[i].text ; i++)
						{
							if (!strcasecmp(value, setting->values[i].text)) { // found a match
								*(int*)setting->target = (int)setting->values[i].value;
								Syscall.UI_Print("^oVariable ^d%s ^oupdated!\n", setting->name);
								return;
							}
						}
						// setting value was not found
						Syscall.UI_Print("^dValue '^2%s^d' is not allowed!\n", value);
						return;
					}
					else
						*(int*)(setting->target) = atoi(value);
					break;
				}
				case SETTING_FLOAT:
					*(float*)(setting->target) = atof(value);
					break;
				case SETTING_BOOL:
					*(bool*)(setting->target) = (strcasecmp(value, "on") == 0);
					break;
				case SETTING_VEC3:
				{
					float *v = (float*)setting->target;
					sscanf(value, "%f %f %f", &v[0], &v[1], &v[2]);
					break;
				}
				case SETTING_VEC4:
				{
					float *v = (float*)setting->target;
					sscanf(value, "%f %f %f %f", &v[0], &v[1], &v[2], &v[3]);
					break;
				}
				case SETTING_BYTE3:
				{
					uchar *b = (uchar*)setting->target;
					int b0, b1, b2;
					sscanf(value, "%i %i %i", &b0, &b1, &b2);
					b[0] = (uchar)b0;
					b[1] = (uchar)b1;
					b[2] = (uchar)b2;
					break;
				}
				case SETTING_STRING:
				{
					strncpy((char *)setting->target, value, MAX_STRING_CHARS);
					break;
				}
				default:
					break;
			}
			char newVal[MAX_STRING_CHARS];
			PrintValue(setting, newVal);
			Syscall.UI_Print("^oVariable ^d%s ^oupdated! new value: ^2%s\n", setting->name, newVal);
		} else if (found > 1)
			Syscall.UI_Print("^dMore then 1 matches found for '^2%s^d'\n", cv);
		else
			Syscall.UI_Print("^dNo matches for '^2%s^d' were found\n", cv);
	}
}

void CSettings::Toggle(void)
{
	int numSettings = sizeof(settingsDefs) / sizeof(settingsDefs[0]);
	const settingdef_t *setting;

	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^dSyntax:  ^o/eth_toggle ^2[^9setting^9]\n");
		return;
	}

	const char *varname = Syscall.UI_Argv(1);

	for (int i=0 ; i<numSettings ; i++)
	{
		setting = &settingsDefs[i];

		if (!strcasecmp(varname, setting->name)) {
			char buffer[256];

			if (setting->type == SETTING_BOOL) {
				*(bool*)setting->target = !(*(bool*)setting->target);
				sprintf(buffer, "^otoggle: ^d%s ^ois now ^d%s", setting->name, (*(bool*)setting->target)?"on":"off");
				//Gui.CenterPrint(buffer, 1);
				return;
			}
			else if (setting->type == SETTING_INT && setting->values[0].text) {
				int cur = *(int*)setting->target;

				if (cur < (int)setting->max)
					cur++;
				 else
					cur = (int)setting->min;

				*(int*)setting->target = cur;

				sprintf(buffer, "^otoggle: ^d%s ^ois now ^d%s", setting->name, setting->values[cur-(int)setting->min].text);
				//Gui.CenterPrint(buffer, 1);
				return;
			}
			else {
				sprintf(buffer, "^d%s ^ocannot be toggled", setting->name);
				//Gui.CenterPrint(buffer, 1);
				return;
			}
		}
	}

	// variable not found
	Syscall.UI_Print("^1ERROR: ^d%s ^ois not defined\n", varname);
}

void CSettings::PrintAll(void)
{
	int numSettings = sizeof(settingsDefs) / sizeof(settingsDefs[0]);

	Syscall.UI_Print("^d%-20s   %s\n", "variable", "value");
	Syscall.UI_Print("^2%-20s   %s\n", "--------", "-----");

	bool altColor = false;
	for (int i=0 ; i<numSettings ; i++) {
		PrintVarLine(&settingsDefs[i], (altColor)?'o':'n');
		altColor = !altColor;
	}
}

void CSettings::PrintVarLine(const settingdef_t *var, char color)
{
	if (!var)
		return;

	switch (var->type)
	{
	case SETTING_INT:
	{
		int value = *(int*)var->target;
		if (var->values[0].text && (value >= (int)var->min) && (value <= (int)var->max))
			Syscall.UI_Print("^%c%-20s   %s\n", color, var->name, var->values[value].text);
		else
			Syscall.UI_Print("^%c%-20s   %i\n", color, var->name, value);
		break;
	}
	case SETTING_FLOAT:
	{
		float value = *(float*)var->target;
		Syscall.UI_Print("^%c%-20s   %.3f\n", color, var->name, value);
		break;
	}
	case SETTING_BOOL:
		Syscall.UI_Print("^%c%-20s   %s\n", color, var->name, *(bool*)var->target ? "on" : "off");
		break;
	case SETTING_VEC3:
	{
		float *v3 = (float*)var->target;
		Syscall.UI_Print("^%c%-20s   %.3f %.3f %.3f\n", color, var->name, v3[0], v3[1], v3[2]);
		break;
	}
	case SETTING_VEC4:
	{
		float *v4 = (float*)var->target;
		Syscall.UI_Print("^%c%-20s   %.3f %.3f %.3f %.3f\n", color, var->name, v4[0], v4[1], v4[2], v4[3]);
		break;
	}
	case SETTING_BYTE3:
	{
		uchar *b3 = (uchar*)var->target;
		Syscall.UI_Print("^%c%-20s   %i %i %i\n", color, var->name, b3[0], b3[1], b3[2]);
		break;
	}
	case SETTING_STRING:
		Syscall.UI_Print("^%c%-20s   %s\n", color, var->name, (char*)var->target);
		break;
	default:
		break;
	}
}

void CSettings::PrintVarDescription(const settingdef_t *var)
{
	if (!var)
		return;

	char args[1024];

	Syscall.UI_Print("\n^oVariable:\n");
	Syscall.UI_Print("  ^d%s\n", var->name);

	switch (var->type)
	{
	case SETTING_INT:
	{
		int value = *(int*)var->target;
		if (var->values[0].text && (value >= (int)var->min) && (value <= (int)var->max))
			strcpy(args, var->values[value].text);
		else
			sprintf(args, "%i", value);
		break;
	}
	case SETTING_FLOAT:
	{
		float value = *(float*)var->target;
		sprintf(args, "%.3f", value);
		break;
	}
	case SETTING_BOOL:
		strcpy(args, *(bool*)var->target ? "on" : "off");
		break;
	case SETTING_VEC3:
	{
		float *v3 = (float*)var->target;
		sprintf(args, "%0.3f %0.3f %0.3f", v3[0], v3[1], v3[2]);
		break;
	}
	case SETTING_VEC4:
	{
		float *v4 = (float*)var->target;
		sprintf(args, "%0.3f %0.3f %0.3f %0.3f", v4[0], v4[1], v4[2], v4[3]);
		break;
	}
	case SETTING_BYTE3:
	{
		uchar *b3 = (uchar*)var->target;
		sprintf(args, "%i %i %i", b3[0], b3[1], b3[2]);
		break;
	}
	case SETTING_STRING:
		sprintf(args, "\"%s\"", (char*)var->target);
		break;
	default:
		args[0] = 0;
		break;
	}

	Syscall.UI_Print("^oCurrent value:\n");
	Syscall.UI_Print("  ^9%s\n", args);

	switch (var->type)
	{
	case SETTING_INT:
		if (var->values[0].text) {
			char temparg[256];
			sprintf(temparg, "^9%s", var->values[0].text);
			strcpy(args, temparg);
			int valuenum = 1;
			while (var->values[valuenum].text) {
				sprintf(temparg, "^2|^9%s", var->values[valuenum].text);
				strcat(args, temparg);
				valuenum++;
			}
		}
		else
			sprintf(args, "^9%i^2-^9%i", (int)var->min, (int)var->max);

		break;
	case SETTING_FLOAT:
		sprintf(args, "^9%.3f^2-^9%.3f", var->min, var->max);
		break;
	case SETTING_BOOL:
		strcpy(args, "^9on^2|^9off");
		break;
	case SETTING_VEC3:
		strcpy(args, "^90.0^2-^91.0 ^90.0^2-^91.0 ^90.0^2-^91.0");
		break;
	case SETTING_VEC4:
		strcpy(args, "^90.0^2-^91.0 ^90.0^2-^91.0 ^90.0^2-^91.0 ^90.0^2-^91.0");
		break;
	case SETTING_BYTE3:
		strcpy(args, "^90^2-^9255 ^90^2-^9255 ^90^2-^9255");
		break;
	case SETTING_STRING:
		strcpy(args, "^9\"string\"");
		break;
	default:
		args[0] = 0;
		break;
	}

	Syscall.UI_Print("^oAllowed values:\n");
	Syscall.UI_Print("  ^2[%s^2]\n", args);

	Syscall.UI_Print("^oDescription:\n");
	Syscall.UI_Print("  ^9%s\n\n", var->description);
}

void CSettings::PrintValue(const settingdef_t *var, char *val)
{
	if (!var || !val)
		return;

	char args[1024];

	switch (var->type)
	{
	case SETTING_INT:
	{
		int value = *(int*)var->target;
		if (var->values[0].text && (value >= (int)var->min) && (value <= (int)var->max))
			strcpy(args, var->values[value].text);
		else
			sprintf(args, "%i", value);
		break;
	}
	case SETTING_FLOAT:
	{
		float value = *(float*)var->target;
		sprintf(args, "%.3f", value);
		break;
	}
	case SETTING_BOOL:
		strcpy(args, *(bool*)var->target ? "on" : "off");
		break;
	case SETTING_VEC3:
	{
		float *v3 = (float*)var->target;
		sprintf(args, "%0.3f %0.3f %0.3f", v3[0], v3[1], v3[2]);
		break;
	}
	case SETTING_VEC4:
	{
		float *v4 = (float*)var->target;
		sprintf(args, "%0.3f %0.3f %0.3f %0.3f", v4[0], v4[1], v4[2], v4[3]);
		break;
	}
	case SETTING_BYTE3:
	{
		uchar *b3 = (uchar*)var->target;
		sprintf(args, "%i %i %i", b3[0], b3[1], b3[2]);
		break;
	}
	case SETTING_STRING:
		sprintf(args, "\"%s\"", (char*)var->target);
		break;
	default:
		args[0] = 0;
		break;
	}

	strncpy(val, args, 1024);
}
