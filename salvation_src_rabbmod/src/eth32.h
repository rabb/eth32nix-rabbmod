// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

#define NEED_DLL	2

#include <stdio.h>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <pthread.h>
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <vector>

using namespace std;

// sdk
#include "sdk/src/cgame/cg_local.h"
#include "sdk/src/ui/ui_public.h"
#include "sdk/src/qcommon/vm_local.h"
#define PASSFLOAT(x) (*(int*)&x)

// some code snippets found on net
#include "tools/StringTokenizer/StringTokenizer.h"	// http://www.partow.net/programming/stringtokenizer/index.html

// eth32 headers
#include "types.h"
#include "funchooks.h"
#include "filters.h"
#include "CAimbot.h"
#include "CDraw.h"
//#include "CDebug.h"
#include "CDetour.h"
#include "CEngine.h"
#include "CGui.h"
#include "CHook.h"
//#include "CNetwork.h"
//#include "CGNetwork.h"
//#include "CPortal.h"
#include "CSettings.h"
#include "CSpectator.h"
#include "CSyscall.h"
#include "CTools.h"
#include "CCam.h"
#include "CXCommands.h"
#include "CSounds.h"

// eth32 defines
//#define ETH32_VERSION	"OVERDOSE"
#define ETH32_SETTINGS	"settings.ini"
#define ETH32_OFFSETS	"offsets.ini"

// kobject: these are used troughout, so just put it here in a central place
static vec3_t xAxis = {1,0,0};
static vec3_t yAxis = {0,1,0};
static vec3_t zAxis = {0,0,1};

//extern CDebug Debug;
extern CDetour Detour;

extern void UpdateKeyTable(uint32 key, int down);
extern uint32 GetAsyncKeyState(int key);

#ifdef ETH32_PRELIM_OFFSETS
	extern bool	offset_pmext;
	extern bool	offset_cginit;
#endif

