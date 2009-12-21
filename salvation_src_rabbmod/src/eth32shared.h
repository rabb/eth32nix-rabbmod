// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

// eth32shared - shared by hack and custom client 

#ifndef ETH32SHARED_H_
#define ETH32SHARED_H_

typedef unsigned int uint32;

typedef struct {
	char			name[64];					// mod name
	char			version[64];				// mod version
	uint32			crc32;						// crc32 checksum
	int				type;						// mod type
	int				automod;					// known mod and stripped
	int				eth32mod;					// eth32's modified client

	char			etproAacSafe[32];			// save backup, for 'undetour' on shutdown

	uint32			refdef;						// addr of refdef
	uint32			refdefViewAngles;			// addr of refdefVA

	uint32			pmext;						// addr of cg.pmext

	// Refer to CG_ConsoleCommand to find these (or vmmain command case 0x2)
	uint32			commands;					// addr of console command list
	int				numCommands;				// number of commands in list
	uint32			predictedPS;				// the predicted playerstate

	// These will be used to fill in the pointers to player data
	uint32			centBase;					// &cg_entities[0]
	uint32			centSize;					// sizeof(centity_t)
	uint32			centCurrentStateOffset;		// currentState offset
	uint32			centLerpOrigin;				// lerpOrgin offset
	
	int				MOD_SUICIDE;				// differs 

	uint32			ciBase;						// &clientInfo[0]
	uint32			ciSize;						// sizeof(clientInfo_t)
	uint32			ciInfoValidOffset;			// infoValid offset
	uint32			ciNameOffset;				// name offset
	uint32			ciTeamOffset;				// team offset
	uint32			ciHealthOffset;				// health offset
	uint32			ciClassOffset;				// class offset

	uint32			CG_Player;					// called for each player every frame
	uint32			CG_AddPlayerWeapon;			// called to add a weapon to entity
	uint32			CG_PositionRotatedEntityOnTag;	// for grabbing head origin and orientation
	uint32			CG_SetLerpFrameAnimationRate;	// used to capture clientinfo base & size for unknown mods
	uint32			CG_FinishWeaponChange;		// changes between 2 weapons. i.e. forced change scoped -> normal
	//uint32			CG_PlayVoiceChat;			// capture voice chat messages for GUI
	uint32			CG_EntityEvent;				// get EntitiyEvent info
	uint32			CG_Trace;					// run entity traces
	uint32			CG_DamageFeedback;			// disable view distortion from being hit
	uint32			CG_WeaponFireRecoil;		// disable view distortion when firing
	uint32			CG_Respawn;					// lots of stuff we can reset or whatever on respawn
	uint32			CG_CalculateReinfTime;		// so we dont have to do calcs. let the cgame do it for us
	uint32			CG_AllocLocalEntity;		// creative with rays (railtrail)

	//uint32			CG_AddPMItem;
	//uint32			CG_AddPMItem2;				// jaymod version
	uint32			CG_DrawPlayerStatusHead;
	uint32			CG_DrawPlayerStats;
	uint32			CG_DrawPlayerStatus;
	//uint32			CG_CenterPrint;
	//uint32			CG_PriorityCenterPrint;
	uint32			CG_ChargeTimesChanged;
	uint32			BG_FindClipForWeapon;
	uint32			BG_FindAmmoForWeapon;
	uint32			CG_CalcViewValues;
} cgMod_t;

#endif /*ETH32SHARED_H_*/
