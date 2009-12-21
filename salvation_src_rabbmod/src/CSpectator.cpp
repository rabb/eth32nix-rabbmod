// ETH32 - an Enemy Territory cheat for windows
// Copyright (c) 2007 eth32 team
// www.cheatersutopia.com & www.nixcoders.org

#include "eth32.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>

#define NET_Q3MARKER "\xff\xff\xff\xff"
#define NET_STATUS_REQUEST (NET_Q3MARKER "getstatus\n")
#define NET_STATUS_RESPONSE (NET_Q3MARKER "statusResponse")
#define NET_STATUS_TIMEOUT 1750

#ifdef vsnprintf
#undef vsnprintf
#endif

CSpectator Spectator;
void *SpectatorThread(void *arg);

CSpectator::CSpectator()
{

}

CSpectator::~CSpectator()
{

}

void CSpectator::Init()
{
	this->mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex,NULL);
	this->sthread = 0;
}

void CSpectator::ClearSpectators()
{
	if (pthread_mutex_lock(mutex)) FATAL_ERROR("could not lock mutex")
	memset(eth32.cg.spectators, 0, sizeof(eth32.cg.spectators));
	if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
}

void CSpectator::AddSpectator(const char *name)
{
	if (pthread_mutex_lock(mutex)) FATAL_ERROR("could not lock mutex")

	eth32.cg.nspectators++;
	for (int i=0; i<MAX_CLIENTS; i++) {
		if (!eth32.cg.spectators[i][0]){
			memset(eth32.cg.spectators[i], 0, sizeof(eth32.cg.spectators[0]));
			strncpy(eth32.cg.spectators[i], name, sizeof(eth32.cg.spectators[i])-1);
			break;
		}
	}
	if (pthread_mutex_unlock(mutex)) FATAL_ERROR("could not unlock mutex")
}

void CSpectator::LaunchThread()
{
	if (this->sthread) {
		wantThreadExit = true;
		pthread_join(this->sthread, NULL);
	}
	if (pthread_create(&this->sthread, NULL, SpectatorThread, NULL)){
#ifdef ETH32_DEBUG
		Debug.Log("error creating spectator thread");
#endif
		this->sthread = 0;
	}
	wantThreadExit = false;
}

void CSpectator::KillThread()
{
	wantThreadExit = true;
}

void CSpectator::UpdateStats(int bs, int rq, char *sv)
{
	if (sv)
		strcpy(this->server, sv);
	bytes = bs;
	reqs = rq;
}

void CSpectator::GetStats(int *bs, int *rq, char *sv)
{
	*bs = bytes;
	*rq = reqs;
	strcpy(sv, this->server);
}

void CSpectator::ParseServerMessage(char *buf)
{
	eth32.cg.nspectators = 0;
	
	if (!eth32.cg.snap)
		return;

	if (eth32.cg.snap->ps.persistant[PERS_SCORE] == 0 || *eth32.cg.self->team == TEAM_SPECTATOR)
		return;

	char name[128];
	ClearSpectators();
	
	for (int i=0; i<MAX_CLIENTS; i++)
		if (Tools.auxSpecList[i] && IS_INFOVALID(i))
			AddSpectator(eth32.cg.players[i].name);

	std::string buffer = buf;
	StringTokenizer players(buffer, "\n");

	// skip header
	players.nextToken();

	// skip server info
	players.nextToken();

	std::string player;
	while (players.hasMoreTokens())
	{
		player = players.nextToken();
		StringTokenizer field(player, " ");

		std::string xp = field.nextToken();

		// skip ping
		field.nextToken();

		std::string name = field.remainingString();

		// skip the " "
		name.erase(0,1);
		name.erase(name.size()-1,1);

		// is player watching us?
		if (atoi(xp.c_str()) == eth32.cg.snap->ps.persistant[PERS_SCORE]) {

			if (!strcmp(name.c_str(), eth32.cg.self->name))
				continue;

			for (int i=0; i<MAX_CLIENTS; i++) {
				if (i==eth32.cg.clientNum)
					continue;

				if (!strcmp(eth32.cg.players[i].name, name.c_str()) &&
					*eth32.cg.players[i].team == TEAM_SPECTATOR)
					AddSpectator(name.c_str());
			}
		}
	}
}

#define INTERVAL 1500

void threadCleanup(void *psock)
{
#ifdef ETH32_DEBUG
	Debug.Log("SpectatorThread: Bye bye!");
#endif

	int sockfd = *(int *)psock;
	if (sockfd > 0)
		close(sockfd);
	Spectator.active = false;
}

void *SpectatorThread(void *arg)
{
	char buffer[4096];
	uint32	lastTime = 0;
	int size;
	struct timeval tv;
	fd_set sock_r, sock_w;
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	// initialize the socket
	int sockfd = 0;
	struct sockaddr_in sin;
	struct hostent *he;

	pthread_cleanup_push(threadCleanup, &sockfd);

#ifdef ETH32_DEBUG
	Debug.Log("SpectatorThread: Starting up! Hi there :)");
#endif

	char server[64];
	
	/*if (eth32.settings.specServer[0]) {
		strcpy(server, eth32.settings.specServer);
		*(char*)(strchr(server, ':')) = '\0';
	} else*/
		sprintf(server, "%i.%i.%i.%i", ((et_netadr_t *)eth32.et->server_ip)->ip[0], ((et_netadr_t *)eth32.et->server_ip)->ip[1], ((et_netadr_t *)eth32.et->server_ip)->ip[2], ((et_netadr_t *)eth32.et->server_ip)->ip[3]);

	Spectator.UpdateStats(0, 0, server);
	
	if ((he = gethostbyname(server)) == NULL) {
#ifdef ETH32_DEBUG
		Debug.Log("SpectatorThread: gethostbyname error (%i)", errno);
#endif
		pthread_exit(NULL);
	}	
	
	// Create socket
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bcopy(he->h_addr, (char *)&sin.sin_addr, he->h_length);
	sin.sin_family = AF_INET;
	/*if (eth32.settings.specServer[0])
		sin.sin_port = htons(atoi((char*)(strchr(eth32.settings.specServer, ':')+1)));
	else*/
		sin.sin_port = ((et_netadr_t *)eth32.et->server_ip)->port;	

    // "connect" to the udp socket
	int result = connect(sockfd, (struct sockaddr *)&sin, sizeof(sin));
	if (result != 0) {
#ifdef ETH32_DEBUG
		Debug.Log("SpectatorThread: error opening socket");
#endif
		pthread_exit(NULL);

	}
	
	Spectator.active = true;
	int req = 0;
	int bytes = 0;
	
	while (1){
		if (Spectator.wantThreadExit)
			pthread_exit(NULL);

		FD_ZERO(&sock_w);
		FD_ZERO(&sock_r);
		FD_SET(sockfd, &sock_r);
		FD_SET(sockfd, &sock_w);
		select(sockfd+3, &sock_r, &sock_w, NULL, &tv); //check if we can

		// ready to read
		if (FD_ISSET(sockfd, &sock_r)){
			memset(buffer, 0, sizeof(buffer));
			size = recv(sockfd, buffer, sizeof(buffer), 0);
				
			if (size < 0){
				#ifdef ETH32_DEBUG
					Debug.Log("SpectatorThread: error reading socket error = %i", errno);
				#endif
				break;
			}	
			bytes += size;
			
			// do stuff if we got a valid response
			if (!strncmp(buffer, NET_STATUS_RESPONSE, strlen(NET_STATUS_RESPONSE))) {
				Spectator.ParseServerMessage(buffer);
				req++;
				Spectator.UpdateStats(bytes, req, NULL);
			}
		}

		// ready to write
		if (FD_ISSET(sockfd, &sock_w)){
			if (*eth32.game.com_frameTime > (lastTime + INTERVAL)){
				if (send(sockfd, NET_STATUS_REQUEST, strlen(NET_STATUS_REQUEST), 0) < 0){
					#ifdef ETH32_DEBUG
						Debug.Log("SpectatorThread: error sending request %i\n", errno);
					#endif
					break;
				}
				lastTime = *eth32.game.com_frameTime;
			}
			Sleep(100);
		}
	}

	pthread_cleanup_pop(1);
	return NULL;
}
