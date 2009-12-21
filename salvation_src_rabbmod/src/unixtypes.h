// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
#pragma once

#include <time.h>
#include <signal.h>

#define Sleep(x)	usleep(x*1000)
#define Time() 		((uint32) time(NULL))
#define ON_VISUALS	if(eth32.settings.drawHackVisuals)

// some helper macros
#define REMOVE_CG_HOOK(func) \
		if (orig_##func){ \
			Detour.RemoveDetour((void *)((uint32)eth32.cg.module+eth32.cgMod->func), (void *)orig_##func); \
			orig_##func = NULL; \
}

#define CG_HOOK(func) \
		if(eth32.cgMod->func) {\
		orig_##func = (func##_t)Detour.CreateDetour((void *)((uint32)eth32.cg.module+eth32.cgMod->func), (void *)&eth32_##func); \
}

#define FATAL_ERROR(format, ...)	\
{\
	fprintf (stderr, "Fatal error: %s:%d (%s) -> ", __FILE__, __LINE__, __FUNCTION__ );	\
	fprintf (stderr, format, ## __VA_ARGS__);	\
	fprintf (stderr, "\n");\
	raise(SIGABRT); \
}

// easy vec3 print
static char ___buffer[8192];
char *pv(float *x, int n);
#define pv1(x) pv(x, 1)
#define pv2(x) pv(x, 2)
#define pv3(x) pv(x, 3)

// translated mouse events
typedef enum {
	WM_LBUTTONDOWN,
	WM_RBUTTONDOWN,
	WM_LBUTTONUP,
	WM_RBUTTONUP
} WM_trans;
