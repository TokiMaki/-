#pragma once

DWORD WINAPI GameServerThread(LPVOID arg);

struct Player
{
	SOCKET clientSocket;
	KeyInput m_key;//Ŭ���̾�Ʈ�� Ű�Է� ����
	Gamestatus m_gamestatus[MAX_PLAYER];//������ ���� ����
	int m_GameClinetNum;
};
struct GameServerThreadData
{
	std::vector<SOCKET*> pClients; //MatchingThread���� �Ѱ� ���� Ŭ���̾�Ʈ ���ϵ�
	std::vector<Player> pPlayers; //�÷��̾� �������� ����
	//CGameTimer m_timer; //������Ʈ���� ������ �ð� ���� �� ���
	void CreateCommThread(void); //Ŭ���̾�Ʈ�� ����� ���������
};

DWORD WINAPI CommThread(LPVOID arg);
