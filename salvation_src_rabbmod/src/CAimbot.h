// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once

#define ANG_CLIP(ang) if(ang>180.0f)ang-=360.0f;else if(ang<-180.0f)ang+=360.0f
#define AIM_FOV_VALUE eth32.settings.fov

typedef enum
{
	RF_PREDICT_OFF,
	RF_PREDICT_LINEAR,
	RF_PREDICT_LINEAR2,
	RF_PREDICT_AVG,
	RF_PREDICT_SMART,
	RF_PREDICT_MAX
} rf_predict_t;

typedef enum {
	SORT_OFF,
	SORT_DISTANCE,
	SORT_ATTACKER,
	SORT_CROSSHAIR,
	SORT_KDRATIO,
	SORT_ACCURACY,
	//SORT_TARGETING,
	SORT_THREAT,
	SORT_MAX
} sorttype_t;


typedef enum {
	SPR_OFF,
	SPR_MANUAL,
	SPR_PING,
	SPR_LEET,
	SPR_MAX
} spredict_t;

extern const char *sortTypeText[];
extern const char *selfPredictText[];
extern const char *humanModeText[];
extern const char *aimModeText[];
extern const char *riflePredictText[];
extern const char *aimprotectText[];

/* enum and struct for advanced trace */
// No need for complex traces in the head since it's small
typedef enum {
	HEAD_CENTER,			// trace center point only
	HEAD_STATIC,			// generate static points
	HEAD_XTRACE,
	HEAD_MAX
} headBoxtrace_t;

typedef enum {
	BODY_CENTER,			// trace center point only
	BODY_CONTOUR,			// trace periphery and plane centers
	BODY_STATIC,			// generate static points
	BODY_XTRACE,
 	BODY_RANDOM_VOLUME,		// generate random points within box volume
   	BODY_RANDOM_SURFACE,		// generate random points on the side(s) of the box that is(are) facing us
	BODY_CAPSULE_VOLUME,		// fill a capsule with random points
	BODY_CAPSULE_SURFACE,		// fill the surface of a capsule with random points
	BODY_MAX
} bodyBoxtrace_t;

extern const char *bodyTraceTypeText[BODY_MAX];
extern const char *headTraceTypeText[HEAD_MAX];

typedef struct {
	vec3_t pt;
	float  d;
} trace_point;

/* mdx box offsets vary on stance, movement */
typedef struct {
	vec3_t stand_offset;
	vec3_t crouch_offset;
	vec3_t prone_offset;
 	vec3_t stand_offset_moving;
 	vec3_t crouch_offset_moving;
	vec3_t size;
} hitbox_t;

typedef enum {
	HITBOX_OFF,
	HITBOX_ETMAIN,
 	HITBOX_ETPUB,
 	HITBOX_ETPRO,
 	HITBOX_GENERIC,
 	HITBOX_CUSTOM,
	HITBOX_MAX
} hitbox_type_t;

extern const char *hitboxText[];

/*
These define the head hitboxes. format is { fwd, right, up }, relative to the tag's orientation.
Matched them as good as possibly with server hitboxes using modded servercode.
Hitboxes don't rotate in world space, but the offset rotates with tag_head.
Always use correct hitbox mode.. wrong vecs == bad.
*/

static hitbox_t head_hitboxes[] =
{
    // stand               crouch         prone              stand moving      crouch moving    hitbox size (x,y,z)
 { { 0.0, 0.0, 0.0}, { 0.0, 0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0,  0.0, 0.0}, { 0.0, 0.0, 0.0} },	// OFF
 { { 0.0, 0.0, 4.0}, { 0.0, 0.0, 4.0}, { 0.0,  0.0, 4.0}, { 0.0,  0.0, 4.0}, { 0.0,  0.0, 4.0}, {12.0,12.0,12.0} },	// ETMAIN
 { { 3.0, 0.0, 6.5}, { 3.0,-0.5, 6.0}, { 1.0,  0.0, 7.0}, {-5.0, -1.0, 6.5}, { 1.0,  3.0, 4.5}, {12.0,12.0,12.0} },	// ETPUB FIXED
 { { 0.3, 0.3, 7.0}, {-0.3, 0.8, 7.0}, { 0.0,  0.3, 6.9}, { 0.0,  0.0, 6.5}, { 0.0, -0.7, 7.0}, {11.0,11.0,12.0} },	// ETPRO b_realhead 1
 { { 0.5, 0.0, 6.5}, { 0.5, 0.0, 6.5}, { 0.5,  0.0, 6.5}, { 0.5,  0.0, 6.5}, { 0.5,  0.0, 6.5}, {12.0,12.0,12.0} },	// GENERIC REALISTIC
};

// look up table for grenade pitch angle corrections
static const float pitchLUT[] =	{
-29.940611, -27.034554, -23.890945, -20.510151, -16.793974, -12.551514, -7.298284, 0.055201,
  0.704957,   1.326615,   1.946186,   2.563671,   3.179049,   3.792287,  4.403395, 5.012323,
  5.619093,   6.223704,   6.826141,   7.426332,   8.024317,   8.620029,  9.213522, 9.804779,
10.393741, 10.980407, 11.564758, 12.146841, 12.726602, 13.304017, 13.879052, 14.451786,
15.022149, 15.590113, 16.155684, 16.718927, 17.279634, 17.838060, 18.394053, 18.947599,
19.498657, 20.047338, 20.593554, 21.137257, 21.678581, 22.217396, 22.753683, 23.287575,
23.818911, 24.347794, 24.874197, 25.398024, 25.919397, 26.438269, 26.954670, 27.468493,
27.979818, 28.488661, 28.994915, 29.498734, 30.000000, 30.498722, 30.994904, 31.488649,
31.979805, 32.468502, 32.954624, 33.438316, 33.919384, 34.398067, 34.874126, 35.347813,
35.818886, 36.287575, 36.753723, 37.217335, 37.678581, 38.137245, 38.593544, 39.047367,
39.498642, 39.947594, 40.394047, 40.838039, 41.279705, 41.718887, 42.155651, 42.590126,
43.022137, 43.451740, 43.879078, 44.304012, 44.726547, 45.146797, 45.564777, 45.980412,
46.393669, 46.804729, 47.213554, 47.620060, 48.024254, 48.426266, 48.826126, 49.223709,
49.619137, 50.012283, 50.403313, 50.792255, 51.179031, 51.563671, 51.946026, 52.326572,
52.704929, 53.081238, 53.455547, 53.827728, 54.197945, 54.566151, 54.932316, 55.296532,
55.658813, 56.019176, 56.377644, 56.734222, 57.088898, 57.441681, 57.792614, 58.141712,
58.489002, 58.834484, 59.178200, 59.520130, 59.860294, 60.198750, 60.535496, 60.870525,
61.203896, 61.535610, 61.865681, 62.194126, 62.520954, 62.846214, 63.169899, 63.492020,
63.812611, 64.131676, 64.449242, 64.765297, 65.079903, 65.393059, 65.704262, 66.015091,
66.324036, 66.631096, 66.937584, 67.242256, 67.546028, 67.848122, 68.149155, 68.448814,
68.747238, 69.044357, 69.340286, 69.635010, 69.928505, 70.220840, 70.512039, 70.802010,
71.091095, 71.378983, 71.665276, 71.951469, 72.235947, 72.519569, 72.802261, 73.083771,
73.364548, 73.644112, 73.922897, 74.200623, 74.477440, 74.753410, 75.028511, 75.302696,
75.576050, 75.848495, 76.120171, 76.391037, 76.661186, 76.930420, 77.199059, 77.466805,
77.733871, 78.000374, 78.266006, 78.531090, 78.795357, 79.059036, 79.322220, 79.584633,
79.846550, 80.107788, 80.368484, 80.628677, 80.888290, 81.147354, 81.405907, 81.664009,
81.921631, 82.178780, 82.435440, 82.691650, 82.947495, 83.202988, 83.458015, 83.712669,
83.966988, 84.220924, 84.474602, 84.727859, 84.980858, 85.233582, 85.486038, 85.738274,
85.990158, 86.241966, 86.493439, 86.744675, 86.995804, 87.246826, 87.497604, 87.748245
};

// lookup table for grenade velocity as a function of view pitch
static const float speedLUT[] = {
899.431458, 895.500000, 891.000000, 886.500000, 882.000000, 877.500000, 873.000000, 868.500000,
864.000000, 859.500000, 855.000000, 850.500000, 846.000000, 841.500000, 837.000000, 832.500000,
828.000000, 823.500000, 819.000000, 814.500000, 810.000000, 805.500000, 801.000000, 796.500000,
792.000000, 787.500000, 783.000000, 778.500000, 774.000000, 769.500000, 765.000000, 760.500000,
756.000000, 751.500000, 747.000000, 742.500000, 738.000000, 733.500000, 729.000000, 724.500000,
720.160461, 719.076416, 718.109741, 717.096497, 716.039795, 714.935425, 713.788635, 712.596497,
711.359802, 710.076477, 708.749878, 707.376526, 705.959778, 704.496521, 702.989868, 701.436646,
699.839966, 698.196716, 696.509766, 694.776855, 692.999573, 691.176941, 689.309448, 687.397095,
685.439331, 683.437256, 681.389221, 679.297424, 677.159058, 674.977356, 672.749084, 670.477356,
668.158997, 665.795715, 663.387695, 660.937256, 658.438904, 655.897278, 653.308899, 650.677246,
647.998901, 645.277222, 642.508972, 639.696716, 636.838196, 633.937317, 630.989075, 627.997437,
624.959167, 621.877502, 618.748779, 615.577271, 612.359436, 609.097168, 605.789551, 602.437012,
599.039551, 595.596863, 592.109741, 588.576721, 585.000000, 579.150024, 573.299988, 567.450012,
561.599976, 555.750000, 549.900024, 544.049988, 538.200012, 532.349976, 526.500000, 520.650024,
514.799988, 508.950012, 503.100006, 497.250000, 491.399994, 485.549988, 479.700012, 473.850006,
468.000000, 462.149994, 456.299957, 450.449982, 444.599976, 438.749969, 432.899963, 427.049957,
421.199951, 415.349976, 409.499969, 403.649963, 397.799957, 391.949982, 386.099976, 380.249969,
374.399963, 368.549957, 362.699982, 356.849976, 351.000000, 345.149994, 339.299988, 333.450012,
327.600006, 321.750000, 315.899994, 310.049988, 304.200012, 298.350006, 292.500000, 286.649994,
280.799988, 274.950012, 269.100006, 263.250000, 257.399994, 251.550003, 245.699997, 239.850006,
234.000000, 228.149994, 222.300003, 216.449997, 210.600006, 204.750000, 198.899994, 193.049988,
187.199982, 181.349991, 175.499985, 169.649979, 163.799988, 157.949982, 152.099991, 146.249985,
140.399979, 134.549988, 128.699982, 122.849983, 117.000000
};

/*typedef enum
{
	ADJUST_NONE,
	ADJUST_SIMPLE_ADD,
	ADJUST_SIMPLE_VECTORMA,
	ADJUST_SOL_TESTING,
	ADJUST_MAX
} adjusttype_t;*/

// master aim mode
typedef enum
{
	AIMMODE_OFF,
	AIMMODE_NORMAL,
	AIMMODE_HUMAN,
	AIMMODE_MAX
} aimMode_t;

typedef enum
{
	HUMAN_AIM_HALF_LOGINS,
	HUMAN_AIM_FULL_LOGINS,
	HUMAN_AIM_MAX
} humanMode_t;

typedef enum
{
	AIM_OFF,
	AIM_ON_FIRE,
	AIM_ON_BUTTON,
	AIM_ALWAYS,
	AIM_TRIGGER,
	AIM_MAX
} aimtype_t;

extern const char *aimTypeText[];

typedef enum {
	BODY_ONLY,
	HEAD_ONLY,
	BODY_HEAD,
	HEAD_BODY,
	HEAD_PRIORITY,
	AP_MAX
} headBody_t;

extern const char *priorityTypeText[];

#define MAX_BULLETS 100

class CAimbot
{
public:
	CAimbot(void);
	void SetSelf( int clientNum );
	void PreFrame(void);
	void PostFrame(void);
	void Respawn(void);
	void AddTarget(player_t *player);
	void LockSensitivity(bool lock);
	bool SensitivityLocked();
	void Autofire(bool enable);
	void Autocrouch(bool enable, bool force);
	void SetUserAttackStatus(bool atkPressed);
	void SetUserCrouchStatus(bool crouched);
	void SetAimkeyStatus(bool state) { aimkeyPressed = state; }
	void DrawHeadBox( int clientNum, bool axes );
	void DrawBodyBox( int clientNum );
	void DrawGrenadelauncherTrace();
	void DrawGrenadeTrace();
	void SetAttackButton( void *ptr );
	void SetCrouchButton( void *ptr );
	//bool TargetingUs(int clientNum);
	float CrosshairDistance(const player_t *player);
	//bool PreCrouchTrace(vec3_t start, vec3_t pt, int skipEntity);

	// specialized for rifle aim
	void History(player_t *player);
	void PredictPlayer(player_t *player, float time, vec3_t pos, int type);

	hitbox_t customHitbox;	// user defined head hitbox. public for now so others can modify
	//int dbgPts;
	//float spreadd;

	// grenadebot
	player_t *grenadeTarget;
	bool GrenadeFireOK;
	bool RifleMultiBounce;
	bool GrenadeTicking;
	int grenadeFireTime;

	bool userCrouching;			// are we crouching
	bool userProning;			// are we proning

	// for cams
	vec3_t	lastImpact;
	float	flyTime;
	float	rifleGrenadeTime;
	float	Lastpitch;

private:
	void PointGrenade(vec3_t vieworg, float pitch);
	void Point(vec3_t vieworg);
	void selectGrenadeTarget(int direction);
	inline int CheckFov(vec3_t origin);

	inline void applySelfPrediction();
	//inline void smg();

	bool traceHeadBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, headBoxtrace_t trType,  vec3_t visible, int maxTraces );
	bool traceBodyBox( vec3_t boxOrigin, float dist, vec3_t size, vec3_t trOrigin, int skipEnt, player_t *player, bodyBoxtrace_t trType,  vec3_t visible, int maxTraces );
	//void BulletSize( int clientNum );
	bool traceHead( orientation_t *head, vec3_t hitPoint, int clientNum );
	bool traceHead( orientation_t *head, vec3_t hitPoint, vec3_t start, int clientNum );
	bool traceBody( vec3_t hitPoint, int clientNum );

	bool rifleEndPos(vec3_t forward, vec3_t impact, float *flytime);
	void grenadeAimbot(vec3_t point, float *fltime, bool *solution, bool pass);
	float speedForPitch(float pitch);
	void grenadePitchCorrection(float pitch, float *z);
	bool grenadePitchAngle(vec3_t start, vec3_t endpos, float maxTime, float *flytime, float *angle, bool longOnly);
	bool ballisticPitchAngle(vec3_t start, vec3_t endpos, float v, float maxTime, float *flytime, float *angle);
	bool ballisticTrajectoryValid(vec3_t start, vec3_t end, float pitch, float flytime, float v);
	bool multibounceAimbot(vec3_t point, float *fltime, float *ptch);

	void DoBulletBot(void);
	//bool IsPointVisible(vec3_t start, vec3_t pt, int skipEntity);

	player_t* frameTargets[MAX_CLIENTS];
	int numFrameTargets;

	bool stopAutoTargets;	// block auto targets if we manually intervene
	bool senslocked;		// mousemovent blocked
	bool firing;			// used for autofire purposes
	bool attackPressed;		// if fire button is down == true
	bool aimkeyPressed;		// if bound aim key is pressed
	bool validFrameTarget;	// used to check if we should allow +attack to pass
	bool autoMode;			// bot controls fire
	bool Trigger;			// trigger aim
	bool acFirstFrame;		// this first AC frame

	kbutton_t *atkBtn;		// ET.exe's attackbutton
	kbutton_t *crhBtn;		// ET.exe's crouchbutton

	//   else set target to NULL
	player_t *target;
	player_t *lastTarget;	// for use by lockTarget

	bool lastTargetValid;
	player_t *self;			// us :)
};

extern CAimbot Aimbot;
