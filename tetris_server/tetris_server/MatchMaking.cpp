#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#include "stdafx.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	SOCKADDR_IN clientaddr;
	int addrlen;
	std::vector<SOCKET>*MatchMakingQ = (std::vector<SOCKET>*)arg;

	while (1) {
		if (isMatchMakingQFull()) {
			// ��Ī ���� �˷���
			CreateGameServerThread();
			MatchMakingQ_DeQ();
		}
		else if(!MatchMakingQ->empty()) {
			// ����϶� �˷���
			for (auto client : *MatchMakingQ) {
				addrlen = sizeof(clientaddr);
				getpeername(client, (SOCKADDR*)&clientaddr, &addrlen);
				printf("[TCP ����] Ŭ���̾�Ʈ �����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			}
			printf("\n");
		}
		Sleep(1000);
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
