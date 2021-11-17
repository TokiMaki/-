#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"
#include "socket_function.h"



DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	MatchSockets* match_sockets = (MatchSockets*)arg;

	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		//각 socket별 커뮤 쓰레드 작성
		//CreateThread();
		//방 정보에 해당 클라이언트 소켓과 play데이터를 추가한다.
		Player newplayerdata;
		newplayerdata.clientSocket = match_sockets->client[i];
		newRoomData.pPlayers.emplace_back(newplayerdata);
	}
	// 각 클라이언트의 소켓들과 소통할 커뮤쓰레드 생성
	newRoomData.CreateCommThread();
	while (1)
	{
		//event사용?
		//받은 데이터들 모아서 업데이트 하기

	}
	return 0;
}
DWORD WINAPI CommThread(LPVOID arg)
{
	Player* playdata = (Player*)arg;
	SOCKET client_sock = playdata->clientSocket;
	int retval;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int temp_num;
	char buf[BUFSIZE + 1];
	std::cout << "commThread running\n" << std::endl;
	//클라이언트 정보 출력
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	int recv_Msg;
	while (1)
	{
		//데이터 주고 받기
		int now_recv_data;
		retval = recvn(client_sock,(char*)&playdata,sizeof(Player),0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

	}


	return 0;
}

void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread에 들어온 클라이언트들을 배열로 제작
	//소켓만 보내지 말고 Player struct를 보내기
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		HANDLE newCommThread = CreateThread(NULL, 0, CommThread, pPlayers[i], 0, NULL);
	}
	
}
