#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"
#include "socket_function.h"

HANDLE hupdate; //Ŭ��� ���� �����͸� �޾Ҵ��� üũ
HANDLE hcheckupdate; //���� �����͸� ������Ʈ �ߴ��� üũ

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
		//�� ������ �ش� Ŭ���̾�Ʈ ���ϰ� play�����͸� �߰��Ѵ�.
		Player newplayerdata;
		newplayerdata.clientSocket = match_sockets->client[i];
		newRoomData.pPlayers.emplace_back(&newplayerdata);
		
	}
	// �� Ŭ���̾�Ʈ�� ���ϵ�� ������ Ŀ�¾����� ����
	newRoomData.CreateCommThread();
	while (1)
	{
		//event���?
		//���� �����͵� ��Ƽ� ������Ʈ �ϱ�
		
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
	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	int recv_Msg;
	while (1)
	{
		//������ �ְ� �ޱ�
		Player tempP;
		int len = 0;


	}

	return 0;
}

void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread�� ���� Ŭ���̾�Ʈ���� �迭�� ����
	//���ϸ� ������ ���� Player struct�� ������
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		HANDLE newCommThread = CreateThread(NULL, 0, CommThread, pPlayers[i], 0, NULL);
	}
	
}
