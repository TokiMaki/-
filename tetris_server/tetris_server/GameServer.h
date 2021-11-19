#pragma once

DWORD WINAPI GameServerThread(LPVOID arg);

struct Player
{
	SOCKET clientSocket;
	KeyInput m_key;//클라이언트의 키입력 저장
	Gamestatus m_gamestatus[MAX_PLAYER];//게임의 상태 저장
	int m_GameClinetNum;
};
struct GameServerThreadData
{
	std::vector<SOCKET*> pClients; //MatchingThread에서 넘겨 받은 클라이언트 소켓들
	std::vector<Player> pPlayers; //플레이어 개개인의 상태
	//CGameTimer m_timer; //업데이트에서 프레임 시간 연산 시 사용
	void CreateCommThread(void); //클라이언트와 통신할 쓰레드생성
};

DWORD WINAPI CommThread(LPVOID arg);
