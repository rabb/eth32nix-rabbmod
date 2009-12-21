// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#pragma once
#include <pthread.h>

class CSpectator
{
	public:
		CSpectator();
		~CSpectator();
		void LaunchThread();
		void Init();
		void ClearSpectators();
		void AddSpectator(const char *name);
		void ParseServerMessage(char *buf);
		void KillThread();
		void UpdateStats(int bs, int rq, char *sv);
		void GetStats(int *bs, int *rq, char *sv);
		bool active;		

		bool wantThreadExit;
	private:
		pthread_mutex_t *mutex;
		pthread_t sthread;
		
		int bytes, reqs;
		char server[64];
};

extern CSpectator Spectator;
