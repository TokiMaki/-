#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"
#include "socket_function.h"

HANDLE hupdate; //Ŭ��� ���� �����͸� �޾Ҵ��� üũ
HANDLE hcheckupdate; //���� �����͸� ������Ʈ �ߴ��� üũ
GlobalGameData roomdata;

DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	MatchSockets* match_sockets = (MatchSockets*)arg;
	
	hupdate = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hupdate == NULL)
	{
		return 1;
	}
	hcheckupdate = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hcheckupdate == NULL)
	{
		return 1;
	}
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		Player newplayerdata;
		//�� ������ �ش� Ŭ���̾�Ʈ ���ϰ� play�����͸� �߰��Ѵ�.
		newplayerdata.clientSocket = match_sockets->client[i];
		newplayerdata.m_GameClinetNum = i;
		newRoomData.pPlayers.push_back(newplayerdata);
	}
	// �� Ŭ���̾�Ʈ�� ���ϵ�� ������ Ŀ�¾����� ����
	newRoomData.CreateCommThread();
	while (1)
	{
		WaitForSingleObject(hupdate, INFINITE); // ���� �Ϸ� ��ٸ���
		//event���?
		//���� �����͵� ��Ƽ� ������Ʈ �ϱ�
		SetEvent(hcheckupdate);
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
	//Ŭ���̾�Ʈ ���� ���
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);
	int recv_Msg;

	Player tempP;
	KeyInput tempKey;
	Gamestatus tempstatus;
	int tempClientNum = -1;
	int len = 0;

	//�ʱ� ���� ������ �ޱ�
	retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}
	len = ntohl(len);
	retval = recvn(client_sock, (char*)&tempstatus, sizeof(Gamestatus), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}

	playdata->m_gamestatus[playdata->m_GameClinetNum] = tempstatus;

	// �ڽ��� ���° ���� �����ֱ�
	len = sizeof(int);
	len = htonl(len);
	retval = send(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}
	retval = send(client_sock, (char*)&tempClientNum, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("recv()");
		return 0;
	}

	playdata->m_GameClinetNum = tempClientNum;

	while (1)
	{
		WaitForSingleObject(hcheckupdate, INFINITE);
		//Ű�Է� ������ �ְ� �ޱ�
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		len = ntohl(len);
		retval = recvn(client_sock, (char*)&tempKey, sizeof(KeyInput), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			break;
		}
		playdata->m_key = tempKey;
		SetEvent(hupdate); // ���� �Ϸ�
	}

	return 0;
}
void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread�� ���� Ŭ���̾�Ʈ���� �迭�� ����
	//���ϸ� ������ ���� Player struct�� ������
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		std::cout << pPlayers[i].clientSocket << std::endl;
		HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pPlayers[i], 0, NULL);
	}
	
}
