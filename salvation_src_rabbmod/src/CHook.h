// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

#define MAX_ET_VERSIONS		4 /* increased to 6 so if new et version gets released, we can still add support to old eth32 */
#define MAX_CG_MODS			128

class CHook
{
public:
	void hookSystem(int Game);
	int isGameSupported(char *etFile);
	void hookGame(bool state);
	int isCgameSupported(char *lpLibName);
	void hookCgame(bool state);
	void hookEtpro(bool state);
	void hookUi(bool state);
	void UnreferenceBadPk3(void);
	void InitCgameData(void);
	void InitWeaponList(void);
	void ModifyCgameCommands();
	bool getSymbols(cgMod_t *mod, void *handle, void *mbase);
	void InitPB();
	void InitCGameHook();
	void GetPredictedPlayerState();
	
	void ToggleHack(bool enabled);

	// Here because weapdefs are static and only visible by CHook
	void SaveWeapons(const char *filename);
	void LoadWeapons(const char *filename);

	cgMod_t cglist[MAX_CG_MODS];		// other offsets.ini defined mods
	cgMod_t	cgEth32Mod;					// eth32's client mod
};

extern CHook Hook;

// for pk3 unlocker - backup good checksums
extern char goodChecksumsPak[BIG_INFO_STRING];
