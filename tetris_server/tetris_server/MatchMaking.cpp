#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#include "stdafx.h"
#include "socket_function.h"
#include "MatchMaking.h"
#include "GameServer.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	std::vector<SOCKET>*MatchMakingQ = (std::vector<SOCKET>*)arg;

	while (1) {
		// 매칭 성립 알려줌
		if (isMatchMakingQFull(MatchMakingQ)) {
			MatchSockets* send = new MatchSockets;
			for (int i = 0; i < MAX_PLAYER; ++i) {
				send->client[i] = MatchMakingQ_DeQ(MatchMakingQ);
				SendMsgtoClient(MSG_MatchMaking::Msg_PlayInGame, send->client[i]);
			}
			CreateGameServerThread(send);
		}
		else if(!MatchMakingQ->empty()) {
			for (auto client : *MatchMakingQ) {
				if (SendMsgtoClient(MSG_MatchMaking::Msg_WaitGame, client) == SOCKET_ERROR) {
					MatchMakingQ_CloseSocket(MatchMakingQ, client);
					continue;
				}
				switch (RecvMsgfromClient(client))
				{
				case SOCKET_ERROR:
				case MSG_MatchMaking::Msg_ReadyCancel:
					MatchMakingQ_CloseSocket(MatchMakingQ, client);
					continue;
				default:
					break;
				}
			}
		}
		//std::cout << "대기인원 " << MatchMakingQ->size() << std::endl;
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

void CreateGameServerThread(MatchSockets* target)
{
	CreateThread(NULL, 0, GameServerThread, target, 0, NULL);
	std::cout << "Called CreateGameServerThread().\n";
}

void MatchMakingQ_CloseSocket(std::vector<SOCKET>* MatchMakingQ, SOCKET client)
{
	MatchMakingQ->erase(std::remove_if(MatchMakingQ->begin(), MatchMakingQ->end(),
		[client](SOCKET target) {return target == client; }),MatchMakingQ->end());
	closesocket(client);
}

SOCKET MatchMakingQ_DeQ(std::vector<SOCKET>* MatchMakingQ)
{
	SOCKET target = *MatchMakingQ->begin();
	MatchMakingQ->erase(MatchMakingQ->begin());
	return target;
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

int RecvMsgfromClient(SOCKET client)
{
	int retval;
	int len = 0;
	int Msg = -1;
	SOCKET client_sock = (SOCKET)client;

	retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return SOCKET_ERROR;
	}
	else if (retval == 0)
		return 0;
	len = ntohl(len);

	retval = recvn(client_sock, (char*)&Msg, len, 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		return SOCKET_ERROR;
	}
	else if (retval == 0)
		return 0;
	Msg = ntohl(Msg);

	return Msg;
}
