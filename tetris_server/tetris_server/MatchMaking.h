#pragma once
DWORD WINAPI MatchMakingThread(LPVOID arg); //매치매이킹 관리 스레드로, 최초 프로세스 시작 시 서버와 함께 실행

bool isMatchMakingQFull(std::vector<SOCKET>*MatchMakingQ);
//현재 MatchMakingQ에서 대기하는 클라이언트의 개수를 파악해 MAX_PLAYER 미만인 경우 MatchMakingQ에 추가되어 있는 클라이언트들에게 Msg_wait을 보냄 
//MAX_PLAYER 이상인 경우 MatchMakingQ에 추가되어 있는 순서대로 MAX_PLAYER개의 클라이언트에게 Msg_PlayInGame을 보내주고 CreateGameServerThread와 MatchMakingQ_DeQ를 실행

void CreateGameServerThread(std::vector<SOCKET>* MatchMakingQ);
//GameServerThread를 생성하며, MatchMakingQ의 소켓들을 전달

void MatchMakingQ_DeQ(std::vector<SOCKET>* MatchMakingQ);
//GameServerThread로 넘어간 MatchMakingQ에 클라이언트 소켓을 제거

enum MSG_MatchMaking //매치매이킹 시스템에서 사용할 메시지
{
	Msg_Ready,			//클라이언트가 서버에게 대기중임을 알림
	Msg_ReadyCancel,	//클라이언트가 서버에게 대기 취소를 알림
	Msg_ConfirmCancel,	//서버가 클라이언트에게 대기 취소 받았음을 돌려줌
	Msg_PlayInGame,		//서버가 클라이언트에게 게임이 시작 됐음을 알려줌
	Msg_WaitGame		//서버가 클라이언트에게 아직 매칭이 되지 않았음을 알려줌
};
