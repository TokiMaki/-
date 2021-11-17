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
		//�� socket�� Ŀ�� ������ �ۼ�
		//CreateThread();
		//�� ������ �ش� Ŭ���̾�Ʈ ���ϰ� play�����͸� �߰��Ѵ�.
		Player newplayerdata;
		newplayerdata.clientSocket = match_sockets->client[i];
		newRoomData.pPlayers.emplace_back(newplayerdata);
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
	//GameServerThread�� ���� Ŭ���̾�Ʈ���� �迭�� ����
	//���ϸ� ������ ���� Player struct�� ������
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		HANDLE newCommThread = CreateThread(NULL, 0, CommThread, pPlayers[i], 0, NULL);
	}
	
}
