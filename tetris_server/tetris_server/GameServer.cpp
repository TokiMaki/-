#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"



DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	MatchSockets* match_sockets = (MatchSockets*)arg;

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		//각 socket별 커뮤 쓰레드 작성
		//CreateThread();
		//방 정보에 해당 클라이언트 소켓을 추가 해놓는다.
		newRoomData.pClients.emplace_back(match_sockets->client);

		//각 클라의 커뮤쓰레드에서 받은 데이터들을 저장용 player데이터 추가
	}

	return 0;
}
DWORD WINAPI CommThread(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int temp_num;
	char buf[BUFSIZE + 1];
	std::cout << "commThread running\n" << std::endl;
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	int recv_Msg;
	while (1)
	{
		
	}


	return 0;
}

void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread에 들어온 클라이언트들을 배열로 제작
	HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pClients[0], 0, NULL);
}
