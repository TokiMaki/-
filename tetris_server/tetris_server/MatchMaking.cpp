#include "stdafx.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	std::vector<SOCKET>* MatchMakingQ = (std::vector<SOCKET>*)arg;
	while (1) {
		if (isMatchMakingQFull()) {
			// ��Ī ���� �˷���
			CreateGameServerThread();
			MatchMakingQ_DeQ();
		}
		else {
			// ����϶� �˷���
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
