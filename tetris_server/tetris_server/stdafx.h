#pragma once

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32")

#include<winsock2.h>
#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include<vector>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUF_SIZE 2048

#define LEFT 75 //�·� �̵�    //Ű���尪�� 
#define RIGHT 77 //��� �̵� 
#define UP 72 //ȸ�� 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
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
#define MAX_PLAYER 2 // �ִ� �ο���

struct KeyInput {
    bool left = false;      //��
    bool right = false;     //��
    bool up = false;        //��
    bool down = false;      //��
    bool shift = false;
    bool space = false;     //hard drop space(�ѹ��� �� ������ ������)
};

struct Flag {
    bool new_block_on = 0; //���ο� ���� �ʿ����� �˸��� flag 
    bool crush_on = 0; //���� �̵����� ����� �浹�������� �˷��ִ� flag

    bool left_flag = 0; // �ϵ����Ҷ� �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool right_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool down_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool space_flag = 0; // �ϵ����Ҷ� �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool up_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag
    bool shift_flag = 0; // ��Ű �� ������ �־ �ѹ��� ����ǰ� ���ִ� flag

    bool gameover_flag = 0; // ���ӿ����� ���� �� �˷��ִ� flag
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
    int board_cpy[BOARD_Y][BOARD_X]; //maincpy�� �������� ����Ϳ� ǥ�õǱ� ���� ������ ������ ���� 
                                  //main�� ��ü�� ��� ����Ϳ� ǥ������ �ʰ�(�̷��� �ϸ� ����Ͱ� �����Ÿ�)
                                  //main_cpy�� �迭�� ���ؼ� ���� �޶��� ���� ����Ϳ� ��ħ
    int AttackedBlock = 0;
    int item;       // 0 Ű ����
                    // 1 ��� �Ͻ��� ���ǵ� ��
                    // 2 �������� �ִ� ��� ��� �ٲٱ�
    int target = 1;

    Flag flag;
};

struct MatchSockets {
    SOCKET client[MAX_PLAYER];
};

class GlobalGameData //CommThread�� Ŭ���̾�Ʈ ���� ����, ���� ������Ʈ�� �Է� �� ��¿� ���̴� ������
{
private:
    SOCKET m_sockets[MAX_PLAYER];            //����ϰ� �ִ� ���ϵ��� ����
    Gamestatus m_gamestatus[MAX_PLAYER];     //ĳ���� ���� ����
    KeyInput m_keyInput[MAX_PLAYER];         //�� Ŭ���̾�Ʈ Ű �Է� ����
};