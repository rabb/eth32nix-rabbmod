// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#define MAX_FRAME_TIMES	11

#define SCREEN_HEIGHT_CENTER 480 / 2	// center screen used for possition
#define SCREEN_WIDTH_CENTER 640 / 2		// center screen used for possition
#define BASE_WIDTH 500					// size width of drawn circle display
#define BASE_HEIGHT 500					// size height of drawn circle display

/*typedef enum {
	HIT_OFF,
	HIT_HEAD,
	HIT_HEADSHOT,
	HIT_BODY, 
	HIT_MAX
} hitSounds_t;*/

class CArty
{
public:
	CArty(int startTime, int id, int team, vec3_t pos);
	int Id();
	bool Compare(gentity_t *bomb);
	bool Finished();
	float *Pos();
	void AddBomb(gentity_t *bomb);
	bool GetBomb(vec3_t pos);
	int Team();
	int GetImpactTime();
	~CArty();

private:
	int _bombs;
	int _bombtime;
	int _start;
	int _id;
	int _team;
	vec3_t _origin;
	vec3_t _bomb_origin;
};

class CMortar
{
public:
	CMortar(gentity_t *shell);
	bool Compare(gentity_t *shell);
	bool Finished();
	void DrawTrace();
	int Id();
	int TimeRemaining();
	float *ImpactPos();
	int Team();
private:
	int impactTime;
	int _team;
	int _id;
	int N;
	gentity_t* mshell;
	vec3_t impactPos;
	vec3_t trajectory[404];
	bool ptvisible[404];
};

class CEngine
{
public:
	CEngine(void);

	void RegisterMedia();
	void SetConsoleShader();
	void MenuCloseActions();
	void GetTag(int clientNum, char *tagName, orientation_t *head);
	void CG_Init_Pre(int serverMessageNum, int serverCommandSequence, int clientNum);
	void CG_Init_Post(int serverMessageNum, int serverCommandSequence, int clientNum);
	int CG_Shutdown(void);
	int CG_DrawActiveFrame(int serverTime, int stereoView, int demoPlayback);

	int CalcViewValues(void);
	void CorrectServerAngles();
	//void addDamageTaken(int dmg, int time);

	void PreFrame(int serverTime);
	void PostFrame(void);
	
	//void CG_ParseScore( team_t team );

	void StartPlayerCapture(centity_t *cent);
	void EndPlayerCapture(void);
	void CapturePlayerHead(refEntity_t *refent);

	void SetDrawWeapon(bool state) { drawWeapon = state; }

	void CmdActivate();
	//void RenderWeapon(int x, int y, int size, int weapon = 0);

	void AddRefEntity(refEntity_t *re);
	void AddPolyToScene(qhandle_t shader, polyVert_t *polyVert);
	
	void addDamageTaken(int dmg, int time);
	void DrawWarningMessages();
	void DrawItemESP();
	void DrawHitIndicator();
	void DrawCustomCrosshairs();

	void PlayerEsp(player_t *player);
	void GentityESP(gentity_t *gentity);
	void RunArty(gentity_t *gentity);
	void ArtyEsp();
	void GentityDecal();
	void GentityESPTCE(gentity_t *gentity);
	void DrawArtyMarkers(gentity_t *gentity);
	void DrawMortarTrace();
	//void DrawOutline();

	void MakeRailTrail( vec3_t start, vec3_t end, bool box, vec3_t color, int lifeTime );
	void SetRailTrailTime( cvar_t *cv );

	// Event Handlers for CG_EntityEvent
	void EV_Obituary(entityState_t *es);
	void Obituary(int attacker, int target);
	void ParseServerSideObituary(char *msg);
	void EV_Bullet(entityState_t *es, bool hitFlesh);

	void ParseWeaponStats();
	void GetSpawntimes();
	float FrameCorrectionTime(int n);

	const char *ConfigString(int index);

	void setMuzzle(void);
	int findSatchel(void);

	bool IsVisible(vec3_t start, vec3_t pt, int skipEntity); // needed one outside of aim class

	// public members
	float *forward;		// moved here so we calculate this only *one* time per frame
	lerpFrame_t *slerp;
	int			lastPBQuery;				// time of last PB Query
	bool		wantPBQuery;				// are we waiting for PB response
	char		PBQuery[128];

	// remove me!!!
	//int sfxplay;
	//bool played;

private:
	player_t	*pDrawPlayer;			// used to see if refent being added is part of a player
	bool		drawWeapon;
	bool		cgHasFirst;				// used for first frame time grabber
	int			multiKill;				// amount of
	int			lastKill;				// time of last kill we made

	int 		frameTimes[MAX_FRAME_TIMES-1];
	int 		frameN;
	int			framePlayers[MAX_CLIENTS];	// indices into our main players array, added through cg_player
	int			numFramePlayers;			// count of actual drawn/valid players this frame
	bool		playerDrawn;				// safety check, used to verify player was actually added to scene by cg_player
	cvar_t *	railtrailTime;				// cvar time for quick reference for our own railtrail

	vector<gentity_t*> entGrenades;		// local storage for preframe chores
	vector<CArty*> Arty;				// local storage for preframe chores
	vector<CMortar*> Mortar;			// local storage for preframe chores
	
	int lastID, lastHP, lastDmg;	// Used for hit indicator

	void _CG_RailTrail( vec3_t start, vec3_t end, vec3_t color, int lifeTime );

	int aReinfOffset[TEAM_NUM_TEAMS];
};

extern CEngine Engine;

extern const char *Shader1Text[];
extern const char *Shader2Text[];
extern const char *Shader3Text[];
extern const char *Shader4Text[];
extern const char *pbssText[];
extern const char *hitsoundsText[];
extern const char *playerEspText[];
extern const char *gentityEspText[];
extern const char *gentityEspTextTCE[];
extern const char *classEspText[];
extern const char *crosshairText[];
extern const char *damageIndicatorText[];
