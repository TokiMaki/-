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

#define SERVERIP   "220.94.221.36"
#define SERVERPORT 9000
#define BUF_SIZE 2048

#define LEFT 75 //좌로 이동    //키보드값들 
#define RIGHT 77 //우로 이동 
#define UP 72 //회전 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define ESC 27 //게임종료 

#define ACTIVE_BLOCK -2 // 움직일 수 있는 블록
#define CEILLING -1     // 천장
#define EMPTY 0         // 비어있음
#define WALL 1          // 벽
#define INACTIVE_BLOCK 2 // 굳어있는 블록

#define BOARD_X 11 //게임판 가로크기
#define BOARD_Y 25 //게임판 세로크기
#define BOARD_X_ADJ 3 //게임판 위치조정 
#define BOARD_Y_ADJ 1 //게임판 위치조정

#define CEILLING_Y BOARD_Y - 20     // 천장 위치

#define STATUS_X_ADJ BOARD_X_ADJ+BOARD_X+1 //게임정보표시 위치조정 
#define MAX_PLAYER 2 // 최대 인원수

struct KeyInput {
    bool left = false;      //←
    bool right = false;     //→
    bool up = false;        //↑
    bool down = false;      //←
    bool shift = false;
    bool space = false;     //hard drop space(한번에 맨 밑으로 내리기)
    bool ctrl = false;
    bool enter = false;
};

struct KeyFlag {
    bool left_flag = 0; // 하드드랍할때 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool right_flag = 0; // 위키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool down_flag = 0; // 위키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool space_flag = 0; // 하드드랍할때 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool up_flag = 0; // 위키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool shift_flag = 0; // 쉬프트키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool ctrl_flag = 0; // 컨트롤키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
};

struct GameFlag {
    bool gameover_flag = 0; // 게임오버가 됬을 때 알려주는 flag
    bool win_flag = 0;      // 이겼을 때 알려주는 flag
    
    bool screen_rotate_flag = 0; // 스크린 돌아가는것을 알려주는 flag
    float fScreenRotateTime = 0.0f;  // 스크린이 몇 초 돌아갓는지 알려주는 변수

    bool speedup_flag = 0; // 스크린 돌아가는것을 알려주는 flag
    float fSpeedUpTime = 0.0f;  // 스피드 업이 몇초 됬는지 알려주는 변수

    bool new_block_on = 0; //새로운 블럭이 필요함을 알리는 flag
    bool crush_on = 0; //현재 이동중인 블록이 충돌상태인지 알려주는 flag
};

struct Gamestatus {
    int bx, by; //이동중인 블록의 게임판상의 x,y좌표
    int b_type; //블록 종류
    int b_rotation; //블록 회전값
    int b_type_next; //다음 블록값
    float speed; //블럭이 내려오는 속도 1이면 1초마다 한칸씩 내려옴
    float fKeyMoveSpeed = 0.1f; //블럭이 키 입력이 됬을 때 좌우나 아래로 움직이는 속도
    float fDropBlockTime = 0.0f;
    float fMoveBlockTime = 0.0f;
    int board_org[BOARD_Y][BOARD_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 main_cpy로 복사됨

    int AttackedBlock = 0;
    int item = -1;  // 0 키 반전
                    // 1 상대 일시적 스피드 업
                    // 2 내려오고 있는 블록 모양 바꾸기
    int target = 1;

    KeyFlag m_KeyFlag;
    GameFlag m_GameFlag;
};

struct MatchSockets {
    SOCKET client[MAX_PLAYER];
};

class GlobalGameData //CommThread와 클라이언트 간의 소켓, 게임 업데이트의 입력 및 출력에 쓰이는 데이터
{
private:
    SOCKET m_sockets[MAX_PLAYER];            //통신하고 있는 소켓들을 저장
    Gamestatus m_gamestatus[MAX_PLAYER];     //캐릭터 상태 저장
    KeyInput m_keyInput[MAX_PLAYER];         //각 클라이언트 키 입력 저장
};