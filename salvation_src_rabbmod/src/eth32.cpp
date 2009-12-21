// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

// lib constructor/destructor, various linux chores

#include "eth32.h"
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <strings.h>

//CDebug Debug;
eth32_t eth32;
#ifdef ETH32_PRELIM_OFFSETS
	bool offset_pmext = false;
	bool offset_cginit = false;
#endif

uint32 key_state[256];

void __attribute__ ((constructor)) eth32Main()
{
	// fetch program name
	char link[PATH_MAX];
	memset(link, 0, sizeof(link));
	if (readlink("/proc/self/exe", link, sizeof(link)) <= 0)
		exit(1);

	// if exe crc32 matches known version of ET, go forward
	int i;
	Tools.initCrc32();
	memset(&key_state, 0, sizeof(key_state));

#ifdef ETH32_DEBUG
	Debug.Init();
	Debug.SetLogFile("/tmp/eth32.log");
#endif
	if ((i = Hook.isGameSupported(link)) >= 0)
		Tools.Init(i);
}

// a hack to prevent ET from swallowing core dumps
__attribute__ ((visibility ("default")))
__sighandler_t signal(int __sig, __sighandler_t __handler)
{
	typedef __sighandler_t (*signal_t)(int __sig, __sighandler_t __handler);
	static signal_t orig_signal = NULL;
	if (!orig_signal)
		orig_signal = (signal_t)dlsym(RTLD_NEXT, "signal");
	return orig_signal(__sig, __handler);	
}

#ifdef ETH32_PRELIM_OFFSETS
__attribute__ ((visibility ("default")))
		void *memset(void *s, int c, size_t n)
{
	typedef void *(*memset_t)(void *s, int c, size_t n);
	static memset_t orig_memset = NULL;
	if (!orig_memset)
		orig_memset = (memset_t)dlsym(RTLD_NEXT, "memset");

	if (offset_cginit){
		uint32 offset = (uint32)s-(uint32)eth32.cg.module;
		if (n > 0x1c0000 && n < 0x2e8000) {
			Debug.Log("memset: possible 1024-sized cent found: 0x%x size=0x%x (0x%x)\n", offset, n, n/0x400);
		}

		if (n > 0x380000 && n < 0x5d0000) {
			Debug.Log("memset: possible 2048-sized cent found: %0x%x size=0x%x (0x%x)\n", offset, n, n/0x800);
		}
	}

	if (offset_pmext){
		uint32 offset = (uint32)s-(uint32)eth32.cg.module;
		Debug.Log("memset: possible cg.pmext found: 0x%x size=0x%x", offset, n);
	}

	return orig_memset(s,c,n);
}
#endif

void __attribute__ ((destructor)) eth32End(void)
{
	Tools.Shutdown();
}

typedef struct {
	uint32 Xkey;
	uint32 Vkey;
} keyTable_t;

keyTable_t key_table[] =
{
	{XK_Escape, 0},
	{XK_F5, 1},
	{XK_F6, 2},
	{XK_F7, 3},
	{XK_F8, 4},
	{XK_F9, 5},
	{XK_F10, 6},
	{XK_F11, 7},
	{XK_F12, 8},
	{XK_Left, 9},
	{XK_Right, 10},
 	{XK_Insert, 11},

  	{0xffffff, 0}
};

void UpdateKeyTable(uint32 key, int down)
{
	int i;
	for(i=0; i<256; i++) {
		if (key_table[i].Xkey == key) {
			key_state[key_table[i].Vkey] = down;
			return;
		}
		if (key_table[i].Xkey == 0xffffff)
			return;
	}
}

uint32 GetAsyncKeyState(int key)
{
	int i;
	for(i=0; i<256; i++) {
		if (key_table[i].Xkey == key){
			int state = key_state[key_table[i].Vkey];
			if (state)
				key_state[key_table[i].Vkey] = 0;
			return state;
		}
		if (key_table[i].Xkey == 0xffffff)
			return 0;
	}
}

// pv: shortcut for printing vec3_t's to precision 'n'
char *pv(float *x, int n)
{
	char buf[32];
	sprintf(buf, "{%c.%if, %c.%if, %c.%if}", '%', n, '%', n, '%', n);
	sprintf(___buffer, buf, x[0], x[1], x[2]);
	return ___buffer;
}
