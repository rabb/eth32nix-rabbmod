// ETH32nix - an Enemy Territory cheat for Linux
// Copyright (c) 2008 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include "puresounds.h"

CSounds Sounds;

int csnd_hash[8192];

void CSounds::Init()
{
	gameSounds = NULL;
	nGameSounds = 0;
	memset(csnd_hash, 0, sizeof(csnd_hash));
}

void CSounds::ShutDown()
{
	if (gameSounds) free(gameSounds);
	nGameSounds = 0;
}

// all legit sounds pass trough here, therefore,
// sounds that dont exist csnd_hash are eth32 sounds
int CSounds::HookSounds(char *name, int compressed)
{
	if (!name)
		return 0;

	gameSounds = (eth32Sounds_t *)realloc(gameSounds, (nGameSounds+1)*sizeof(eth32Sounds_t));

	sfxHandle_t snd = Syscall.S_RegisterSound(name, (qboolean)compressed);

	int j = 0;
	gameSounds[nGameSounds].pure = false;
	while (pureSounds[j]) {
		if (!strcmp(pureSounds[j], name)){
			gameSounds[nGameSounds].pure = true;
			break;
		}
		j++;
	}

	csnd_hash[(int)snd] = nGameSounds;
	nGameSounds++;
	return snd;
}

bool CSounds::Process(int type, int *args)
{
	if (!gameSounds)
		return true;

	int sndHnd = (type == CG_S_STARTLOCALSOUND) ? args[0] : args[3];
	eth32Sounds_t *snd = &gameSounds[csnd_hash[sndHnd]];

	if (eth32.settings.pureSounds && !snd->pure)
		return false;

	return true;
}

void CSounds::PlaySpreeSounds() {	
	if (eth32.settings.randomSpreeSounds) {	// Random Spree sounds
		if ((eth32.cg.cgTime - eth32.cg.lastKillTime) < (eth32.settings.mkResetTime)) {
			typedef struct { int start; int end; } spreeLevel_t;
			#define SPREE_LEVEL_SIZE 4
			spreeLevel_t spreeLevels[SPREE_LEVEL_SIZE] = {
				{ SOUND_DOUBLEKILL1,	SOUND_PERFECT }	,	// Spree level 1
				{ SOUND_GODLIKE1,	SOUND_TRIPLEKILL },
				{ SOUND_DOMINATING1,	SOUND_MEGAKILL },
				{ SOUND_MONSTERKILL1,	SOUND_WICKEDSICK }	// Spree level 4
			};
			int spreeLevelMax = SPREE_LEVEL_SIZE - 1;

			// Modify level+sound values to fit to spreeLevels_t and eth32.sounds order
			int level = eth32.cg.killSpreeCount - 2; // never '< 0' because first time here is with 2 kills
			if (level > spreeLevelMax)
				level = spreeLevelMax;
			int levelSize = spreeLevels[level].end - spreeLevels[level].start;
			int sound = (int)((float)(levelSize + 1) * rand() / (RAND_MAX + 1.0f));
			sound += spreeLevels[level].start;
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[sound], CHAN_LOCAL_SOUND );
		}
	} 
	else { // Sequential Spree sounds
		static int oldSpree;
		// Multikills
		if (eth32.cg.killSpreeCount == 2)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_DOUBLEKILL1], CHAN_LOCAL_SOUND ); 
		else if (eth32.cg.killSpreeCount == 3)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_TRIPLEKILL], CHAN_LOCAL_SOUND );
		else if (eth32.cg.killSpreeCount == 4)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_MULTIKILL3], CHAN_LOCAL_SOUND );
		else if (eth32.cg.killSpreeCount == 5)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_MEGAKILL], CHAN_LOCAL_SOUND );
		else if (eth32.cg.killSpreeCount == 6)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_ULTRAKILL1], CHAN_LOCAL_SOUND );
		else if (eth32.cg.killSpreeCount == 7)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_MONSTERKILL2], CHAN_LOCAL_SOUND );
		else if (eth32.cg.killSpreeCount == 8)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_UNREAL], CHAN_LOCAL_SOUND );
		else if (eth32.cg.killSpreeCount >= 9)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_HOLYSHIT], CHAN_LOCAL_SOUND );
		
		// Current Spree	
		if (eth32.cg.self->stats.spree == 5)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_KILLINGSPREE1], CHAN_LOCAL_SOUND );
		else if (eth32.cg.self->stats.spree == 10)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_MULTIKILL2], CHAN_LOCAL_SOUND );
		else if (eth32.cg.self->stats.spree == 15)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_DOMINATING1], CHAN_LOCAL_SOUND );
		else if (eth32.cg.self->stats.spree == 20)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_UNSTOPPABLE1], CHAN_LOCAL_SOUND );
		else if (eth32.cg.self->stats.spree == 25)
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_GODLIKE1], CHAN_LOCAL_SOUND );
		else if (eth32.cg.self->stats.spree == 30) {
			oldSpree = eth32.cg.self->stats.spree;
			Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_WICKEDSICK], CHAN_LOCAL_SOUND );
		} else if (eth32.cg.self->stats.spree >= 35) {
			// Let the shitstorm continue :P
			if (eth32.cg.self->stats.spree - oldSpree == 5) {
				oldSpree = eth32.cg.self->stats.spree;
				Syscall.S_StartLocalSound( eth32.cg.media.sounds.spree[SOUND_HOLYSHIT], CHAN_LOCAL_SOUND );
			}
		}
	}
}

void CSounds::RegisterSounds( void )
{
	// Hitsounds
	eth32.cg.media.sounds.hitsounds[HIT_HEAD] = Syscall.S_RegisterSound("sounds/hitsound/head.wav", qtrue);
	eth32.cg.media.sounds.hitsounds[HIT_HEADSHOT] = Syscall.S_RegisterSound("sounds/hitsound/headshot.wav", qtrue);
	eth32.cg.media.sounds.hitsounds[HIT_BODY] = Syscall.S_RegisterSound("sounds/hitsound/body.wav", qtrue);
	eth32.cg.media.sounds.hitsounds[HIT_TEAM] = Syscall.S_RegisterSound("sounds/hitsound/team.wav", qtrue);

	// Spree sounds
	// level 1
	eth32.cg.media.sounds.spree[SOUND_DOUBLEKILL1] = Syscall.S_RegisterSound("sounds/spree/level1/doublekill1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_EXCELLENT] = Syscall.S_RegisterSound("sounds/spree/level1/excellent.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_IMPRESSIVE] = Syscall.S_RegisterSound("sounds/spree/level1/boomheadshot.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_KILLINGSPREE1] = Syscall.S_RegisterSound("sounds/spree/level1/killingspree1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_KILLINGSPREE2] = Syscall.S_RegisterSound("sounds/spree/level1/combowhore.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_PERFECT] = Syscall.S_RegisterSound("sounds/spree/level1/hattrick.wav", qtrue);
	// level 2
	eth32.cg.media.sounds.spree[SOUND_GODLIKE1] = Syscall.S_RegisterSound("sounds/spree/level2/godlike1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_GODLIKE2] = Syscall.S_RegisterSound("sounds/spree/level2/godlike2.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_MULTIKILL1] = Syscall.S_RegisterSound("sounds/spree/level2/multikill1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_MULTIKILL2] = Syscall.S_RegisterSound("sounds/spree/level2/rampage.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_MULTIKILL3] = Syscall.S_RegisterSound("sounds/spree/level2/multikill3.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_TRIPLEKILL] = Syscall.S_RegisterSound("sounds/spree/level2/triplekill.wav", qtrue);
	// level 3
	eth32.cg.media.sounds.spree[SOUND_DOMINATING1] = Syscall.S_RegisterSound("sounds/spree/level3/dominating1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_ULTRAKILL1] = Syscall.S_RegisterSound("sounds/spree/level3/ultrakill1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_ULTRAKILL2] = Syscall.S_RegisterSound("sounds/spree/level3/unreal.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_ULTRAKILL3] = Syscall.S_RegisterSound("sounds/spree/level3/ultrakill3.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_UNREAL] = Syscall.S_RegisterSound("sounds/spree/level3/ludic.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_MEGAKILL] = Syscall.S_RegisterSound("sounds/spree/level3/megakill.wav", qtrue);
	// level 4
	eth32.cg.media.sounds.spree[SOUND_MONSTERKILL1] = Syscall.S_RegisterSound("sounds/spree/level4/monsterkill1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_MONSTERKILL2] = Syscall.S_RegisterSound("sounds/spree/level4/monsterkill2.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_HOLYSHIT] = Syscall.S_RegisterSound("sounds/spree/level4/holyshit.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_UNSTOPPABLE1] = Syscall.S_RegisterSound("sounds/spree/level4/unstoppable1.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_UNSTOPPABLE2] = Syscall.S_RegisterSound("sounds/spree/level4/unstoppable2.wav", qtrue);
	eth32.cg.media.sounds.spree[SOUND_WICKEDSICK] = Syscall.S_RegisterSound("sounds/spree/level4/wickedsick.wav", qtrue);

#ifdef ETH32_DEBUG
	Debug.Log("All Game Sounds Registered...");
#endif
}
