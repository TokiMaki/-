#pragma once

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "msimg32.lib")

#define _WINSOCKAPI_
#include<windows.h>
#include<winsock2.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<iostream>
#include<string.h>


#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 720

#define LEFT 75 //�·� �̵�    //Ű���尪�� 
#define RIGHT 77 //��� �̵� 
#define UP 72 //ȸ�� 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define p 112 //�Ͻ����� 
#define P 80 //�Ͻ�����
#define ESC 27 //�������� 

#define ACTIVE_BLOCK -2 // ������ �� �ִ� ���
#define CEILLING -1     // õ��
#define EMPTY 0         // �������
#define WALL 1          // ��
#define INACTIVE_BLOCK 2 // �����ִ� ���


#define BOARD_X 11 //������ ����ũ��
#define BOARD_Y 25 //������ ����ũ��
#define BOARD_X_ADJ 3 //������ ��ġ����
#define BOARD_Y_ADJ 1 //������ ��ġ����

#define CEILLING_Y BOARD_Y - 20     // õ�� ��ġ

#define STATUS_X_ADJ BOARD_X_ADJ+BOARD_X+1 //��������ǥ�� ��ġ���� 
#define MAX_PLAYER 3 // �ִ� �ο���

struct KeyInput {
    bool left = false;      //��
    bool right = false;     //��
    bool up = false;        //��
    bool down = false;      //��
    bool shift = false;
    bool space = false;     //hard drop space(�ѹ��� �� ������ ������)
    bool ctrl = false;
};

struct KeyFlag {
    bool new_block_on = 0; //���ο� ���� �ʿ����� �˸��� flag
    bool crush_on = 0; //���� �̵����� ����� �浹�������� �˷��ִ� flag

    bool left_flag = 0; // �ϵ����Ҷ� �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool right_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool down_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool space_flag = 0; // �ϵ����Ҷ� �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool up_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool shift_flag = 0; // ����ƮŰ �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool ctrl_flag = 0; // ����ƮŰ �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag

    bool gameover_flag = 0; // ���ӿ����� ���� �� �˷��ִ� flag
};

struct GameFlag {
    bool gameover_flag = 0; // ���ӿ����� ���� �� �˷��ִ� flag

    bool screen_rotate_flag = 0; // ��ũ�� ���ư��°��� �˷��ִ� flag
    float fScreenRotateTime = 0.0f;  // ��ũ���� �� �� ���ư����� �˷��ִ� ����

    bool speedup_flag = 0; // ��ũ�� ���ư��°��� �˷��ִ� flag
    float fSpeedUpTime = 0.0f;  // ���ǵ� ���� ���� ����� �˷��ִ� ����

    bool new_block_on = 0; //���ο� ���� �ʿ����� �˸��� flag
    bool crush_on = 0; //���� �̵����� ����� �浹�������� �˷��ִ� flag
};

struct Gamestatus {
    int bx, by; //�̵����� ����� �����ǻ��� x,y��ǥ
    int b_type; //��� ����
    int b_rotation; //��� ȸ����
    int b_type_next; //���� ��ϰ�
    int level; //���� level
    float speed; //���� �������� �ӵ� 1�̸� 1�ʸ��� ��ĭ�� ������
    float fKeyMoveSpeed = 0.1f; //���� Ű �Է��� ���� �� �¿쳪 �Ʒ��� �����̴� �ӵ�
    float fDropBlockTime = 0.0f;
    float fMoveBlockTime = 0.0f;
    int board_org[BOARD_Y][BOARD_X]; //�������� ������ �����ϴ� �迭 ����Ϳ� ǥ���Ŀ� main_cpy�� �����

    int AttackedBlock = 0;        //���ݹ��� ��� ��
    int item = -1;       // ������ ��� ����
                    // 0 Ű ����
                    // 1 ��� �Ͻ��� ���ǵ� ��
                    // 2 �������� �ִ� ��� ��� �ٲٱ�
    int target = 1;

    KeyFlag m_KeyFlag;
    GameFlag m_GameFlag;
};

enum MSG_MatchMaking //��ġ����ŷ �ý��ۿ��� ����� �޽���
{
    Msg_Ready,			//Ŭ���̾�Ʈ�� �������� ��������� �˸�
    Msg_ReadyCancel,	//Ŭ���̾�Ʈ�� �������� ��� ��Ҹ� �˸�
    Msg_ConfirmCancel,	//������ Ŭ���̾�Ʈ���� ��� ��� �޾����� ������
    Msg_PlayInGame,		//������ Ŭ���̾�Ʈ���� ������ ���� ������ �˷���
    Msg_WaitGame		//������ Ŭ���̾�Ʈ���� ���� ��Ī�� ���� �ʾ����� �˷���
};

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; //Ŀ������� �Լ��� ���Ǵ� ������

inline void gotoxy(int x, int y) { //gotoxy�Լ� 
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

inline void setcursortype(CURSOR_TYPE c) { //Ŀ������� �Լ� 
    CONSOLE_CURSOR_INFO CurInfo;

    switch (c) {
    case NOCURSOR:
        CurInfo.dwSize = 1;
        CurInfo.bVisible = FALSE;
        break;
    case SOLIDCURSOR:
        CurInfo.dwSize = 100;
        CurInfo.bVisible = TRUE;
        break;
    case NORMALCURSOR:
        CurInfo.dwSize = 20;
        CurInfo.bVisible = TRUE;
        break;
    }
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}