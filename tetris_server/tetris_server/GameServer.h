#pragma once

DWORD WINAPI GameServerThread(LPVOID arg);

struct Player
{
	SOCKET clientSocket;
	KeyInput m_key[MAX_PLAYER];//Ŭ���̾�Ʈ�� Ű�Է� ����
	Gamestatus m_gamestatus[MAX_PLAYER];//������ ���� ����
};
class GameServerThreadData
{
public:
	std::vector<SOCKET*> pClients; //MatchingThread���� �Ѱ� ���� Ŭ���̾�Ʈ ���ϵ�
	std::vector<Player> pPlayers; //�÷��̾� �������� ����
	//CGameTimer m_timer; //������Ʈ���� ������ �ð� ���� �� ���
	void CreateCommThread(void); //Ŭ���̾�Ʈ�� ����� ���������
};

DWORD WINAPI CommThread(LPVOID arg);
