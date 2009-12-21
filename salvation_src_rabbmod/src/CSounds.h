#pragma once

typedef struct eth32Sounds_s {
	bool pure;
} eth32Sounds_t;

class CSounds
{
public:
	void	Init();
	void	ShutDown();
	void	RegisterSounds();
	void	PlaySpreeSounds();
	bool	Process(int type, int *args);
	int	HookSounds(char *name, int compressed);

private:
	int		nGameSounds;		// registered gamesounds
	eth32Sounds_t	*gameSounds;		// associated structure
};
extern CSounds Sounds;
