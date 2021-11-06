#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"



DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	SOCKET pSocket = (SOCKET)arg;
	newRoomData.pClients.emplace_back(&pSocket);
	//matchmakingThread로부터 소켓을 받온 후 실행하는 함수
	return 0;
}
DWORD WINAPI CommThread(LPVOID arg)
{
	std::cout << "Connect Client success\n" << std::endl;
	return 0;
}

void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread에 들어온 클라이언트들을 배열로 제작
	HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pClients[0], 0, NULL);
}
