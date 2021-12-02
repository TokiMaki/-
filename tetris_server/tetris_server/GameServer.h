#pragma once
#include "Timer.h"

DWORD WINAPI GameServerThread(LPVOID arg);

struct Player
{
    SOCKET clientSocket;
    KeyInput m_keys;//클라이언트의 키입력 저장
    Gamestatus m_gamestatus[MAX_PLAYER];//게임의 상태 저장
    int m_GameClientNum;

    HANDLE hupdate; //클라로 부터 데이터를 받았는지 체크
    HANDLE hcheckupdate; //받은 데이터를 업데이트 했는지 체크
    CRITICAL_SECTION cs;
};

struct GameServerThreadData
{
    std::vector<SOCKET*> pClients; //MatchingThread에서 넘겨 받은 클라이언트 소켓들
    std::vector<Player> pPlayers; //플레이어 개개인의 상태
    CGameTimer m_GameTimer; //업데이트에서 프레임 시간 연산 시 사용

    HANDLE hupdate;
    HANDLE hcheckupdate;


    CRITICAL_SECTION cs;

    int Level = 0; //���� level
    float fLevelUpTime = 0.f;

	void CreateCommThread(void); //Ŭ���̾�Ʈ�� ����� ���������

    void reset();
    void reset_main();
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
    void attack(int ClientNum, int Combo);
    void attacked(int ClientNum);
    void check_level_up(float fTimeElapsed);
    void ActiveItem(int ClientNum, float fTimeElapsed);

    int blocks[7][4][4][4] = {
    {{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},           // 0 ㅁ모양 블럭
     {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},           // 1 1 모양 블럭 
     {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
    {{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},           // 2 ㄱㄴ모양 블럭
     {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
    {{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},           // 3 역 ㄱㄴ모양 블럭
     {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
    {{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},           // 4 역ㄴ모양 블럭
     {0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
    {{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},           // 5 ㄴ모양 블럭
     {0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
    {{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},           // 6 ㅗ모양 블럭
     {0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
    }; //블록모양 저장 4*4공간에 블록을 표현 blcoks[b_type][b_rotation][i][j]로 사용


};

DWORD WINAPI CommThread(LPVOID arg);
