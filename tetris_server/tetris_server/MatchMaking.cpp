#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#include "stdafx.h"
#include "socket_function.h"
#include "MatchMaking.h"
#include "GameServer.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;
	std::vector<SOCKET>*MatchMakingQ = (std::vector<SOCKET>*)arg;

	while (1) {
		// 매칭 성립 알려줌
		if (isMatchMakingQFull(MatchMakingQ)) {
			CreateGameServerThread(MatchMakingQ);
			//MatchMakingQ_DeQ(MatchMakingQ);
		}
		else if(!MatchMakingQ->empty()) {
			for (auto client : *MatchMakingQ) {
				if (SendMsgtoClient(MSG_MatchMaking::Msg_WaitGame, client) == SOCKET_ERROR) {
					MatchMakingQ_CloseSocket(MatchMakingQ, client);
				}
			}
			//printf("\n");
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
	CreateThread(NULL, 0, GameServerThread, NULL, 0, NULL);
	std::cout << "Called CreateGameServerThread().\n";
}

void MatchMakingQ_CloseSocket(std::vector<SOCKET>* MatchMakingQ, SOCKET client)
{
	MatchMakingQ->erase(std::remove_if(MatchMakingQ->begin(), MatchMakingQ->end(),
		[client](SOCKET target) {return target == client; }));
	closesocket(client);
}

void MatchMakingQ_DeQ(std::vector<SOCKET>* MatchMakingQ, SOCKET client)
{
	MatchMakingQ->erase(std::remove_if(MatchMakingQ->begin(), MatchMakingQ->end(),
		[client](SOCKET target) {return target == client; }));
}

int SendMsgtoClient(int Msg, SOCKET client)
{
	int retval;
	int sendMsg = htonl(Msg);

	int MSG_len = htonl(sizeof(int));
	retval = send(client, (char*)&MSG_len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return SOCKET_ERROR;
	}

	retval = send(client, (char*)&sendMsg, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
		return SOCKET_ERROR;
	}

	return 0;
}
