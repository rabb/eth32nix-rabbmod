/*
 * name:	cg_local.h
 *
 * desc:	The entire cgame module is unloaded and reloaded on each level change,
 *			so there is NO persistant data between levels on the client side.
 *			If you absolutely need something stored, it can either be kept
 *			by the server in the server stored userinfos, or stashed in a cvar.

 *
*/

#include "../game/q_shared.h"
#include "tr_types.h"
#include "../game/bg_public.h"
#include "cg_public.h"
#include "../ui/ui_shared.h"


#define MAX_LOCATIONS		256
#define	POWERUP_BLINKS		5

#define STATS_FADE_TIME		200.0f
#define	POWERUP_BLINK_TIME	1000
#define	FADE_TIME			200
#define	DAMAGE_DEFLECT_TIME	100
#define	DAMAGE_RETURN_TIME	400
#define DAMAGE_TIME			500
#define	LAND_DEFLECT_TIME	150
#define	LAND_RETURN_TIME	300
#define	STEP_TIME			200
#define	DUCK_TIME			100
#define	PAIN_TWITCH_TIME	200
#define	ZOOM_TIME			150
#define	MUZZLE_FLASH_TIME	30
#define	SINK_TIME			1000		// time for fragments to sink into ground before going away
#define	REWARD_TIME			3000

#define PRONE_TIME			500

#define	PULSE_SCALE			1.5			// amount to scale up the icons when activating

#define	MAX_STEP_CHANGE		32

#define	MAX_VERTS_ON_POLY	10
#define	MAX_MARK_POLYS		256 // JPW NERVE was 1024

#define STAT_MINUS			10	// num frame for '-' stats digit

#define	ICON_SIZE			48
#define	CHAR_WIDTH			32
#define	CHAR_HEIGHT			48
#define	TEXT_ICON_SPACE		4

#define	TEAMCHAT_WIDTH		70
#define TEAMCHAT_HEIGHT		8

#define	NOTIFY_WIDTH		80
#define NOTIFY_HEIGHT		5

// very large characters
#define	GIANT_WIDTH			32
#define	GIANT_HEIGHT		48

#define	NUM_CROSSHAIRS		10

// Ridah, trails
#define	STYPE_STRETCH	0
#define	STYPE_REPEAT	1

#define	TJFL_FADEIN		(1<<0)
#define	TJFL_CROSSOVER	(1<<1)
#define	TJFL_NOCULL		(1<<2)
#define	TJFL_FIXDISTORT	(1<<3)
#define TJFL_SPARKHEADFLARE (1<<4)
#define	TJFL_NOPOLYMERGE	(1<<5)
// done.

// OSP
// Autoaction values
#define AA_DEMORECORD	0x01
#define AA_SCREENSHOT	0x02
#define AA_STATSDUMP	0x04

// Cursor
#define CURSOR_OFFSETX	13
#define CURSOR_OFFSETY	12

// Demo controls
#define DEMO_THIRDPERSONUPDATE	0
#define DEMO_RANGEDELTA			6
#define DEMO_ANGLEDELTA			4

// MV overlay
#define MVINFO_TEXTSIZE		10
#define MVINFO_RIGHT		640 - 3
#define MVINFO_TOP			100

#define MAX_WINDOW_COUNT		10
#define MAX_WINDOW_LINES		64

#define MAX_STRINGS				80
#define MAX_STRING_POOL_LENGTH	128

#define WINDOW_FONTWIDTH	8		// For non-true-type: width to scale from
#define WINDOW_FONTHEIGHT	8		// For non-true-type: height to scale from

#define WID_NONE			0x00	// General window
#define WID_STATS			0x01	// Stats (reusable due to scroll effect)
#define WID_TOPSHOTS		0x02	// Top/Bottom-shots
#define WID_MOTD			0x04	// MOTD
//#define WID_DEMOHELP		0x08	// Demo key control info
//#define WID_SPECHELP		0x10	// MV spectator key control info

#define WFX_TEXTSIZING		0x01	// Size the window based on text/font setting
#define WFX_FLASH			0x02	// Alternate between bg and b2 every half second
#define WFX_TRUETYPE		0x04	// Use truetype fonts for text
#define WFX_MULTIVIEW		0x08	// Multiview window
// These need to be last
#define WFX_FADEIN			0x10	// Fade the window in (and back out when closing)
#define WFX_SCROLLUP		0x20	// Scroll window up from the bottom (and back down when closing)
#define WFX_SCROLLDOWN		0x40	// Scroll window down from the top (and back up when closing)
#define WFX_SCROLLLEFT		0x80	// Scroll window in from the left (and back right when closing)
#define WFX_SCROLLRIGHT		0x100	// Scroll window in from the right (and back left when closing)

#define WSTATE_COMPLETE		0x00	// Window is up with startup effects complete
#define WSTATE_START		0x01	// Window is "initializing" w/effects
#define WSTATE_SHUTDOWN		0x02	// Window is shutting down with effects
#define WSTATE_OFF			0x04	// Window is completely shutdown

#define MV_PID				0x00FF	// Bits available for player IDs for MultiView windows
#define MV_SELECTED			0x0100	// MultiView selected window flag is the 9th bit

typedef struct {
	vec4_t colorBorder;			// Window border color
	vec4_t colorBackground;		// Window fill color
	vec4_t colorBackground2;	// Window fill color2 (for flashing)
	int curX;					// Scrolling X position
	int curY;					// Scrolling Y position
	int effects;				// Window effects
	int flashMidpoint;			// Flashing transition point (in ms)
	int flashPeriod;			// Background flashing period (in ms)
	int fontHeight;				// For non-truetype font drawing
	float fontScaleX;			// Font scale factor
	float fontScaleY;			// Font scale factor
	int fontWidth;				// For non-truetype font drawing
	float h;					// Height
	int	id;						// Window ID for special handling (i.e. stats, motd, etc.)
	qboolean inuse;				// Activity flag
	int lineCount;				// Number of lines to display
	int lineHeight[MAX_WINDOW_LINES];	// Height property for each line
	char *lineText[MAX_WINDOW_LINES];	// Text info
	float m_x;					// Mouse X position
	float m_y;					// Mouse Y position
	int mvInfo;					// lower 8 = player id, 9 = is_selected
	int targetTime;				// Time to complete any defined effect
	int state;					// Current state of the window
	int time;					// Current window time
	float w;					// Width
	float x;					// Target x-coordinate
								//    negative values will align the window from the right minus the (window width + offset(x))
	float y;					// Target y-coordinate
								//    negative values will align the window from the bottom minus the (window height + offset(y))
} cg_window_t;

typedef struct {
	qboolean fActive;
	char str[MAX_STRING_POOL_LENGTH];
} cg_string_t;

typedef struct {
	int				activeWindows[MAX_WINDOW_COUNT];	// List of active windows
	int				numActiveWindows;					// Number of active windows in use
	cg_window_t	window[MAX_WINDOW_COUNT];			// Static allocation of all windows
} cg_windowHandler_t;

typedef struct {
	int				pID;		// Player ID
	int				classID;	// Player's current class
	int				width;		// Width of text box
	char			info[8];	// On-screen info (w/color coding)
	qboolean		fActive;	// Overlay element is active
	cg_window_t	*w;			// Window handle (may be NULL)
} cg_mvinfo_t;
// OSP




// START Mad Doc - TDF
#define NUM_OVERLAY_FACES 1
// END Mad Doc - TDF

//=================================================

// player entities need to track more information
// than any other type of entity.

// note that not every player entity is a client entity,
// because corpses after respawn are outside the normal
// client numbering range

// when changing animation, set animationTime to frameTime + lerping time
// The current lerp will finish out, then it will lerp to the new animation
typedef struct {
	int			oldFrame;
	int			oldFrameTime;		// time when ->oldFrame was exactly on
	qhandle_t	oldFrameModel;

	int			frame;
	int			frameTime;			// time when ->frame will be exactly on
	qhandle_t	frameModel;

	float		backlerp;

	float		yawAngle;
	qboolean	yawing;
	float		pitchAngle;
	qboolean	pitching;

	int			animationNumber;	// may include ANIM_TOGGLEBIT
	int			oldAnimationNumber;	// may include ANIM_TOGGLEBIT
	animation_t	*animation;
	int			animationTime;		// time when the first frame of the animation will be exact


	// Ridah, variable speed anims
	vec3_t		oldFramePos;
	float		animSpeedScale;
	int			oldFrameSnapshotTime;
	headAnimation_t	*headAnim;
	// done.

} lerpFrame_t;

typedef struct {
	lerpFrame_t		legs, torso;
	lerpFrame_t		head;
	lerpFrame_t		weap;	//----(SA)	autonomous weapon animations
	lerpFrame_t		hudhead;

	int				painTime;
	int				painDuration;
	int				painDirection;	// flip from 0 to 1
	int				painAnimTorso;
	int				painAnimLegs;
	int				lightningFiring;

	// Ridah, so we can do fast tag grabbing
	refEntity_t		bodyRefEnt, headRefEnt, gunRefEnt;
	int				gunRefEntFrame;

	float			animSpeed;	// for manual adjustment

	int				lastFiredWeaponTime;
	int				weaponFireTime;
} playerEntity_t;

//=================================================

typedef struct tag_s {
	vec3_t		origin;
	vec3_t		axis[3];
} tag_t;


// centity_t have a direct corespondence with gentity_t in the game, but
// only the entityState_t is directly communicated to the cgame
typedef struct centity_s {
	entityState_t	currentState;	// from cg.frame
	entityState_t	nextState;		// from cg.nextFrame, if available
	qboolean		interpolate;	// true if next is valid to interpolate to
	qboolean		currentValid;	// true if cg.frame holds this entity

	int				muzzleFlashTime;	// move to playerEntity?
	int				overheatTime;
	int				previousEvent;
	int				previousEventSequence;	// Ridah
	int				teleportFlag;

	int				trailTime;		// so missile trails can handle dropped initial packets
	int				miscTime;
	int				soundTime;		// ydnar: so looping sounds can start when triggered

	playerEntity_t	pe;

//	int				errorTime;		// decay the error from this time
//	vec3_t			errorOrigin;
//	vec3_t			errorAngles;

//	qboolean		extrapolated;	// false if origin / angles is an interpolation
	vec3_t			rawOrigin;
	vec3_t			rawAngles;

	// exact interpolated position of entity on this frame
	vec3_t			lerpOrigin;
	vec3_t			lerpAngles;

	vec3_t			lastLerpAngles;	// (SA) for remembering the last position when a state changes
	vec3_t			lastLerpOrigin; // Gordon: Added for linked trains player adjust prediction

	// Ridah, trail effects
	int				headJuncIndex, headJuncIndex2;
	int				lastTrailTime;
	// done.

	// Ridah
	vec3_t			fireRiseDir;	// if standing still this will be up, otherwise it'll point away from movement dir
	int				lastFuseSparkTime;

	// client side dlights
	int				dl_frame;
	int				dl_oldframe;
	float			dl_backlerp;
	int				dl_time;
	char			dl_stylestring[64];
	int				dl_sound;
	int				dl_atten;

	lerpFrame_t		lerpFrame;	//----(SA)	added
	vec3_t			highlightOrigin;	// center of the geometry.  for things like corona placement on treasure
	qboolean		usehighlightOrigin;

	refEntity_t		refEnt;
	int				processedFrame;		// frame we were last added to the scene

	int				voiceChatSprite;		// DHM - Nerve
	int				voiceChatSpriteTime;	// DHM - Nerve

	// item highlighting
	int				highlightTime;
	qboolean		highlighted;

	// spline stuff
	vec3_t			origin2;
	splinePath_t*	backspline;
	float			backdelta;
	qboolean		back;
	qboolean		moving;


	int				tankframe;
	int				tankparent;
	tag_t			mountedMG42Base;
	tag_t			mountedMG42Nest;
	tag_t			mountedMG42;
	tag_t			mountedMG42Player;
	tag_t			mountedMG42Flash;

	qboolean		akimboFire;

	// Gordon: tagconnect cleanup..
	int				tagParent;
	char			tagName[MAX_QPATH];
} centity_t;


//======================================================================

// local entities are created as a result of events or predicted actions,
// and live independantly from all server transmitted entities

typedef struct markPoly_s {
	struct markPoly_s	*prevMark, *nextMark;
	int			time;
	qhandle_t	markShader;
	qboolean	alphaFade;		// fade alpha instead of rgb
	float		color[4];
	poly_t		poly;
	polyVert_t	verts[MAX_VERTS_ON_POLY];

	int			duration;	// Ridah
} markPoly_t;

//----(SA)	moved in from cg_view.c
typedef enum {
	ZOOM_NONE,
	ZOOM_BINOC,
	ZOOM_SNIPER,
	ZOOM_SNOOPER,
	ZOOM_FG42SCOPE,
	ZOOM_MG42,
	ZOOM_MAX_ZOOMS
} EZoom_t;

typedef enum {
	ZOOM_OUT,	// widest angle
	ZOOM_IN	// tightest angle (approaching 0)
} EZoomInOut_t;

extern float zoomTable[ZOOM_MAX_ZOOMS][2];

//----(SA)	end

typedef enum {
	LE_MARK,
	LE_EXPLOSION,
	LE_SPRITE_EXPLOSION,
	LE_FRAGMENT,
	LE_MOVE_SCALE_FADE,
	LE_FALL_SCALE_FADE,
	LE_FADE_RGB,
	LE_SCALE_FADE,
	LE_SPARK,
	LE_DEBRIS,
	LE_BLOOD,
	LE_FUSE_SPARK,
//	LE_ZOMBIE_SPIRIT,
//	LE_ZOMBIE_BAT,
	LE_MOVING_TRACER,
	LE_EMITTER
} leType_t;

typedef enum {
	LEF_PUFF_DONT_SCALE  = 0x0001			// do not scale size over time
	,LEF_TUMBLE			 = 0x0002			// tumble over time, used for ejecting shells
	,LEF_NOFADEALPHA	 = 0x0004			// Ridah, sparks
	,LEF_SMOKING		 = 0x0008			// (SA) smoking
	,LEF_TUMBLE_SLOW	 = 0x0010			// slow down tumble on hitting ground
} leFlag_t;

typedef enum {
	LEMT_NONE,
	LEMT_BLOOD
} leMarkType_t;			// fragment local entities can leave marks on walls

typedef enum {
	LEBS_NONE,
	LEBS_BLOOD,
	LEBS_ROCK,
	LEBS_WOOD,
	LEBS_BRASS,
	LEBS_METAL,
	LEBS_BONE
} leBounceSoundType_t;	// fragment local entities can make sounds on impacts

typedef struct localEntity_s {
	struct localEntity_s	*prev, *next;
	leType_t		leType;
	int				leFlags;

	int				startTime;
	int				endTime;
	int				fadeInTime;

	float			lifeRate;			// 1.0 / (endTime - startTime)

	trajectory_t	pos;
	trajectory_t	angles;

	float			bounceFactor;		// 0.0 = no bounce, 1.0 = perfect

	float			color[4];

	float			radius;

	float			light;
	vec3_t			lightColor;

	leMarkType_t		leMarkType;		// mark to leave on fragment impact
	leBounceSoundType_t	leBounceSoundType;

	refEntity_t		refEntity;

	// Ridah
	int				lightOverdraw;
	int				lastTrailTime;
	int				headJuncIndex, headJuncIndex2;
	float			effectWidth;
	int				effectFlags;
	struct localEntity_s	*chain;		// used for grouping entities (like for flamethrower junctions)
	int				onFireStart, onFireEnd;
	int				ownerNum;
	int				lastSpiritDmgTime;

	int				loopingSound;

	int				breakCount;			// break-up this many times before we can break no more
	float			sizeScale;
	// done.

} localEntity_t;

//======================================================================


typedef struct {
	int				client;
	int				score;
	int				ping;
	int				time;
	int				powerUps;
	int				team;
	int				playerClass;		// NERVE - SMF
	int				respawnsLeft;		// NERVE - SMF
} score_t;

// each client has an associated clientInfo_t
// that contains media references necessary to present the
// client model and other color coded effects
// this is regenerated each time a client's configstring changes,
// usually as a result of a userinfo (name, model, etc) change
#define	MAX_CUSTOM_SOUNDS	32
typedef struct clientInfo_s {
	qboolean		infoValid;

	int				clientNum;

	char			name[MAX_QPATH];
	char			cleanname[MAX_QPATH];
	team_t			team;

	int				botSkill;		// 0 = not bot, 1-5 = bot
	int				score;			// updated by score servercmds
	int				location[2];	// location in 2d for team mode
	int				health;			// you only get this info about your teammates
	int				curWeapon;
	int				powerups;		// so can display quad/flag status
	int				breathPuffTime;
	int				cls;
	int				blinkTime;	//----(SA)

	int				handshake;
	int				rank;
	qboolean		ccSelected;
	int				fireteam;
	int				medals[SK_NUM_SKILLS];
	int				skill[SK_NUM_SKILLS];
	int				skillpoints[SK_NUM_SKILLS];	// filled OOB by +wstats

	char			disguiseName[MAX_QPATH];
	int				disguiseRank;

	int				weapon;
	int				secondaryweapon;
	int				latchedweapon;

	int				refStatus;

	bg_character_t	*character;

	// Gordon: caching fireteam pointer here, better than trying to work it out all the time
	fireteamData_t* fireteamData;

	// Gordon: for fireteams, has been selected
	qboolean		selected;

	// Gordon: Intermission stats
	int				totalWeapAcc;
	int				kills;
	int				deaths;

	// OSP - per client MV ps info
	int				ammo;
	int				ammoclip;
	int				chargeTime;
	qboolean		fCrewgun;
	int				cursorHint;
	int				grenadeTimeLeft;	// Actual time remaining
	int				grenadeTimeStart;	// Time trigger base to compute TimeLeft
	int				hintTime;
	int				sprintTime;
	int				weapHeat;
	int				weaponState;
	int				weaponState_last;
} clientInfo_t;

typedef enum {
	W_PART_1,
	W_PART_2,
	W_PART_3,
	W_PART_4,
	W_PART_5,
	W_PART_6,
	W_PART_7,
	W_MAX_PARTS
} barrelType_t;

typedef enum {
	W_TP_MODEL,			//	third person model
	W_FP_MODEL,			//	first person model
	W_PU_MODEL,			//	pickup model
	W_NUM_TYPES
} modelViewType_t;

typedef struct partModel_s {
	char		tagName[MAX_QPATH];
	qhandle_t	model;
	qhandle_t	skin[3];			// 0: neutral, 1: axis, 2: allied
} partModel_t;

typedef struct weaponModel_s {
	qhandle_t	model;
	qhandle_t	skin[3];			// 0: neutral, 1: axis, 2: allied
} weaponModel_t;

// each WP_* weapon enum has an associated weaponInfo_t
// that contains media references necessary to present the
// weapon and its effects
typedef struct weaponInfo_s {
	qboolean		registered;

	animation_t		weapAnimations[MAX_WP_ANIMATIONS];

	qhandle_t		handsModel;			// the hands don't actually draw, they just position the weapon

	qhandle_t		standModel;			// not drawn.  tags used for positioning weapons for pickup
	qboolean		droppedAnglesHack;

	weaponModel_t	weaponModel[W_NUM_TYPES];
	partModel_t		partModels[W_NUM_TYPES][W_MAX_PARTS];
	qhandle_t		flashModel[W_NUM_TYPES];
	qhandle_t		modModels[6];	// like the scope for the rifles

	vec3_t			flashDlightColor;
	sfxHandle_t		flashSound[4];		// fast firing weapons randomly choose
	sfxHandle_t		flashEchoSound[4];	//----(SA)	added - distant gun firing sound
	sfxHandle_t		lastShotSound[4];	// sound of the last shot can be different (mauser doesn't have bolt action on last shot for example)

	qhandle_t		weaponIcon[2];		//----(SA)	[0] is weap icon, [1] is highlight icon
	qhandle_t		ammoIcon;

	qhandle_t		missileModel;
	qhandle_t		missileAlliedSkin;
	qhandle_t		missileAxisSkin;
	sfxHandle_t		missileSound;
	void			(*missileTrailFunc)( centity_t *, const struct weaponInfo_s *wi );
	float			missileDlight;
	vec3_t			missileDlightColor;
	int				missileRenderfx;

	void			(*ejectBrassFunc)( centity_t * );

	sfxHandle_t		readySound;			// an amibient sound the weapon makes when it's /not/ firing
	sfxHandle_t		firingSound;
	sfxHandle_t		overheatSound;
	sfxHandle_t		reloadSound;
	sfxHandle_t		reloadFastSound;

	sfxHandle_t		spinupSound;	//----(SA)	added // sound started when fire button goes down, and stepped on when the first fire event happens
	sfxHandle_t		spindownSound;	//----(SA)	added // sound called if the above is running but player doesn't follow through and fire

	sfxHandle_t		switchSound;
} weaponInfo_t;


// each IT_* item has an associated itemInfo_t
// that constains media references necessary to present the
// item and its effects
typedef struct {
	qboolean		registered;
	qhandle_t		models[MAX_ITEM_MODELS];
	qhandle_t		icons[MAX_ITEM_ICONS];
} itemInfo_t;


typedef struct {
	int				itemNum;
} powerupInfo_t;

#define	MAX_VIEWDAMAGE	8
typedef struct {
	int		damageTime, damageDuration;
	float	damageX, damageY, damageValue;
} viewDamage_t;

#define MAX_REWARDSTACK		5

//======================================================================

// all cg.stepTime, cg.duckTime, cg.landTime, etc are set to cg.time when the action
// occurs, and they will have visible effects for #define STEP_TIME or whatever msec after

#define MAX_PREDICTED_EVENTS	16

#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	2048


#define MAX_SPAWNPOINTS 32
#define MAX_SPAWNDESC	128

#define MAX_BUFFERED_SOUNDSCRIPTS 16

#define MAX_SOUNDSCRIPT_SOUNDS 16

typedef struct soundScriptHandle_s {
	char		filename[MAX_QPATH];
	sfxHandle_t	sfxHandle;
} soundScriptHandle_t;

typedef struct soundScriptSound_s {
	soundScriptHandle_t sounds[MAX_SOUNDSCRIPT_SOUNDS];

	int		numsounds;
	int		lastPlayed;

	struct soundScriptSound_s	*next;
} soundScriptSound_t;

typedef struct soundScript_s
{
	int		index;
	char	name[MAX_QPATH];
	int		channel;
	int		attenuation;
	qboolean	streaming;
	qboolean	looping;
	qboolean	random;	// TODO
	int		numSounds;
	soundScriptSound_t	*soundList;			// pointer into the global list of soundScriptSounds (defined below)

	struct soundScript_s	*nextHash;		// next soundScript in our hashTable list position
} soundScript_t;

typedef struct {
	int				x, y, z;
	int				yaw;
	int				data;
	char			type;
//	int				status;

//	qboolean		selected;

	vec2_t			transformed;
	vec2_t			automapTransformed;

	team_t			team;
} mapEntityData_t;

// START	xkan, 8/29/2002
// the most buddies we can have
#define MAX_NUM_BUDDY  6
// END		xkan, 8/29/2002

typedef enum {
	SHOW_OFF,
	SHOW_SHUTDOWN,
	SHOW_ON
} showView_t;

void CG_ParseMapEntityInfo( int axis_number, int allied_number );

#define MAX_BACKUP_STATES (CMD_BACKUP + 2)

typedef struct {
	int			clientFrame;		// incremented each frame

	int			clientNum;
	int			xp;
	int			xpChangeTime;

	qboolean	demoPlayback;
	qboolean	loading;			// don't defer players at initial startup
	qboolean	intermissionStarted;	// don't play voice rewards, because game will end shortly

	// there are only one or two snapshot_t that are relevent at a time
	int			latestSnapshotNum;	// the number of snapshots the client system has received
	int			latestSnapshotTime;	// the time from latestSnapshotNum, so we don't need to read the snapshot yet

	snapshot_t	*snap;				// cg.snap->serverTime <= cg.time
	snapshot_t	*nextSnap;			// cg.nextSnap->serverTime > cg.time, or NULL
	snapshot_t	activeSnapshots[2];

	float		frameInterpolation;	// (float)( cg.time - cg.frame->serverTime ) / (cg.nextFrame->serverTime - cg.frame->serverTime)

	qboolean	thisFrameTeleport;
	qboolean	nextFrameTeleport;

	int			frametime;		// cg.time - cg.oldTime

	int			time;			// this is the time value that the client
								// is rendering at.
	int			oldTime;		// time at last frame, used for missile trails and prediction checking

	int			physicsTime;	// either cg.snap->time or cg.nextSnap->time

	int			timelimitWarnings;	// 5 min, 1 min, overtime

	qboolean	mapRestart;			// set on a map restart to set back the weapon

	qboolean	renderingThirdPerson;		// during deaths, chasecams, etc

	// prediction state
	qboolean	hyperspace;				// true if prediction has hit a trigger_teleport
	playerState_t	predictedPlayerState;
	centity_t		predictedPlayerEntity;
	qboolean	validPPS;				// clear until the first call to CG_PredictPlayerState
	int			predictedErrorTime;
	vec3_t		predictedError;

	int			eventSequence;
	int			predictableEvents[MAX_PREDICTED_EVENTS];

	float		stepChange;				// for stair up smoothing
	int			stepTime;

	float		duckChange;				// for duck viewheight smoothing
	int			duckTime;

	float		landChange;				// for landing hard
	int			landTime;

	// input state sent to server
	int			weaponSelect;

	// auto rotating items
	vec3_t		autoAnglesSlow;
	vec3_t		autoAxisSlow[3];
	vec3_t		autoAngles;
	vec3_t		autoAxis[3];
	vec3_t		autoAnglesFast;
	vec3_t		autoAxisFast[3];

	// view rendering
	refdef_t	refdef;
	vec3_t		refdefViewAngles;		// will be converted to refdef.viewaxis

	// zoom key
	qboolean	zoomed;
	qboolean	zoomedBinoc;
	int			zoomedScope;	//----(SA)	changed to int
	int			zoomTime;
	float		zoomSensitivity;
	float		zoomval;


	// information screen text during loading
	char		infoScreenText[MAX_STRING_CHARS];

	// scoreboard
	int			scoresRequestTime;
	int			numScores;
	int			selectedScore;
	int			teamScores[2];
	int			teamPlayers[TEAM_NUM_TEAMS]; // JPW NERVE for scoreboard
	score_t		scores[MAX_CLIENTS];
	qboolean	showScores;
	qboolean	scoreBoardShowing;
	int			scoreFadeTime;
	char		killerName[MAX_NAME_LENGTH];
	char			spectatorList[MAX_STRING_CHARS];		// list of names
	int				spectatorLen;												// length of list
	float			spectatorWidth;											// width in device units
	int				spectatorTime;											// next time to offset
	int				spectatorPaintX;										// current paint x
	int				spectatorPaintX2;										// current paint x
	int				spectatorOffset;										// current offset from start
	int				spectatorPaintLen; 									// current offset from start

	//qboolean	showItems;
	//int			itemFadeTime;

	qboolean	lightstylesInited;

	// centerprinting
	int			centerPrintTime;
	int			centerPrintCharWidth;
	int			centerPrintY;
	char		centerPrint[1024];
	int			centerPrintLines;
	int			centerPrintPriority;			// NERVE - SMF

	// fade in/out
	int			fadeTime;
	float		fadeRate;
	vec4_t		fadeColor1;
	vec4_t		fadeColor2;

	// game stats
	int			exitStatsTime;
	int			exitStatsFade;

	// low ammo warning state
	int			lowAmmoWarning;		// 1 = low, 2 = empty

	// kill timers for carnage reward
	int			lastKillTime;

	// crosshair client ID
	int			crosshairClientNum;
	int			crosshairClientTime;

	qboolean	crosshairNotLookingAtClient;
	int			crosshairSPClientTime;
	int			crosshairVerticalShift;
	qboolean	crosshairClientNoShoot;
	qboolean	crosshairTerrain;

	int			teamFirstBlood;				// 0: allies 1: axis -1: nobody
	int			teamWonRounds[2];

	qboolean	filtercams;

	int			crosshairPowerupNum;
	int			crosshairPowerupTime;

//	int			identifyClientNum;			// NERVE - SMF
//	int			identifyClientHealth;		// NERVE - SMF
	int			identifyClientRequest;		// NERVE - SMF

//----(SA)	added
	// cursorhints
	int			cursorHintIcon;
	int			cursorHintTime;
	int			cursorHintFade;
	int			cursorHintValue;
//----(SA)	end

	// powerup active flashing
	int			powerupActive;
	int			powerupTime;

	// attacking player
	int			attackerTime;
	int			voiceTime;

	// reward tmedals
	int			rewardStack;
	int			rewardTime;
	int			rewardCount[MAX_REWARDSTACK];
	qhandle_t	rewardShader[MAX_REWARDSTACK];
	qhandle_t	rewardSound[MAX_REWARDSTACK];

	// warmup countdown
	int			warmup;
	int			warmupCount;

	//==========================

	int			itemPickup;
	int			itemPickupTime;
	int			itemPickupBlendTime;	// the pulse around the crosshair is timed seperately

	int			weaponSelectTime;
	int			weaponAnimation;
	int			weaponAnimationTime;

	// blend blobs
	viewDamage_t	viewDamage[MAX_VIEWDAMAGE];
	float		damageTime;		// last time any kind of damage was recieved
	int			damageIndex;	// slot that was filled in
	float		damageX, damageY, damageValue;

	int			grenLastTime;

	int			switchbackWeapon;
	int			lastFiredWeapon;
	int			lastFiredWeaponTime;
	int			painTime;
	int			weaponFireTime;
	int			nextIdleTime;
	int			lastIdleTimeEnd;
	int			idleAnim;
	int			lastWeapSelInBank[MAX_WEAP_BANKS_MP];	// remember which weapon was last selected in a bank for 'weaponbank' commands //----(SA)	added
// JPW FIXME NOTE: max_weap_banks > max_weap_banks_mp so this should be OK, but if that changes, change this too

	// status bar head
	float		headYaw;
	float		headEndPitch;
	float		headEndYaw;
	int			headEndTime;
	float		headStartPitch;
	float		headStartYaw;
	int			headStartTime;

	// view movement
	float		v_dmg_time;
	float		v_dmg_pitch;
	float		v_dmg_roll;

	vec3_t		kick_angles;	// weapon kicks
	vec3_t		kick_origin;

	// RF, view flames when getting burnt
	int			v_fireTime, v_noFireTime;
	vec3_t		v_fireRiseDir;

	// temp working variables for player view
	float		bobfracsin;
	int			bobcycle;
	float		lastvalidBobfracsin;
	int			lastvalidBobcycle;
	float		xyspeed;
	int			nextOrbitTime;

	// development tool
	refEntity_t		testModelEntity;
	char			testModelName[MAX_QPATH];
	qboolean		testGun;

	// RF, new kick angles
	vec3_t		kickAVel;		// for damage feedback, weapon recoil, etc
								// This is the angular velocity, to give a smooth
								// rotational feedback, rather than sudden jerks
	vec3_t		kickAngles;		// for damage feedback, weapon recoil, etc
								// NOTE: this is not transmitted through MSG.C stream
								// since weapon kicks are client-side, and damage feedback
								// is rare enough that we can transmit that as an event
	float		recoilPitch, recoilPitchAngle;

	// Duffy
	qboolean	cameraMode;		// if rendering from a camera
	// Duffy end

	// NERVE - SMF - Objective info display
	qboolean	limboMenu;

	int			oidTeam;
	int			oidPrintTime;
	int			oidPrintCharWidth;
	int			oidPrintY;
	char		oidPrint[1024];
	int			oidPrintLines;

	// for voice chat buffer
	int			voiceChatTime;
	int			voiceChatBufferIn;
	int			voiceChatBufferOut;

	int			newCrosshairIndex;
	qhandle_t	crosshairShaderAlt[NUM_CROSSHAIRS];

	int			cameraShakeTime;
	float		cameraShakePhase;
	float		cameraShakeScale;
	float		cameraShakeLength;

	qboolean	latchAutoActions;
	qboolean	latchVictorySound;
	// -NERVE - SMF

	// spawn variables
	qboolean	spawning;				// the CG_Spawn*() functions are valid
	int			numSpawnVars;
	char		*spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
	int			numSpawnVarChars;
	char		spawnVarChars[MAX_SPAWN_VARS_CHARS];

	vec2_t		mapcoordsMins;
	vec2_t		mapcoordsMaxs;
	vec2_t		mapcoordsScale;
	qboolean	mapcoordsValid;


	int			numMiscGameModels;


	qboolean	showCampaignBriefing;
	qboolean	showGameView;
	qboolean	showFireteamMenu;

	char		spawnPoints[MAX_SPAWNPOINTS][MAX_SPAWNDESC];
	vec3_t		spawnCoordsUntransformed[MAX_SPAWNPOINTS];
	vec3_t		spawnCoords[MAX_SPAWNPOINTS];
	team_t		spawnTeams[MAX_SPAWNPOINTS];
	team_t		spawnTeams_old[MAX_SPAWNPOINTS];
	int			spawnTeams_changeTime[MAX_SPAWNPOINTS];
	int			spawnPlayerCounts[MAX_SPAWNPOINTS];
	int			spawnCount;
	int			selectedSpawnPoint;

	cg_string_t			aStringPool[MAX_STRINGS];
	int					demohelpWindow;
	cg_window_t			*motdWindow;
	cg_window_t			*msgWstatsWindow;
	cg_window_t			*msgWtopshotsWindow;
	int					mv_cnt;					// Number of active MV windows
	int					mvClientList;			// Cached client listing of who is merged
	cg_window_t			*mvCurrentActive;		// Client ID of current active window (-1 = none)
	cg_window_t			*mvCurrentMainview;		// Client ID used in the main display (should always be set if mv_cnt > 0)
	cg_mvinfo_t			mvOverlay[MAX_MVCLIENTS];// Cached info for MV overlay
	int					mvTeamList[TEAM_NUM_TEAMS][MAX_MVCLIENTS];
	int					mvTotalClients;			// Total # of clients available for MV processing
	int					mvTotalTeam[TEAM_NUM_TEAMS];
	refdef_t			*refdef_current;		// Handling of some drawing elements for MV
	qboolean			showStats;
	int					spechelpWindow;
	int					statsRequestTime;
	cg_window_t			*statsWindow;
	int					topshotsRequestTime;
	cg_window_t			*topshotsWindow;
	cg_window_t			*windowCurrent;			// Current window to update.. a bit of a hack :p
	cg_windowHandler_t	winHandler;
	vec4_t				xhairColor;
	vec4_t				xhairColorAlt;

	// Arnout: allow overriding of countdown sounds
	char		fiveMinuteSound_g[MAX_QPATH];
	char		fiveMinuteSound_a[MAX_QPATH];
	char		twoMinuteSound_g[MAX_QPATH];
	char		twoMinuteSound_a[MAX_QPATH];
	char		thirtySecondSound_g[MAX_QPATH];
	char		thirtySecondSound_a[MAX_QPATH];

	pmoveExt_t	pmext;

	int				numOIDtriggers2;
	char			oidTriggerInfoAllies[MAX_OID_TRIGGERS][256];
	char			oidTriggerInfoAxis[MAX_OID_TRIGGERS][256];

	int			ltChargeTime[2];
	int			soldierChargeTime[2];
	int			engineerChargeTime[2];
	int			medicChargeTime[2];
	int			covertopsChargeTime[2];
	// START	xkan, 8/29/2002
	// which bots are currently selected
	int			selectedBotClientNumber[MAX_NUM_BUDDY];
	// END		xkan, 8/29/2002
	int			binocZoomTime;
	int			limboEndCinematicTime;
	int			proneMovingTime;
	fireteamData_t fireTeams[32];

	// TAT 10/23/2002
	//		For the bot hud, we keep a bit mask for which bot_action icons to show
	int			botMenuIcons;
	//		And we need to know which one is the selected one
	int			botSelectedCommand;

	// START Mad Doc - TDF
	int orderFade;
	int orderTime;
	// END Mad Doc - TDF

	centity_t	*satchelCharge;

	playerState_t	backupStates[MAX_BACKUP_STATES];
	int				backupStateTop;
	int				backupStateTail;
	int				lastPredictedCommand;
	int				lastPhysicsTime;

	qboolean		skyboxEnabled;
	vec3_t			skyboxViewOrg;
	vec_t			skyboxViewFov;

	vec3_t			tankflashorg;

	qboolean		editingSpeakers;

	qboolean		serverRespawning;

	// mortar hud
	vec2_t			mortarFireAngles;
	int				mortarImpactTime;
	vec3_t			mortarImpactPos;
	qboolean		mortarImpactOutOfMap;

	// artillery requests
	vec3_t			artilleryRequestPos[MAX_CLIENTS];
	int				artilleryRequestTime[MAX_CLIENTS];

	soundScript_t*	bufferSoundScripts[MAX_BUFFERED_SOUNDSCRIPTS];
	int				bufferedSoundScriptEndTime;
	int				numbufferedSoundScripts;

	char			objMapDescription_Axis[384];
	char			objMapDescription_Allied[384];
	char			objMapDescription_Neutral[384];
	char			objDescription_Axis[MAX_OBJECTIVES][256];
	char			objDescription_Allied[MAX_OBJECTIVES][256];

	int				waterundertime;
	bool			hooked;
} cg_t;

#define	NUM_FUNNEL_SPRITES	21

#define MAX_LOCKER_DEBRIS	5

// all of the model, shader, and sound references that are
// loaded at gamestate time are stored in cgMedia_t
// Other media that can be tied to clients, weapons, or items are
// stored in the clientInfo_t, itemInfo_t, weaponInfo_t, and powerupInfo_t
typedef struct {
	qhandle_t	charsetShader;
	// JOSEPH 4-17-00
	qhandle_t	menucharsetShader;
	// END JOSEPH
	qhandle_t	charsetProp;
	qhandle_t	charsetPropGlow;
	qhandle_t	charsetPropB;
	qhandle_t	whiteShader;

	qhandle_t	armorModel;

// JPW NERVE
	qhandle_t	hudSprintBar;
	qhandle_t	hudAxisHelmet;
	qhandle_t	hudAlliedHelmet;
	qhandle_t	redColorBar;
	qhandle_t	blueColorBar;
// jpw
	qhandle_t	teamStatusBar;

	qhandle_t	deferShader;

	// gib explosions
	qhandle_t	gibAbdomen;
	qhandle_t	gibArm;
	qhandle_t	gibChest;
	qhandle_t	gibFist;
	qhandle_t	gibFoot;
	qhandle_t	gibForearm;
	qhandle_t	gibIntestine;
	qhandle_t	gibLeg;
	qhandle_t	gibSkull;
	qhandle_t	gibBrain;

	// debris
	qhandle_t	debBlock[6];
	qhandle_t	debRock[3];
	qhandle_t	debFabric[3];
	qhandle_t	debWood[6];

	qhandle_t	targetEffectExplosionShader;

	qhandle_t	machinegunBrassModel;
	qhandle_t	panzerfaustBrassModel;	//----(SA)	added

	// Rafael
	qhandle_t	smallgunBrassModel;

	qhandle_t	shotgunBrassModel;

	qhandle_t	railRingsShader;
	qhandle_t	railCoreShader;
	qhandle_t	ropeShader;

	qhandle_t	lightningShader;

	qhandle_t	friendShader;

	qhandle_t	spawnInvincibleShader;
	qhandle_t	scoreEliminatedShader;

	qhandle_t	medicReviveShader;
	qhandle_t	voiceChatShader;
	qhandle_t	balloonShader;
	qhandle_t	objectiveShader;

	qhandle_t	vehicleShader;
	qhandle_t	destroyShader;

//	qhandle_t	selectShader;
	qhandle_t	viewBloodShader;
	qhandle_t	tracerShader;
	qhandle_t	crosshairShader[NUM_CROSSHAIRS];
	qhandle_t	lagometerShader;
	qhandle_t	backTileShader;
	qhandle_t	noammoShader;

	qhandle_t	reticleShader;
	qhandle_t	reticleShaderSimple;
	qhandle_t	snooperShader;
//	qhandle_t	snooperShaderSimple;
	qhandle_t	binocShader;
	qhandle_t	binocShaderSimple;
// JPW NERVE
	qhandle_t	fleshSmokePuffShader; // JPW NERVE for bullet hit flesh smoke puffs
	qhandle_t	nerveTestShader;
	qhandle_t	idTestShader;
	qhandle_t	hud1Shader;
	qhandle_t	hud2Shader;
	qhandle_t	hud3Shader;
	qhandle_t	hud4Shader;
	qhandle_t	hud5Shader;
// jpw
	qhandle_t	smokePuffShader;
	qhandle_t	smokePuffRageProShader;
	qhandle_t	shotgunSmokePuffShader;
	qhandle_t	waterBubbleShader;
	qhandle_t	bloodTrailShader;

//	qhandle_t	nailPuffShader;

//----(SA)	cursor hints
	// would be nice to specify these in the menu scripts instead of permanent handles...
	qhandle_t	usableHintShader;
	qhandle_t	notUsableHintShader;
	qhandle_t	doorHintShader;
	qhandle_t	doorRotateHintShader;
	qhandle_t	doorLockHintShader;
	qhandle_t	doorRotateLockHintShader;
	qhandle_t	mg42HintShader;
	qhandle_t	breakableHintShader;
	qhandle_t	chairHintShader;
	qhandle_t	alarmHintShader;
	qhandle_t	healthHintShader;
	qhandle_t	treasureHintShader;
	qhandle_t	knifeHintShader;
	qhandle_t	ladderHintShader;
	qhandle_t	buttonHintShader;
	qhandle_t	waterHintShader;
	qhandle_t	cautionHintShader;
	qhandle_t	dangerHintShader;
	qhandle_t	secretHintShader;
	qhandle_t	qeustionHintShader;
	qhandle_t	exclamationHintShader;
	qhandle_t	clipboardHintShader;
	qhandle_t	weaponHintShader;
	qhandle_t	ammoHintShader;
	qhandle_t	armorHintShader;
	qhandle_t	powerupHintShader;
	qhandle_t	holdableHintShader;
	qhandle_t	inventoryHintShader;

	qhandle_t	hintPlrFriendShader;
	qhandle_t	hintPlrNeutralShader;
	qhandle_t	hintPlrEnemyShader;
	qhandle_t	hintPlrUnknownShader;

	// DHM - Nerve :: Multiplayer hints
	qhandle_t	buildHintShader;
	qhandle_t	disarmHintShader;
	qhandle_t	reviveHintShader;
	qhandle_t	dynamiteHintShader;
	// dhm - end


	qhandle_t	tankHintShader;
	qhandle_t	satchelchargeHintShader;
	qhandle_t	uniformHintShader;
	qhandle_t	waypointAttackShader;
	qhandle_t	waypointDefendShader;
	qhandle_t	waypointRegroupShader;
	// TAT 8/29/2002 - a shader for the bot indicator
	qhandle_t	waypointBotShader;
	//		for a queued bot order (Listen Up/Go Go Go)
	qhandle_t	waypointBotQueuedShader;
	qhandle_t	waypointCompassAttackShader;
	qhandle_t	waypointCompassDefendShader;
	qhandle_t	waypointCompassRegroupShader;
	qhandle_t	commandCentreWoodShader;
	qhandle_t	commandCentreMapShader[MAX_COMMANDMAP_LAYERS];
	qhandle_t	commandCentreMapShaderTrans[MAX_COMMANDMAP_LAYERS];
	qhandle_t	commandCentreAutomapShader[MAX_COMMANDMAP_LAYERS];
	qhandle_t	commandCentreAutomapMaskShader;
	qhandle_t	commandCentreAutomapBorderShader;
	qhandle_t	commandCentreAutomapBorder2Shader;
	qhandle_t	commandCentreAutomapCornerShader;
	qhandle_t	commandCentreAxisMineShader;
	qhandle_t	commandCentreAlliedMineShader;
	qhandle_t	commandCentreSpawnShader[2];

	// Mad Doc - TDF
	qhandle_t	ingameAutomapBackground;

	qhandle_t	landmineHintShader;
	qhandle_t	compassConstructShader;
	qhandle_t	compassDestroyShader;
	qhandle_t	buddyShader;
	qhandle_t	hudBorderVert;
	qhandle_t	hudBorderVert2;

	qhandle_t	waypointMarker;

	qhandle_t	slashShader;
	qhandle_t	compassShader;
	qhandle_t	compass2Shader;

	// Rafael
	qhandle_t	snowShader;
	qhandle_t	oilParticle;
	qhandle_t	oilSlick;
	// done.

	// Rafael - cannon
	qhandle_t	smokePuffShaderdirty;
	qhandle_t	smokePuffShaderb1;
	qhandle_t	smokePuffShaderb2;
	qhandle_t	smokePuffShaderb3;
	qhandle_t	smokePuffShaderb4;
	qhandle_t	smokePuffShaderb5;
	// done

	// Rafael - blood pool
	qhandle_t	bloodPool;

	// Ridah, viewscreen blood animation
	qhandle_t	viewBloodAni[5];
	qhandle_t	viewFlashBlood;
	qhandle_t	viewFlashFire[16];
	// done

	// Rafael shards
	qhandle_t	shardGlass1;
	qhandle_t	shardGlass2;
	qhandle_t	shardWood1;
	qhandle_t	shardWood2;
	qhandle_t	shardMetal1;
	qhandle_t	shardMetal2;
//	qhandle_t	shardCeramic1;
//	qhandle_t	shardCeramic2;
	// done

	qhandle_t	shardRubble1;
	qhandle_t	shardRubble2;
	qhandle_t	shardRubble3;


	qhandle_t	shardJunk[MAX_LOCKER_DEBRIS];

	qhandle_t	numberShaders[11];

	qhandle_t	shadowFootShader;
	qhandle_t	shadowTorsoShader;

	// wall mark shaders
	qhandle_t	wakeMarkShader;
	qhandle_t	wakeMarkShaderAnim;
	qhandle_t	bloodMarkShaders[5];
	qhandle_t	bloodDotShaders[5];
	qhandle_t	bulletMarkShader;
	qhandle_t	bulletMarkShaderMetal;
	qhandle_t	bulletMarkShaderWood;
	qhandle_t	bulletMarkShaderGlass;
	qhandle_t	burnMarkShader;

	qhandle_t	flamebarrel;
	qhandle_t	mg42muzzleflash;

	qhandle_t	waterSplashModel;
	qhandle_t	waterSplashShader;

	qhandle_t	thirdPersonBinocModel;	//----(SA)	added

	// weapon effect shaders
	qhandle_t	railExplosionShader;
	qhandle_t	bulletExplosionShader;
	qhandle_t	rocketExplosionShader;
	qhandle_t	grenadeExplosionShader;
	qhandle_t	bfgExplosionShader;
	qhandle_t	bloodExplosionShader;

	// special effects models
	qhandle_t	teleportEffectModel;
	qhandle_t	teleportEffectShader;

	// Ridah
	qhandle_t	bloodCloudShader;
	qhandle_t	sparkParticleShader;
	qhandle_t	smokeTrailShader;
	qhandle_t	fireTrailShader;
	//qhandle_t	lightningBoltShader;
	qhandle_t	flamethrowerFireStream;
	qhandle_t	flamethrowerBlueStream;
	qhandle_t	flamethrowerFuelStream;
	qhandle_t	flamethrowerFuelShader;
	qhandle_t	onFireShader, onFireShader2;
	qhandle_t	viewFadeBlack;
	qhandle_t	sparkFlareShader;
	qhandle_t	funnelFireShader[NUM_FUNNEL_SPRITES];
	qhandle_t	spotLightShader;
	qhandle_t	spotLightBeamShader;
	qhandle_t	bulletParticleTrailShader;
	qhandle_t	smokeParticleShader;

	// DHM - Nerve :: bullet hitting dirt
	qhandle_t	dirtParticle1Shader;
	qhandle_t	dirtParticle2Shader;
	qhandle_t	dirtParticle3Shader;

	qhandle_t	genericConstructionShader;
	//qhandle_t	genericConstructionShaderBrush;
	//qhandle_t	genericConstructionShaderModel;
	qhandle_t	alliedUniformShader;
	qhandle_t	axisUniformShader;

	sfxHandle_t	sfx_artilleryExp[3];
	sfxHandle_t	sfx_artilleryDist;

	sfxHandle_t	sfx_airstrikeExp[3];
	sfxHandle_t	sfx_airstrikeDist;

	// sounds
	sfxHandle_t	noFireUnderwater;
	sfxHandle_t	selectSound;
	sfxHandle_t	landHurt;

	sfxHandle_t	footsteps[FOOTSTEP_TOTAL][4];
	sfxHandle_t	sfx_rockexp;
	sfxHandle_t	sfx_rockexpDist;
	sfxHandle_t	sfx_rockexpWater;
	sfxHandle_t sfx_satchelexp;
	sfxHandle_t sfx_satchelexpDist;
	sfxHandle_t sfx_landmineexp;
	sfxHandle_t sfx_landmineexpDist;
	sfxHandle_t sfx_mortarexp[4];
	sfxHandle_t sfx_mortarexpDist;
	sfxHandle_t	sfx_grenexp;
	sfxHandle_t	sfx_grenexpDist;
	sfxHandle_t sfx_brassSound[BRASSSOUND_MAX][3];
	sfxHandle_t	sfx_rubbleBounce[3];

	sfxHandle_t	sfx_bullet_fleshhit[5];
	sfxHandle_t	sfx_bullet_metalhit[5];
	sfxHandle_t	sfx_bullet_woodhit[5];
	sfxHandle_t	sfx_bullet_glasshit[5];
	sfxHandle_t	sfx_bullet_stonehit[5];
	sfxHandle_t	sfx_bullet_waterhit[5];

	sfxHandle_t	sfx_dynamiteexp;
	sfxHandle_t sfx_dynamiteexpDist;
	sfxHandle_t	sfx_spearhit;
	sfxHandle_t	sfx_knifehit[5];
	sfxHandle_t	gibSound;
	sfxHandle_t	noAmmoSound;
	sfxHandle_t landSound[FOOTSTEP_TOTAL];

	sfxHandle_t fiveMinuteSound_g, fiveMinuteSound_a;
	sfxHandle_t twoMinuteSound_g, twoMinuteSound_a;
	sfxHandle_t thirtySecondSound_g, thirtySecondSound_a;

	sfxHandle_t watrInSound;
	sfxHandle_t watrOutSound;
	sfxHandle_t watrUnSound;
	sfxHandle_t watrGaspSound;

	sfxHandle_t underWaterSound;
	sfxHandle_t fireSound;
	sfxHandle_t waterSound;

	sfxHandle_t	grenadePulseSound4;
	sfxHandle_t	grenadePulseSound3;
	sfxHandle_t	grenadePulseSound2;
	sfxHandle_t	grenadePulseSound1;
//	sfxHandle_t	sparkSounds;

	// Ridah
	sfxHandle_t	flameSound;
	sfxHandle_t	flameBlowSound;
	sfxHandle_t	flameStartSound;
	sfxHandle_t	flameStreamSound;
	sfxHandle_t	flameCrackSound;
	sfxHandle_t	boneBounceSound;

	//sfxHandle_t grenadebounce1;
	//sfxHandle_t grenadebounce2;
	sfxHandle_t grenadebounce[FOOTSTEP_TOTAL][2];

	sfxHandle_t dynamitebounce1;	//----(SA)	added
	sfxHandle_t landminebounce1;

	sfxHandle_t fkickwall;
	sfxHandle_t fkickflesh;

	sfxHandle_t fkickmiss;

	int			bulletHitFleshScript;

	sfxHandle_t	satchelbounce1;

	qhandle_t cursor;
	qhandle_t selectCursor;
	qhandle_t sizeCursor;

	sfxHandle_t	uniformPickup;
	sfxHandle_t	minePrimedSound;
	sfxHandle_t	buildSound[4];
	sfxHandle_t	buildDecayedSound;

	sfxHandle_t	sndLimboSelect;
	sfxHandle_t	sndLimboFocus;
	sfxHandle_t	sndLimboFilter;
	sfxHandle_t	sndLimboCancel;

	sfxHandle_t	sndRankUp;
	sfxHandle_t	sndSkillUp;

	sfxHandle_t sndMedicCall[2];

	qhandle_t	ccStamps[2];
	qhandle_t	ccFilterPics[10];
	qhandle_t	ccFilterBackOn;
	qhandle_t	ccFilterBackOff;
	qhandle_t	ccPaper;
	qhandle_t	ccPaperConsole;
	qhandle_t	ccBars[3];
	qhandle_t	ccFlags[3];
	qhandle_t	ccLeather;
	//qhandle_t	ccArrow;
	qhandle_t	ccPlayerHighlight;
	qhandle_t	ccConstructIcon[2];
	qhandle_t	ccCmdPost[2];
	qhandle_t	ccDestructIcon[3][2];
	qhandle_t	ccTankIcon;
	qhandle_t	skillPics[SK_NUM_SKILLS];
	qhandle_t	ccMortarHit;
	qhandle_t	ccMortarTarget;
	qhandle_t	ccMortarTargetArrow;

	qhandle_t	currentSquadBackground;
	qhandle_t	SPTeamOverlayUnitBackground;
	qhandle_t	SPTeamOverlayUnitSelected;
	qhandle_t	SPTeamOverlayBotOrders[BOT_ACTION_MAX];
	qhandle_t	SPTeamOverlayBotOrdersBkg;
	qhandle_t	SPPlayerInfoSpecialIcon;
	qhandle_t	SPPlayerInfoHealthIcon;
	qhandle_t	SPPlayerInfoStaminaIcon;
	qhandle_t	SPPlayerInfoAmmoIcon;

	// Gordon: for commandmap
	qhandle_t	medicIcon;

	qhandle_t	hWeaponSnd;
	qhandle_t	hWeaponEchoSnd;
	qhandle_t	hWeaponHeatSnd;

	qhandle_t	hWeaponSnd_2;
	qhandle_t	hWeaponEchoSnd_2;
	qhandle_t	hWeaponHeatSnd_2;

//	qhandle_t	hflakWeaponSnd;

	qhandle_t	hMountedMG42Base;	//	trap_R_RegisterModel( "models/mapobjects/tanks_sd/mg42nestbase.md3" );
	qhandle_t	hMountedMG42Nest;	//	trap_R_RegisterModel( "models/mapobjects/tanks_sd/mg42nest.md3" );
	qhandle_t	hMountedMG42;		//	trap_R_RegisterModel( "models/mapobjects/tanks_sd/mg42.md3" );
	qhandle_t	hMountedBrowning;
	qhandle_t	hMountedFPMG42;
	qhandle_t	hMountedFPBrowning;

	// Gordon: medals
	qhandle_t		medals[SK_NUM_SKILLS];
	qhandle_t		medal_back;

	// Gordon: new limbo stuff
	fontInfo_t		limboFont1;
	fontInfo_t		limboFont1_lo;
	fontInfo_t		limboFont2;
	qhandle_t		limboNumber_roll;
	qhandle_t		limboNumber_back;
	qhandle_t		limboStar_roll;
	qhandle_t		limboStar_back;
	qhandle_t		limboWeaponNumber_off;
	qhandle_t		limboWeaponNumber_on;
	qhandle_t		limboWeaponCard;
	qhandle_t		limboWeaponCardSurroundH;
	qhandle_t		limboWeaponCardSurroundV;
	qhandle_t		limboWeaponCardSurroundC;
	qhandle_t		limboWeaponCardOOS;
	qhandle_t		limboLight_on;
	qhandle_t		limboLight_on2;
	qhandle_t		limboLight_off;

	qhandle_t		limboClassButtons[NUM_PLAYER_CLASSES];
	//qhandle_t		limboClassButtonBack;

	qhandle_t		limboClassButton2Back_on;
	qhandle_t		limboClassButton2Back_off;
	qhandle_t		limboClassButton2Wedge_on;
	qhandle_t		limboClassButton2Wedge_off;
	qhandle_t		limboClassButtons2[NUM_PLAYER_CLASSES];

//	skill_back_on
//	skill_back_off
//	skill_4pieces
//	skill_glass_top_layer
//	skill_testicon

	qhandle_t		limboTeamButtonBack_on;
	qhandle_t		limboTeamButtonBack_off;
	qhandle_t		limboTeamButtonAllies;
	qhandle_t		limboTeamButtonAxis;
	qhandle_t		limboTeamButtonSpec;
	qhandle_t		limboBlendThingy;
	qhandle_t		limboWeaponBlendThingy;
	qhandle_t		limboSkillsLW;
	qhandle_t		limboSkillsBS;
	//qhandle_t		limboCursor_on;
	//qhandle_t		limboCursor_off;
	qhandle_t		limboCounterBorder;
	qhandle_t		limboWeaponCard1;
	qhandle_t		limboWeaponCard2;
	qhandle_t		limboWeaponCardArrow;
	qhandle_t		limboObjectiveBack[3];
	qhandle_t		limboClassBar;
	qhandle_t		limboBriefingButtonOn;
	qhandle_t		limboBriefingButtonOff;
	qhandle_t		limboBriefingButtonStopOn;
	qhandle_t		limboBriefingButtonStopOff;

	qhandle_t		limboSpectator;
	qhandle_t		limboRadioBroadcast;

	qhandle_t		cursorIcon;



	qhandle_t		hudPowerIcon;
	qhandle_t		hudSprintIcon;
	qhandle_t		hudHealthIcon;

	qhandle_t		pmImages[PM_NUM_TYPES];
	qhandle_t		pmImageAlliesConstruct;
	qhandle_t		pmImageAxisConstruct;
	qhandle_t		pmImageAlliesMine;
	qhandle_t		pmImageAxisMine;
	qhandle_t		hintKey;

	qhandle_t		hudDamagedStates[4];

	qhandle_t		browningIcon;

	qhandle_t		axisFlag;
	qhandle_t		alliedFlag;

	qhandle_t		disconnectIcon;

	qhandle_t		fireteamicons[6];
} cgMedia_t;

typedef struct {
	char lmsdescription[1024];
	char description[1024];
	char axiswintext[1024];
	char alliedwintext[1024];
	char longname[128];
	vec2_t mappos;
} arenaInfo_t;

typedef struct {
	char		campaignDescription[2048];
	char		campaignName[128];
	char		mapnames[MAX_MAPS_PER_CAMPAIGN][MAX_QPATH];
	vec2_t		mappos[MAX_MAPS_PER_CAMPAIGN];
	arenaInfo_t	arenas[MAX_MAPS_PER_CAMPAIGN];
	int			mapCount;
	int			current;
	vec2_t		mapTC[2];
} cg_campaignInfo_t;

#define MAX_COMMAND_INFO MAX_CLIENTS

#define MAX_STATIC_GAMEMODELS	1024

typedef struct cg_gamemodel_s {
	qhandle_t		model;
	vec3_t			org;
	vec3_t			axes[3];
	vec_t			radius;
} cg_gamemodel_t;

typedef struct cg_weaponstats_s {
	int numKills;
	int numHits;
	int numShots;
} cg_weaponstats_t;

typedef struct {
	char strWS[WS_MAX][MAX_STRING_TOKENS];
	char strExtra[2][MAX_STRING_TOKENS];
	char strRank[MAX_STRING_TOKENS];
	char strSkillz[SK_NUM_SKILLS][MAX_STRING_TOKENS];
	int cWeapons;
	int cSkills;
	qboolean fHasStats;
	int nClientID;
	int nRounds;
	int fadeTime;
	int show;
	int requestTime;
} gameStats_t;

typedef struct {
	char strWS[WS_MAX*2][MAX_STRING_TOKENS];
	int cWeapons;
	int fadeTime;
	int show;
	int requestTime;
} topshotStats_t;

typedef struct oidInfo_s {
	int spawnflags;
	qhandle_t customimageallies;
	qhandle_t customimageaxis;
	int entityNum;
	int objflags;
	char name[MAX_QPATH];
	vec3_t origin;
} oidInfo_t;

#define NUM_ENDGAME_AWARDS 14


// The client game static (cgs) structure hold everything
// loaded or calculated from the gamestate.  It will NOT
// be cleared when a tournement restart is done, allowing
// all clients to begin playing instantly
typedef struct {
	gameState_t		gameState;			// gamestate from server
	glconfig_t		glconfig;			// rendering configuration
	float			screenXScale;		// derived from glconfig
	float			screenYScale;
	float			screenXBias;

	int				serverCommandSequence;	// reliable command stream counter
	int				processedSnapshotNum;// the number of snapshots cgame has requested

	qboolean		localServer;		// detected on startup by checking sv_running

	// parsed from serverinfo
	gametype_t		gametype;
	int				antilag;

	float			timelimit;				// NERVE - SMF - made this a float
	int				maxclients;
	char			mapname[MAX_QPATH];
	char			rawmapname[MAX_QPATH];
	char			redTeam[MAX_QPATH];		// A team
	char			blueTeam[MAX_QPATH];	// B team
	float			weaponRestrictions;

	int				voteTime;
	int				voteYes;
	int				voteNo;
	qboolean		voteModified;			// beep whenever changed
	char			voteString[MAX_STRING_TOKENS];

	int				teamVoteTime[2];
	int				teamVoteYes[2];
	int				teamVoteNo[2];
	qboolean		teamVoteModified[2];	// beep whenever changed
	char			teamVoteString[2][MAX_STRING_TOKENS];

	int				levelStartTime;
	int				intermissionStartTime;

	//
	// locally derived information from gamestate
	//
	qhandle_t		gameModels[MAX_MODELS];
	char			gameShaderNames[MAX_CS_SHADERS][MAX_QPATH];
	qhandle_t		gameShaders[MAX_CS_SHADERS];
	qhandle_t		gameModelSkins[MAX_MODELS];
	bg_character_t	*gameCharacters[MAX_CHARACTERS];
	sfxHandle_t		gameSounds[MAX_SOUNDS];

	int				numInlineModels;
	qhandle_t		inlineDrawModel[MAX_MODELS];
	vec3_t			inlineModelMidpoints[MAX_MODELS];

	clientInfo_t	clientinfo[MAX_CLIENTS];

	// teamchat width is *3 because of embedded color codes
	char			teamChatMsgs[TEAMCHAT_HEIGHT][TEAMCHAT_WIDTH*3+1];
	int				teamChatMsgTimes[TEAMCHAT_HEIGHT];
	team_t			teamChatMsgTeams[TEAMCHAT_HEIGHT];
	int				teamChatPos;
	int				teamLastChatPos;

	// New notify mechanism for obits
	char			notifyMsgs[NOTIFY_HEIGHT][NOTIFY_WIDTH*3+1];
	int				notifyMsgTimes[NOTIFY_HEIGHT];
	int				notifyPos;
	int				notifyLastPos;

	int cursorX;
	int cursorY;
	qboolean eventHandling;
	qboolean mouseCaptured;
	qboolean sizingHud;
	void *capturedItem;
	qhandle_t activeCursor;

	// screen fading
	float	fadeAlpha, fadeAlphaCurrent;
	int		fadeStartTime;
	int		fadeDuration;

	// media
	cgMedia_t		media;

	// player/AI model scripting (client repository)
	animScriptData_t	animScriptData;

	int			currentVoiceClient;
	int			currentRound;
	float		nextTimeLimit;
	int			minclients;
	gamestate_t	gamestate;
	char		*currentCampaign;
	int			currentCampaignMap;

	int complaintClient;		// DHM - Nerve
	int complaintEndTime;		// DHM - Nerve
	float smokeWindDir; // JPW NERVE for smoke puffs & wind (arty, airstrikes, bullet impacts)

	playerStats_t		playerStats;
	int					numOIDtriggers;
	int					teamobjectiveStats[MAX_OID_TRIGGERS];

	qboolean			campaignInfoLoaded;
	cg_campaignInfo_t	campaignData;

	qboolean			arenaInfoLoaded;
	arenaInfo_t			arenaData;

	centity_t *			gameManager;

	int					ccLayers;
	int					ccLayerCeils[MAX_COMMANDMAP_LAYERS];
	float				ccZoomFactor;

	int					invitationClient;
	int					invitationEndTime;

	int					applicationClient;
	int					applicationEndTime;

	int					propositionClient;
	int					propositionClient2;
	int					propositionEndTime;

	int					autoFireteamEndTime;
	int					autoFireteamNum;

	int					autoFireteamCreateEndTime;
	int					autoFireteamCreateNum;

	int					autoFireteamJoinEndTime;
	int					autoFireteamJoinNum;


	qboolean			autoMapExpanded;
	int					autoMapExpandTime;

	qboolean			autoMapOff;		// is automap on or off

	bg_character_t		*offscreenCmdr;

	// OSP
	int					aviDemoRate;					// Demo playback recording
	int					aReinfOffset[TEAM_NUM_TEAMS];	// Team reinforcement offsets
	int					cursorUpdate;					// Timeout for mouse pointer view
	fileHandle_t		dumpStatsFile;					// File to dump stats
	char*				dumpStatsFileName;				// Name of file to dump stats
	int					dumpStatsTime;					// Next stats command that comes back will be written to a logfile
	int					game_versioninfo;				// game base version
	gameStats_t			gamestats;
	topshotStats_t		topshots;
	qboolean			fResize;						// MV window "resize" status
	qboolean			fSelect;						// MV window "select" status
	qboolean			fKeyPressed[256];				// Key status to get around console issues
	int					timescaleUpdate;				// Timescale display for demo playback
	int					thirdpersonUpdate;

	cg_gamemodel_t		miscGameModels[MAX_STATIC_GAMEMODELS];

	vec2_t				ccMenuPos;
	qboolean			ccMenuShowing;
	int					ccMenuType;
	mapEntityData_t		ccMenuEnt;
	int					ccSelectedLayer;
	int					ccSelectedObjective;
	int					ccSelectedTeam;		// ( 1 = ALLIES, 0 = AXIS )
	int					ccSelectedWeaponNumber;
	int					ccSelectedClass;
	int					ccSelectedWeapon;
	int					ccSelectedWeapon2;
	int					ccWeaponShots;
	int					ccWeaponHits;
	vec3_t				ccPortalPos;
	vec3_t				ccPortalAngles;
	int					ccPortalEnt;
	int					ccFilter;
	int					ccCurrentCamObjective;
	int					ccRequestedObjective;
	int					ccLastObjectiveRequestTime;

	int					loadingLatch; // ( 0 = nothing yet, 1 = latched )

//	qboolean			playedLimboMusic;

	int					dbSortedClients[MAX_CLIENTS];
	int					dbSelectedClient;

	int					dbMode;
	qboolean			dbShowing;
	qboolean			dbAccuraciesRecieved;
	qboolean			dbPlayerKillsDeathsRecieved;
	qboolean			dbWeaponStatsRecieved;
	qboolean			dbAwardsParsed;
	char*				dbAwardNames[NUM_ENDGAME_AWARDS];
	team_t				dbAwardTeams[NUM_ENDGAME_AWARDS];
	char				dbAwardNamesBuffer[1024];
	int					dbLastRequestTime;
	int					dbLastScoreRequest;
	int					dbPlayerListOffset;
	int					dbWeaponListOffset;
	cg_weaponstats_t	dbWeaponStats[WS_MAX];
	int					dbChatMode;

	int					tdbAxisMapsXP[SK_NUM_SKILLS][MAX_MAPS_PER_CAMPAIGN];
	int					tdbAlliedMapsXP[SK_NUM_SKILLS][MAX_MAPS_PER_CAMPAIGN];
	int					tdbMapListOffset;
	int					tdbSelectedMap;

	int					ftMenuPos;
	int					ftMenuMode;
	int					ftMenuModeEx;

	qboolean			limboLoadoutSelected;
	qboolean			limboLoadoutModified;

	oidInfo_t			oidInfo[MAX_OID_TRIGGERS];

	qboolean			initing;
} cgs_t;

#define SL_NOTRACE			0x001	// don't do a trace check for shortening the beam, always draw at full 'range' length
#define SL_NODLIGHT			0x002	// don't put a dlight at the end
#define SL_NOSTARTCAP		0x004	// dont' cap the start circle
#define SL_LOCKTRACETORANGE	0x010	// only trace out as far as the specified range (rather than to max spot range)
#define SL_NOFLARE			0x020	// don't draw a flare when the light is pointing at the camera
#define SL_NOIMPACT			0x040	// don't draw the impact mark
#define SL_LOCKUV			0x080	// lock the texture coordinates at the 'true' length of the requested beam.
#define SL_NOCORE			0x100	// don't draw the center 'core' beam
#define SL_TRACEWORLDONLY	0x200

#define CAM_PRIMARY 0	// the main camera for cutscenes, etc.

typedef struct {
	weapon_t 	weapindex;
	const char	*desc;
} weaponType_t;

#define CC_FILTER_AXIS			(1 << 0)
#define CC_FILTER_ALLIES		(1 << 1)
#define CC_FILTER_SPAWNS		(1 << 2)
#define CC_FILTER_CMDPOST		(1 << 3)
#define CC_FILTER_HACABINETS	(1 << 4)
#define CC_FILTER_CONSTRUCTIONS	(1 << 5)
#define CC_FILTER_DESTRUCTIONS	(1 << 6)
#define CC_FILTER_OBJECTIVES	(1 << 7)
//#define CC_FILTER_WAYPOINTS		(1 << 7)
//#define CC_FILTER_OBJECTIVES	(1 << 8)

typedef struct {
	qhandle_t shader;
	const char *iconname;
	int width;
	int height;
} rankicon_t;

#define TAB_LEFT_WIDTH 178
#define TAB_LEFT_EDGE (640-TAB_LEFT_WIDTH)
// START Mad Doc - TDF

#define MAX_SQUAD_SIZE 6

//
// merged the common UI elements
//
#define UI_CAMPAIGN_BRIEFING 0
#define UI_COMMAND_MAP 1
#define UI_SQUAD_SELECT 2
#define ORDER_ICON_FADE_TIME 3500
#define /*fireteamData_t**/ CG_IsOnFireteam( /*int*/ clientNum ) /*{ return*/ cgs.clientinfo[clientNum].fireteamData /*}*/

// OSP
#define Pri(x) CG_Printf("[cgnotify]%s", CG_LocalizeServerCommand(x))
#define CPri(x) CG_CenterPrint(CG_LocalizeServerCommand(x), SCREEN_HEIGHT - (SCREEN_HEIGHT * 0.2), SMALLCHAR_WIDTH);
//
// cg_commandmap.c
//
// A scissored map always has the player in the center
typedef struct mapScissor_s {
	qboolean circular;	// if qfalse, rect
	float zoomFactor;
	vec2_t tl;
	vec2_t br;
} mapScissor_t;

#define LIMBO_3D_X	287	//%	280
#define LIMBO_3D_Y	382
#define LIMBO_3D_W	128
#define LIMBO_3D_H	96	//%	94

#define CC_2D_X 64
#define CC_2D_Y 23
#define CC_2D_W 352
#define CC_2D_H 352
