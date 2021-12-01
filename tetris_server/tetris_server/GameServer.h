#pragma once
#include "Timer.h"

DWORD WINAPI GameServerThread(LPVOID arg);

struct Player
{
	SOCKET clientSocket;
	KeyInput m_keys;//Ŭ���̾�Ʈ�� Ű�Է� ����
	Gamestatus m_gamestatus[MAX_PLAYER];//������ ���� ����
	int m_GameClientNum;

    HANDLE hupdate; //Ŭ��� ���� �����͸� �޾Ҵ��� üũ
    HANDLE hcheckupdate; //���� �����͸� ������Ʈ �ߴ��� üũ
    CRITICAL_SECTION cs;
};
struct GameServerThreadData
{
	std::vector<SOCKET*> pClients; //MatchingThread���� �Ѱ� ���� Ŭ���̾�Ʈ ���ϵ�
	std::vector<Player> pPlayers; //�÷��̾� �������� ����
	CGameTimer m_GameTimer; //������Ʈ���� ������ �ð� ���� �� ���

    HANDLE hupdate;
    HANDLE hcheckupdate;

    CRITICAL_SECTION cs;

	void CreateCommThread(void); //Ŭ���̾�Ʈ�� ����� ���������

    void reset();
    void reset_main();
    void reset_main_cpy();
    void new_block(int ClientNum);
    void check_key();
    void KeyUpdate(int clientNum, float fTimeElapsed);
    void TargetShift(int clientNum, float fTimeElapsed);
    void drop_block(int ClientNum, float fTimeElapsed);
    void hard_drop_block(int ClientNum);
    bool check_crush(int ClientNum, int bx, int by, int b_rotation);
    void move_block(int ClientNum, int dir);
    void check_line(int ClientNum);
    void copy_another_map(void);
    //void check_level_up(void);
    void check_game_over(int ClientNum);
    void active_item(void);
    void attack(int ClientNum, int Combo);
    void attacked(int ClientNum);

    int blocks[7][4][4][4] = {
    {{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},           // 0 ����� ����
     {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},           // 1 1 ��� ���� 
     {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
    {{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},           // 2 ������� ����
     {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
    {{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},           // 3 �� ������� ����
     {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
    {{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},           // 4 ������� ����
     {0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
    {{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},           // 5 ����� ����
     {0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
    {{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},           // 6 �Ǹ�� ����
     {0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
    }; //���ϸ�� ���� 4*4������ ������ ǥ�� blcoks[b_type][b_rotation][i][j]�� ���


};

DWORD WINAPI CommThread(LPVOID arg);
