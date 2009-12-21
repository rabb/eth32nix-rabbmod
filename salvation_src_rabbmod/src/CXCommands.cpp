// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

// kobject: the idea is to put our own console commands in here, and
// leave hooked engine/cgame commands in CEngine


// Member functions are from Mars, ordinary functions are from Venus.

#include "eth32.h"
#include "settings.h"

#ifdef ETH32_PRIV
	#include "../priv/priv.h"
#endif


CXCommands CCommands;
extern void *etpro_A;
extern void *etpro_B;
extern void *etpro_C;
extern void *etpro_E;
extern uint32 etpro_win32;
extern uint32 etpro_linux;

void xcmd_Namesteal()
{
	static int old = eth32.settings.NamestealDelay;
	eth32.settings.NamestealDelay = 0;
	Syscall.UI_ExecuteText("eth_set namesteal on; wait 100; eth_set namesteal off\n");
	eth32.settings.NamestealDelay = old;
}

void loadCustomHitbox()
{
	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^dSyntax: ^o/command ^2[^9list^2/^9stand^2] ^1x^9,^1y^9,^1z\n");
		return;
	}

	char *cmd = strdup(Syscall.UI_Argv(1));
	char *newValue;
	vec3_t vec;

	if (!strcmp(cmd,"list")){
		hitbox_t *hbox;
		if( eth32.settings.hitboxType == HITBOX_CUSTOM )
			hbox = &Aimbot.customHitbox;
		else
			hbox = &head_hitboxes[eth32.settings.hitboxType];

		Syscall.UI_Print("^dstand          ^9{^2% .1f, % .1f, % .1f^9}\n",
			hbox->stand_offset[0], hbox->stand_offset[1], hbox->stand_offset[2]);
		Syscall.UI_Print("^drun            ^9{^2% .1f, % .1f, % .1f^9}\n",
			hbox->stand_offset_moving[0], hbox->stand_offset_moving[1], hbox->stand_offset_moving[2]);
		Syscall.UI_Print("^dcrouch         ^9{^2% .1f, % .1f, % .1f^9}\n",
			hbox->crouch_offset[0], hbox->crouch_offset[1], hbox->crouch_offset[2]);
		Syscall.UI_Print("^dcrawl          ^9{^2% .1f, % .1f, % .1f^9}\n",
			hbox->crouch_offset_moving[0], hbox->crouch_offset_moving[1], hbox->crouch_offset_moving[2]);
		Syscall.UI_Print("^dprone          ^9{^2% .1f, % .1f, % .1f^9}\n",
			hbox->prone_offset[0], hbox->prone_offset[1], hbox->prone_offset[2]);
		Syscall.UI_Print("^dsize           ^9{^2% .1f, % .1f, % .1f^9}\n",
			hbox->size[0], hbox->size[1], hbox->size[2]);

		free(cmd);
		return;
	}

	if (Syscall.UI_Argc() != 3 ){
		Syscall.UI_Print("^1Error: ^oinsufficient arguments\n");
		free(cmd);
		return;
	} else
		newValue = strdup(Syscall.UI_Argv(2));

	VectorCopy(vec3_origin, vec);
	sscanf(newValue,"%f,%f,%f", &vec[0], &vec[1], &vec[2]);

	if (!strcmp(cmd, "stand")) {
		VectorCopy(vec,Aimbot.customHitbox.stand_offset);
	} else if (!strcmp(cmd, "run")) {
		VectorCopy(vec,Aimbot.customHitbox.stand_offset_moving);
	} else if (!strcmp(cmd, "crouch")) {
		VectorCopy(vec,Aimbot.customHitbox.crouch_offset);
	} else if (!strcmp(cmd, "crawl")) {
		VectorCopy(vec,Aimbot.customHitbox.crouch_offset_moving);
	} else if (!strcmp(cmd, "prone")) {
		VectorCopy(vec,Aimbot.customHitbox.prone_offset);
	} else if (!strcmp(cmd, "size")) {
		VectorCopy(vec,Aimbot.customHitbox.size);
	} else
		Syscall.UI_Print("^1Error: ^ounknown command\n");

	free(cmd);
	free(newValue);
}

void xcmd_RandName()
{
	if (!Tools.nNames) {
		Syscall.UI_Print("^3No player names loaded\n");
		return;
	}

	int n = (int)(random()*Tools.nNames);
	Syscall.UI_ExecuteText("set name \"%s\"\n", Tools.nameList[n]);
}

void xcmd_RandGuid()
{
	char buf[1024];
	sprintf(buf, "%08X%08X%08X%08X%08X",
		(uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF));
	strncpy(eth32.settings.etproGuid, buf, 1024);
	Syscall.UI_Print("^dNew etpro guid: ^7%s\n^dSetting won't take effect until next map (or do /reconnect)!\n", eth32.settings.etproGuid);
}

void xcmd_RandMac()
{
	char buf[16];
	sprintf(buf, "%02x-%02x-%02x-%02x",  rand() % 256,  rand() % 256,  rand() % 256,  rand() % 256);
	strncpy(eth32.settings.jayMac, buf, 16);
	Syscall.UI_Print("^dNew Jaymac: ^7%s\n^dSetting won't take effect until next map (or do /reconnect)!\n", eth32.settings.jayMac);
}

void xcmd_unbanCommand()
{
	system("rm -rf ~/.etwolf/etmain/etkey");
	Syscall.UI_ExecuteText("pb_cdkeyreg; wait 50\n");
	Syscall.UI_ExecuteText("eth_randguid; eth_randmac; wait 50; eth_randname; reconnect\n");
}

void xcmd_HackSpam()
{	// sol: added simple spam, feel free to modify however you like
	Syscall.UI_ExecuteText("say \"^0[^2ETH32^3nix ^0- ^5SALVATION ^0- ^1rabbmod^0]\n\"\n");
}

void goHome()
{
	Syscall.UI_ExecuteText("connect 213.163.68.37:27960\n");
}

void formatSetting(char *out, size_t o_s, const settingdef_t *setting)
{
	switch (setting->type) {
		case SETTING_INT:
			sprintf(out, "^9%i", *(int *)setting->target);
			break;
		case SETTING_FLOAT:
			sprintf(out, "^9%.3f", *(float *)setting->target);
			break;
		case SETTING_BOOL:
			(*(bool *)setting->target) ? strcpy(out, "^2true") : strcpy(out, "^1false");
			break;
		case SETTING_VEC3:
		{
			float *v = (float*)setting->target;
			sprintf(out, "^2{^9%.3f^2, ^9%.3f^2, ^9%.3f^2}", v[0], v[1], v[2]);
			break;
		}
		case SETTING_VEC4:
		{
			float *v = (float*)setting->target;
			sprintf(out, "^2{^9%.3f^2, ^9%.3f^2, ^9%.3f^2, ^9%.3f^2}", v[0], v[1], v[2], v[3]);
			break;
		}
		case SETTING_BYTE3:
		{
			char *b = (char*)setting->target;
			sprintf(out, "^9%3i %3i %3i", b[0], b[1], b[2]);
			break;
		}
		case SETTING_STRING:
		{
			sprintf(out, "^o%s", (char *)setting->target);
			break;
		}
		default:
			strcpy(out, "^3unknown type");
			break;
	}
}

// handles main settings only for now
void xcmd_Settings()
{
	Settings.ConsoleCommand();
}

void xcmd_Toggle()
{
	Settings.Toggle();
}

void xcmd_CvarForce()
{
	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^dCvar Force List\n");
		cvarInfo_t *c = Tools.userCvarList;
		int i = 0;
		while (c) {
			Syscall.UI_Print("^3%03i  ^2%16s   ^9\"^o%s^9\"\n",i, c->name, c->ourValue);
			c = c->next;
			i++;
		}

		return;
	} else if (Syscall.UI_Argc() == 2){
		const char *s = Syscall.UI_Argv(1);
		cvarInfo_t *c = Tools.userCvarList;
		while (c) {
			if (!strcasecmp(s, c->name))
				Syscall.UI_Print("^2%16s   ^9\"^o%s^9\"\n", c->name, c->ourValue);
			c = c->next;
		}
		return;
	}

	char *cvarName = strdup(Syscall.UI_Argv(1));
	char *val = strdup(Syscall.UI_Argv(2));

	// delete a cvar (( FIX the 0th cvar)
	if (!strcasecmp(cvarName, "del")){
		int num = atoi(val);
		if (num > 0){
			if (Tools.userCvarList[num].next)
				Tools.userCvarList[num-1].next = &Tools.userCvarList[num+1];
			else
				Tools.userCvarList[num-1].next = NULL;

			Syscall.UI_Print("^dDeleted ^2%s\n", Tools.userCvarList[num].name);
		}
	}

	// see if we should update an already present cvar
	bool found = false;
	cvarInfo_t *c = Tools.userCvarList;
	while (c) {
		if (!strcasecmp(cvarName, c->name)){
			strncpy(c->ourValue, val, MAX_STRING_CHARS);
			Syscall.UI_Print("^dUpdated:\n^2%16s   ^9\"^o%s^9\"\n", c->name, c->ourValue);
			found = true;
			break;
		}
		c = c->next;
	}

	if (!found && strcasecmp(cvarName, "del")){
		Syscall.UI_Print("^dAdded new entry:\n^2%16s   ^9\"^o%s^9\"\n", cvarName, val);
		Tools.AddUserCvar(cvarName, val);
	}

	Tools.SaveUserCvars();

	free(val);
	free(cvarName);
}

void xcmd_specstats()
{
	if (!eth32.settings.getSpeclist) {
		Syscall.UI_Print("^dspeclisting not activated\n");
		return;
	}

	if (!eth32.cg.hooked){
		Syscall.UI_Print("^dNot connected\n");
		return;
	}

	int bs, rq;
	char server[64];
	Spectator.GetStats(&bs, &rq, server);
	Syscall.UI_Print("^dServer: ^2%s ^dBytes rec: ^2%i ^dReq: ^2%i\n", bs, rq, server);
}

void xcmd_AimKeyDown()
{
	Aimbot.SetAimkeyStatus(true);
}

void xcmd_AimKeyUp()
{
	Aimbot.SetAimkeyStatus(false);
}

void setEtproOS()
{
	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^2%s", eth32.settings.etproOs ? "win32" : "linux");
		return;
	}

	char *cmd = strdup(Syscall.UI_Argv(1));

	if (!strcasecmp(cmd, "win"))
		eth32.settings.etproOs = true;
	else
		eth32.settings.etproOs = false;

	Syscall.UI_Print("^2new etpro OS set to %s", eth32.settings.etproOs ? "win32" : "linux");
	free(cmd);

	if (eth32.cgMod->type == MOD_ETPRO)
		orig_etproAntiCheat(etpro_A, etpro_B, etpro_C, eth32.settings.etproOs ? etpro_win32 : etpro_linux, etpro_E, eth32.settings.etproGuid);
}

void xcmd_killspam()
{
	if (Syscall.UI_Argc() == 1){
		if (Tools.TotalKillspam() == 0) {
			Syscall.UI_Print("^dno kill spam loaded\n");
			return;
		}
		Syscall.UI_Print("^2%i killspam definitions loaded\n", Tools.TotalKillspam());
		Syscall.UI_Print("^9--------------------------------\n");
		for (int i=0; i<Tools.TotalKillspam(); i++)
			Syscall.UI_Print("^3%0.2i    %s\n", i, Tools.GetKillspam(i));

		return;
	}

	if (!strcmp(Syscall.UI_Argv(1), "clear")) {
		Tools.ClearKillspam();
		return;
	}

	Tools.AddKillspam((char*)Syscall.UI_Argv(1));
	Syscall.UI_Print("^2added %s\n", Syscall.UI_Argv(1));
}

void xcmd_multikillspam()
{
	if (Syscall.UI_Argc() == 1){
		if (Tools.TotalMultikillspam() == 0) {
			Syscall.UI_Print("^dno multikill spam loaded\n");
			return;
		}
		Syscall.UI_Print("^2%i multikillspam definitions loaded\n", Tools.TotalMultikillspam());
		Syscall.UI_Print("^9--------------------------------\n");
		for (int i=0; i<Tools.TotalMultikillspam(); i++)
			Syscall.UI_Print("^3%0.2i    %s\n", i, Tools.GetMultikillspam(i));

		return;
	}

	if (!strcmp(Syscall.UI_Argv(1), "clear")) {
		Tools.ClearMultikillspam();
		return;
	}

	Tools.AddMultikillspam((char*)Syscall.UI_Argv(1));
	Syscall.UI_Print("^2added %s\n", Syscall.UI_Argv(1));
}

void xcmd_selfkillspam()
{
	if (Syscall.UI_Argc() == 1){
		if (Tools.TotalSelfkillspam() == 0) {
			Syscall.UI_Print("^dno kill spam loaded\n");
			return;
		}
		Syscall.UI_Print("^2%i killspam definitions loaded\n", Tools.TotalSelfkillspam());
		Syscall.UI_Print("^9--------------------------------\n");
		for (int i=0; i<Tools.TotalSelfkillspam(); i++)
			Syscall.UI_Print("^3%0.2i    %s\n", i, Tools.GetSelfkillspam(i));

		return;
	}

	if (!strcmp(Syscall.UI_Argv(1), "clear")) {
		Tools.ClearSelfkillspam();
		return;
	}

	Tools.AddSelfkillspam((char*)Syscall.UI_Argv(1));
	Syscall.UI_Print("^2added %s\n", Syscall.UI_Argv(1));
}

void xcmd_suicidespam()
{
	if (Syscall.UI_Argc() == 1){
		if (Tools.TotalSuicidespam() == 0) {
			Syscall.UI_Print("^dno suicide spam loaded\n");
			return;
		}
		Syscall.UI_Print("^2%i suicide spam definitions loaded\n", Tools.TotalSuicidespam());
		Syscall.UI_Print("^9--------------------------------\n");
		for (int i=0; i<Tools.TotalSuicidespam(); i++)
			Syscall.UI_Print("^3%0.2i    %s\n", i, Tools.GetSuicidespam(i));

		return;
	}

	if (!strcmp(Syscall.UI_Argv(1), "clear")) {
		Tools.ClearSuicidespam();
		return;
	}

	Tools.AddSuicidespam((char*)Syscall.UI_Argv(1));
	Syscall.UI_Print("^2added %s\n", Syscall.UI_Argv(1));
}



void xcmd_deathspam()
{
	if (Syscall.UI_Argc() == 1){
		if (Tools.TotalDeathspam() == 0) {
			Syscall.UI_Print("^dno death spam loaded\n");
			return;
		}
		Syscall.UI_Print("^2%i deathspam definitions loaded\n", Tools.TotalDeathspam());
		Syscall.UI_Print("^9--------------------------------\n");
		for (int i=0; i<Tools.TotalDeathspam(); i++)
			Syscall.UI_Print("^3%0.2i    %s\n", i, Tools.GetDeathspam(i));

		return;
	}

	if (!strcmp(Syscall.UI_Argv(1), "clear")) {
		Tools.ClearDeathspam();
		return;
	}

	Tools.AddDeathspam((char*)Syscall.UI_Argv(1));
	Syscall.UI_Print("^2added %s\n", Syscall.UI_Argv(1));
}

void xcmd_loadsettings()
{
	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^2must specify a file\n");
		return;
	}

	Settings.Load(Syscall.UI_Argv(1));
	Syscall.UI_Print("^2done.\n");
}

void xcmd_savesettings()
{
	if (Syscall.UI_Argc() == 1){
		Syscall.UI_Print("^2must specify a file\n");
		return;
	}

	char file[256];
	sprintf(file, "%s/%s", eth32.path, Syscall.UI_Argv(1));

	if (Settings.SaveUserSettings((const char*)file))
		Syscall.UI_Print("^2done.\n");
	else
		Syscall.UI_Print("^1error while saving to %s\n", Syscall.UI_Argv(1));
}

CXCommands::CXCommands(void)
{
	cmdsRegistered = false;
}

void CXCommands::RegisterCommands()
{
	if (cmdsRegistered || !orig_Cmd_AddCommand)
		return;

	orig_Cmd_AddCommand("eth_vec", &loadCustomHitbox);
	orig_Cmd_AddCommand("eth_home", &goHome);
	orig_Cmd_AddCommand("eth_hackspam", &xcmd_HackSpam);
	orig_Cmd_AddCommand("eth_namesteal", &xcmd_Namesteal);
	orig_Cmd_AddCommand("eth_randname", &xcmd_RandName);
	orig_Cmd_AddCommand("eth_cvar", &xcmd_CvarForce);
	orig_Cmd_AddCommand(va("%s",eth32.settings.cvarPrefix), &xcmd_Settings);
	orig_Cmd_AddCommand("eth_toggle", &xcmd_Toggle);
	orig_Cmd_AddCommand("eth_unban", &xcmd_unbanCommand);
	orig_Cmd_AddCommand("eth_randguid", &xcmd_RandGuid);
	orig_Cmd_AddCommand("eth_randmac", &xcmd_RandMac);
	orig_Cmd_AddCommand("eth_load", &xcmd_loadsettings);
	orig_Cmd_AddCommand("eth_save", &xcmd_savesettings);

	// spamz
	orig_Cmd_AddCommand("eth_killspam", &xcmd_killspam);
	orig_Cmd_AddCommand("eth_selfkillspam", &xcmd_selfkillspam);
	orig_Cmd_AddCommand("eth_suicidespam", &xcmd_suicidespam);
	orig_Cmd_AddCommand("eth_deathspam", &xcmd_deathspam);
	orig_Cmd_AddCommand("eth_multikillspam", &xcmd_multikillspam);

	orig_Cmd_AddCommand("eth_specstats", &xcmd_specstats);

	orig_Cmd_AddCommand("+aim", &xcmd_AimKeyDown);
	orig_Cmd_AddCommand("-aim", &xcmd_AimKeyUp);
	orig_Cmd_AddCommand("eth_etproos", &setEtproOS);

	#ifdef ETH32_PRIV
		RegisterPrivCommands();
	#endif


	cmdsRegistered = true;
}
