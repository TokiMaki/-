#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"



DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	SOCKET pSocket = (SOCKET)arg;
	newRoomData.pClients.emplace_back(&pSocket);
	//matchmakingThread�κ��� ������ �޿� �� �����ϴ� �Լ�


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
	//GameServerThread�� ���� Ŭ���̾�Ʈ���� �迭�� ����
	HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pClients[0], 0, NULL);
}
