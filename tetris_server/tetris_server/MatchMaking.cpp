#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#include "stdafx.h"
#include "socket_function.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	std::vector<SOCKET>*MatchMakingQ = (std::vector<SOCKET>*)arg;

	while (1) {
		// ��Ī ���� �˷���
		if (isMatchMakingQFull(MatchMakingQ)) {
			CreateGameServerThread(MatchMakingQ);
			MatchMakingQ_DeQ(MatchMakingQ);
		}
		else if(!MatchMakingQ->empty()) {
			for (auto client : *MatchMakingQ) {
				addrlen = sizeof(clientaddr);
				getpeername(client, (SOCKADDR*)&clientaddr, &addrlen);
				printf("[TCP ����] Ŭ���̾�Ʈ �����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

				int MSG_len = htonl(sizeof(int));
				int MSG = MSG_MatchMaking::Msg_WaitGame;
				retval = send(client, (char*)&MSG_len, sizeof(int), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					MatchMakingQ->erase(std::remove_if(MatchMakingQ->begin(), MatchMakingQ->end(), 
						[client](SOCKET target) {return target == client; }));
					closesocket(client);
					continue;
				}

				retval = send(client, (char*)&MSG, sizeof(int), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					MatchMakingQ->erase(std::remove_if(MatchMakingQ->begin(), MatchMakingQ->end(),
						[client](SOCKET target) {return target == client; }));
					closesocket(client);
					continue;
				}
			}
			printf("\n");
		}
		Sleep(1000);
	}
	return 0;
}

bool isMatchMakingQFull(std::vector<SOCKET>*MatchMakingQ)
{
	if (MatchMakingQ->size() >= MAX_PLAYER){
		std::cout << "There are more than 3 Waiting Clients.\n";
		return true;
	}
	else return false;
}

void CreateGameServerThread(std::vector<SOCKET>* MatchMakingQ)
{
	std::cout << "Called CreateGameServerThread().\n";
}

void MatchMakingQ_DeQ(std::vector<SOCKET>* MatchMakingQ)
{
	std::cout << "Called MatchMakingQ_DeQ().\n";
}
