#include "stdafx.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	std::vector<SOCKET>* MatchMakingQ = (std::vector<SOCKET>*)arg;
	while (1) {
		if (isMatchMakingQFull()) {
			// 매칭 성립 알려줌
			CreateGameServerThread();
			MatchMakingQ_DeQ();
		}
		else {
			// 대기하라 알려줌
		}
	}
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
