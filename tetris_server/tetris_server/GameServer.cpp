#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"
#include "socket_function.h"

HANDLE hupdate; //클라로 부터 데이터를 받았는지 체크
HANDLE hcheckupdate; //받은 데이터를 업데이트 했는지 체크
GlobalGameData roomdata;

DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	MatchSockets* match_sockets = (MatchSockets*)arg;
	
	hupdate = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hupdate == NULL)
	{
		return 1;
	}
	hcheckupdate = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hcheckupdate == NULL)
	{
		return 1;
	}
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		Player newplayerdata;
		//방 정보에 해당 클라이언트 소켓과 play데이터를 추가한다.
		newplayerdata.clientSocket = match_sockets->client[i];
		newRoomData.pPlayers.push_back(newplayerdata);
	}
	// 각 클라이언트의 소켓들과 소통할 커뮤쓰레드 생성
	newRoomData.CreateCommThread();
	while (1)
	{
		//event사용?
		//받은 데이터들 모아서 업데이트 하기
		
		
	}
	delete match_sockets;
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
	std::cout << client_sock << std::endl;
	//클라이언트 정보 출력
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	int recv_Msg;

	Player tempP;
	KeyInput tempKey;
	Gamestatus tempstatus;
	int len = 0;

	//초기 게임 데이터 받기
	retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}
	len = ntohl(len);
	retval = recvn(client_sock, (char*)&tempstatus, len, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}

	while (1)
	{
		//키입력 데이터 주고 받기
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		len = ntohl(len);
		retval = recvn(client_sock, (char*)&tempKey, len, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
	}

	return 0;
}
void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread에 들어온 클라이언트들을 배열로 제작
	//소켓만 보내지 말고 Player struct를 보내기
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		std::cout << pPlayers[i].clientSocket << std::endl;
		HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pPlayers[i], 0, NULL);
	}
	
}
