#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"

DWORD WINAPI GameServerThread(LPVOID arg)
{
	Player newclient;
	newclient.clientSocket = (SOCKET)arg;

	//matchmakingThread�κ��� ������ �޿� �� �����ϴ� �Լ�
	return 0;
}
DWORD WINAPI CommThread(LPVOID arg)
{
	std::cout << "Connect Client success\n" << std::endl;
	return 0;
}

void GameServerThreadDate::CreateCommThread(void)
{
	//GameServerThread�� ���� Ŭ���̾�Ʈ���� �迭�� ����
	HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pClients[0], 0, NULL);
}
