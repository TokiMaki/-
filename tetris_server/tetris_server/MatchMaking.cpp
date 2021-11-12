#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#include "stdafx.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	SOCKADDR_IN clientaddr;
	int addrlen;
	std::vector<SOCKET>*MatchMakingQ = (std::vector<SOCKET>*)arg;

	while (1) {
		// 매칭 성립 알려줌
		if (isMatchMakingQFull(MatchMakingQ)) {
			CreateGameServerThread();
			MatchMakingQ_DeQ();
		}
		else if(!MatchMakingQ->empty()) {
			for (auto client : *MatchMakingQ) {
				addrlen = sizeof(clientaddr);
				getpeername(client, (SOCKADDR*)&clientaddr, &addrlen);
				printf("[TCP 서버] 클라이언트 대기중: IP 주소=%s, 포트 번호=%d\n",
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
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

void CreateGameServerThread()
{
	std::cout << "Called CreateGameServerThread().\n";
}

void MatchMakingQ_DeQ()
{
	std::cout << "Called MatchMakingQ_DeQ().\n";
}
