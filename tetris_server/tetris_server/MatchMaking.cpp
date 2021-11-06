#include "stdafx.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	std::queue<SOCKADDR>* MatchMakingQ = (std::queue<SOCKADDR>*)arg;
	
	return 0;
}

bool isMatchMakingQFull()
{
	return false;
}

void CreateGameServerThread()
{
}

void MatchMakingQ_DeQ()
{
}
