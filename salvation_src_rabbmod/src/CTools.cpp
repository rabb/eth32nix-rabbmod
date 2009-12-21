// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org
// (ported to unix by kobj and blof)

#include "eth32.h"
#include "cvars.h"
#include "settings.h"

CTools Tools;

const char *namestealText[NAMESTEAL_MAX] =
{
	"Team only",
	"Enemy only",
	"Everyone",
	"File",
};

const char *killspamText[KILLSPAM_MAX] =
{
	"everyone",
	"selected players"
};

void CTools::Init(int Game)
{
	char *env;

	nThreads = NULL;
	sThreads = 0;
	memset(&eth32, 0, sizeof(eth32_t));

	if (eth32.path != getcwd(eth32.path, sizeof(eth32.path))) FATAL_ERROR("Couldn't get current working directory")

	env = getenv("ETH32_DIR");
	if (!env) FATAL_ERROR("No Eth32 home dir specified! export it in 'ETH32_DIR'")
	if (env) sprintf(eth32.path, env);

	// load in name, guid and jaymac lists
	char file[256];
	sprintf(file, "%s/namelist.txt", eth32.path);
	FILE *fp1 = fopen(file, "r");
	nNames = 0;
	if (fp1) {
		char name[256];
		while (fgets(name, 256, fp1)) {
			nameList = (char (*)[256]) realloc(nameList, (nNames+1)*256);
			char *q = strchr(name, '\n');
			if (q)
				*q = '\0';
			strcpy(nameList[nNames], name);
			nNames++;
		}
	}
	
#ifdef ETH32_DEBUG
	Debug.Log("Loaded %i names", nNames);
#endif


	fp_mutex = new pthread_mutex_t;
	pthread_mutex_init(fp_mutex,NULL);
	cc_mutex = new pthread_mutex_t;
	pthread_mutex_init(cc_mutex,NULL);
	print_mutex = new pthread_mutex_t;
	pthread_mutex_init(print_mutex,NULL);

	Spectator.Init();

	Tools.LoadUserCvars();

	if (!loadOffsets()){
	#ifdef ETH32_DEBUG
		Debug.Log("%s", errorMessage);
	#endif
		FATAL_ERROR("couldn't load offsets from %s/offsets.ini, check your eth32 path?", eth32.path)
	} else {
	#ifdef ETH32_DEBUG
		Debug.Log("Offsets successfully loaded from ini file");
	#endif
	}

	Hook.hookSystem(Game);
}

void CTools::Shutdown()
{
	Hook.hookGame(false);

	if (Tools.userCvarList)
		free(Tools.userCvarList);

	// Added this because CG_Shutdown not called if exiting right out of ET
	SaveSettings();

#ifdef ETH32_DEBUG
	Debug.Log("Unloading eth32, see u next time");
#endif
}

CTools::CTools()
{
}

void CTools::initCrc32()
{
	uint32 crc, poly;
	int i, j;

	poly = 0xedb88320L;
	for (i = 0; i < 256; i++){
		crc = i;
		for (j = 8; j > 0; j--){
			if (crc & 1)
				crc = (crc >> 1) ^ poly;
			else
				crc >>= 1;
		}
		crc_tab[i] = crc;
	}
}

uint32 CTools::block_chksum(uchar *block, uint32 length)
{
	register uint32 crc;
	uint32 i;

	crc = 0xffffffff;
	for (i = 0; i < length; i++)
		crc = ((crc >> 8) & 0x00ffffff) ^ crc_tab[(crc ^ *block++) & 0xff];

	return (crc ^ 0xffffffff);
}

uint32 CTools::crc32FromFile(const char *filename)
{
	FILE *fd = fopen( filename, "rb" );
	if( !fd )
		return 0;

	fseek(fd, 0, SEEK_END );
	int flen = ftell( fd );
	fseek(fd, 0, SEEK_SET );
	uchar* file_block = (uchar *)malloc( flen );

	if( !file_block ){
		fprintf( stderr, "misc.c: file_crc32: insufficient memory to allocate %i bytes\n", flen );
		fclose(fd);
		return 0;
	}

	fread( file_block, 1, flen, fd );
	u_int32_t crc32 = block_chksum( file_block, flen );
	free(file_block);
	fclose(fd);
	return crc32;
}

unsigned int CTools::ClientNumForAddr(uint32 dwCentAddr)
{
	// need to be sure centBase & centSize is defined for the mod
	// don't want to add a lot of checking here because it will probably be used quite a lot
	// TODO: maybe save &cg_entities[0] in cgame_t to avoid unneeded calcs
	return (dwCentAddr - eth32.cgMod->centBase - (uint32)eth32.cg.module) / eth32.cgMod->centSize;
}

int CTools::ScanCrosshairEntity(void)
{
	vec3_t angle, end;
	trace_t		trace;

	VectorCopy( eth32.cg.refdef->vieworg, angle);
	VectorMA( angle, 8192, eth32.cg.refdef->viewaxis[0], end );

	orig_CG_Trace( &trace, angle, NULL, NULL, end, eth32.cg.clientNum, CONTENTS_BODY );

	if(trace.entityNum < MAX_CLIENTS && eth32.cg.players[trace.entityNum].currentState->eType == ET_PLAYER)
	{
		return trace.entityNum;
	}

	return -1;
}

pack_t *CTools::getPack(char *pk3filename)
{
	searchpath_t *browse = *(searchpath_t **)eth32.et->fs_searchpaths;
	for (; browse; browse = browse->next)
		if (browse->pack && strstr(browse->pack->pakFilename, pk3filename))
			return browse->pack;
	return NULL;
}

void CTools::CharToVec3(const char *txt, vec3_t v)
{
	sscanf(txt, "%f %f %f", &v[0], &v[1], &v[2]);
}

void CTools::CharToVec4(const char *txt, vec4_t v)
{
	sscanf(txt, "%f %f %f %f", &v[0], &v[1], &v[2], &v[3]);
}

void CTools::DataAsHex(char *out, void *in, size_t size)
{
#define ENOUGH 1024
	unsigned char *buf = (unsigned char *)in;
	char tmp[32];
	int i = 0;
	out[0] = '\0';
	for (; i<size; i++){
		sprintf(tmp, "%02X ", (uint32) buf[i]);
		strcat(out, tmp);
	}
}

void CTools::SaveSettings(void)
{
	if (!eth32.settings.loaded)
		return;

	char settingsFile[MAX_PATH];
	sprintf(settingsFile, "%s/%s", eth32.path, ETH32_SETTINGS);
	Settings.Save(settingsFile);
	Hook.SaveWeapons(settingsFile);
	Gui.SaveSettings(settingsFile);
	SaveSpam(settingsFile);
}

uint32 CTools::GetPrivateProfileString(const char* lpAppName,const char* lpKeyName,const char* lpDefault, char* lpReturnedString,uint32 nSize,const char* lpFileName)
{
	FILE* fp;
	bool found = false;
	int rightApp=0;
	int n = 0;
	char line[1024];
	char *c1,*c2;
	*lpReturnedString=0;

	if (!lpReturnedString)
		return 0;

	if (pthread_mutex_lock(fp_mutex)) FATAL_ERROR("couldn't lock mutex")

	fp=fopen(lpFileName,"r");
	if (!fp){
		int ret = lpDefault ? strlen(lpDefault) : 0;
		if (lpDefault)
			strcpy(lpReturnedString, lpDefault);
		if (pthread_mutex_unlock(fp_mutex)) FATAL_ERROR("couldn't unlock mutex")
		return ret;
	}

	if (!lpAppName)
		memset(lpReturnedString, 0, nSize);

	while (fgets(line,1024,fp)){
		// truncate line after comment char (;)
		if (*line==';') continue;
		for (c1=line;c1<(line+strlen(line));c1++) if (*(c1)==';') *(c1)=0;

		// check for apps
		if ((*line=='[')&&(c2=strchr(line,']'))){
			c1=line+1;
			*c2=0;

			// in this case, return all sections
			if (!lpAppName){
				memcpy(lpReturnedString+n, c1, strlen(c1));
				n+=(strlen(c1)+1);
			} else if (!strcasecmp(c1,lpAppName))
				rightApp=1;
			else
				rightApp=0;

			continue;
		}

		// check for key
		if (lpAppName && rightApp==1){
			c1=line;
			if (c2=strchr(c1,'=')){
				*c2=0;
				if (!strcasecmp(c1,lpKeyName)){
					c2++;
					if (c2!=0){
						// exclude blanks and CR/LF at value end
						c1=c2+strlen(c2)-1;
						while ((*c1==' ')||(*c1=='\t')||(*c1=='\n')||(*c1=='\r')) c1--;
						*(c1+1)=0;
						// copy value and return
						snprintf(lpReturnedString,nSize,c2);
						found = true;
						break;
					}
				}
			}
		}
	}
	fclose(fp);

	if (!found && lpKeyName)
		snprintf(lpReturnedString, nSize, lpDefault);
	int ret = strlen(lpReturnedString);

	if (pthread_mutex_unlock(fp_mutex)) FATAL_ERROR("couldn't unlock mutex")
	return ret;
}

bool CTools::WritePrivateProfileString(const char *lpAppName, const char *lpKeyName, const char *lpString, const char *lpFileName)
{
	FILE* fp, *fp1;
	int n = -1;
	char line[1024];
	char *c1,*c2;
	char (*fline)[1024] = NULL;
	int start;
	bool foundApp = false;
	bool insert = false;

	if (pthread_mutex_lock(fp_mutex)) FATAL_ERROR("couldn't lock mutex")

	fp=fopen(lpFileName,"r+");
	if (fp) {
		bool thisApp = false;
		bool foundKey = false;
		while (fgets(line,1024,fp)){
			n++;
			fline = (char (*)[1024]) realloc(fline, (n+1)*1024);
			strcpy(fline[n], line);

			if (*line==';') continue;
			for (c1=line;c1<(line+strlen(line));c1++) if (*(c1)==';') *(c1)=0;

			// check for apps
			if ((*line=='[')&&(c2=strchr(line,']'))){
				c1=line+1;
				*c2=0;
				if (!strcasecmp(c1,lpAppName)){
					start = n;
					thisApp = true;
					foundApp = true;
					insert = true;
				} else {
					thisApp = false;
				}

				continue;
			}

			if (thisApp) {
				c1=line;
				if (c2=strchr(c1,'=')){
					*c2=0;
					if (!strcasecmp(c1,lpKeyName)){
						insert = false;
						sprintf(fline[n], "%s=%s\n", lpKeyName, lpString);
						foundKey = true;
					}
				}
			}
		}

		rewind(fp);
		int i;
		if (n>=0){
			for(i=0; i<n+1; i++){
				fprintf(fp, "%s", fline[i]);
				if (insert && i == start)
					fprintf(fp, "%s=%s\n", lpKeyName, lpString);
			}

			if (!foundApp)
				fprintf(fp, "\n[%s]\n%s=%s\n", lpAppName, lpKeyName, lpString);
		}
		fclose(fp);
	} else {
		fp = fopen(lpFileName, "w");
		if (fp){
			fprintf(fp, "[%s]\n%s=%s\n", lpAppName, lpKeyName, lpString);
			fclose(fp);
		}
	}

	if (fline)
		free(fline);

	if (pthread_mutex_unlock(fp_mutex)) FATAL_ERROR("couldn't unlock mutex")
}

void CTools::LoadSettings(bool loadGui)
{
	char settingsFile[MAX_PATH];
	sprintf(settingsFile, "%s/%s", eth32.path, ETH32_SETTINGS);

	Settings.Load(settingsFile);
	Hook.LoadWeapons(settingsFile);

	LoadSpam(settingsFile);

	if (loadGui)
		Gui.LoadSettings(settingsFile);

	eth32.settings.loaded = true;
}

void CTools::getOffsetFromINI( const char *key, uint32 *offset )
{
	char value[64];
	int i;
	GetPrivateProfileString(__section, key, "0x0", value, 64, offsetFile);

	for( i=0; i<(int)strlen(value); i++ )
		value[i] = tolower(value[i]);

	sscanf( value, "0x%x", offset );
}

void CTools::getOffsetFromINI( const char *key, int *offset )
{
	char value[64];
	int i;
	GetPrivateProfileString(__section, key, "0", value, 64, offsetFile);

	for( i=0; i<(int)strlen(value); i++ )
		value[i] = tolower(value[i]);

	sscanf( value, "%i", offset );
}

bool CTools::loadOffsets()
{
	char allSections[10000];
	int	 cScOff = 0;
	int  numCG = 0;
	bool automod = false;
	char tmp[64];

	cgMod_t	*newCG;

	sprintf(offsetFile, "%s/%s", eth32.path, ETH32_OFFSETS);

	GetPrivateProfileString(NULL,NULL,NULL,allSections, 10000, offsetFile);

	if (!allSections[0]){
		sprintf( errorMessage, "%s not found, aborting", offsetFile );
		return false;
	}

	while( allSections[cScOff]!='\0' ){
		strcpy(__section,allSections+cScOff);
	#ifdef ETH32_DEBUG
		Debug.Log("offsets.ini: found section %s", __section);
	#endif

		if (__section[0] == 'C' && __section[1] == 'G'){
			//handle CG offsets
			newCG = new cgMod_t;
			sscanf( __section+3, "0x%X", &newCG->crc32 );

			GetPrivateProfileString(__section, "auto", "false", tmp, 64, offsetFile);
			newCG->automod = (!strcasecmp(tmp, "true")) ? true : false;

			GetPrivateProfileString(__section, "eth32mod", "false", tmp, 64, offsetFile);
			newCG->eth32mod = (!strcasecmp(tmp, "true")) ? true : false;

			if (newCG->eth32mod)
				eth32.eth32mod = true;

			GetPrivateProfileString(__section, "modname", "unknown", newCG->name, 64, offsetFile);
			GetPrivateProfileString(__section, "modversion", "unknown", newCG->version, 64, offsetFile);

			GetPrivateProfileString(__section, "MOD_SUICIDE", "-1", tmp, 64, offsetFile);
			newCG->MOD_SUICIDE = atoi(tmp);

			getOffsetFromINI( "modtype", &newCG->type );

			if (!newCG->automod) {
				getOffsetFromINI( "refdef", &newCG->refdef );
				getOffsetFromINI( "refdefViewAngles", &newCG->refdefViewAngles  );
				getOffsetFromINI( "pmext", &newCG->pmext   );
				getOffsetFromINI( "commands", &newCG->commands );
				getOffsetFromINI( "numCommands", &newCG->numCommands  );
				getOffsetFromINI( "predictedPS", &newCG->predictedPS );
				getOffsetFromINI( "centityBase", &newCG->centBase  );
				getOffsetFromINI( "centitySize", &newCG->centSize  );
				getOffsetFromINI( "centCurrentState", &newCG->centCurrentStateOffset  );
				getOffsetFromINI( "centLerpOrigin", &newCG->centLerpOrigin );
				getOffsetFromINI( "clientInfoBase", &newCG->ciBase  );
				getOffsetFromINI( "clientInfoSize", &newCG->ciSize   );
				getOffsetFromINI( "cliInfoValid", &newCG->ciInfoValidOffset  );
				getOffsetFromINI( "cliName", &newCG->ciNameOffset  );
				getOffsetFromINI( "cliTeam", &newCG->ciTeamOffset  );
				getOffsetFromINI( "cliHealth", &newCG->ciHealthOffset  );
				getOffsetFromINI( "cliClass", &newCG->ciClassOffset  );
				getOffsetFromINI( "CG_Player", &newCG->CG_Player  );
				getOffsetFromINI( "CG_AddPlayerWeapon", &newCG->CG_AddPlayerWeapon  );
				getOffsetFromINI( "CG_PositionRotatedEntityOnTag", &newCG->CG_PositionRotatedEntityOnTag );
				getOffsetFromINI( "CG_SetLerpFrameAnimationRate", &newCG->CG_SetLerpFrameAnimationRate  );
				getOffsetFromINI( "CG_FinishWeaponChange", &newCG->CG_FinishWeaponChange  );
				getOffsetFromINI( "CG_EntityEvent", &newCG->CG_EntityEvent  );
				getOffsetFromINI( "CG_Trace", &newCG->CG_Trace  );
				getOffsetFromINI( "CG_DamageFeedback", &newCG->CG_DamageFeedback  );
				getOffsetFromINI( "CG_WeaponFireRecoil", &newCG->CG_WeaponFireRecoil  );
				getOffsetFromINI( "CG_Respawn", &newCG->CG_Respawn  );
				getOffsetFromINI( "CG_CalculateReinfTime", &newCG->CG_CalculateReinfTime );
				getOffsetFromINI( "CG_AllocLocalEntity", &newCG->CG_AllocLocalEntity );
				getOffsetFromINI( "CG_DrawPlayerStatusHead", &newCG->CG_DrawPlayerStatusHead  );
				getOffsetFromINI( "CG_DrawPlayerStats", &newCG->CG_DrawPlayerStats  );
				getOffsetFromINI( "CG_DrawPlayerStatus", &newCG->CG_DrawPlayerStatus  );
				getOffsetFromINI( "CG_ChargeTimesChanged", &newCG->CG_ChargeTimesChanged  );
				getOffsetFromINI( "BG_FindClipForWeapon", &newCG->BG_FindClipForWeapon  );
				getOffsetFromINI( "BG_FindAmmoForWeapon", &newCG->BG_FindAmmoForWeapon  );
				getOffsetFromINI( "CG_CalcViewValues", &newCG->CG_CalcViewValues );
			}

			memcpy( (void *) &Hook.cglist[numCG], newCG, sizeof(cgMod_t) );
			numCG++;

			if (numCG > (MAX_CG_MODS-1) ){
				delete newCG;
				sprintf( errorMessage, "too many CG versions defined" );
				return false;
			}

			delete newCG;
		}

		cScOff += strlen(__section)+1;
	}

	return true;
}

bool CTools::SkipCgameCvar(vmCvar_t *cvar, const char *cvarName)
{
	int numCvars = sizeof(cvarList) / sizeof(cvarInfo_t);

	if (!strcasecmp(cvarName, "cg_teamchatheight"))
		eth32.cg.cg_teamchatheight = cvar;

	for (int i=0 ; i<numCvars ; i++) {	
		if (!strcasecmp(cvarName, cvarList[i].name)) {
			strcpy(cvar->string, cvarList[i].ourValue);
			cvar->integer = atoi(cvar->string);
			cvar->value = atof(cvar->string);
			return true;	// tell syscall to not register this
		}
	}
	return false;	// tell syscall to register this cvar as normal
}



void CTools::LoadUserCvars()
{
	char path[MAX_STRING_CHARS];
	char cvar[MAX_STRING_CHARS];
	char cvarval[MAX_STRING_CHARS];
	char val[MAX_STRING_CHARS];
	FILE *cvarFile;
	char line[MAX_STRING_CHARS];
	sprintf(path, "%s/cvarforce.txt", eth32.path);
	userCvarList = NULL;

	cvarFile = fopen(path, "r");
	if (!cvarFile)
		return;

	int j;
	bool start = false;
	while (fgets(line, MAX_STRING_CHARS-1, cvarFile))
	{
		char *end = strchr(line, ' ');
		if (!end || strlen(line)<4)
			continue;

		strncpy(cvar, line, end-line);
		strncpy(val, end+1, 1024);

		// strip leading spaces and the quotes around the value
		j = 0; start = false;
		for(int i=0; i<MAX_STRING_CHARS;i++){
			if (!start && val[i]==' ')
				continue;
			if (!start && val[i]=='\"'){
				start = true;
				continue;
			}
			if ( (start && val[i]=='\"' && (val[i+1]=='\0' || val[i+1]==0xd || val[i+1]==0xa)) || val[i]=='\0' || val[i+1]==0xd || val[i+1]==0xa){
				cvarval[j++] = '\0';
				break;
			}
			cvarval[j++] = val[i];
		}

		AddUserCvar(cvar, cvarval);
	}

	fclose(cvarFile);
}

void CTools::AddUserCvar(char *cvarName, char *cvarValue)
{
	int cnt = 0;
	cvarInfo_t *c = userCvarList;
	while (c) {
		cnt++;
		c = c->next;
	}

	uint32 oldBase = (uint32)userCvarList;
	userCvarList = (cvarInfo_t *)realloc(userCvarList, (cnt+1)*sizeof(cvarInfo_t));
	userCvarList[cnt].next = NULL;
	// realloc can also relocate the memory block, so we need to adjust the ptrs accordingly
	// (it does not fragment)
	c = userCvarList;
	while (c) {
		if (c->next)
			c->next = (cvarInfo_t *)((uint32)c->next+(uint32)userCvarList-oldBase);
		c = c->next;
	}

	strcpy(userCvarList[cnt].name, cvarName);
	strcpy(userCvarList[cnt].ourValue, cvarValue) ;
	if (cnt>0)
		userCvarList[cnt-1].next = &userCvarList[cnt];
	userCvarList[cnt].isSet = false;
}

void CTools::SaveUserCvars()
{
	char path[MAX_STRING_CHARS];
	FILE *cvarFile;
	sprintf(path, "%s/cvarforce.txt", eth32.path);

	cvarFile = fopen(path, "w");
	if (!cvarFile)
		return;

	cvarInfo_t *c = Tools.userCvarList;
	while (c) {
		fprintf(cvarFile, "%s \"%s\"\n", c->name, c->ourValue);
		c = c->next;
	}
	fclose(cvarFile);
}

void CTools::SetUserCvars()
{
	if (!userCvarList)
		return;

	cvarInfo_t *c = userCvarList;
	while (c){
		Syscall.Cvar_Set(c->name, c->ourValue);
		c = c->next;
	}
}

char *CTools::CleanString(char *string)
{
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;

	// get rid of leading whitespace
	while (*s == ' ')
		s++;

	while ((c = *s) != 0 ) {
		if ( Q_IsColorString( s ) ) {
			s++;
		}
		else if ( c >= 0x20 && c <= 0x7E ) {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

void CTools::UrlString(char *string, int size)
{
	if (!string || !*string || size < 2)
		return;

	int cnt = 0;
	char *s = string;
	char *d = new char[size];

	while (*s && cnt < size-1)
	{
		if (*s == ' ') {
			if (cnt > size-4)
				break;
			strcpy(&d[cnt], "%20");
			cnt += 3;
		}
		else {
			d[cnt] = *s;
			cnt++;
		}
		s++;
	}
	d[cnt] = 0;
	strcpy(string, d);
	delete [] d;
}

#define IS_URL_SPACE(p)		( p && *(p) == '%' && *((p)+1) == '2' && *((p)+2) == '0' )
void CTools::RemoveUrlSpaces(char *string)
{
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( IS_URL_SPACE( s ) ) {
			s += 2;
			*d++ = ' ';
		}
		else {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';
}

const char *CTools::GetEtVersionString(int version)
{
	if (version == 0)
		return "2.55";
	else if (version == 1)
		return "2.56";
	else if (version == 2)
		return "2.60";
	else if (version == 3)
		return "2.60b";
	else
		return "unknown";
}

const char *CTools::GetModVersionString(int version)
{
	if (version == 0)
		return "etmain";
	else if (version == 1)
		return "etpub";
	else if (version == 2)
		return "jaymod 1.x";
	else if (version == 3)
		return "jaymod 2.x";
	else if (version == 4)
		return "noquarter";
	else if (version == 5)
		return "etpro";
	else if (version == 6)
		return "tce";
	else
		return "unknown";
}

const char* CTools::GetRandomName(void)
{
	const char* names[MAX_CLIENTS];
	int cnt = 0;

	for (int i=0 ; i<MAX_CLIENTS ; i++) {
		if (IS_INFOVALID(i) && i != eth32.cg.clientNum) {
			if (!strcmp(eth32.cg.players[i].name, eth32.cg.players[eth32.cg.clientNum].name))
				continue;

			names[cnt] = eth32.cg.players[i].name;
			cnt++;
		}
	}

	if (!cnt)
		return NULL;

	srand(eth32.cg.time);
	return names[rand() % cnt];
}

/*
 *		common format keys
 *
 * [n] - target name
 * [N] - clean target name
 * [s] - my name
 * [S] - my clean name
 * [d] - my deaths
 * [k] - my kills
 * [r] - my kd ratio
 * [.] - bullets fired
 * [,] - bullets hit
 * [/] - headshots
 * [<] - headshots per kill
 * [>] - headshots per hits
 * [?] - headshots per shots
 * [h] - my health
 * [a] - my ammo
 * [H] - servername
 * [p] - longest spree
 * [q] - current spree
 * [l] - last guy to kill me
 * [L] - no. of teammates in limbo
 */
void CTools::CommonFormat(char *fmt, char *str)
{
	char buf[64];
	char b[2];

	*str = '\0';
	b[1] = '\0';
	while (*fmt) {
		if ( *fmt == '[' && *(fmt+1) && *(fmt+2) == ']' ) {
			switch (*(fmt+1)) {
				case 's':
					strcat(str, eth32.cg.self->name);
					fmt+=3;
					continue;
				case 'S':
				{
					char name[256];
					strcpy(name, eth32.cg.self->name);
					strcat(str, CleanString(name));
					fmt+=3;
					continue;
				}
				case 'n':
					if (targetID >= 0 && targetID < MAX_CLIENTS)
						strcat(str, eth32.cg.players[targetID].name);
					fmt+=3;
					continue;
				case 'N':
				{
					if (targetID >= 0 && targetID < MAX_CLIENTS) {
						char name[256];
						strcpy(name, eth32.cg.players[targetID].name);
						strcat(str, CleanString(name));
					}
					fmt+=3;
					continue;
				}
				case 'h':
					//sprintf(buf, "%i", *eth32.cg.self->health);
					sprintf(buf, "%i", eth32.cg.snap->ps.stats[STAT_HEALTH]);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'a':
					if (getAmmo()) {
						sprintf(buf, "%i/%i", clip, ammo);
						strcat(str,  buf);
					}
					fmt+=3;
					continue;
				case 'k':
					sprintf(buf, "%i", eth32.cg.self->stats.kills);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'd':
					sprintf(buf, "%i", eth32.cg.self->stats.deaths);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'P':
					sprintf(buf, "%i", eth32.cg.snap->ping);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'p':
					sprintf(buf, "%i", eth32.cg.self->stats.longestSpree);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'q':
					sprintf(buf, "%i", eth32.cg.self->stats.spree);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'r':
					sprintf(buf, "%.1f", eth32.cg.self->stats.kdRatio);
					strcat(str,  buf);
					fmt+=3;
					continue;
				case 'H':
					strcat(str,  eth32.cg.serverName);
					fmt+=3;
					continue;
				case 'l':
					if (eth32.cg.self->stats.killer >=0)
						strcat(str,  eth32.cg.players[eth32.cg.self->stats.killer].name);
					fmt+=3;
					continue;
				case '.': {
					sprintf(buf, "%i", eth32.cg.self->stats.bulletsFiredWS);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				case ',': {
					sprintf(buf, "%i", eth32.cg.self->stats.bulletsHitWS);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				case '/': {
					sprintf(buf, "%i", eth32.cg.self->stats.headshotsWS);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				case '<': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.hr_kills);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				case '>': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.hr_hits);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				case '?': {
					sprintf(buf, "%.1f", eth32.cg.self->stats.hr_shots);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				case 'L': {
					sprintf(buf, "%i", eth32.cg.limbo);
					strcat(str,  buf);
					fmt+=3;
					continue;
				}
				default:
					fmt+=3;
					continue;
			}
		} else {
			b[0] = *fmt;
			strcat(str,  b);
			fmt++;
		}
	}
}

void CTools::SetTarget(int n) { targetID = n; }
bool CTools::getAmmo()
{
	ammo = clip = -1;

	if (!orig_BG_FindClipForWeapon || !orig_BG_FindAmmoForWeapon)
		return false;

	if (!eth32.cg.snap || !eth32.cg.snap->ps.weapon)
		return false;

	if (eth32.cg.snap->ps.eFlags & EF_MG42_ACTIVE || eth32.cg.snap->ps.eFlags & EF_MOUNTEDTANK)
		return false;

	int weap = eth32.cg.snap->ps.weapon;

	if (IS_WEAPATTRIB(weap, WA_NO_AMMO))
		return false;

	clip = eth32.cg.snap->ps.ammoclip[orig_BG_FindClipForWeapon(weap)];

	if (IS_WEAPATTRIB(weap, WA_ONLY_CLIP))
		return true;

	ammo = eth32.cg.snap->ps.ammo[orig_BG_FindAmmoForWeapon(weap)];

	return true;
}

void CTools::LoadSpam(char *file)
{
	char value[512];
	char key[64];
	int i;

	// load killspam
	ClearKillspam();
	for (i=0; ;i++){
		sprintf(key, "killspamfmt%i", i);
		GetPrivateProfileString("Spam", key, "", value, sizeof(value), file);
		if (value[0]) {
			killspamFmt = (char (*)[512])realloc(killspamFmt, 512*(i+1));
			strcpy(killspamFmt[i], value);
		} else {
			nKillspam = i;
			break;
		}
	}

	// load multikillspam
	ClearMultikillspam();
	for (i=0; ;i++){
		sprintf(key, "multikillspamfmt%i", i);
		GetPrivateProfileString("Spam", key, "", value, sizeof(value), file);
		if (value[0]) {
			multikillspamFmt = (char (*)[512])realloc(multikillspamFmt, 512*(i+1));
			strcpy(multikillspamFmt[i], value);
		} else {
			nMultikillspam = i;
			break;
		}
	}

	// load deathspam
	ClearDeathspam();
	for (i=0; ;i++){
		sprintf(key, "deathspamfmt%i", i);
		GetPrivateProfileString("Spam", key, "", value, sizeof(value), file);
		if (value[0]) {
			deathspamFmt = (char (*)[512])realloc(deathspamFmt, 512*(i+1));
			strcpy(deathspamFmt[i], value);
		} else {
			nDeathspam = i;
			break;
		}
	}

	// load suicidespam
	ClearSuicidespam();
	for (i=0; ;i++){
		sprintf(key, "suicidespamfmt%i", i);
		GetPrivateProfileString("Spam", key, "", value, sizeof(value), file);
		if (value[0]) {
			suicidespamFmt = (char (*)[512])realloc(suicidespamFmt, 512*(i+1));
			strcpy(suicidespamFmt[i], value);
		} else {
			nSuicidespam = i;
			break;
		}
	}

	// load selfkillspam
	ClearSelfkillspam();
	for (i=0; ;i++){
		sprintf(key, "selfkillspamfmt%i", i);
		GetPrivateProfileString("Spam", key, "", value, sizeof(value), file);
		if (value[0]) {
			selfkillspamFmt = (char (*)[512])realloc(selfkillspamFmt, 512*(i+1));
			strcpy(selfkillspamFmt[i], value);
		} else {
			nSelfkillspam = i;
			break;
		}
	}

#ifdef ETH32_DEBUG
	Debug.Log("Loaded %i kill spams", nKillspam);
	Debug.Log("Loaded %i mulitkill spams", nMultikillspam);
	Debug.Log("Loaded %i selfkill spams", nSelfkillspam);
	Debug.Log("Loaded %i suicide spams", nSuicidespam);
	Debug.Log("Loaded %i death spams", nDeathspam);
#endif
}

void CTools::SaveSpam(char *file)
{
	int i;
	char key[64];

	for (i=0; i<nKillspam; i++) {
		sprintf(key, "killspamfmt%i", i);
		Tools.WritePrivateProfileString("Spam", key, killspamFmt[i], file);
	}

	for (i=0; i<nMultikillspam; i++) {
		sprintf(key, "multikillspamfmt%i", i);
		Tools.WritePrivateProfileString("Spam", key, multikillspamFmt[i], file);
	}

	for (i=0; i<nSuicidespam; i++) {
		sprintf(key, "suicidespamfmt%i", i);
		Tools.WritePrivateProfileString("Spam", key, suicidespamFmt[i], file);
	}

	for (i=0; i<nDeathspam; i++) {
		sprintf(key, "deathspamfmt%i", i);
		Tools.WritePrivateProfileString("Spam", key, deathspamFmt[i], file);
	}

	for (i=0; i<nSelfkillspam; i++) {
		sprintf(key, "selfkillspamfmt%i", i);
		Tools.WritePrivateProfileString("Spam", key, selfkillspamFmt[i], file);
	}
}

bool CTools::KillSpam()
{
	if (!nKillspam)
		return false;

	if (eth32.settings.KillspamType == KILLSPAM_SELECTED && !eth32.cg.players[targetID].allowPMSpam)
		return false;

	int n = (int)(random()*nKillspam);
	char msg[1024];
	CommonFormat(killspamFmt[n], msg);

	if (eth32.settings.KillspamType == KILLSPAM_SELECTED || eth32.settings.pmKillspam)
		Syscall.SendClientCommand("m %i \"%s\"\n", targetID, msg);
	else
		Syscall.SendClientCommand("say \"%s\"\n", msg);

	return true;
}

bool CTools::DeathSpam()
{
	if (!nDeathspam)
		return false;

	int n = (int)(random()*nDeathspam);
	char msg[1024];
	CommonFormat(deathspamFmt[n], msg);

	Syscall.SendClientCommand("say \"%s\"\n", msg);
	return true;
}

bool CTools::MultikillSpam(int spree)
{
	if (!nMultikillspam)
		return false;

	int *m = NULL;
	int s = 0;
	char *l;

	// get random mkspam matching spree level
	for (int i=0; i<nMultikillspam; i++) {
		l = &multikillspamFmt[i][1];
		if (atoi(l) == spree){
			m = (int *)realloc(m, sizeof(int)*(++s));
			m[s-1] = i;
		}
	}

	if (!s)
		return false;

	int n = (int)(random()*s);
	char msg[1024];

	CommonFormat(strchr(multikillspamFmt[m[n]], ']')+1, msg);
	//printf("MKS: %s\n", msg);
	Syscall.SendClientCommand("say \"%s\"\n", msg);
	free(m);
	return true;
}

bool CTools::SuicideSpam()
{
	if (!nSuicidespam)
		return false;

	int n = (int)(random()*nSuicidespam);
	char msg[1024];
	CommonFormat(suicidespamFmt[n], msg);
	Syscall.SendClientCommand("m %i \"%s\"\n", targetID, msg);
	return true;
}

bool CTools::SelfkillSpam()
{
	if (!nSelfkillspam)
		return false;

	int n = (int)(random()*nSelfkillspam);
	char msg[1024];
	CommonFormat(selfkillspamFmt[n], msg);
	Syscall.SendClientCommand("say \"%s\"\n", msg);
	return true;
}

char *CTools::GetKillspam(int idx)		{ if (idx >= 0 && idx < nKillspam) return killspamFmt[idx]; else return NULL; }
char *CTools::GetMultikillspam(int idx)	{ if (idx >= 0 && idx < nMultikillspam) return multikillspamFmt[idx]; else return NULL; }
char *CTools::GetSuicidespam(int idx)	{ if (idx >= 0 && idx < nSuicidespam) return suicidespamFmt[idx]; else return NULL; }
char *CTools::GetSelfkillspam(int idx)	{ if (idx >= 0 && idx < nSelfkillspam) return selfkillspamFmt[idx]; else return NULL; }
char *CTools::GetDeathspam(int idx)		{ if (idx >= 0 && idx < nDeathspam) return deathspamFmt[idx]; else return NULL; }
int CTools::TotalKillspam() 		{ return nKillspam; }
int CTools::TotalMultikillspam() 	{ return nMultikillspam; }
int CTools::TotalSuicidespam() 		{ return nSuicidespam; }
int CTools::TotalDeathspam() 		{ return nDeathspam; }
int CTools::TotalSelfkillspam() 	{ return nSelfkillspam; }
void CTools::AddKillspam(char *fmt)			{ killspamFmt = (char (*)[512])realloc(killspamFmt, 512*(nKillspam+1));	strcpy(killspamFmt[nKillspam], fmt); nKillspam++; }
void CTools::AddMultikillspam(char *fmt)	{ multikillspamFmt = (char (*)[512])realloc(multikillspamFmt, 512*(nMultikillspam+1)); strcpy(multikillspamFmt[nMultikillspam], fmt); nMultikillspam++; }
void CTools::AddSuicidespam(char *fmt)		{ suicidespamFmt = (char (*)[512])realloc(suicidespamFmt, 512*(nSuicidespam+1)); strcpy(suicidespamFmt[nSuicidespam], fmt); nSuicidespam++; }
void CTools::AddDeathspam(char *fmt)		{ deathspamFmt = (char (*)[512])realloc(deathspamFmt, 512*(nDeathspam+1)); strcpy(deathspamFmt[nDeathspam], fmt); nDeathspam++; }
void CTools::AddSelfkillspam(char *fmt)		{ selfkillspamFmt = (char (*)[512])realloc(selfkillspamFmt, 512*(nSelfkillspam+1)); strcpy(selfkillspamFmt[nSelfkillspam], fmt); nSelfkillspam++; }
void CTools::ClearKillspam() { if (killspamFmt){ free(killspamFmt); nKillspam = 0; killspamFmt = NULL; } }
void CTools::ClearMultikillspam() { if (multikillspamFmt){ free(multikillspamFmt); nMultikillspam = 0; multikillspamFmt = NULL; } }
void CTools::ClearSelfkillspam() { if (selfkillspamFmt){ free(selfkillspamFmt); nSelfkillspam = 0; selfkillspamFmt = NULL; } }
void CTools::ClearSuicidespam() { if (suicidespamFmt){ free(suicidespamFmt); nSuicidespam = 0; suicidespamFmt = NULL; } }
void CTools::ClearDeathspam() { if (deathspamFmt){ free(deathspamFmt); nDeathspam = 0; deathspamFmt = NULL; } }

void CTools::PBNamespoof(char *name)
{
	char clean[256];
	strcpy(clean, name);
	CleanString(clean);

	if (!eth32.settings.nsSmartMode) {
		// replace based on visual similarity - most powerfull ones first
		struct repl_s {
			char in;
			char out;
		};

		// replacement array, in->out
		struct repl_s rep[] =
		{
			{'l', '1'},
			{'1', 'l'},
			{'i', '1'},
			{'.', ','},
			{',', '.'},
			{'(', '{'},
			{'{', '('},
			{')', '}'},
			{'}', ')'},
			{'o', '0'},
			{'0', 'o'},
			{'e', '3'},
			{'3', 'e'},
			{'4', 'a'},
			{'a', '4'},
		};

		int i;
		for (i=0; i<sizeof(rep)/sizeof(rep[0]); i++) {
			if (strchr(clean, rep[i].in))
			break;
		}

		// if no suitable replacement found, fix it like this
		if (i == sizeof(rep)/sizeof(rep[0])) {
			strcat(name, "^9.");
			return;
		} else {
			for (int j=0; j<strlen(name); j++) {
				if (name[j] == '^' && name[j+1] && name[j+1] != '^')
					j+= 2;
					
				if (name[j] == rep[i].in){
					name[j] = rep[i].out;
					break;
				}
			}
		} 
	} else {
			strcat(name, " ^.");
			return;
	}
}

void CTools::NameSteal(int recurse)
{
	if (!eth32.settings.doNamesteal || (eth32.cg.time - eth32.cg.zerohour < eth32.settings.NamestealGrace))
		return;

	if (eth32.cg.time - lastNamestealTime < eth32.settings.NamestealDelay)
		return;

	char name[128];

	if (eth32.settings.NamestealMode == NAMESTEAL_FILE && nNames) {
		int n = (int)(random()*nNames);
		strcpy(name, nameList[n]);
	} else {
		int Teammates[MAX_CLIENTS];
		int Enemies[MAX_CLIENTS];
		int Everyone[MAX_CLIENTS];
		int n,j,k,l,i;

		memset(Teammates, -1, sizeof(Teammates));
		memset(Enemies, -1, sizeof(Enemies));
		memset(Everyone, -1, sizeof(Everyone));

		for (n=0, j=0, i=0, l=0; n<MAX_CLIENTS; n++) {
			if (n == eth32.cg.clientNum || !IS_INFOVALID(n))
				continue;

			Everyone[l++] = n;

			if (IS_SAMETEAM(eth32.cg.clientNum, n))
				Teammates[j++] = n;
			else if (!IS_SPECTATOR(n))
				Enemies[i++] = n;
		}

		int type = IS_SPECTATOR(eth32.cg.clientNum) ? NAMESTEAL_ALL : eth32.settings.NamestealMode;
		switch (type) {
			case NAMESTEAL_TEAM:
				if (j != 0) {
					n = (int)(random()*j);
					strcpy(name, eth32.cg.players[Teammates[n]].name);
				} else {
					lastNamestealTime = eth32.cg.time;
					return;
				}
				break;
			case NAMESTEAL_ENEMY:
				if (i != 0) {
					n = (int)(random()*i);
					strcpy(name, eth32.cg.players[Enemies[n]].name);
				} else {
					lastNamestealTime = eth32.cg.time;
					return;
				}
				break;
			case NAMESTEAL_ALL:
				if (l > 0) {
					n = (int)(random()*l);
					strcpy(name, eth32.cg.players[Everyone[n]].name);
				} else {
					lastNamestealTime = eth32.cg.time;
					return;
				}
				break;
			default:
				lastNamestealTime = eth32.cg.time;
				return;
		}

		// if non-PB do exact steal, else intelligent steal
		if (eth32.cg.pbserver)
			PBNamespoof(name);
	}

	// check if this name isnt already taken
	if (eth32.cg.pbserver) {
		for (int n=0; n<MAX_CLIENTS; n++) {
			if (!IS_INFOVALID(n) || n == eth32.cg.clientNum)
				continue;

			if (!strcmp(name, eth32.cg.players[n].name)) {
				if (recurse > 2) {
					lastNamestealTime = eth32.cg.time;
					return;
				}
				NameSteal(++recurse);
				return;
			}
		}
	}

	char buf[256];
	sprintf(buf, "set name \"%s\"\n", name);
	Syscall.CG_SendConsoleCommand(buf); 

	lastNamestealTime = eth32.cg.time;
}

void CTools::RegisterThread(uint32 tID, const char *name)
{
	if (!this->nThreads)
		nThreads = (threadInfo_t *)malloc(sizeof(threadInfo_t));

	sThreads++;
	nThreads = (threadInfo_t *) realloc(nThreads,(sizeof(threadInfo_t)*sThreads));

	strcpy(nThreads[sThreads-1].name, name);
	nThreads[sThreads-1].tID = tID;

#ifdef ETH32_DEBUG
	Debug.Log("Register Thread 0x%X %s", tID, name);
#endif
}

void CTools::VectorAngles(const vec3_t forward, vec3_t angles)
{
    float tmp, yaw, pitch;

    if (forward[1] == 0 && forward[0] == 0) {
	yaw = 0;
	if (forward[2] > 0)
	    pitch = 90;
	else
	    pitch = 270;
    } else {
	yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
	if (yaw < 0)
	    yaw += 360;

	tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
	pitch = (atan2(forward[2], tmp) * 180 / M_PI);
	if (pitch < 0)
	    pitch += 360;
    }

    angles[0] = pitch;
    angles[1] = yaw;
    angles[2] = 0;
}

void CTools::Print(const char *txtFmt, ...)
{
	if (pthread_mutex_lock(print_mutex)) FATAL_ERROR("could not lock mutex")
	char text[1024];

	va_list arglist;
	va_start(arglist, txtFmt );
	vsprintf(text, txtFmt, arglist);
	va_end(arglist);

	if (UI_VM) orig_syscall(UI_PRINT, text);
	else if (CG_VM) orig_syscall(CG_PRINT, text);

	if (pthread_mutex_unlock(print_mutex)) FATAL_ERROR("could not unlock mutex")
}

// terribly inefficient, dont use for serious number crunching
void *CTools::FindPattern(void *haystack, int haystacklen, const char *pattern, const char *mask)
{
	int c,b, p;
	char *h = (char *)haystack;
	for (c=0; c<haystacklen; c++) {
		p = 0;
		for (b=0; b<strlen(mask); b++) {
			if (mask[b] == '.')
				continue;
			if (*(h+b) != pattern[p++])
				break;
		}
		if (b==strlen(mask))
			return (void *)h;
		h++;
	}

	return NULL;
}

// case insensitive strstr
char *stristr (const char * str1, const char * str2)
{
        char *cp = (char *) str1;
        char *s1, *s2;

        if ( !*str2 )
            return((char *)str1);

        while (*cp)
        {
                s1 = cp;
                s2 = (char *) str2;

                while ( *s1 && *s2 && !(tolower(*s1)-tolower(*s2)) )
                        s1++, s2++;

                if (!*s2)
                        return(cp);

                cp++;
        }

        return NULL;
}
