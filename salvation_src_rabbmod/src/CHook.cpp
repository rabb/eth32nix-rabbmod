// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

#include "eth32.h"
#include "offsets.h"
#include "weapdefs.h"
#include "cvars.h"
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <link.h>

#ifdef ETH32_PRIV
	#include "../priv/priv.h"
#endif

void *etpro_A, *etpro_B, *etpro_C, *etpro_E;
uint32 etpro_win32 = 0x30c3e725;
uint32 etpro_linux = 0x395789a0;

#define CDECL

dlopen_t orig_dlopen = NULL;
dlsym_t orig_dlsym = NULL;
dlclose_t orig_dlclose = NULL;


CHook Hook;
Cvar_Get_t				orig_Cvar_Get = NULL;
Cvar_Set2_t				orig_Cvar_Set2 = NULL;
Cvar_VariableString_t			orig_Cvar_VariableString = NULL;
syscall_t				orig_syscall = NULL;
vmMain_t				orig_CG_vmMain = NULL;
vmMain_t				orig_UI_vmMain = NULL;
CG_Player_t				orig_CG_Player = NULL;
CG_AddPlayerWeapon_t			orig_CG_AddPlayerWeapon = NULL;
CG_PositionRotatedEntityOnTag_t		orig_CG_PositionRotatedEntityOnTag = NULL;
CG_SetLerpFrameAnimationRate_t		orig_CG_SetLerpFrameAnimationRate = NULL;
CG_FinishWeaponChange_t			orig_CG_FinishWeaponChange = NULL;
dllEntry_t				orig_CG_dllEntry = NULL;
dllEntry_t				orig_UI_dllEntry = NULL;
FS_PureServerSetLoadedPaks_t		orig_FS_PureServerSetLoadedPaks = NULL;
FS_AddGameDirectory_t			orig_FS_AddGameDirectory = NULL;
FS_AddGameDirectory2_t			orig_FS_AddGameDirectory2 = NULL;
FS_Startup_t				orig_FS_Startup = NULL;
FS_Startup2_t				orig_FS_Startup2 = NULL;
Cmd_AddCommand_t			orig_Cmd_AddCommand = NULL;
Sys_LoadDll_t				orig_Sys_LoadDll = NULL;
CG_EntityEvent_t			orig_CG_EntityEvent = NULL;
CG_Trace_t				orig_CG_Trace = NULL;
CG_DamageFeedback_t			orig_CG_DamageFeedback = NULL;
CG_WeaponFireRecoil_t			orig_CG_WeaponFireRecoil = NULL;
CG_Respawn_t				orig_CG_Respawn = NULL;
CG_CalculateReinfTime_t			orig_CG_CalculateReinfTime = NULL;

// Standard UI functions we need to remove or mod
CG_DrawPlayerStatusHead_t		orig_CG_DrawPlayerStatusHead = NULL;
CG_DrawPlayerStats_t			orig_CG_DrawPlayerStats = NULL;
CG_DrawPlayerStatus_t			orig_CG_DrawPlayerStatus = NULL;
CG_ChargeTimesChanged_t			orig_CG_ChargeTimesChanged = NULL;
BG_FindClipForWeapon_t			orig_BG_FindClipForWeapon = NULL;
BG_FindAmmoForWeapon_t			orig_BG_FindAmmoForWeapon = NULL;
CG_AllocLocalEntity_t			orig_CG_AllocLocalEntity = NULL;
etproAntiCheat_t			orig_etproAntiCheat = NULL;
etproAntiCheat2_t			orig_etproAntiCheat2 = NULL;
CG_CalcViewValues_t			orig_CG_CalcViewValues = NULL;

LoadTGA_t orig_LoadTGA;

//extern void etpro_CatchVoiceChat(void);
extern void rd_pb_hooks();
uint32 savedESP;

char goodChecksumsPak[BIG_INFO_STRING];

int eth32_dlclose(void *handle)
{
	//Debug.Log("DlClose %x", handle);
	if (handle == eth32.cg.handle) {
#ifdef ETH32_DEBUG
		Debug.Log("FreeLibrary(cgame)");
#endif
		eth32.cgameLoaded = false;
		memset(&eth32.cg, 0, sizeof(cgame_t));
	} else if (handle == eth32.ui.handle) {
#ifdef ETH32_DEBUG
		Debug.Log("FreeLibrary(ui)");
#endif
		memset(&eth32.ui, 0, sizeof(ui_t));
		Hook.hookUi(false);
	}
	return orig_dlclose(handle);
}
#ifdef ETH32_PRELIM_OFFSETS
int section_search(void *needle, int n, char *section, const char *file)
{
	GElf_Shdr *tshdr = Detour.getSectionHeader((char*)file, section);
	if (!tshdr)
		return -1;

	char *sdata = (char *)malloc((uint32)tshdr->sh_size);

	FILE *fp = fopen(file, "r");
	fseek (fp, tshdr->sh_offset, SEEK_SET);
	fread(sdata, 1, (uint32)tshdr->sh_size, fp);

	fclose(fp);

	void *loc = memmem(sdata, (uint32)tshdr->sh_size, needle, n);

	if (!loc){
		free(sdata);
		return -1;
	}

	uint32 p;
	p = (uint32)loc - (uint32)sdata;
	p += (uint32)tshdr->sh_addr;
	free(sdata);
	return p;
}
#endif

void CHook::GetPredictedPlayerState()
{
	// snap is near the beginning of the cg struct, use it to bound it.
	if (!eth32.cg.snap)
		return;

	void *found = memmem((void*)eth32.cg.snap, 0x989680, &eth32.cg.snap->ps.origin, 0xa8);
	while(found) {
		found = memmem((void*)((uint32)found+1), 0x989680, &eth32.cg.snap->ps.origin, 0xa8);
		if (!found)
			return;
		//Debug.Log("Possible predictedPlayerState found: 0x%x\n", (uint32)found - 0x14 - (uint32)eth32.cg.module);
	}
}

void *eth32_dlopen(const char *filename, int flag) {
	//Debug.Log("dlopen called %s %i", filename, flag);
	if (!filename)
		return orig_dlopen(filename, flag);

	if (strstr(filename, "cgame.mp.i386.so")) {
#ifdef ETH32_DEBUG
		Debug.Log("Loading %s [crc32: 0x%x]", filename, Tools.crc32FromFile(filename));
#endif
		eth32.eth32mod = false;

		// this automatically gets the offset for cgame console commands
#ifdef ETH32_PRELIM_OFFSETS
		GElf_Shdr *tshdr;
		GElf_Shdr rodata_shdr;
		GElf_Shdr data_shdr;

		tshdr = Detour.getSectionHeader((char *)filename, ".rodata");
		memcpy(&rodata_shdr, tshdr, sizeof(rodata_shdr));

		FILE *fp = fopen(filename, "r");
		if (!fp)
			Debug.Log("couldnt open cgame :(");
		else {
			char *rodata = (char *)malloc((uint32)rodata_shdr.sh_size);
			fseek (fp, rodata_shdr.sh_offset, SEEK_SET);
			fread(rodata, 1, (uint32)rodata_shdr.sh_size, fp);
			fclose(fp);

			char str[] = "testgun";
			void *cmdstart = memmem(rodata, (uint32)rodata_shdr.sh_size, str, 8);

			if (!cmdstart) {
				Debug.Log("couldn't find 'testgun' in .rodata");
			} else {
				int loc;
				uint32 p = (uint32)cmdstart - (uint32)rodata;
				p += (uint32)rodata_shdr.sh_addr;

				loc = section_search(&p, 4, ".data", filename);
				if (loc < 0)
					loc = section_search(&p, 4, ".data.rel.ro", filename);

				if (loc < 0)
					Debug.Log("'testgun' found in rodata, but not command...");
				else
					Debug.Log("commands found at 0x%x", loc);
			}

			free(rodata);
		}
#endif
//void eth32_get_offsets(void *p, void *b)
		int supported = Hook.isCgameSupported((char *)filename);
		if (supported == -1){
			if (strstr(filename, "etace") || strstr(filename, "etpub") || strstr(filename, "etmain") || strstr(filename, "shrub") ) {
				sprintf((char*)filename, "%s/dll/cgame.mp.i386.so", eth32.path);
	#ifdef ETH32_DEBUG
				Debug.Log("cgame not supported, loading automod %s",filename);
	#endif
				void *result = orig_dlopen(filename, flag);
				if (!result) {
					orig_syscall(UI_ERROR, "Problem loading automod... check eth32nix/dll directory for cgame.mp.i386.so");
					return NULL;
				}

				typedef void (*offset_t)(void *p, void *b);
				offset_t eth32_get_offsets = (offset_t)orig_dlsym(result, "eth32_get_offsets");

				if (!eth32_get_offsets){
					orig_syscall(UI_ERROR, va("This mod isn't supported, check portal for latest news and updates regarding ETH32.\n\n\nhttp://eth32.nixcoders.org\n\nMod's crc32: [crc32: 0x%x]",Tools.crc32FromFile(filename)));
					return NULL;
				}
				eth32.eth32mod = true;
				memset(&eth32.cg, 0, sizeof(cgame_t));
				eth32.cg.handle = result;
				return result;
			} else {
				orig_syscall(UI_ERROR, va("This mod isn't supported, check portal for latest news and updates regarding ETH32.\n\n\nhttp://eth32.nixcoders.org\n\nMod's crc32: [crc32: 0x%x]",Tools.crc32FromFile(filename)));
				return NULL;
			}
		}
	}

	void *result = orig_dlopen(filename, flag);

	if (strcasestr(filename, "pbcl.so") && result){
		eth32.pb.loaded = true;
		strcpy(eth32.pb.file, filename);
		eth32.pb.handle = result;
		Hook.InitPB();
	}

	if (result) {
		if (strstr(filename, "cgame.mp.i386.so")) {
			memset(&eth32.cg, 0, sizeof(cgame_t));
			eth32.cg.handle = result;
			eth32.cg.supported = Hook.isCgameSupported((char *)filename);
			eth32.cgMod = &Hook.cglist[eth32.cg.supported];
		} else if (strstr(filename, "ui.mp.i386.so")) {
			eth32.ui.handle = result;

#ifdef ETH32_DEBUG
			Debug.Log("Loading ui [crc32: 0x%x] at 0x%x", Tools.crc32FromFile(filename), (uint32) eth32.ui.handle);
#endif

			Hook.hookUi(true);
			return result;
		}
	}

	return result;
}

typedef void (*glReadPixels_t)( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
glReadPixels_t orig_glReadPixels = NULL;

void eth32_glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
	// if the call came from ET (avidemo, /screenshotjpeg, etc...) allow it
	uint32 caller = (uint32)__builtin_return_address(0);
	if (caller > eth32.game.textSectionStart && caller < eth32.game.textSectionEnd){
		orig_glReadPixels(x,y,width,height,format,type,pixels);
		return;
	}

	if (eth32.cg.hooked)
		eth32.cg.pbss++;

	switch(eth32.settings.pbScreenShot) {
		case PB_SS_CLEAN:
		{
			typedef void (*SCR_UpdateScreen_t)(void);
			SCR_UpdateScreen_t SCR_UpdateScreen = (SCR_UpdateScreen_t)eth32.et->SCR_UpdateScreen;

			Hook.ToggleHack(false);

			// redraw a few times to flush all opengl buffers
			SCR_UpdateScreen();
			SCR_UpdateScreen();
			SCR_UpdateScreen();
			SCR_UpdateScreen();

			Hook.ToggleHack(true);

			orig_glReadPixels(x,y,width,height,format,type,pixels);
			return;
		}
		case PB_SS_NORMAL:
			break;
		case PB_SS_CUSTOM:
		{
			char picture[256];
			sprintf(picture, "%s/customss.ppm", eth32.path);

			//640x480 color PPM image for simplicity
			FILE *fp = fopen(picture,"rb");
			if( !fp ){
#ifdef ETH32_DEBUG
				Debug.Log("Couldn't open custom PB SS picture %s", picture);
#endif
				return;
			}

			int pos,r,w,c;
			int pW = 640;
			int pH = 480;
			unsigned char *pic = (unsigned char *) malloc(pH*pW*3);
			for( r=0; r<pH; r++){
				pos = pW*3*r;
				fseek(fp, pos-pW*pH*3, SEEK_END );
				fread(pic+pW*(pH-1)*3-pos,1,pW*3,fp);
			}
			fclose(fp);

			float X,Y,dx,dy,q11,q12,q21,q22;
			int x1,y1;
			float sx = (float) pW / (float) width;
			float sy = (float) pH / (float) height;
			unsigned char *line = (unsigned char *)malloc(width*3);
			for( r=0; r<height-1; r++ ){
				for( w=0;w<width-1;w++) {
					x1 =(int) w*sx;
					y1 =(int) r*sy;
					X = w*sx;
					Y = r*sy;
					dx = X-x1;
					dy = Y-y1;

					for( c=0; c<3; c++ ){
						q11 = (float) pic[y1*pW*3 + x1*3 + c];
						q12 = (float) pic[(y1+1)*pW*3 + x1*3 +c];
						q21 = (float) pic[y1*pW*3 + (x1+1)*3 + c];
						q22 = (float) pic[(y1+1)*pW*3 + (x1+1)*3+c];
						line[w*3+c] = (unsigned char) ((1-dx)*(1-dy)*q11+(1-dx)*dy*q12+dx*(1-dy)*q21+dx*dy*q22);
					}
				}
				memcpy( (void*)((uint32)pixels+r*width*3), line, width*3 );
			}
			free(line);
			free(pic);
		}
	}
}

void *eth32_dlsym(void *handle, const char *symbol) {
	//Debug.Log("dlsym called %x %s", handle, symbol);

	void *result = orig_dlsym(handle, symbol);

	if (handle == eth32.cg.handle) {
		if (!strcmp(symbol, "dllEntry")) {
			orig_CG_dllEntry = (dllEntry_t) result;
			return (void *)eth32_CG_dllEntry;
		} else if (!strcmp(symbol, "vmMain")) {
			orig_CG_vmMain = (vmMain_t) result;
			return (void *)eth32_CG_vmMain;
		}
	} else if (handle == eth32.ui.handle) {
		if (!strcmp(symbol, "dllEntry")) {
			orig_UI_dllEntry = (dllEntry_t) result;
			return (void *)eth32_UI_dllEntry;
		} else if (!strcmp(symbol, "vmMain")) {
			orig_UI_vmMain = (vmMain_t) result;
			return (void *)eth32_UI_vmMain;
		}
	}

	if( !strcmp(symbol,"glReadPixels") ){
		orig_glReadPixels = (glReadPixels_t) result;
		return (void *)eth32_glReadPixels;
	}

	return result;
}

// Hooked for direct keys/mouse access
int eth32_XNextEvent(Display *display, XEvent *event) {
	int result = XNextEvent(display, event);

	bool guiInputActive = Gui.InputActive();

	switch (event->type) {
		case KeyPress:
		{
			uint32 key = XLookupKeysym(&event->xkey, (event->xkey.state & 1));
			UpdateKeyTable(key, 1);
			// XK_Shift_L is the start of the modifier keys
			if (guiInputActive && (key < XK_Shift_L)){
				Gui.CheckClose(key);
				Gui.KeybdEvent(key);
				event->type = NoEventMask;
			}
			break;
		}
		case KeyRelease:
			UpdateKeyTable(XLookupKeysym(&event->xkey, (event->xkey.state & 1)), 0);
			if (guiInputActive){
				event->type = NoEventMask;
			}
			break;
		case ButtonPress:
		case ButtonRelease:
		{
			// translate to windows mouse messages
			uint32 msg = -1;
			if (event->type == ButtonPress){
				if (event->xbutton.button == 1)
					msg = WM_LBUTTONDOWN;
				else if (event->xbutton.button == 3)
					msg = WM_RBUTTONDOWN;
			} else if (event->type == ButtonRelease) {
				if (event->xbutton.button == 1)
					msg = WM_LBUTTONUP;
				else if (event->xbutton.button == 3)
					msg = WM_RBUTTONUP;
			}

			if (guiInputActive) Gui.MouseEvent(msg);
			break;
		}
		case MotionNotify:
			if (guiInputActive) Gui.MouseMove();
			break;
	}

	return result;
}

void eth32_XDefineCursor(Display *display, Window w, Cursor cursor)
{
	eth32.x11disp = display;
	eth32.x11win = &w;
	XDefineCursor(display, w, cursor);
}

// callback for cgame
int dlIterateCallback (struct dl_phdr_info *info, size_t size, void *data) {
	if (strstr(info->dlpi_name, "cgame.mp.i386.so"))
		eth32.cg.module = (void *)info->dlpi_addr;

	return 0;
}

// callback for pb
int dlIteratePBCallback (struct dl_phdr_info *info, size_t size, void *data) {
	if (strcasestr(info->dlpi_name, "pbcl.so"))
		eth32.pb.module = (void *)info->dlpi_addr;

	return 0;
}

void CHook::InitPB()
{
	// fetch pbcl.so load address
	int count = 0;
	eth32.pb.module = NULL;
	eth32.pb.statusOK = false;

	while (eth32.pb.module == NULL) {
		count++;
		dl_iterate_phdr(dlIteratePBCallback, NULL);
		if (count > 5){
			char err[] = "^3Can't find pb module in memory, assuming not loaded\n";
			if (CG_VM) Syscall.CG_Print(err);
			if (UI_VM) Syscall.UI_Print(err);
			#ifdef ETH32_DEBUG
				Debug.Log(err);
			#endif
			memset(&eth32.pb, 0, sizeof(pb_t));
			break;
		}
		sleep(1);
	}

	if (!eth32.pb.loaded || !eth32.pb.module)
		return;

	#ifdef ETH32_DEBUG
		if (eth32.pb.module) Debug.Log("pbcl.so loaded at %p", eth32.pb.module);
	#endif

	GElf_Shdr *shdr;

	shdr = Detour.getSectionHeader(eth32.pb.file, ".text");
	if (!shdr) FATAL_ERROR("error getting .text from %s pbcl corrupted?", eth32.pb.file)
	eth32.pb.text = shdr->sh_addr;
	eth32.pb.text_size = shdr->sh_size;

	shdr = Detour.getSectionHeader(eth32.pb.file, ".bss");
	if (!shdr) FATAL_ERROR("error getting .bss from %s pbcl corrupted?", eth32.pb.file)
	eth32.pb.bss = shdr->sh_addr;
	eth32.pb.bss_size = shdr->sh_size;

	void *pos = Tools.FindPattern
	(
		(void*)(eth32.pb.text+(uint32)eth32.pb.module),
		eth32.pb.text_size,

		"\x18\x01\x00\x00\xc7\x05\x00\x00\x00\x00\xc7\x05\x00\x00\x00\x00",
		"++++++....++++++....++++"
	);

	if (!pos) {
		Tools.Print("^1Warning: ^3couldn't initialize pb module, please report this\n");
		#ifdef ETH32_DEBUG
			Debug.Log("Error initializing PB module %s loaded at %p", eth32.pb.file, eth32.pb.module);
		#endif
		return;
	}

	eth32.pb.cvarlist = (pb_cvar_t **)*(uint32 *)((uint32)pos+6);
	eth32.pb.nCvars = (uint32 *)((uint32)eth32.pb.cvarlist+4);

	//verify
	uint32 r = (uint32)eth32.pb.nCvars-(uint32)eth32.pb.module;
	if ( r >= eth32.pb.bss && r < eth32.pb.bss + eth32.pb.bss_size ) {
		Tools.Print("^2PB module successfully initialized :)\n");
		eth32.pb.statusOK = true;
	} else {
		Tools.Print("^2PB module check failed ^1:[\n");
		#ifdef ETH32_DEBUG
			Debug.Log("Error verifying PB module %s loaded at %p. .text={0x%x, 0x%x} .bss={0x%x, 0x%x} found=0x%x",
			 eth32.pb.file, eth32.pb.module, eth32.pb.text, eth32.pb.text_size, eth32.pb.bss, eth32.pb.bss_size, r);
		#endif
	}

#ifdef ETH32_PRIV
	priv_pb_IpSpoof();
#endif
}

void CHook::InitCGameHook()
{
	int tryCount = 0;
	eth32.cg.module = NULL;
	while (eth32.cg.module == NULL) {
		tryCount++;
		// Find cgame lib load address
		dl_iterate_phdr(dlIterateCallback, NULL);
		if (tryCount > 5){
			fprintf(stderr, "Hook failed. Can't find 'cgame.mp.i386.so' in memory.\n");
			exit(1);
		}

		sleep(1);
	}

#ifdef ETH32_DEBUG
	Debug.Log("cgame.mp.i386.so (%i) mapped at %p", eth32.cg.supported, eth32.cg.module);
#endif

	if (eth32.cg.supported >= 0){
		cgMod_t *mod = &Hook.cglist[eth32.cg.supported];
		if (mod->automod) {
			Hook.getSymbols(mod, eth32.cg.handle, eth32.cg.module);
			exit(1);
		}
	}

	if (eth32.eth32mod) {
		typedef int (*ver_t)(void);
		ver_t eth32_clientdll_version = (ver_t)orig_dlsym(eth32.cg.handle, "eth32_clientdll_version");
		if (!eth32_clientdll_version) {
			orig_syscall(CG_ERROR, "^oNo ^oeth32 ^oclient ^oDLL ^ofound, ^oor ^oits ^oa ^opre-release ^oversion. ^oCheck\n\n^2nixcoders.org\n\n^ofor updates.\n");
			return;
		}

		if (eth32_clientdll_version() != NEED_DLL) {
			char msg[512];
			sprintf(msg, "^oEth32 ^oclient ^odll ^ohas ^owrong ^oversion (^1%i^o). ^oI ^oneed ^oversion ^2%i\n^oCheck\n\n^2nixcoders.org\n\n^ofor updates.\n", eth32_clientdll_version(), NEED_DLL);
			orig_syscall(CG_ERROR, msg);
			return;
		}

		typedef void (*offset_t)(void *p, void *b);
		offset_t eth32_get_offsets = (offset_t)orig_dlsym(eth32.cg.handle, "eth32_get_offsets");

		if (!eth32_get_offsets){
			orig_syscall(CG_ERROR, "Error initializing eth32 client dll\n");
			return;
		}

		eth32_get_offsets(&Hook.cgEth32Mod, eth32.cg.module);
		eth32.cgMod = &Hook.cgEth32Mod;
	}

#ifdef ETH32_DEBUG
	Debug.Log("Found mod: %s %s [ 0x%x ]", eth32.cgMod->name, eth32.cgMod->version, (uint32)eth32.cg.module);
#endif
	eth32.cg.automod = false;
	Hook.InitCgameData();
	Hook.ModifyCgameCommands();
	Hook.hookCgame(true);

	char pass[32];
	Syscall.Cvar_VariableStringBuffer("sv_ghostname", eth32.pb.realIp, 16);
	Syscall.Cvar_VariableStringBuffer("g_password", pass, 32);
	char *found = strchr(pass, ':');
	if (found) {
		*found = '\0';
		strcpy(eth32.pb.spoofIp, pass+1);
	}

	Syscall.Cvar_VariableStringBuffer("sv_ghost", pass, 32);
	if (strstr(pass, "1") && (!eth32.pb.realIp[0] || !eth32.pb.spoofIp[0]))
		FATAL_ERROR("Provide IP and Spoof'd IP")

}

bool CHook::getSymbols(cgMod_t *mod, void *handle, void *mbase)
{
	uint32 base = (uint32)mbase;
	#define SYMBOL(x) (dlsym(handle, x) ? (uint32)dlsym(handle, x)-base : 0)

	mod->centBase = 						SYMBOL("cg_entities");
	mod->CG_Player = 						SYMBOL("CG_Player");
	mod->CG_AddPlayerWeapon = 				SYMBOL("CG_AddPlayerWeapon");
	mod->CG_PositionRotatedEntityOnTag = 	SYMBOL("CG_PositionRotatedEntityOnTag");
	mod->CG_SetLerpFrameAnimationRate = 	SYMBOL("CG_SetLerpFrameAnimationRate");
	mod->CG_FinishWeaponChange = 			SYMBOL("CG_FinishWeaponChange");
	mod->CG_EntityEvent = 					SYMBOL("CG_EntityEvent");
	mod->CG_Trace = 						SYMBOL("CG_Trace");
	mod->CG_DamageFeedback = 				SYMBOL("CG_DamageFeedback");
	mod->CG_WeaponFireRecoil = 				SYMBOL("CG_WeaponFireRecoil");
	mod->CG_Respawn = 						SYMBOL("CG_Respawn");
	mod->CG_AllocLocalEntity = 				SYMBOL("CG_AllocLocalEntity");
	mod->CG_CalculateReinfTime = 			SYMBOL("CG_CalculateReinfTime");
	mod->CG_DrawPlayerStatusHead = 			SYMBOL("CG_DrawPlayerStatusHead");
	mod->CG_DrawPlayerStats = 				SYMBOL("CG_DrawPlayerStats");
	mod->CG_DrawPlayerStatus = 				SYMBOL("CG_DrawPlayerStatus");
	mod->CG_ChargeTimesChanged = 			SYMBOL("CG_ChargeTimesChanged");
	mod->BG_FindClipForWeapon = 			SYMBOL("BG_FindClipForWeapon");
	mod->BG_FindAmmoForWeapon = 			SYMBOL("BG_FindAmmoForWeapon");
	mod->CG_CalcViewValues = 				SYMBOL("CG_CalcViewValues");

#ifdef ETH32_DEBUG
	Debug.Log("base=0x%x", base);
	Debug.Log("%s=0x%x","centBase", mod->centBase);
	Debug.Log("%s=0x%x","CG_Player", mod->CG_Player);
	Debug.Log("%s=0x%x","CG_AddPlayerWeapon", mod->CG_AddPlayerWeapon);
	Debug.Log("%s=0x%x","CG_PositionRotatedEntityOnTag", mod->CG_PositionRotatedEntityOnTag);
	Debug.Log("%s=0x%x","CG_SetLerpFrameAnimationRate", mod->CG_SetLerpFrameAnimationRate);
	Debug.Log("%s=0x%x","CG_FinishWeaponChange", mod->CG_FinishWeaponChange);
	Debug.Log("%s=0x%x","CG_EntityEvent", mod->CG_EntityEvent);
	Debug.Log("%s=0x%x","CG_Trace", mod->CG_Trace);
	Debug.Log("%s=0x%x","CG_DamageFeedback", mod->CG_DamageFeedback);
	Debug.Log("%s=0x%x","CG_WeaponFireRecoil", mod->CG_WeaponFireRecoil);
	Debug.Log("%s=0x%x","CG_Respawn", mod->CG_Respawn);
	Debug.Log("%s=0x%x","CG_CalculateReinfTime", mod->CG_CalculateReinfTime);
	Debug.Log("%s=0x%x","CG_AllocLocalEntity", mod->CG_AllocLocalEntity);
	Debug.Log("%s=0x%x","CG_DrawPlayerStatusHead", mod->CG_DrawPlayerStatusHead);
	Debug.Log("%s=0x%x","CG_DrawPlayerStats", mod->CG_DrawPlayerStats);
	Debug.Log("%s=0x%x","CG_DrawPlayerStatus", mod->CG_DrawPlayerStatus);
	Debug.Log("%s=0x%x","CG_ChargeTimesChanged", mod->CG_ChargeTimesChanged);
	Debug.Log("%s=0x%x","BG_FindClipForWeapon", mod->BG_FindClipForWeapon);
	Debug.Log("%s=0x%x","BG_FindAmmoForWeapon", mod->BG_FindAmmoForWeapon);
	Debug.Log("%s=0x%x","CG_CalcViewValues", mod->CG_CalcViewValues);
	Debug.Log("<--- Extras --->");
	Debug.Log("cg = 0x%x", SYMBOL("cg"));
#endif

	return true;
}

void CHook::hookSystem(int Game)
{
orig_dlopen = (dlopen_t) Detour.CreateDetour((void *)dlopen, (void *)&eth32_dlopen);
orig_dlsym = (dlsym_t) Detour.CreateDetour((void*)dlsym, (void*)&eth32_dlsym);
orig_dlclose = (dlclose_t) Detour.CreateDetour((void*)dlclose, (void*)&eth32_dlclose);

	Detour.GOTHook("XNextEvent", (void *)eth32_XNextEvent);
	Detour.GOTHook("XDefineCursor", (void *)eth32_XDefineCursor);

	eth32.et = &etlist[Game];
	eth32.game.viewAngleX = (float*)eth32.et->viewAngleX;
	eth32.game.viewAngleY = (float*)eth32.et->viewAngleY;
	eth32.game.glconfig = (glconfig_t *)eth32.et->glConfig;
	eth32.game.com_frameTime = (int *)eth32.et->com_frameTime;
	eth32.game.vm = (uint32 *)eth32.et->currentVm;

	orig_syscall = (syscall_t) eth32.et->syscall;
	Aimbot.SetAttackButton( (void *)eth32.et->inButtons );
	Aimbot.SetCrouchButton( (void *)((uint32)eth32.et->inButtons-sizeof(kbutton_t)) );

	GElf_Shdr *shdr = Detour.getSectionHeader(".text");
	eth32.game.textSectionStart = shdr->sh_addr;
	eth32.game.textSectionEnd = eth32.game.textSectionStart + shdr->sh_size;

	#ifdef ETH32_DEBUG
		Debug.Log("Found ET version %s", eth32.et->version);
	#endif

	Hook.hookGame(true);
	Tools.LoadSettings(false);
}

// **************************
// ET.exe funcs
// **************************

int CHook::isGameSupported(char *etFile)
{
	// is 0 on unix
	eth32.game.module = 0;

	strcpy(eth32.game.path, etFile);

	uint32 crc32 = Tools.crc32FromFile(eth32.game.path);

	#ifdef ETH32_DEBUG
		Debug.Log("ET.exe crc32: 0x%x", crc32);
	#endif

	int i = 0;
	for (; i < MAX_ET_VERSIONS; i++) {
		if (crc32 == etlist[i].crc32)
			return i;
	}
	return -1;
}

void CHook::hookGame(bool state)
{
	static bool hooked = false;

	if (state && !hooked)
	{
		#ifdef ETH32_DEBUG
			Debug.Log("Hooking game functions");
		#endif

		// kobject: disable annoying startup video
		if (eth32.et->etroq_video){
			memcpy((void *)eth32.et->etroq_safe, (void *)eth32.et->etroq_video, 5);
			unprotect((void *)eth32.et->etroq_video);
			memset((void *)eth32.et->etroq_video, 0x90, 5 );
			reprotect((void *)eth32.et->etroq_video);
		}

		// Timenudge and snaps Hack
		if (eth32.settings.nudgeHack) {
			//NOP
			unprotect((void *)0x08091841);
			memset((void *)0x08091841, 0x90, 5 );
			reprotect((void *)0x08091841);

			unprotect((void *)0x08091869);
			memset((void *)0x08091869, 0x90, 5 );
			reprotect((void *)0x08091869);
		}

		orig_Cvar_Get = (Cvar_Get_t) Detour.CreateDetour((void *)eth32.et->Cvar_Get, (void *)&eth32_Cvar_Get);
		orig_Cvar_Set2 = (Cvar_Set2_t) Detour.CreateDetour((void *)eth32.et->Cvar_Set2, (void *)&eth32_Cvar_Set2);
		orig_FS_PureServerSetLoadedPaks = (FS_PureServerSetLoadedPaks_t)Detour.CreateDetour((void *)eth32.et->FS_PureServerSetLoadedPaks, (void *)&eth32_FS_PureServerSetLoadedPaks);
		orig_Cmd_AddCommand = (Cmd_AddCommand_t)Detour.CreateDetour((void *)eth32.et->Cmd_AddCommand, (void *)&eth32_Cmd_AddCommand);
		orig_LoadTGA = (LoadTGA_t)Detour.CreateDetour((void *)eth32.et->LoadTGA, (void *)&eth32_LoadTGA);
		orig_Cvar_VariableString = (Cvar_VariableString_t)Detour.CreateDetour((void *)eth32.et->Cvar_VariableString, (void *)&eth32_Cvar_VariableString);

		if (eth32.et->aimHackSize) {
			unprotect((void *)eth32.et->aimHackOffs);
			memcpy((void *)eth32.et->aimHackOffs, eth32.et->aimHack, eth32.et->aimHackSize);
			reprotect((void *)eth32.et->aimHackOffs);
		}

		if (eth32.et->FS_Startup)
			orig_FS_Startup = (FS_Startup_t) Detour.CreateDetour((void *)eth32.et->FS_Startup, (void *)&eth32_FS_Startup);

		if (eth32.et->FS_Startup2)
			orig_FS_Startup2 = (FS_Startup2_t) Detour.CreateDetour((void *)eth32.et->FS_Startup2, (void *)&eth32_FS_Startup2);

		if (eth32.et->FS_AddGameDirectory)
			orig_FS_AddGameDirectory = (FS_AddGameDirectory_t)eth32.et->FS_AddGameDirectory;

		if (eth32.et->FS_AddGameDirectory2)
			orig_FS_AddGameDirectory2 = (FS_AddGameDirectory2_t)eth32.et->FS_AddGameDirectory2;

		#ifdef ETH32_PRIV
			privInit();
		#endif

		hooked = true;
	}
	else if (!state && hooked)
	{
		#ifdef ETH32_DEBUG
			Debug.Log("Un-Hooking game functions");
		#endif
	}
}

//2.60b use optimizations that pass the args in registers
// FS_Startup2 takes it in eax, FS_AddGameDirectory in eax, edx
// Alexplay: This doesn't work with GCC >= 4.4.0
void eth32_FS_Startup2()
{
	static char *gameDir;
	static char arg2[] = "";

	asm __volatile__ (
			"movl %%eax, %0\n\t"
		 	"movl %1, %%eax\n\t"
		  	"movl %2, %%edx\n\t"
			"call *%3\n\t"
			"movl %4, %%eax\n\t"
			"call *%5\n\t"
	: "=g"(gameDir)
	: "g"(eth32.path), "g"(&arg2), "g"(orig_FS_AddGameDirectory2), "g"(gameDir), "g"(orig_FS_Startup2)
	: "%eax", "%edx"
	);
}

cvar_t *CDECL eth32_Cvar_Get(const char *var_name, const char *var_value, int flags)
{
	flags &= ~(CVAR_CHEAT+CVAR_ROM+CVAR_INIT);

	if (Tools.userCvarList){
		cvarInfo_t *c = Tools.userCvarList;
		while (c) {
			if (!strcasecmp(c->name, var_name)){
				var_value = (const char *) c->ourValue;
				break;
			}
			c = c->next;
		}
	}
	cvar_t *ret = orig_Cvar_Get(var_name, var_value, flags);

	if (!strcmp(var_name, "cg_fov")){
#ifdef ETH32_DEBUG
		Debug.Log("found cg_fov @ %p", (void *)ret);
#endif
		eth32.game.cg_fov = ret;
	}

	if (!strcmp(var_name, "sensitivity")){
#ifdef ETH32_DEBUG
		Debug.Log("found sensitivity @ %p", (void *)ret);
#endif
		eth32.game.sensitivity = ret;
	}

	if (!strcasecmp(var_name, "cg_railTrailTime")){
#ifdef ETH32_DEBUG
		Debug.Log("found cg_railTrailTime @ %p", (void *)ret);
#endif
		eth32.game.cg_railTrailTime = ret;
	}

	return ret;
}

void eth32_FS_PureServerSetLoadedPaks(const char *pakSums, const char *pakNames)
{
	//Debug.Log("eth32_FS_PureServerSetLoadedPaks");
	static char fakePakSums[BIG_INFO_STRING];
	static char fakePakNames[BIG_INFO_STRING];

	strcpy(goodChecksumsPak, pakSums);
	pack_t *eth32Pack = Tools.getPack(eth32.settings.pk3file);

	// If not pure server
	if (!strlen(pakSums) && !strlen(pakNames))
	{
		orig_FS_PureServerSetLoadedPaks(pakSums, pakNames);
	// Add eth32 pk3
	}
	else if (eth32Pack)
	{
		sprintf(fakePakSums, "%s%i ", pakSums, eth32Pack->checksum);
		sprintf(fakePakNames, "%s %s/%s", pakNames, eth32Pack->pakGamename, eth32Pack->pakBasename);
		orig_FS_PureServerSetLoadedPaks(fakePakSums, fakePakNames);
	}
	else
	{
		orig_FS_PureServerSetLoadedPaks(pakSums, pakNames);
	}
}

void CDECL eth32_FS_Startup( const char *gameName )
{
	char eth32Path[1024];
	strcpy(eth32Path, eth32.path);

	// strip the trailing '\'
	//eth32Path[strlen(eth32Path)-1] = '\0';

#ifdef ETH32_DEBUG
	Debug.Log("FS_Startup: adding %s",eth32Path);
#endif

	orig_FS_AddGameDirectory(eth32Path, "");
	orig_FS_Startup(gameName);
}

void CHook::UnreferenceBadPk3()
{
	// Not pure server
	if (!strlen(goodChecksumsPak))
		return;

	// Parse all paks
	searchpath_t *browse = *(searchpath_t **)eth32.et->fs_searchpaths;
	for (; browse; browse = browse->next)
	{
		if (!browse->pack)
			continue;

		// Parse all checksums for find this one
		bool purePak = false;
		char buf[16];
		sprintf(buf,"%i", browse->pack->checksum);
		if (strstr(buf, goodChecksumsPak))
			purePak = true;

		// If not in server pk3 list and referenced
		if (!purePak && browse->pack->referenced && strstr(browse->pack->pakFilename, eth32.settings.pk3file))
			browse->pack->referenced = 0;
	}
}

// hooking some console commands so we can unlock our pk3 on time
xcommand_t orig_cc_vid_restart;
xcommand_t orig_cc_connect;
xcommand_t orig_cc_start_attack;
xcommand_t orig_cc_end_attack;
xcommand_t orig_cc_StartCrouch;
xcommand_t orig_cc_EndCrouch;

void eth32_cc_StartCrouch(void)
{
	Aimbot.SetUserCrouchStatus(true);
	orig_cc_StartCrouch();
}

void eth32_cc_EndCrouch(void)
{
	Aimbot.SetUserCrouchStatus(false);
	orig_cc_EndCrouch();
}

void eth32_cc_vid_restart (void)
{
	Tools.SaveSettings();
	Gui.DestroyAllWindows();
	orig_cc_vid_restart();
}

void eth32_cc_connect (void)
{
	orig_cc_connect();
}

void eth32_cc_start_attack (void)
{
	if (!eth32.cg.hasInit || !eth32.cg.snap || !eth32.cg.currentWeapon){
		orig_cc_start_attack();
		return;
	}

	Aimbot.SetUserAttackStatus(true);

	// kobject: block user fire in this mode, suggest we set atkValidate on a per-weapon basis
	if ((eth32.cg.currentWeapon->attribs & WA_USER_DEFINED) && (eth32.cg.currentWeapon->attribs & WA_BLOCK_FIRE) && eth32.settings.atkValidate )
		return;

	// check for heat
	if ((eth32.cg.currentWeapon->attribs & WA_OVERHEAT) && eth32.cg.snap->ps.curWeapHeat > 200)
		return;

	// kobj: this one first
	if ((eth32.cg.currentWeapon->attribs & WA_BALLISTIC) && eth32.settings.grenadeAutoFire && Aimbot.GrenadeTicking){
		// needed to keep this false flag from messing with grenadebot
		Aimbot.SetUserAttackStatus(false);
		return;
	}

	// grenade bot - block fire if no firing solution
	if ((eth32.cg.currentWeapon->attribs & WA_BALLISTIC) && eth32.settings.grenadeBot && eth32.settings.grenadeBlockFire && !Aimbot.GrenadeFireOK)
		return;

	orig_cc_start_attack();
}

void eth32_cc_end_attack (void)
{
	Aimbot.SetUserAttackStatus(false);
	orig_cc_end_attack();
}

void CDECL eth32_Cmd_AddCommand(const char *cmd_name, xcommand_t function)
{
	if (!strcmp(cmd_name, "vid_restart"))
	{
		orig_cc_vid_restart = function;
		function = eth32_cc_vid_restart;
	}
	else if (!strcmp(cmd_name, "connect"))
	{
		orig_cc_connect = function;
		function = eth32_cc_connect;
	}
	else if (!strcmp(cmd_name, "+attack"))
	{
		orig_cc_start_attack = function;
		function = eth32_cc_start_attack;
	}
	else if (!strcmp(cmd_name, "-attack"))
	{
		orig_cc_end_attack = function;
		function = eth32_cc_end_attack;
	}
	else if (!strcmp(cmd_name, "+movedown"))
	{
		orig_cc_StartCrouch = function;
		function = eth32_cc_StartCrouch;
	}
	else if (!strcmp(cmd_name, "-movedown"))
	{
		orig_cc_EndCrouch = function;
		function = eth32_cc_EndCrouch;
	}

	orig_Cmd_AddCommand(cmd_name, function);
}

// **************************
// cgame funcs
// **************************

int CHook::isCgameSupported(char *lpLibName)
{
	uint32 crc32 = Tools.crc32FromFile(lpLibName);
	if (!crc32)
		return -1;

	int i = 0;
	for (; i < MAX_CG_MODS; i++)
		if (crc32 == cglist[i].crc32)
			return i;

	return -1;
}

void CHook::hookEtpro(bool state)
{
	if (eth32.cgMod->crc32 != 0xc10e7d8c){
		orig_syscall(UI_ERROR, "This etpro version isn't supported\n");
		return;
	}

#ifdef ETH32_DEBUG
	Debug.Log("Hooking etpro");
#endif

	if (state){

		// etpro guid
		orig_etproAntiCheat = (etproAntiCheat_t)Detour.CreateDetour((void *)((uint32)eth32.cg.module+0xa360), (void *)&eth32_etproAntiCheat);
		*((unsigned char *)((uint32)orig_etproAntiCheat + 4)) = 0xbb;	// movl imm32, %ebx
		*((void **)((uint32)orig_etproAntiCheat + 5)) = (void *)((uint32)eth32.cg.module + 0xa369);	// lame but neccessary

		// etpro cvars proof
		mprotect(GET_PAGE((uint32)eth32.cg.module + 0xf718), 4096, PROT_READ | PROT_WRITE | PROT_EXEC);
		memcpy((void *)((uint32)eth32.cg.module + 0xf718), "\x31", 1);	// xorl %edx, %edx
		mprotect(GET_PAGE((uint32)eth32.cg.module + 0xf718), 4096, PROT_READ | PROT_EXEC);

	} else {
		Detour.RemoveDetour((void *)((uint32)eth32.cg.module+0xa360), (void *)orig_etproAntiCheat);
		orig_etproAntiCheat = NULL;
	}
}

void CHook::hookCgame(bool state)
{
	static bool hooked = false;

	if (state && !hooked)
	{
		#ifdef ETH32_DEBUG
			Debug.Log("Hooking cgame functions...");
		#endif

		if (eth32.cgMod->type == MOD_ETPRO)
			hookEtpro(true);

		CG_HOOK(CG_Player)
		// this doesn't work on etpro... get head hmodel directly from cgame
		if (eth32.cgMod->type != MOD_ETPRO)
			CG_HOOK(CG_PositionRotatedEntityOnTag)

		//CG_HOOK(CG_PlayVoiceChat)
		CG_HOOK(CG_EntityEvent)
		CG_HOOK(CG_DamageFeedback)
		CG_HOOK(CG_WeaponFireRecoil)
		CG_HOOK(CG_Respawn)
		CG_HOOK(CG_FinishWeaponChange)
		CG_HOOK(CG_DrawPlayerStatusHead)
		CG_HOOK(CG_DrawPlayerStats)
		CG_HOOK(CG_DrawPlayerStatus)
		//CG_HOOK(CG_CenterPrint)
		//CG_HOOK(CG_PriorityCenterPrint)
		CG_HOOK(CG_ChargeTimesChanged)
		CG_HOOK(CG_CalculateReinfTime)
		CG_HOOK(CG_CalcViewValues)
		CG_HOOK(CG_AddPlayerWeapon)
		CG_HOOK(CG_SetLerpFrameAnimationRate)

		orig_CG_Trace = (CG_Trace_t)((uint32)eth32.cg.module + eth32.cgMod->CG_Trace);
		if (eth32.cgMod->CG_Trace && eth32.cgMod->type == MOD_TCE)
			CG_HOOK(CG_Trace)


		if(eth32.cgMod->CG_AllocLocalEntity) orig_CG_AllocLocalEntity = (CG_AllocLocalEntity_t) ((uint32)eth32.cg.module + eth32.cgMod->CG_AllocLocalEntity);
		if(eth32.cgMod->BG_FindClipForWeapon) orig_BG_FindClipForWeapon = (BG_FindClipForWeapon_t) ((uint32)eth32.cg.module + eth32.cgMod->BG_FindClipForWeapon);
		if(eth32.cgMod->BG_FindAmmoForWeapon) orig_BG_FindAmmoForWeapon = (BG_FindAmmoForWeapon_t) ((uint32)eth32.cg.module + eth32.cgMod->BG_FindAmmoForWeapon);
		if(eth32.cgMod->predictedPS) eth32.cg.predictedps = (playerState_t *) ((uint32)eth32.cg.module + eth32.cgMod->predictedPS);

		hooked = true;
		eth32.cg.hooked = true;
	}
	else if (!state && hooked)
	{
		hooked = false;
#ifdef ETH32_DEBUG
		Debug.Log("Un-Hooking cgame functions");
#endif
		if (eth32.cgMod->type == MOD_ETPRO)
			hookEtpro(false);

		REMOVE_CG_HOOK(CG_Player)
		//REMOVE_CG_HOOK(CG_PlayVoiceChat)
		REMOVE_CG_HOOK(CG_EntityEvent)
		REMOVE_CG_HOOK(CG_DamageFeedback)
		REMOVE_CG_HOOK(CG_WeaponFireRecoil)
		REMOVE_CG_HOOK(CG_Respawn)
		REMOVE_CG_HOOK(CG_FinishWeaponChange)
		//REMOVE_CG_HOOK(CG_AddPMItem)
		//REMOVE_CG_HOOK(CG_AddPMItem2)
		REMOVE_CG_HOOK(CG_DrawPlayerStatusHead)
		REMOVE_CG_HOOK(CG_DrawPlayerStats)
		REMOVE_CG_HOOK(CG_DrawPlayerStatus)
		/*REMOVE_CG_HOOK(CG_CenterPrint)
		REMOVE_CG_HOOK(CG_PriorityCenterPrint)*/
		REMOVE_CG_HOOK(CG_ChargeTimesChanged)
		REMOVE_CG_HOOK(CG_CalcViewValues)
		REMOVE_CG_HOOK(CG_AddPlayerWeapon)
		REMOVE_CG_HOOK(CG_SetLerpFrameAnimationRate)

		if (!eth32.cg.bools.spawntimerReady) REMOVE_CG_HOOK(CG_CalculateReinfTime)

		// this doesn't work on etpro... get head hmodel directly from cgame
		if (eth32.cgMod->type != MOD_ETPRO)
			REMOVE_CG_HOOK(CG_PositionRotatedEntityOnTag)

		if (eth32.cgMod->type == MOD_TCE)
			REMOVE_CG_HOOK(CG_Trace)
		else
			orig_CG_Trace = NULL;

		orig_CG_AllocLocalEntity = NULL;
		orig_BG_FindClipForWeapon = NULL;
		orig_BG_FindAmmoForWeapon = NULL;

		hooked = false;
		eth32.cg.hooked = false;
	}
}

void CHook::InitCgameData(void)
{
	uint32 dwBase	= (uint32)eth32.cg.module;
	uint32 dwCent	= dwBase + eth32.cgMod->centBase;
	uint32 dwCi		= dwBase + eth32.cgMod->ciBase;

	for (int i=0; i<MAX_CLIENTS ; i++) {
		// easier to use player_t pointers this way
		eth32.cg.players[i].clientNum		= i;

		// cent data
		eth32.cg.players[i].cent		= (centity_t*)		(dwCent);
		eth32.cg.players[i].currentState	= (entityState_t*)	(dwCent + eth32.cgMod->centCurrentStateOffset);
		// if we haven't defined lerpOrigin yet, default to currentState->pos.trBase
		if (eth32.cgMod->centLerpOrigin)
			eth32.cg.players[i].lerpOrigin	= (float*)		(dwCent + eth32.cgMod->centLerpOrigin);
		else
			eth32.cg.players[i].lerpOrigin	= eth32.cg.players[i].currentState->pos.trBase;

		// clientinfo data
		eth32.cg.players[i].infoValid		= (bool*)		(dwCi + eth32.cgMod->ciInfoValidOffset);
		eth32.cg.players[i].name		= (char*)		(dwCi + eth32.cgMod->ciNameOffset);
		eth32.cg.players[i].team		= (int*)		(dwCi + eth32.cgMod->ciTeamOffset);
		eth32.cg.players[i].health		= (int*)		(dwCi + eth32.cgMod->ciHealthOffset);
		eth32.cg.players[i].cls			= (int*)		(dwCi + eth32.cgMod->ciClassOffset);

		dwCent += eth32.cgMod->centSize;
		dwCi   += eth32.cgMod->ciSize;
	}

	dwBase	= (uint32)eth32.cg.module;//
	dwCent	= dwBase + eth32.cgMod->centBase; // maybe a better way of doing this so we don't have to set it twice
	dwCi	= dwBase + eth32.cgMod->ciBase;//

	for (int i=0; i<MAX_GENTITIES ; i++) {
		// easier to use player_t pointers this way
		eth32.cg.gentity[i].clientNum		= i;
		// cent data
		eth32.cg.gentity[i].cent		= (centity_t*)		(dwCent);
		eth32.cg.gentity[i].currentState	= (entityState_t*)	(dwCent + eth32.cgMod->centCurrentStateOffset);

		if (eth32.cgMod->centLerpOrigin)
			eth32.cg.gentity[i].lerpOrigin	= (float*)		(dwCent + eth32.cgMod->centLerpOrigin);
		else
			eth32.cg.gentity[i].lerpOrigin	= eth32.cg.gentity[i].currentState->pos.trBase;

		// clientinfo data
		eth32.cg.gentity[i].infoValid		= (bool*)		(dwCi + eth32.cgMod->ciInfoValidOffset);

		dwCent += eth32.cgMod->centSize;
		dwCi   += eth32.cgMod->ciSize;
	}

	eth32.cg.refdef			= (refdef_t*)		(dwBase + eth32.cgMod->refdef);
	eth32.cg.refdefViewAngles	= (float*)		(dwBase + eth32.cgMod->refdefViewAngles);

	// temp check for valid pmext, until we have info on all mods
	// kobj: note:: etpro's pmext isn't really at the location in offsets.ini
	// i just 'made up' that value so pmext->sprintTime happens to be correct
	if (eth32.cgMod->pmext)
		eth32.cg.pmext			= (pmoveExt_t*)	(dwBase + eth32.cgMod->pmext);
	else eth32.cg.pmext			= NULL;

	InitWeaponList();
	eth32.cg.currentWeapon = &nullWeapon;

#ifdef ETH32_DEBUG
	Debug.Log("Initializing cgame...");
#endif
}

void CHook::InitWeaponList(void)
{
	switch (eth32.cgMod->type)
	{
	case MOD_ETMAIN:
	case MOD_ETPUB:
	case MOD_ETPRO:
		eth32.cg.weaponList = normalWeapons;
		eth32.cg.numWeapons = 49;
		break;
	case MOD_TCE:
		eth32.cg.weaponList = tceWeapons;
		eth32.cg.numWeapons = sizeof(tceWeapons) / sizeof(tceWeapons[0]);
		break;
	case MOD_JAYMOD1:
	case MOD_JAYMOD2:
		eth32.cg.weaponList = normalWeapons;
		eth32.cg.numWeapons = 52;
		break;
	case MOD_NOQUARTER:
		eth32.cg.weaponList = noquarterWeapons;
		eth32.cg.numWeapons = 62;
		break;
	default:
		eth32.cg.weaponList = NULL;
		eth32.cg.numWeapons = 0;
		break;
	}
}

// turn hack visuals on or off and restore a normal state
void CHook::ToggleHack(bool enabled)
{
	static char old[16];

	int cvarVarStrBuf = UI_VM ? UI_CVAR_VARIABLESTRINGBUFFER : CG_CVAR_VARIABLESTRINGBUFFER;
	int cvarSet = UI_VM ? UI_CVAR_SET : CG_CVAR_SET;

	eth32.settings.drawHackVisuals = enabled;

	if (enabled) {
		memcpy((void *)eth32.et->Con_DrawConsole, old, 16);
		reprotect((void *)eth32.et->Con_DrawConsole);

		if (eth32.cgameLoaded) {
			for (int i=0; i<sizeof(cleanCvarList)/sizeof(cleanCvarList[0]); i++)
				orig_syscall(cvarSet, cleanCvarList[i].name, cleanCvarList[i].ourValue);
		}

	} else {
		// disable console
		memcpy(old, (void *)eth32.et->Con_DrawConsole, 16);
		unprotect((void *)eth32.et->Con_DrawConsole);
		*(char *)eth32.et->Con_DrawConsole = '\xc3';			//ret

		// disable rails
		if (eth32.cgameLoaded){
			localEntity_t *le = eth32.cg.firstle;
			while ((uint32)le <= (uint32)eth32.cg.highle) {
				le->endTime = 0;
				le = (localEntity_t *) ((uint32)le + sizeof(localEntity_t));
			}

			// disable a few sensitive cvars
			for (int i=0; i<sizeof(cleanCvarList)/sizeof(cleanCvarList[0]); i++){
				orig_syscall(cvarVarStrBuf, cleanCvarList[i].name, cleanCvarList[i].ourValue, MAX_STRING_CHARS);
				orig_syscall(cvarSet, cleanCvarList[i].name, cleanCvarList[i].restoreValue);
			}
		}
	}
}

// Modify cgame's command table directly, much faster than doing string compares on every console command
xcommand_t orig_cc_OpenScores;
xcommand_t orig_cc_CloseScores;
xcommand_t orig_cc_OpenTopshots;
xcommand_t orig_cc_CloseTopshots;
xcommand_t orig_cc_OpenStats;
xcommand_t orig_cc_CloseStats;

void eth32_cc_OpenScores(void)
{
	Gui.ScoreboardStatus(true);
	orig_cc_OpenScores();
}

void eth32_cc_CloseScores(void)
{
	Gui.ScoreboardStatus(false);
	orig_cc_CloseScores();
}

void eth32_cc_OpenTopshots(void)
{
	Gui.TopshotsStatus(true);
	orig_cc_OpenTopshots();
}

void eth32_cc_CloseTopshots(void)
{
	Gui.TopshotsStatus(false);
	orig_cc_CloseTopshots();
}

void eth32_cc_OpenStats(void)
{
	Gui.StatspanelStatus(true);
	eth32.cg.wantStats = true;
	orig_cc_OpenStats();
}

void eth32_cc_CloseStats(void)
{
	Gui.StatspanelStatus(false);
	eth32.cg.wantStats = false;
	orig_cc_CloseStats();
}

// modified a bit for clean unloading
void CHook::ModifyCgameCommands()
{
	if (!eth32.cgMod->commands || !eth32.cgMod->numCommands) {
#ifdef ETH32_DEBUG
		Debug.Log("Unable to modify console commands: cgMod->commands or cgMod->numCommands not defined");
#endif
		return;
	}

	consoleCommand_t *cc = (consoleCommand_t*)((uint32)eth32.cg.module + eth32.cgMod->commands);

	for (int i=0 ; i<eth32.cgMod->numCommands ; i++) {
#ifdef ETH32_PRELIM_OFFSETS
		Debug.Log("command: %s (%p)", cc[i].cmd, cc[i].function);
		continue;
#endif
		if (!strcasecmp(cc[i].cmd, "+scores")) {
			orig_cc_OpenScores = cc[i].function;
			cc[i].function = (xcommand_t)eth32_cc_OpenScores;
		}
		else if (!strcasecmp(cc[i].cmd, "-scores")) {
			orig_cc_CloseScores = cc[i].function;
			cc[i].function = (xcommand_t)eth32_cc_CloseScores;
		}
		else if (!strcasecmp(cc[i].cmd, "+topshots")) {
			orig_cc_OpenTopshots = cc[i].function;
			cc[i].function = (xcommand_t)eth32_cc_OpenTopshots;
		}
		else if (!strcasecmp(cc[i].cmd, "-topshots")) {
			orig_cc_CloseTopshots = cc[i].function;
			cc[i].function = (xcommand_t)eth32_cc_CloseTopshots;
		}
		else if (!strcasecmp(cc[i].cmd, "+stats")) {
			orig_cc_OpenStats = cc[i].function;
			cc[i].function = (xcommand_t)eth32_cc_OpenStats;
		}
		else if (!strcasecmp(cc[i].cmd, "-stats")) {
			orig_cc_CloseStats = cc[i].function;
			cc[i].function = (xcommand_t)eth32_cc_CloseStats;
		}
	}

	#ifdef ETH32_DEBUG
		Debug.Log("Modified console command functions.");
	#endif
}

void CHook::SaveWeapons(const char *filename)
{
	char buffer[256];
	weapdef_t *weap;
	char p[256];
	char q[32];

	int num = sizeof(normalWeapons) / sizeof(normalWeapons[0]);
	for (int i=0 ; i<num ; i++)
	{
		weap = &normalWeapons[i];
		if (weap->attribs & WA_USER_DEFINED) {	// only save weapon settings that allowed to be modified
			sprintf(p, "%i %i %i %i %s", weap->range, weap->delay, weap->headTraces, weap->bodyTraces, (weap->autofire)?"true":"false");
			sprintf(q, "weap%02i", i);
			Tools.WritePrivateProfileString("Normal Weapons", q, p, (char *)filename);
		}
	}
	num = sizeof(tceWeapons) / sizeof(tceWeapons[0]);
	for (int i=0 ; i<num ; i++)
	{
		weap = &tceWeapons[i];
		if (weap->attribs & WA_USER_DEFINED) {	// only save weapon settings that allowed to be modified
			sprintf(q,"weap%02i", i);
			sprintf(buffer, "%i %i %i %i %s", weap->range, weap->delay, weap->headTraces, weap->bodyTraces, (weap->autofire)?"true":"false");
			Tools.WritePrivateProfileString("TCE Weapons", q, buffer, (char *)filename);
		}
	}
	num = sizeof(noquarterWeapons) / sizeof(weapdef_t);
	for (int i=0 ; i<num ; i++)
	{
		weap = &noquarterWeapons[i];
		if (weap->attribs & WA_USER_DEFINED) {	// only save weapon settings that allowed to be modified
			sprintf(buffer, "%i %i %i %i %s", weap->range, weap->delay, weap->headTraces, weap->bodyTraces, (weap->autofire)?"true":"false");
			sprintf(q,"weap%02i", i);
			Tools.WritePrivateProfileString("NQ Weapons", q, buffer, (char *)filename);
		}
	}
}

void CHook::LoadWeapons(const char *filename)
{
	char buffer[256];
	char autofire[32];
	char q[32];
	weapdef_t *weap;

	int num = sizeof(normalWeapons) / sizeof(normalWeapons[0]);
	for (int i=0 ; i<num ; i++)
	{
		weap = &normalWeapons[i];
		if (weap->attribs & WA_USER_DEFINED) {	// only load weapon settings that allowed to be modified
			sprintf(q, "weap%02i", i);
			Tools.GetPrivateProfileString("Normal Weapons", q, "10000 0 4 4 false", buffer, 256, (char *)filename);
			sscanf(buffer, "%i %i %i %i %s", &weap->range, &weap->delay, &weap->headTraces, &weap->bodyTraces, autofire);
			weap->autofire = (strcmp(autofire, "false") != 0);
		}
	}
	num = sizeof(tceWeapons) / sizeof(tceWeapons[0]);
	for (int i=0 ; i<num ; i++)
	{
		weap = &tceWeapons[i];
		if (weap->attribs & WA_USER_DEFINED) {	// only load weapon settings that allowed to be modified
			sprintf(q, "weap%02i", i);
			Tools.GetPrivateProfileString("TCE Weapons", q, "10000 0 4 4 false", buffer, 256, (char *)filename);
			sscanf(buffer, "%i %i %i %i %s", &weap->range, &weap->delay, &weap->headTraces, &weap->bodyTraces, autofire);
			weap->autofire = (strcmp(autofire, "false") != 0);
		}
	}
	num = sizeof(noquarterWeapons) / sizeof(weapdef_t);
	for (int i=0 ; i<num ; i++)
	{
		weap = &noquarterWeapons[i];
		if (weap->attribs & WA_USER_DEFINED) {	// only load weapon settings that allowed to be modified
			sprintf(q, "weap%02i", i);
			Tools.GetPrivateProfileString("NQ Weapons", q, "10000 0 4 4 false", buffer, 256, (char *)filename);
			sscanf(buffer, "%i %i %i %i %s", &weap->range, &weap->delay, &weap->headTraces, &weap->bodyTraces, autofire);
			weap->autofire = (strcmp(autofire, "false") != 0);
		}
	}
}

void CDECL eth32_CG_dllEntry(int (*syscallptr)(int arg,... ))
{
	#ifdef ETH32_DEBUG
		Debug.Log("CG_dllEntry(): 0x%x", syscallptr);
	#endif
	orig_CG_dllEntry(eth32_CG_syscall);
}

static void CG_ParseScore( team_t team ) {
	int		i, j;
	int		offset;
	eth32.ocg.numScores = 0;	// Must be set here otherwise client numbers will mess up.

	if( team == TEAM_AXIS ) {
		offset = 4;
	} else {
		offset = 2;
	}

	eth32.cg.numScores = atoi( Syscall.CG_Argv( offset - 1 ) );

	for(j = 0; j < (eth32.cg.numScores); j++) {
		i = eth32.ocg.numScores;

		eth32.ocg.scores[i].client = atoi(			Syscall.CG_Argv( offset + 0 + (j*7)));
		eth32.ocg.scores[i].ping = atoi(			Syscall.CG_Argv( offset + 2 + (j*7)));

		if ( eth32.ocg.scores[i].client < 0 || eth32.ocg.scores[i].client >= MAX_CLIENTS ) {
			eth32.ocg.scores[i].client = 0;
		}

		eth32.ocg.numScores++;
	}
}

// solcrushr - modified flow of syscall
// later on we may not want to call original for things like adding smoke/scope shader/etc
int CDECL eth32_CG_syscall(int command, ...)
{
	int arg[10];
	va_list arglist;
	va_start(arglist, command);
	int count = 0;
	for (; count < 10; count++)
		arg[count] = va_arg(arglist, int);
	va_end(arglist);

	// we need to block some syscalls (i.e. drawing of sniper blackout)
	bool callOrig = true;
	int retval;

	switch (command)
	{
		case CG_R_ADDPOLYTOSCENE:
		{
			qhandle_t hShader = (qhandle_t) arg[0];
			if (hShader == eth32.cg.media.smokePuff) {
				if (!eth32.settings.smoketrnsp)
					return 0;

				Engine.AddPolyToScene((qhandle_t) arg[0], (polyVert_t *) arg[2]);
			}
		break;
		}
		case CG_R_ADDREFENTITYTOSCENE:
			Engine.AddRefEntity((refEntity_t*)arg[0]);
			break;
		case CG_R_RENDERSCENE:
		{
#ifdef ETH32_DEBUG
			static bool firstRun = true;
			if (firstRun && (uint32)arg[0] > (uint32)eth32.cg.module) {
				uint32 refdefAddr = (uint32)arg[0] - (uint32)eth32.cg.module;
				Debug.Log("Refdef found... [ 0x%x ] RefdefVA assumed... [ 0x%x ]", refdefAddr, refdefAddr + 0x1AC);
				firstRun = false;
			}
			refdef_t *rd = (refdef_t *) arg[0];
			VectorCopy(rd->viewaxis[0], eth32.cg.tempRenderVector);
#endif

			//kobject: TODO - make mod IS_SCOPED_WEAPON mod-safe
			//sol: think I might of resolved this
			if (eth32.cg.snap && eth32.game.cg_fov && IS_WEAPATTRIB(eth32.cg.snap->ps.weapon, WA_SCOPED) && (eth32.cgMod->type != MOD_TCE) && !eth32.settings.weaponZoom)
			{
				// modifying refdef values in PreFrame didn't do anything, so i had to put it in here
				refdef_t *refdef = (refdef_t *) arg[0];
				refdef->fov_x = eth32.game.cg_fov->value;
				float x = refdef->width / tan( refdef->fov_x / 360 * M_PI );
				refdef->fov_y = atan2( refdef->height, x );
				refdef->fov_y = refdef->fov_y * 360 / M_PI;
			}
			break;
		}
		case CG_GETSNAPSHOT:
		{
			int oldTime = 0;
			if (eth32.cg.snap)
				oldTime = eth32.cg.snap->serverTime;

			int ret = orig_syscall(command, arg[0], arg[1]);
			eth32.cg.snap = (snapshot_t*)arg[1];

			// Check if it's safe to vote again (To prevent excess of flood)
			// Mostly to be used in CG_TRANSLATE_STRING
			if (eth32.settings.autoComplaint) {
				static int lastVoteTime = 0;
				if (eth32.cg.cgTime - lastVoteTime > 1000) {
					eth32.cg.SafeVote = true;
					lastVoteTime = eth32.cg.cgTime;
				}
			}

			Engine.CorrectServerAngles();

			eth32.cg.ping = (eth32.cg.snap->serverTime - oldTime)*0.001;
			return ret;
		}
		case CG_PRINT: {
			char *analise_str = (char *)arg[0];

			if( !analise_str )
				break;

			if (eth32.settings.autoVote) {
				// Auto vote
				// Do nothing if we called the vote (to avoid canceling it).
				if (strstr(analise_str, va("%s^7 called a vote.  Voting for: ",eth32.cg.players[eth32.cg.clientNum].name)))
					break;
				// If we aren't the caller then vote no.
				else {
					for(int i=0; i<MAX_CLIENTS; i++) {
						if (i != eth32.cg.clientNum) {
							if (strstr(analise_str, va("%s^7 called a vote.  Voting for: ",eth32.cg.players[i].name))) {
								Syscall.CG_SendConsoleCommand("vote no\n");
								break;	// Exit after voting to prevent flood kick.
							}
						}
					}
				}
			}
			// Anti inactivity kick
			if (strstr(analise_str, "^310 seconds until inactivity drop!\n")) {
				Syscall.CG_SendConsoleCommand("+leanright; -leanright\n");
			}

			break;
		}
		case CG_TRANSLATE_STRING:
		{
			char *analise_str = (char *)arg[0];

			if( !analise_str )
				break;

			// Auto file complaint
			// Alexplay: The complaint message stays for a few ms even after we voted so we need
			// to check if it's safe to vote so we don't get kicked for excess of flood
			if (eth32.settings.autoComplaint) {
				if (strstr(analise_str, "File complaint against ")) {
					if (eth32.cg.SafeVote)
						Syscall.CG_SendConsoleCommand("vote yes\n");
					eth32.cg.SafeVote = false;	// Don't spam the command
				}
			}

			break;
		}
		case CG_GETSERVERCOMMAND:
		{
			// solcrushr - fixed: needed to call original before we check args
			int ret = orig_syscall(command, arg[0]);
			if (!arg[0])
				return ret;

			char *cmd = strdup(Syscall.CG_Argv(0));

			if (!cmd || !*cmd){
				if (cmd)
					free(cmd);
				return ret;
			}

			if (!strcmp(cmd, "gstats"))
				Engine.ParseWeaponStats();

			if (eth32.cgMod->type != MOD_ETPRO && eth32.settings.autoPredictBots) {
				if ( !strcmp( cmd, "sc0" ) ) {
					CG_ParseScore(TEAM_AXIS);
					return ret;
				} else if ( !strcmp( cmd, "sc1" ) ) {
					CG_ParseScore(TEAM_ALLIES);
					return ret;
				}
			}

			if ((!strcmp(cmd, "chat") || !strcmp(cmd, "tchat"))) {
				const char *s;
				if (atoi(Syscall.CG_Argv(3)))
					s = Syscall.CG_LocalizeServerCommand(Syscall.CG_Argv(1));
				else
					s = Syscall.CG_Argv(1);

				if (!strncmp(s, "^oss: ", 6)) {
					int num;
					sscanf(s, "^oss: %i", &num);
					if (num >= 0 && num < MAX_CLIENTS){
						Tools.auxSpecList[num] = true;
					}
				}

				if (!strncmp(s, "^oss_stop:", 10)) {
					int num;
					sscanf(s, "^oss_stop: %i", &num);
					if (num >= 0 && num < MAX_CLIENTS){
						Tools.auxSpecList[num] = false;
					}
				}
			}

			// handle server-side obits
			if (!eth32.cg.serverObits && !strcmp(cmd, "cpm")) {
				Engine.ParseServerSideObituary((char*)Syscall.CG_Argv(1));
			}
			// prepare for pk3 unlocking
			else if (!strcmp(cmd, "spawnserver"))
				Hook.hookGame(true);

			free(cmd);

			return ret;
		}
		case CG_GETGAMESTATE:
		{
			if (!eth32.cg.gameState)
				eth32.cg.gameState = (gameState_t *) arg[0];

			/*for (int i=0; i<MAX_CLIENTS; i++) {
				const char *info = Engine.ConfigString(i + CS_PLAYERS);
				eth32.cg.players[i].Class = atoi(Info_ValueForKey( info, "c" ));
			}*/
			const char *info;
			for (int i=0; i<MAX_CLIENTS; i++) {
				int offset = eth32.cg.gameState->stringOffsets[i + CS_PLAYERS];
				if (offset != 0) {
					info = Info_ValueForKey((char *)(eth32.cg.gameState->stringData + offset), "c" );
					eth32.cg.players[i].Class = atoi(info);
				}
			}
			break;
		}
#ifdef ETH32_PRELIM_OFFSETS
		// searching for lerpOrigin offset
		case CG_CM_BOXTRACE:
		{
			static bool found = false;

			if (!found) {
				uint32 startAddr = (uint32)arg[1];

				for (int i=0 ; i < MAX_CLIENTS ; i++) {
					uint32 centStart = (uint32)eth32.cg.players[i].cent;
					uint32 centEnd = centStart + eth32.cgMod->centSize;

					if (startAddr > centStart && startAddr < centEnd) {
						Debug.Log("Found lerpOrigin offset: 0x%x", startAddr - centStart);
						found = true;
						break;
					}
				}
			}
			break;
		}
#endif
		case CG_R_DRAWSTRETCHPIC:
			callOrig = Draw.CheckPic(*(float *)&arg[0], *(float *)&arg[1], *(float *)&arg[2], *(float *)&arg[3], *(float *)&arg[4], *(float *)&arg[5], *(float *)&arg[6], *(float *)&arg[7], (qhandle_t)arg[8]);
			break;
		case CG_CVAR_REGISTER:
			if (arg[3] & CVAR_CHEAT)
				arg[3] &= ~CVAR_CHEAT;
			if (arg[3] & CVAR_ROM)
				arg[3] &= ~CVAR_ROM;

			callOrig = !Tools.SkipCgameCvar((vmCvar_t*)arg[0], (const char*)arg[1]);
			break;
		case CG_CVAR_SET:
			// jay mac sp00f
			if (!strcmp((char *)arg[0], "cl_mac")){
				if (strlen(eth32.settings.jayMac) == 1 && eth32.settings.jayMac[0] == '0')
					sprintf(eth32.settings.jayMac, "%02x-%02x-%02x-%02x",  rand() % 256,  rand() % 256,  rand() % 256,  rand() % 256);
				arg[1] = (uint32) &eth32.settings.jayMac[0];
				break;
			} else if (Tools.userCvarList) {
				cvarInfo_t *c = Tools.userCvarList;
				while (c) {
					if (!strcasecmp(c->name, (char *)arg[0])){
						arg[1] = (uint32)c->ourValue;
						break;
					}
					c = c->next;
				}
			}
			break;
		case CG_SETUSERCMDVALUE:
			Gui.LimbopanelStatus(arg[1] != 0x0);

			if (Aimbot.SensitivityLocked()){
				arg[2] = 0x0;
				break;
			} else  if (eth32.cg.currentWeapon && (eth32.cg.currentWeapon->attribs & WA_SCOPED) && eth32.settings.weaponZoom) {
				*(float *) &arg[2] = eth32.settings.scopedTurnSpeed;
			} else {
				*(float *) &arg[2] = 1.0f;
			}
			break;
		case CG_S_REGISTERSOUND:
			return Sounds.HookSounds((char*)arg[0], arg[1]);
		case CG_S_STARTSTREAMINGSOUND: {
			const char *snd = (const char *) arg[0];
			if( !snd )
				break;

			// Don't play HQ sounds
			if( !eth32.settings.hqSounds && strstr(snd,"hq_") )
				return 0;

			break;
		}
		case CG_S_STARTSOUND:
		case CG_S_STARTSOUNDEX:
		case CG_S_ADDREALLOOPINGSOUND:
		case CG_S_ADDLOOPINGSOUND:
		case CG_S_STARTLOCALSOUND:
			callOrig = Sounds.Process(command, arg);
			break;
	}

	if (callOrig)
		retval = orig_syscall(command, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9]);
	else return 0;

	switch (command) {
		case CG_R_REGISTERSHADERNOMIP:
		{
			if (!strcmp((char*)arg[0], "ui/cursor/3_cursor3")) {
				//printf("CG_R: %i\n", retval);
				eth32.cg.media.cursor = retval;
			}
			break;
		}
		case CG_R_CLEARSCENE:
			Engine.GentityDecal();
			break;
	}

	return retval;
}

int CDECL eth32_CG_vmMain(int command, ...)
{
	int arg[12];
	int ret;
	va_list arglist;
	va_start(arglist, command);
	int count = 0;
	for (; count < 12; count++)
		arg[count] = va_arg(arglist, int);
	va_end(arglist);

	switch (command)
	{
		case CG_INIT: {
			eth32.game.cgvm = *eth32.game.vm;
			Hook.InitCGameHook();
			Engine.CG_Init_Pre(arg[0], arg[1], arg[2]);
			break;
		}
		case CG_SHUTDOWN:
			return Engine.CG_Shutdown();
		case CG_DRAW_ACTIVE_FRAME:{
			return Engine.CG_DrawActiveFrame(arg[0], arg[1], arg[2]);
		}
	}

	ret = orig_CG_vmMain(command, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11]);

	// postcall stuff
	switch (command) {
		case CG_INIT:
			Engine.CG_Init_Post(arg[0], arg[1], arg[2]);
			eth32.cg.hasInit = true;
			break;
	}

	return ret;
}

int CDECL eth32_UI_vmMain(int command, ...)
{
	int arg[12];
	int ret;
	va_list arglist;
	va_start(arglist, command);
	int count = 0;
	for (; count < 12; count++)
		arg[count] = va_arg(arglist, int);
	va_end(arglist);

	switch (command)
	{
		case UI_INIT:
			eth32.game.uivm = *eth32.game.vm;
			break;
	}

	ret = orig_UI_vmMain(command, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11]);

	return ret;
}

void CDECL eth32_CG_SetLerpFrameAnimationRate(centity_t *cent, clientInfo_t *ci, lerpFrame_t *lf, int anim)
{
	orig_CG_SetLerpFrameAnimationRate(cent, ci, lf, anim);

	if (cent->currentState.number >= 0 && cent->currentState.number < MAX_CLIENTS)
		eth32.cg.players[cent->currentState.number].lf = lf;

#ifdef ETH32_PRELIM_OFFSETS
	static bool found = false;
	static int iPrevClientNum = -1;
	static uint32 dwPrevCi = 0;

	if (!found) {
		if (!eth32.cgMod->centBase || !eth32.cgMod->centSize) {
			Debug.Log("Unable to calc clientinfo (Cent base & size not defined)");
			found = true;
			return;
		}

		int iNewClientNum = Tools.ClientNumForAddr((uint32)cent);

		if (iPrevClientNum < 0) {
			iPrevClientNum = iNewClientNum;
			dwPrevCi = (uint32)ci;
		}
		else if (iNewClientNum != iPrevClientNum) {
			uint32 dwCiSize;
			uint32 dwCiBase;
			uint32 dwNewCi = (uint32)ci;

			if (iNewClientNum < iPrevClientNum)
				dwCiSize = (dwPrevCi - dwNewCi) / (iPrevClientNum - iNewClientNum);
			else dwCiSize = (dwNewCi - dwPrevCi) / (iNewClientNum - iPrevClientNum);
				dwCiBase = dwNewCi - (iNewClientNum * dwCiSize) - (uint32)eth32.cg.module;

			Debug.Log("Found clientinfo... [ Base: 0x%x ] [ Size: 0x%x ]", dwCiBase, dwCiSize);

			found = true;
			return;
		}
	}
#endif // ETH32_PRELIM_OFFSETS
}

void CDECL eth32_CG_FinishWeaponChange(int lastweap, int newweap)
{
	if ((eth32.cgMod->type != MOD_TCE) && IS_WEAPATTRIB(lastweap, WA_SCOPED) && IS_WEAPATTRIB(newweap, WA_UNSCOPED) && !(Syscall.isKeyActionDown("+reload") || Syscall.isKeyActionDown("weapalt")))
			return;

	// reset autofire/senslock on weapon change
	Aimbot.Autofire(false);
	Aimbot.LockSensitivity(false);

	if (eth32.cg.weaponList && eth32.cg.snap && newweap < eth32.cg.numWeapons)
		eth32.cg.currentWeapon = &eth32.cg.weaponList[newweap];
	else
		eth32.cg.currentWeapon = &nullWeapon;

	//Engine.SetWeaponSpread();

	orig_CG_FinishWeaponChange(lastweap, newweap);
}

void CDECL eth32_CG_Respawn(qboolean revived)
{
#ifdef ETH32_PRELIM_OFFSETS
	offset_pmext = true;
#endif

	orig_CG_Respawn(revived);

#ifdef ETH32_PRELIM_OFFSETS
	offset_pmext = false;
#endif

	// reset aimbot
	Aimbot.Respawn();

	//Sounds.Respawn();
}

void CDECL eth32_CG_DrawPlayerStatusHead(void)
{
	if(eth32.settings.drawHackVisuals && !eth32.settings.guiOriginal) return;
	else orig_CG_DrawPlayerStatusHead();
}

void CDECL eth32_CG_DrawPlayerStats(void)
{
	if(eth32.settings.drawHackVisuals && !eth32.settings.guiOriginal) return;
	else orig_CG_DrawPlayerStats();
}

void CDECL eth32_CG_DrawPlayerStatus(void)
{
	if(eth32.settings.drawHackVisuals && !eth32.settings.guiOriginal) return;
	else orig_CG_DrawPlayerStatus();
}

//kobject: what about TCE?
void CDECL eth32_CG_ChargeTimesChanged(void)
{
	// NoQuarter changed CS_CHARGETIMES
	int index = (eth32.cgMod->type == MOD_NOQUARTER) ? 0x23 : 0x26;
	const char *info = Engine.ConfigString(index);

	eth32.cg.chargeTimes.soldier[0] = atoi(Info_ValueForKey( info, "axs_sld" ));
	eth32.cg.chargeTimes.soldier[1] = atoi(Info_ValueForKey( info, "ald_sld" ));
	eth32.cg.chargeTimes.medic[0] = atoi(Info_ValueForKey( info, "axs_mdc" ));
	eth32.cg.chargeTimes.medic[1] = atoi(Info_ValueForKey( info, "ald_mdc" ));
	eth32.cg.chargeTimes.engineer[0] = atoi(Info_ValueForKey( info, "axs_eng" ));
	eth32.cg.chargeTimes.engineer[1] = atoi(Info_ValueForKey( info, "ald_eng" ));
	eth32.cg.chargeTimes.lt[0] = atoi(Info_ValueForKey( info, "axs_lnt" ));
	eth32.cg.chargeTimes.lt[1] = atoi(Info_ValueForKey( info, "ald_lnt" ));
	eth32.cg.chargeTimes.covops[0] = atoi(Info_ValueForKey( info, "axs_cvo" ));
	eth32.cg.chargeTimes.covops[1] = atoi(Info_ValueForKey( info, "ald_cvo" ));

	orig_CG_ChargeTimesChanged();
}

// **************************
// ui funcs
// **************************

void CHook::hookUi(bool state)
{
	static bool hooked = false;

	// TODO: WTF?! why UI loading sometimes fails?
	if (state && eth32.ui.handle)
	{
		#ifdef ETH32_DEBUG
			Debug.Log("Hooking ui functions");
		#endif

		CCommands.RegisterCommands();
		hooked = true;
	}
	else if (!state && eth32.ui.handle)
	{
		#ifdef ETH32_DEBUG
			Debug.Log("Un-Hooking ui functions");
		#endif

		hooked = false;
	}
}

void CDECL eth32_UI_dllEntry(int (*syscallptr)(int arg,... ))
{
	#ifdef ETH32_DEBUG
		Debug.Log("UI_dllEntry(): 0x%x", syscallptr);
	#endif
	orig_UI_dllEntry(eth32_UI_syscall);
}

int CDECL eth32_UI_syscall(int command, ...)
{
	int arg[10];
	va_list arglist;
	va_start(arglist, command);
	int count = 0;
	for (; count < 10; count++)
		arg[count] = va_arg(arglist, int);
	va_end(arglist);

	//printf("UI_Ssycall %i %p\n", command, orig_syscall);

	switch (command)
	{
		case UI_R_DRAWSTRETCHPIC:
		{
			if (eth32.cg.media.cursor == (qhandle_t) arg[8])
			{
				Gui.SetMouse((float*) &arg[0] , (float*) &arg[1], &arg[8]);
			}
			break;
		}
		case UI_CVAR_REGISTER:
		{
			if (arg[3] & CVAR_CHEAT)
				arg[3] &= ~CVAR_CHEAT;
			if (arg[3] & CVAR_ROM)
				arg[3] &= ~CVAR_ROM;
			break;
		}
		default:
			break;
	}

	int retval = orig_syscall(command, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9]);

	switch (command)
	{
		case UI_R_REGISTERSHADERNOMIP:
		{
			if (!strcmp((char*)arg[0], "ui/cursor/3_cursor3")) {
				//printf("UI_R: %i\n", retval);
				eth32.cg.media.cursor = retval;
			}
		}
	}

	return retval;
}

int eth32_CG_CalcViewValues(void)
{
	int ret;
	if (eth32.settings.origViewValues || eth32.settings.dmgFeedback || eth32.cgMod->type == MOD_ETPRO || eth32.cgMod->type == MOD_TCE)
		ret = orig_CG_CalcViewValues();
	else if ((eth32.cg.currentWeapon && eth32.cg.currentWeapon->attribs & WA_SCOPED))
		ret = orig_CG_CalcViewValues();
	else
		ret = Engine.CalcViewValues();

	// set forward here for quick access and to prevent calculting it elsewhere
	Engine.forward = eth32.cg.refdef->viewaxis[0];
	return ret;
}

cvar_t* CDECL eth32_Cvar_Set2( const char *var_name, const char *value, qboolean force )
{
	if (Tools.userCvarList){
		cvarInfo_t *c = Tools.userCvarList;
		while (c) {
			if (!strcasecmp(c->name, var_name)){
				value = (const char *) c->ourValue;
				force = qtrue;
				break;
			}
			c = c->next;
		}
	}

	return orig_Cvar_Set2(var_name, value, force);
}

void CDECL eth32_LoadTGA( const char *name, byte **pic, int *width, int *height)
{
	if (!strcmp(name,"ui/assets/et_logo_huge.tga"))
	{
		strcpy((char *)name, "gfx/2d/eth32_logo_huge.tga");
		orig_LoadTGA(name, pic, width, height);

		// see if image failed to load (for xample wrong/bad pk3)
		if (!*pic){
			strcpy((char *)name, "ui/assets/et_logo_huge.tga");
			orig_LoadTGA(name, pic, width, height);
		}
	}
	else if (!strcmp(name,"ui/assets/et_logo.tga"))
	{
		strcpy((char *)name, "gfx/2d/eth32_logo.tga");
		orig_LoadTGA(name, pic, width, height);

		// see if image failed to load (for xample wrong/bad pk3)
		if (!*pic){
			strcpy((char *)name, "ui/assets/et_logo.tga");
			orig_LoadTGA(name, pic, width, height);
		}
	}
	else if (!strcmp(name,"ui/assets/background_mask.tga"))
	{
		strcpy((char *)name, "gfx/2d/eth32_background.tga");
		orig_LoadTGA(name, pic, width, height);

		// see if image failed to load (for xample wrong/bad pk3)
		if (!*pic){
			strcpy((char *)name, "ui/assets/background_mask.tga");
			orig_LoadTGA(name, pic, width, height);
		}
	}

	orig_LoadTGA(name, pic, width, height);

	// Remove smoke particles when hitting players
	if (eth32.settings.removeParticles) {
		if (!strcmp(name,"gfx/misc/smokepuff.tga"))
 		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
 			if (!*pic){
				strcpy((char *)name, "gfx/misc/smokepuff.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
	}

	// Foliage Removal
	if (eth32.settings.removeFoliage) {
 		if (!strcmp(name,"models/mapobjects/plants_sd/grass_green1.tga"))
 		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
 			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/grass_green1.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/grassfoliage1.tga"))
		{
 			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
 				strcpy((char *)name, "models/mapobjects/plants_sd/grassfoliage1.tga");
 				orig_LoadTGA(name, pic, width, height);
 			}
		}
		if (!strcmp(name,"models/mapobjects/tree_temperate_sd/leaves_temperate1.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/tree_temperate_sd/leaves_temperate1.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/tree_temperate_sd/leaves_temperate2.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/tree_temperate_sd/leaves_temperate2.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/tree_temperate_sd/leaves_temperate3.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/tree_temperate_sd/leaves_temperate3.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/tree_temperate_sd/trunk_temperate.tga"))
		{
			strcpy((char *)name, "gfx/misc/temperate_burn.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/tree_temperate_sd/trunk_temperate.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/tree_desert_sd/palm_leaf1.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/tree_desert_sd/palm_leaf1.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/tree_desert_sd/palm_trunk.tga"))
		{
			strcpy((char *)name, "gfx/misc/palm_burn.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/tree_desert_sd/palm_trunk.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/trees_sd/winterbranch01.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/trees_sd/winterbranch01.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/trees_sd/wintertrunk01.tga"))
		{
			strcpy((char *)name, "gfx/misc/winter_burn.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/trees_sd/wintertrunk01.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/bush_desert1.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/bush_desert1.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/bush_desert2.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/bush_desert2.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/deadbranch2.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/deadbranch2.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/deadbranch1.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/deadbranch1.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/deadbranch1_damp.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/deadbranch1_damp.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/deadbranch3.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/deadbranch3.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/grass_dry3.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/grass_dry3.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/catail2.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/catail2.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
		if (!strcmp(name,"models/mapobjects/plants_sd/catail1.tga"))
		{
			strcpy((char *)name, "gfx/2d/nullshader.tga");
			orig_LoadTGA(name, pic, width, height);

			// see if image failed to load (for xample wrong/bad pk3)
			if (!*pic){
				strcpy((char *)name, "models/mapobjects/plants_sd/catail1.tga");
				orig_LoadTGA(name, pic, width, height);
			}
		}
	}
}

void CDECL eth32_etproAntiCheat(void *a, void *b, void *c, int checksum, void *e, char *orig_guid)
{
#ifdef ETH32_DEBUG
	Debug.Log("Etpro AC: %p %p %p 0x%X %p %p", a,b,c,checksum,e,orig_guid);
	Debug.Log("Etpro AC: orig guid %s", orig_guid);
#endif

	if (eth32.settings.etproGuid && strlen(eth32.settings.etproGuid) > 1){
#ifdef ETH32_DEBUG
		Debug.Log("setting custom guid");
#endif
			// to avoid being kicked by lua scripts that check for non-hex chars in guid
		memset(orig_guid, '1', 40);
		orig_guid[40] = '\0';
		strncpy(orig_guid, eth32.settings.etproGuid, 41);
	} else {
			// assume first time set, randomize etpro guid, and save it.
		sprintf(eth32.settings.etproGuid, "%08X%08X%08X%08X%08X",
				(uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF), (uint32)(random()*0xFFFFFFFF));
		strncpy(orig_guid, eth32.settings.etproGuid, 41);
	}

	// Backup a,b,c,e args for later setting of guid/os
	etpro_A = a;
	etpro_B = b;
	etpro_C = c;
	etpro_E = e;

	orig_etproAntiCheat(a, b, c, eth32.settings.etproOs ? etpro_win32 : etpro_linux, e, eth32.settings.etproGuid);
}

bool cvarValid(const char *var_name, pb_cvar_t *cv)
{
	char buffer[64];
	int command = UI_VM ? UI_CVAR_VARIABLESTRINGBUFFER : CG_CVAR_VARIABLESTRINGBUFFER;
	orig_syscall(command, var_name, buffer, 64);
	if (cv->type == PB_CVAR_IN) {
		if (atof(buffer) > cv->low && atof(buffer) < cv->high){
			return true;
		}
	} else if (cv->type == PB_CVAR_OUT) {
		if (atof(buffer) < cv->low || atof(buffer) > cv->high){
			return true;
		}
	}
	return false;
}

char *eth32_Cvar_VariableString(const char *var_name)
{
	static char buffer[64];
	static bool hasWarned = false;

	if (!eth32.cgameLoaded || !strcmp(var_name, "r_glDriver") || !strcmp(var_name, "cl_profile") || !strcmp(var_name, "gamestate"))
		return orig_Cvar_VariableString(var_name);

	if (eth32.pb.statusOK) {
		if (!strcmp(var_name, "cg_thirdperson") || !strcmp(var_name, "r_singleshader") || !strcmp(var_name, "r_shownormals")) {
			buffer[0] = '0';
			buffer[1] = '\0';
			return buffer;
		}

		if (!strcmp(var_name, "r_wolffog") || !strcmp(var_name, "r_drawfoliage")) {
			buffer[0] = '1';
			buffer[1] = '\0';
			return buffer;
		}

		pb_cvar_t *cv = *eth32.pb.cvarlist;

		if (cv && eth32.pb.nCvars && *eth32.pb.nCvars) {
			for (int n=0; n<*eth32.pb.nCvars; n++) {
				if (!strcmp(var_name, cv->name)) {
					//char *ignoreme = orig_Cvar_VariableString(var_name);
					if (cv->type == PB_CVAR_IN) {
						if (!cvarValid(var_name, cv)) {
							snprintf(buffer, sizeof(buffer), "%f", cv->low);
							return buffer;
						}
					} else if (cv->type == PB_CVAR_OUT) {
						if (!cvarValid(var_name, cv)) {
							snprintf(buffer, sizeof(buffer), "%f", cv->high+1.0);
							return buffer;
						}
					} else if (cv->type == PB_CVAR_INCLUDE) {
						snprintf(buffer, sizeof(buffer), "%s", cv->string);
						return buffer;
					} else if (cv->type == PB_CVAR_EXCLUDE) {

					} else {
						if (!hasWarned) {
							hasWarned = true;
							Tools.Print("^1Warning: unknown PB cvar restriction type. Report this.\n");
						}
					}
					break;
				}
				cv = (pb_cvar_t *)((uint32)*eth32.pb.cvarlist+(n+1)*sizeof(pb_cvar_t));
			}
		}
	}

	return orig_Cvar_VariableString(var_name);
}

// kobject: special care must be taken to not accidentally unhook CG_CalculateReinfTime
// while its trampoline is already freed
int CDECL eth32_CG_CalculateReinfTime(qboolean menu)
{
	//Debug.Log("eth32_CG_CalculateReinfTime");
	eth32.cg.bools.spawntimerReady = true;
	REMOVE_CG_HOOK(CG_CalculateReinfTime)
	orig_CG_CalculateReinfTime = (CG_CalculateReinfTime_t) ((uint32) eth32.cg.module + eth32.cgMod->CG_CalculateReinfTime);
	return orig_CG_CalculateReinfTime(menu);
}


// kobject: CG_Player directly sets up head/body refents and adds them, so its safe to use these checks in there
void CDECL eth32_CG_Player(centity_t *cent)
{
	// Using CG_Player to filter valid players for us
	Engine.StartPlayerCapture(cent);
	orig_CG_Player(cent);
	Engine.EndPlayerCapture();
}

void CDECL eth32_CG_AddPlayerWeapon(refEntity_t *parent, playerState_t *ps, centity_t *cent)
{
	Engine.SetDrawWeapon(true);
	orig_CG_AddPlayerWeapon(parent, ps, cent);
	Engine.SetDrawWeapon(false);
}

void CDECL eth32_CG_PositionRotatedEntityOnTag(refEntity_t *entity, const refEntity_t *parent, const char *tagName)
{
	orig_CG_PositionRotatedEntityOnTag(entity, parent, tagName);

	if (!strcmp(tagName, "tag_head")){
		Engine.CapturePlayerHead(entity);
	}
}

void CDECL eth32_CG_EntityEvent(centity_t *cent, vec3_t position)
{
	entityState_t *es;
	es = &cent->currentState;

	int event;
	event = es->event & ~EV_EVENT_BITS;

	switch(event)
	{
	case EV_FALL_SHORT:
        case EV_FALL_DMG_10:
        case EV_FALL_DMG_15:
        case EV_FALL_DMG_25:
        case EV_FALL_DMG_50:
        	if (es->number == eth32.cg.clientNum)
                	return;
	case EV_SHAKE:
		return;
	case EV_OBITUARY:
		Engine.EV_Obituary(es);
		break;
	case EV_BULLET_HIT_WALL:
		Engine.EV_Bullet(es, false);
		break;
	case EV_BULLET_HIT_FLESH:
		Engine.EV_Bullet(es, true);
		break;
	case EV_POPUPMESSAGE:
		if (es->effect1Time == PM_TEAM && es->density == 1) { // client disconnected
			int num = Tools.ClientNumForAddr((uint32)cent);
			if (num >= 0 && num < MAX_CLIENTS) // sanity check
				memset(&eth32.cg.players[num].stats, 0, sizeof(stats_t)); // clear stats on disconnecting client
		}
		break;
	default:
		break;
	}

	orig_CG_EntityEvent(cent, position);
}

void CDECL eth32_CG_Trace(trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipNumber, int mask)
{
	orig_CG_Trace(result, start, mins, maxs, end, skipNumber, mask);

	// TC:E checks if a player is visible to avoid drawing invisible ones
	// so we make it think it's visible
	if (eth32.cg.snap && skipNumber == eth32.cg.snap->ps.clientNum)
		result->fraction = 1.0f;
}

void CDECL eth32_CG_DamageFeedback(int yawByte, int pitchByte, int damage)
{
	// Hmm... did we forget to kick the player's view when hit? oops...
	if (eth32.settings.dmgFeedback)
		orig_CG_DamageFeedback(yawByte, pitchByte, damage);
}

void CDECL eth32_CG_WeaponFireRecoil(centity_t *cent){}

