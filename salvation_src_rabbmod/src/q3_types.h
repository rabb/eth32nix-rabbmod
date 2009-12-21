// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

// Contains structs and other things that we use from ET.exe, but aren't avaliable in ET sdk

typedef struct fileInPack_s {
	char					*name;		// name of the file
	unsigned long			pos;		// file info position in zip
	struct	fileInPack_s*	next;		// next file in the hash
} fileInPack_t;

typedef struct {
	char			pakFilename[MAX_OSPATH];	// c:\quake3\baseq3\pak0.pk3
	char			pakBasename[MAX_OSPATH];	// pak0
	char			pakGamename[MAX_OSPATH];	// baseq3
	void*			handle;						// handle to zip file
	int				checksum;					// regular checksum
	int				pure_checksum;				// checksum for pure
	int				numfiles;					// number of files in pk3
	int				referenced;					// referenced file flags
	int				hashSize;					// hash table size (power of 2)
	fileInPack_t*	*hashTable;					// hash table
	fileInPack_t*	buildBuffer;				// buffer with the filenames etc.
} pack_t;

typedef struct searchpath_s {
	struct searchpath_s *next;
	pack_t *pack;
} searchpath_t;

/*typedef struct bufferedVoiceChat_s
{
	int clientNum;
	sfxHandle_t snd;
	qhandle_t	sprite;
	int voiceOnly;
	char cmd[MAX_SAY_TEXT];
	char message[MAX_SAY_TEXT];
	vec3_t origin;			// NERVE - SMF
} bufferedVoiceChat_t;*/

typedef void (*xcommand_t) (void);

typedef struct {
	char	*cmd;
	xcommand_t function;
} consoleCommand_t;

typedef struct {
	byte ip[4];
	byte UNUSED[10];
	unsigned short port;
} et_netadr_t;
