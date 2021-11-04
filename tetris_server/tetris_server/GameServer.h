#pragma once

DWORD WINAPI GameServerThread(LPVOID arg);

struct Player
{
	SOCKET clientSocket;
	KeyInput m_ley[MAX_PLAYER];
	Gamestatus m_gamestatus[MAX_PLAYER];
};
struct GameServerThreadDate
{
	std::vector<SOCKET*> pClients;
	std::vector<Player*> pPlayers;
	//CGameTimer m_timer;
	void CreateCommThread(void);
};

DWORD WINAPI CommThread(LPVOID arg);