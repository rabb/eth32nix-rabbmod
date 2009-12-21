// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#include "CAimbot.h"

#define SETFLOAT(x)			((int)(x * 1000))		// only keeping up to 0.001
#define GETFLOAT(x)			(x / 1000.f)

// NOTE: cast arg0-arg4 to int if used, use SETFLOAT/GETFLOAT for floats

static const windef_t windows[] =
{
	{
		"Status",			// title
		WIN_STATUS,			// type
		GUI_MAINVIEW,			// view
		5, 422, 120, 60,			// x, y, w, h
		0,				// num controls
	},
	{
		"Weapon",			// title
		WIN_WEAPON,			// type
		GUI_MAINVIEW,			// view
		95, 422, 120, 60,		// x, y, w, h
		0,				// num controls
	},
	{
		"Spectators",			// title
  		WIN_SPECTATOR,			// type
		GUI_MAINVIEW,			// view
		50, 193, 100, 160,		// x, y, w, h
		0,				// num controls
	},
	{
		"Respawn",		 	// title
		WIN_RESPAWN,			// type
		GUI_MAINVIEW,			// view
		280, 5, 38, 20,			// x, y, w, h
		0,				// num controls
	},
	{
		"Radar",			// title
		WIN_RADAR,			// type
		GUI_MAINVIEW,			// view
		395, 302, 120, 120,		// x, y, w, h
		0,						// num controls
	},
	{
		"Cameras",			// title
		WIN_CAMERA,			// type
		GUI_MAINVIEW,			// view
		5, 5, 5, 5,			// x, y, w, h : these don't matter though
		0,				// num controls
	},
	{
		"Clients",			// title
		WIN_CLIENTS,			// type
		GUI_CLIENTVIEW,			// view
		5, 5, 630, 470,			// x, y, w, h
		0,				// num controls
	},
	{
		"Banner",
		WIN_BANNER,			// type
		GUI_MAINVIEW,			// view
		20, 20, 1, 1,			// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					AIMBOT
	******************************************************************* **/
	{
		"Aimbot",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 200, 350,		// x, y, w, h
		14,				// num controls
		{
			// Type			Label			X		Y		W		H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Aimbot Mode", 		5, 		5,		190,		23,		0, AIMMODE_MAX-1, (int)&eth32.settings.aimMode, (int)aimModeText },
			{ CTRL_DROPBOX,		"Aim Type",		5,		34,		190,		23,		AIM_OFF, AIM_MAX-1, (int)&eth32.settings.aimType, (int)aimTypeText },
			{ CTRL_CHECKBOX,	"Autofire",		5,		63,		190,		8,		(int)&eth32.settings.autofire },
			{ CTRL_CHECKBOX,	"Validate Attack",	5,		76,		190,		8,		(int)&eth32.settings.atkValidate },
			{ CTRL_CHECKBOX,	"Target Lock",		5,		89,		190,		8,		(int)&eth32.settings.lockTarget },
			{ CTRL_FLOATSLIDER, 	"FOV",			5,		102,		190,		20,		SETFLOAT(0), SETFLOAT(360), (int)&eth32.settings.fov },
			{ CTRL_DROPBOX,		"Target Sort",		5,		127,		190,		23,		SORT_OFF, SORT_MAX-1, (int)&eth32.settings.aimSort, (int)sortTypeText },
			{ CTRL_DROPBOX,		"Aim Priority",		5,		159,		190,		23,		0, AP_MAX-1, (int)&eth32.settings.headbody, (int)priorityTypeText },
			{ CTRL_DROPBOX,		"Head Trace Style",	5,		185,		190,		23,		HEAD_CENTER, HEAD_MAX-1, (int)&eth32.settings.headTraceType, (int)headTraceTypeText },
			{ CTRL_DROPBOX,		"Body Trace Style",	5,		210,		190,		23,		BODY_CENTER, BODY_MAX-1, (int)&eth32.settings.bodyTraceType, (int)bodyTraceTypeText },
			{ CTRL_FLOATSLIDER,	"Dynamic Hitbox",       5,		235, 		190,		20,		SETFLOAT(0), SETFLOAT(3), (int)&eth32.settings.dynamicHitboxScale },
			{ CTRL_FLOATSLIDER, 	"Anim. Correction", 	5,		260,		190,		20,		SETFLOAT(-10), SETFLOAT(10), (int)&eth32.settings.animCorrection },
			{ CTRL_CHECKBOX,	"Auto Crouch",		5,		285,		190,		8,		(int)&eth32.settings.autoCrouch },
			{ CTRL_DROPBOX,		"Hitbox Style",		5,		298,	140,	23,		HITBOX_OFF, HITBOX_MAX-1, (int)&eth32.settings.hitboxType, (int)hitboxText },
		}
	},
	{
		"Grenade Bot",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		320, 55, 150, 300,		// x, y, w, h
		16,						// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Grenade Aimbot",		5,		5,		140,		8,		(int)&eth32.settings.grenadeBot },
			{ CTRL_CHECKBOX,	"Riflenade Aimbot",		5,		20,		140,		8,		(int)&eth32.settings.rifleBot },
			{ CTRL_CHECKBOX,	"Block fire",			5,		35,		140,		8,		(int)&eth32.settings.grenadeBlockFire },
			{ CTRL_CHECKBOX,	"Grenade Trajectory",		5,		50,		140,		8,		(int)&eth32.settings.valGrenTrajectory },
			{ CTRL_CHECKBOX,	"Rifle Trajectory",		5,		65,		140,		8,		(int)&eth32.settings.valRifleTrajectory },
			{ CTRL_CHECKBOX,	"Grenade Senslock",		5,		78,		140,		8,		(int)&eth32.settings.grenadeSenslock },
			{ CTRL_FLOATSLIDER,	"Rifle Z Corr.",		5,		93,		140,		20,		SETFLOAT(-50), SETFLOAT(50), (int)&eth32.settings.riflenadeZ },
			{ CTRL_FLOATSLIDER,	"Grenade Z Corr.",		5,		118,		140,		20,		SETFLOAT(-50), SETFLOAT(50), (int)&eth32.settings.grenadeZ },
			{ CTRL_INTSLIDER,	"Grenade Fire Delay",		5,		142,		140,		20,		0, 1000, (int)&eth32.settings.grenadeFireDelay },
			{ CTRL_CHECKBOX,	"Grenade Autofire",		5,		165,		140,		8,		(int)&eth32.settings.grenadeAutoFire },
			{ CTRL_CHECKBOX,	"Riflenade Autofire",		5,		178,		140,		8,		(int)&eth32.settings.rifleAutoFire },
			{ CTRL_DROPBOX,		"Target Predict",		5,		191,		140,		23,		0, RF_PREDICT_MAX-1, (int)&eth32.settings.ballisticPredict, (int)riflePredictText},
			{ CTRL_CHECKBOX,	"Check Radius Damage",		5,		220,		140,		8,		(int)&eth32.settings.ballisticRadiusDamage },
			{ CTRL_FLOATSLIDER,	"Blast radius",			5,		233,		140,		20,		SETFLOAT(30), SETFLOAT(500), (int)&eth32.settings.radiusDamage },
			{ CTRL_CHECKBOX,	"Auto Targets",			5,		258,		140,		8,		(int)&eth32.settings.autoGrenTargets },
			{ CTRL_CHECKBOX,	"Multi Bounce",			5,		271,		140,		8,		(int)&eth32.settings.allowMultiBounce },
		}
	},
	{
		"Human Aim",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		325, 60, 200, 354,		// x, y, w, h
		14,						// num controls
		{
			// Type			Label				X	Y	W	H		Arg0 ... Arg4
	    		{ CTRL_DROPBOX,		"Human Mode", 		        5,      5,	190,	23,		0, HUMAN_AIM_MAX-1, (int)&eth32.settings.humanMode, (int)humanModeText },
	    		{ CTRL_FLOATSLIDER,	"Simple Logins - Speed",	5,	34,	190,	20,		SETFLOAT(0), SETFLOAT(0.2), (int)&eth32.settings.human1_speed },
	    		{ CTRL_FLOATSLIDER,	"Full Logins - Human Value",	5,	59,	190,	20,		SETFLOAT(0), SETFLOAT(0.2), (int)&eth32.settings.human2_humanValue },
            		{ CTRL_FLOATSLIDER,	"Full Logins - SpeedX",		5,	84,	190,	20,		SETFLOAT(0), SETFLOAT(10), (int)&eth32.settings.human2_aimX },
            		{ CTRL_FLOATSLIDER,	"Full Logins - SpeedY",		5,	109,	190,	20,		SETFLOAT(0), SETFLOAT(10), (int)&eth32.settings.human2_aimY },
            		{ CTRL_FLOATSLIDER,	"Full Logins - DivMin",		5,	134,	190,	20,		SETFLOAT(0), SETFLOAT(10), (int)&eth32.settings.human2_divMin },
            		{ CTRL_FLOATSLIDER,	"Full Logins - DivMax",		5,	159,	190,	20,		SETFLOAT(0), SETFLOAT(10), (int)&eth32.settings.human2_divMax },
            		{ CTRL_DROPBOX,		"Aim Protect", 		      	5, 	184,    190,	23,		0, PROTECT_MAX-1, (int)&eth32.settings.aimprotect, (int)aimprotectText },
            		{ CTRL_CHECKBOX,	"Lock mouse while aiming",	5,	213,	190,	8,		(int)&eth32.settings.lockMouse },
			{ CTRL_CHECKBOX,	"Randomized Aim",		5,	226,	190,	8,		(int)&eth32.settings.randomAim },
			{ CTRL_INTSLIDER,	"Shake Frequency ms",		5,	239,	190,	20,		500, 20000, (int)&eth32.settings.shakeFreq },
			{ CTRL_INTSLIDER,	"Random Factor X",		5,	264,	190,	20,		0, 1000, (int)&eth32.settings.randFactX },
			{ CTRL_INTSLIDER,	"Random Factor Y",		5,	289,	190,	20,		0, 1000, (int)&eth32.settings.randFactY },
			{ CTRL_INTSLIDER,	"Random Factor Z",		5,	314,	190,	20,		0, 1000, (int)&eth32.settings.randFactZ },
		}
	},

	{
		"Weapon Config",		// title
		WIN_WEAPCONFIG,			// type
		GUI_MENUVIEW,			// view
		157, 55, 150, 225,		// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					AIMBOT EXTRA
	******************************************************************* **/
	{
		"Corrections",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 190,		// x, y, w, h
		8,				// num controls
		{
			{ CTRL_FLOATSLIDER,	"Head Hitbox Size",			    5,	5, 	    240,	20,		SETFLOAT(1), SETFLOAT(15), (int)&eth32.settings.headBoxSize },
			{ CTRL_FLOATSLIDER,	"Body Hitbox Size",	    		5,	30, 	240,	20,		SETFLOAT(1), SETFLOAT(40), (int)&eth32.settings.bodybox },
			{ CTRL_CHECKBOX,	"Auto weapon Delay",   			5,	55,	    240,	8,		(int)&eth32.settings.autoDelay },
			{ CTRL_INTSLIDER,	"Weapon Delay Close",       		5,	    68, 	240,	20,		0,50, (int)&eth32.settings.delayClose },
			{ CTRL_INTSLIDER,	"Weapon Delay Med",       		5,	93, 	240,	20,		0,50, (int)&eth32.settings.delayMed },
			{ CTRL_INTSLIDER,	"Weapon Delay Far",       		5,	118, 	240,	20,		0,50, (int)&eth32.settings.delayFar },
			//{ CTRL_INTSLIDER,	"Bullet size",       			5,	143, 	240,	20,		0,5, (int)&eth32.settings.bulletSize },
			{ CTRL_CHECKBOX,	"Auto X Vecs Calculation",		5,	142,	240,	8,		(int)&eth32.settings.autoVecX },
			{ CTRL_CHECKBOX,	"Auto Z Vecs Calculation",		5,	152,	240,	8,		(int)&eth32.settings.autoVecZ },
		}
	},
	{
		"Predictions",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 300, 250, 213,		// x, y, w, h
		9,				// num controls
		{
			// Type			Label				X	Y	W	H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Preshoot",		        5,	5,      240,	8,		(int)&eth32.settings.preShoot },
			{ CTRL_CHECKBOX,	"Preaim",		        5,	18,     240,	8,		(int)&eth32.settings.preAim },
			{ CTRL_FLOATSLIDER,	"Preshoot Time",	        5,	31,	240,	20,		SETFLOAT(0), SETFLOAT(300), (int)&eth32.settings.preShootTime },
			{ CTRL_FLOATSLIDER,	"Preaim Time",		        5,	56, 	240,	20,		SETFLOAT(0), SETFLOAT(300), (int)&eth32.settings.preAimTime },
			{ CTRL_DROPBOX,		"Self Predict Type",		5,	81,	240,	23,		SPR_OFF, SPR_MAX-1, (int)&eth32.settings.predSelfType, (int)selfPredictText },
			{ CTRL_FLOATSLIDER,	"Self Predict",			5,	110,	240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (int)&eth32.settings.predSelf },
			{ CTRL_CHECKBOX,	"Auto predict BOTs",		5,	135,     240,	8,		(int)&eth32.settings.autoPredictBots },
			{ CTRL_FLOATSLIDER,	"Target Predict",	        5,     	148,	240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (int)&eth32.settings.pred },
			{ CTRL_FLOATSLIDER,	"BOT Predict",	                5,     	173,	240,	20,		SETFLOAT(-0.1), SETFLOAT(0.1), (int)&eth32.settings.predbot },
		}
	},
	{
		"Hitbox",		// title
		WIN_HITBOX,			// type
		GUI_MENUVIEW,			// view
		7, 55, 250, 225,		// x, y, w, h
		0,				// num controls
	},
	/** *******************************************************************
					VISUALS
	******************************************************************* **/
	{
		"Visuals",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 150, 299,		// x, y, w, h
		15,				// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Original HUD",			5,		5,		140,		8,		(int)&eth32.settings.guiOriginal },
			{ CTRL_CHECKBOX,	"Draw Hackvisuals",		5,		18,		140,		8,		(int)&eth32.settings.drawHackVisuals },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		31,		140,		8,		(int)&eth32.settings.wallhack },
			{ CTRL_CHECKBOX,	"Draw Scope Blackout",		5,		44,		140,		8,		(int)&eth32.settings.blackout },
			{ CTRL_CHECKBOX,	"Weapon Zoom",			5,		57,		140,		8,		(int)&eth32.settings.weaponZoom },
			{ CTRL_FLOATSLIDER,	"Scoped Turning",		5,		70,		140,		20,		SETFLOAT(0.1), SETFLOAT(1.0), (int)&eth32.settings.scopedTurnSpeed },
			{ CTRL_INTSLIDER,	"Smoke Visibility",		5,		95,		140,		20,		0, 100, (int)&eth32.settings.smoketrnsp },
			{ CTRL_FLOATSLIDER,	"Radar Range",			5,		120,		140,		20,		SETFLOAT(100), SETFLOAT(10000), (int)&eth32.settings.radarRange },
			{ CTRL_CHECKBOX,	"Banner",			5,		145,		140,		8,		(int)&eth32.settings.guiBanner },
			{ CTRL_FLOATSLIDER,	"Banner size",			5,		158,		140,		20,		SETFLOAT(0), SETFLOAT(3), (int)&eth32.settings.BannerScale },
			{ CTRL_CHECKBOX,	"Remove Foliage",		5,		183,		140,		8,		(int)&eth32.settings.removeFoliage },
			{ CTRL_CHECKBOX,	"Draw Warning Messages",	5,		196,		140,		8,		(int)&eth32.settings.showReload },
			{ CTRL_DROPBOX,		"Damage Indicator Type",	5,		209,		140,		23,		DMG_OFF, DMG_MAX-1, (int)&eth32.settings.dmgIndicator, (int)damageIndicatorText },
			{ CTRL_FLOATSLIDER,	"Dmg Arrow Size",		5,		234,		140,		20,		SETFLOAT(16), SETFLOAT(128), (int)&eth32.settings.dmgArrSize },
			{ CTRL_CHECKBOX,	"Remove Hit Particles",		5,		259,		140,		8,		(int)&eth32.settings.removeParticles },
		},
	},

	{
		"Hitbox Display",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 55, 150, 148,		// x, y, w, h
		8,				// num controls
		{
			// Type			Label			X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Head",			5,		5,		140,		8,		(int)&eth32.settings.drawHeadHitbox },
			{ CTRL_CHECKBOX,	"Head Axes",		5,		18,		140,		8,		(int)&eth32.settings.drawHeadAxes },
			{ CTRL_CHECKBOX,	"Body",			5,		31,		140,		8,		(int)&eth32.settings.drawBodyHitbox },
			{ CTRL_CHECKBOX,	"Show Aimpoints",	5,		44,		140,		8,		(int)&eth32.settings.debugPoints},
			{ CTRL_CHECKBOX,	"Bullet Rail",		5,		57,		140,		8,		(int)&eth32.settings.drawBulletRail },
			{ CTRL_CHECKBOX,	"Rail Wallhack",	5,		70,		140,		8,		(int)&eth32.settings.railWallhack },
			{ CTRL_INTSLIDER,	"Head Trail Time",	5,		83,		140,		20,		0, 300, (int)&eth32.settings.headRailTime },
			{ CTRL_INTSLIDER,	"Body Trail Time",	5,		108,		140,		20,		0, 300, (int)&eth32.settings.bodyRailTime },
		},
	},

	{
		"ESP",				// title
		WIN_ESPCONFIG,			// type
		GUI_MENUVIEW,			// view
		310, 55, 150, 424,		// x, y, w, h
		15,				// num controls
		{
			{ CTRL_CHECKBOX,	"Missile Blast Light",		5,		130,		140,		8,		(int)&eth32.settings.grenadeDlight },
			{ CTRL_CHECKBOX,	"Mortar Blast Light",		5,		143,		140,		8,		(int)&eth32.settings.mortarDlight },
			{ CTRL_CHECKBOX,	"Disguised ESP",		5,		156,		140,		8,		(int)&eth32.settings.drawDisguised },
			{ CTRL_CHECKBOX,	"Mortar Trace and Esp",		5,		169,		140,		8,		(int)&eth32.settings.mortarTrace },
			{ CTRL_CHECKBOX,	"Arty Markers",			5,		182,		140,		8,		(int)&eth32.settings.artyMarkers },
			{ CTRL_CHECKBOX,	"Old ESP Icons",		5,		195,		140,		8,		(int)&eth32.settings.oldClsIcons },
			{ CTRL_DROPBOX,  	"Class ESP",			5,              208,      	140,            23,        	0, CLS_MAX-1, (int)&eth32.settings.classEspType, (int)classEspText },
			{ CTRL_FLOATSLIDER,	"Class ESP Size",		5,		233,		140,		20,		SETFLOAT(0), SETFLOAT(30), (int)&eth32.settings.clsSize },
			{ CTRL_FLOATSLIDER,	"Class ESP Opacity",		5,		258,		140,		20,		SETFLOAT(0), SETFLOAT(1), (int)&eth32.settings.clsOpacity },
			{ CTRL_CHECKBOX,	"Items ESP",			5,		283,		140,		8,		(int)&eth32.settings.itemEsp },
			{ CTRL_FLOATSLIDER,	"Items ESP Size",		5,		296,		140,		20,		SETFLOAT(0), SETFLOAT(30), (int)&eth32.settings.itemEspSize },
			{ CTRL_FLOATSLIDER,	"Items ESP Opacity",		5,		321,		140,		20,		SETFLOAT(0), SETFLOAT(1), (int)&eth32.settings.itemEspOpacity },
			{ CTRL_CHECKBOX,	"Box ESP",			5,		346,		140,		8,		(int)&eth32.settings.boxEsp },
			{ CTRL_INTSLIDER,	"Box ESP Border Size",		5,		359,		140,		20,		0, 5, (int)&eth32.settings.boxEspBorder },
			{ CTRL_FLOATSLIDER,	"Box ESP Opacity",		5,		384,		140,		20,		SETFLOAT(0), SETFLOAT(1), (int)&eth32.settings.boxEspOpacity },
		},
	},
	{
		"Camera Settings",
		WIN_CAMCONFIG,
		GUI_MENUVIEW,
		500, 55, 150, 150,
		0,
	},
	/** *******************************************************************
					VISUAL EXTRA
	******************************************************************* **/
	{
		"Chams",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 300, 200,		// x, y, w, h
		16,				// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_DROPBOX,		"Team Shader 1",		5,		5,		140,		23,		0, SHADER1_MAX-1, (int)&eth32.settings.teamShader1, (int)Shader1Text },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		30,		140,		8,		(int)&eth32.settings.teamShader1Wallhack },
			{ CTRL_DROPBOX,		"Team Shader 2",		5,		43,		140,		23,		0, SHADER2_MAX-1, (int)&eth32.settings.teamShader2, (int)Shader2Text },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		68,		140,		8,		(int)&eth32.settings.teamShader2Wallhack },

			{ CTRL_DROPBOX,		"Enemy Shader 1",		155,		5,		140,		23,		0, SHADER1_MAX-1, (int)&eth32.settings.enemyShader1, (int)Shader1Text },
			{ CTRL_CHECKBOX,	"Wallhack",			155,		30,		140,		8,		(int)&eth32.settings.enemyShader1Wallhack },
			{ CTRL_DROPBOX,		"Enemy Shader 2",		155,		43,		140,		23,		0, SHADER2_MAX-1, (int)&eth32.settings.enemyShader2, (int)Shader2Text },
			{ CTRL_CHECKBOX,	"Wallhack",			155,		68,		140,		8,		(int)&eth32.settings.enemyShader2Wallhack },

			{ CTRL_DROPBOX,		"Weapon Shader 1",		5,		93,		140,		23,		0, SHADER3_MAX-1, (int)&eth32.settings.weaponShader1, (int)Shader3Text },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		118,		140,		8,		(int)&eth32.settings.weaponShader1Wallhack },
			{ CTRL_DROPBOX,		"Weapon Shader 2",		5,		131,		140,		23,		0, SHADER4_MAX-1, (int)&eth32.settings.weaponShader2, (int)Shader4Text },
			{ CTRL_CHECKBOX,	"Wallhack",			5,		156,		140,		8,		(int)&eth32.settings.weaponShader2Wallhack },

			{ CTRL_DROPBOX,		"Item Shader 1",		155,		93,		140,		23,		0, SHADER3_MAX-1, (int)&eth32.settings.itemShader1, (int)Shader3Text },
			{ CTRL_CHECKBOX,	"Wallhack",			155,		118,		140,		8,		(int)&eth32.settings.itemShader1Wallhack },
			{ CTRL_DROPBOX,		"Item Shader 2",		155,		131,		140,		23,		0, SHADER4_MAX-1, (int)&eth32.settings.itemShader2, (int)Shader4Text },
			{ CTRL_CHECKBOX,	"Wallhack",			155,		156,		140,		8,		(int)&eth32.settings.itemShader2Wallhack },
		},
	},
	{
		"Custom Models",		// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 150, 120,			// x, y, w, h
		6,				// num controls
		{
			{ CTRL_CHECKBOX,	"Reaper Head",			5,		5,		140,		8,		(int)&eth32.settings.reaperHead },
			{ CTRL_CHECKBOX,	"Hitler Head",			5,		18,		140,		8,		(int)&eth32.settings.hitlerHead },
			{ CTRL_CHECKBOX,	"Hitler Body",			5,		31,		140,		8,		(int)&eth32.settings.hitlerBody },
			{ CTRL_CHECKBOX,	"Skeleton Head",		5,		44,		140,		8,		(int)&eth32.settings.skeletonHead },
			{ CTRL_CHECKBOX,	"Pig Head",			    5,		57,		140,		8,		(int)&eth32.settings.pigHead },
			{ CTRL_CHECKBOX,	"Grinch Head",			5,		70,		140,		8,		(int)&eth32.settings.grinchHead },
		},
	},
	{
		"Crosshair",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 200, 95,		// x, y, w, h
		3,				// num controls
		{
			{ CTRL_DROPBOX,		"Custom Crosshair",	5,		5,		190,	23,		0, XHAIR_MAX-1, (int)&eth32.settings.customXhair, (int)crosshairText },
			{ CTRL_FLOATSLIDER,	"Size",			5,		30,		190,	20,		SETFLOAT(0), SETFLOAT(500), (int)&eth32.settings.crossSize },
			{ CTRL_FLOATSLIDER,	"Opacity",		5,		55,		190,	20,		SETFLOAT(0), SETFLOAT(1), (int)&eth32.settings.xhairOpacity },
		},
	},
	/** *******************************************************************
					COLOR PICKER
	******************************************************************* **/
	{
		"Colors",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		5, 20, 630, 176,		// x, y, w, h
		1,				// num controls
		{
			// Type		    Label			X		Y		W		H		Arg0 ... Arg4
			{ CTRL_COLORPICKER, "Picker",			5,		5,		620,		156 },
		},
	},


	/** *******************************************************************
					MISC
	******************************************************************* **/
	{
		"Misc",				// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		7, 55, 150, 217,		// x, y, w, h
		13,				// num controls
		{
			{ CTRL_CHECKBOX,	"Spec list", 		5, 		5, 		140, 	8, 		(int)&eth32.settings.getSpeclist },
			{ CTRL_CHECKBOX,	"Transparent Console",	5,		18,		140,	8,		(int)&eth32.settings.transparantConsole },
			{ CTRL_CHECKBOX,	"Respawn Timers",	5,		31,		140,	8,		(int)&eth32.settings.respawnTimers },
			{ CTRL_CHECKBOX,	"Auto Tapout",		5,		44,		140,	8,		(int)&eth32.settings.autoTapout },
			{ CTRL_DROPBOX,		"PB Screenshot",	5,		57,		140,	23,		0, PB_SS_MAX-1, (int)&eth32.settings.pbScreenShot, (int)pbssText },
			{ CTRL_CHECKBOX,	"Original Viewvalues",	5,		86,		140,	8,		(int)&eth32.settings.origViewValues },
			{ CTRL_CHECKBOX,	"Interpolated PS",	5,		99,		140,	8,		(int)&eth32.settings.interpolatedPs },
			{ CTRL_CHECKBOX,	"Damage Feedback",	5,		112,		140,	8,		(int)&eth32.settings.dmgFeedback },
			{ CTRL_CHECKBOX,	"Auto Vote",		5,		125,		140,	8,		(int)&eth32.settings.autoVote },
			{ CTRL_CHECKBOX,	"Auto Complaint",	5,		138,		140,	8,		(int)&eth32.settings.autoComplaint },
			{ CTRL_CHECKBOX,	"Anti Teamkill",	5,		151,		140,	8,		(int)&eth32.settings.antiTk },
			{ CTRL_CHECKBOX,	"Spoof OS",		5,		164,		140,	8,		(int)&eth32.settings.etproOs },
			{ CTRL_CHECKBOX,	"Timenudge Hack",	5,		177,		140,	8,		(int)&eth32.settings.nudgeHack },
		},
	},

	{
		"Sound",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		157, 250, 150, 122,		// x, y, w, h
		6,				// num controls
		{
			{ CTRL_CHECKBOX,	"Spree Sounds",		        5,		5,		140,	8,		(int)&eth32.settings.killSounds },
			{ CTRL_CHECKBOX,	"Random Spree Sounds",		5,		18,		140,	8,		(int)&eth32.settings.randomSpreeSounds },
			{ CTRL_DROPBOX,		"Hitsound Type",		5,		31,		140,	23,		0, HIT_MAX-2, (int)&eth32.settings.hitsounds, (int)hitsoundsText },
			{ CTRL_CHECKBOX,	"Pure Only",		        5,		56,		140,	8,		(int)&eth32.settings.pureSounds },
			{ CTRL_CHECKBOX,	"HQ sounds",		        5,		69,		140,	8,		(int)&eth32.settings.hqSounds },
			{ CTRL_FLOATSLIDER,	"MK Reset Time",		5,		82,		140,	20,		SETFLOAT(0), SETFLOAT(10000), (int)&eth32.settings.mkResetTime },
		},
	},
	{
		"Name Stealer",			// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		320, 370, 150, 133,		// x, y, w, h
		5,				// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Name Steal",			5,		5,		140,		8,		(int)&eth32.settings.doNamesteal },
			{ CTRL_INTSLIDER,	"Delay",			5,		18,		140,		20,		0, 20000, (int)&eth32.settings.NamestealDelay },
			{ CTRL_INTSLIDER,	"Init Grace",			5,		41,		140,		20,		0, 20000, (int)&eth32.settings.NamestealGrace },
			{ CTRL_DROPBOX,		"Steal type",			5,		64,		140,		23,		0, NAMESTEAL_MAX-1, (int)&eth32.settings.NamestealMode, (int)namestealText },
			{ CTRL_CHECKBOX,	"PB Exact Namesteal",		5,		93,		140,		8,		(int)&eth32.settings.nsSmartMode },
		}
	},
	{
		"Spam",					// title
		WIN_STANDARD,			// type
		GUI_MENUVIEW,			// view
		320, 150, 150, 120,		// x, y, w, h
		6,				// num controls
		{
			// Type			Label				X		Y		W		H		Arg0 ... Arg4
			{ CTRL_CHECKBOX,	"Kill Spam",			5,		5,		140,		8,		(int)&eth32.settings.killSpam },
			{ CTRL_CHECKBOX,	"Multikill Spam",		5,		18,		140,		8,		(int)&eth32.settings.multikillSpam },
			{ CTRL_CHECKBOX,	"Death Spam",			5,		31,		140,		8,		(int)&eth32.settings.deathSpam },
			{ CTRL_CHECKBOX,	"Suicide Spam",			5,		44,		140,		8,		(int)&eth32.settings.suicideSpam },
			{ CTRL_CHECKBOX,	"PrivateMsg killspam",		5,		57,		140,		8,		(int)&eth32.settings.pmKillspam },
			{ CTRL_DROPBOX,		"Killspam Type",		5,		70,		140,		23,		0, KILLSPAM_MAX-1, (int)&eth32.settings.KillspamType, (int)killspamText },
		}
	},
};

static const assetdef_t assetDefs[] =
{
//	  Key			Type				Target
	{ "titlecolor",		ASSET_VEC4,			(void*)eth32.guiAssets.titleColor },
	{ "textcolor1",		ASSET_VEC4,			(void*)eth32.guiAssets.textColor1 },
	{ "textcolor2",		ASSET_VEC4,			(void*)eth32.guiAssets.textColor2 },
	{ "titleleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleLeft },
	{ "titlecenter",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleCenter },
	{ "titleright",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.titleRight },
	{ "winleft",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winLeft },
	{ "wintop",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTop },
	{ "wintopl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winTopLeft },
	{ "wincenter",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.winCenter },
	{ "txtinputl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputLeft },
	{ "txtinputc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputCenter },
	{ "txtinputr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.txtinputRight },
	{ "btnl",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnLeft },
	{ "btnc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnCenter },
	{ "btnr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnRight },
	{ "btnsell",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselLeft },
	{ "btnselc",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselCenter },
	{ "btnselr",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.btnselRight },
	{ "check",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.check },
	{ "checkbox",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.checkBox },
	{ "mouse",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.mousePtr },
	{ "dropboxarrow",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.dropboxArrow },
	{ "sliderbtn",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderBtn },
	{ "slidertrack",	ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.sliderTrack },
	{ "camcorner",		ASSET_SHADERNOMIP,		(void*)&eth32.guiAssets.camCorner },
};
