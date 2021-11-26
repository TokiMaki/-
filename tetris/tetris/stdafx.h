#pragma once

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32")

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

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define LEFT 75 //좌로 이동    //키보드값들 
#define RIGHT 77 //우로 이동 
#define UP 72 //회전 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define p 112 //일시정지 
#define P 80 //일시정지
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
#define MAX_PLAYER 1 // 최대 인원수

struct KeyInput {
    bool left = false;      //←
    bool right = false;     //→
    bool up = false;        //↑
    bool down = false;      //←
    bool space = false;     //hard drop space(한번에 맨 밑으로 내리기)
};

struct Flag {
    bool new_block_on = 0; //새로운 블럭이 필요함을 알리는 flag
    bool crush_on = 0; //현재 이동중인 블록이 충돌상태인지 알려주는 flag

    bool left_flag = 0; // 하드드랍할때 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool right_flag = 0; // 위키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool down_flag = 0; // 위키 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool space_flag = 0; // 하드드랍할때 꾹 누르고 있어도 한번만 적용되게 해주는 flag
    bool up_flag = 0; // 위키 꾹 누르고 있어도 한번만 적용되게 해주는 flag

    bool gameover_flag = 0; // 게임오버가 됬을 때 알려주는 flag
};

struct Gamestatus {
    int bx, by; //이동중인 블록의 게임판상의 x,y좌표
    int b_type; //블록 종류
    int b_rotation; //블록 회전값
    int b_type_next; //다음 블록값
    int level; //현재 level
    float speed; //블럭이 내려오는 속도 1이면 1초마다 한칸씩 내려옴
    float fKeyMoveSpeed = 0.1f; //블럭이 키 입력이 됬을 때 좌우나 아래로 움직이는 속도
    float fDropBlockTime = 0.0f;
    float fMoveBlockTime = 0.0f;
    int board_org[BOARD_Y][BOARD_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 main_cpy로 복사됨
    int board_cpy[BOARD_Y][BOARD_X]; //maincpy는 게임판이 모니터에 표시되기 전의 정보를 가지고 있음 
                                  //main의 전체를 계속 모니터에 표시하지 않고(이렇게 하면 모니터가 깜빡거림)
                                  //main_cpy와 배열을 비교해서 값이 달라진 곳만 모니터에 고침
    int item;       // 0 키 반전
                    // 1 상대 일시적 스피드 업
                    // 2 내려오고 있는 블록 모양 바꾸기
    int target;

    Flag flag;
};

enum MSG_MatchMaking //매치매이킹 시스템에서 사용할 메시지
{
    Msg_Ready,			//클라이언트가 서버에게 대기중임을 알림
    Msg_ReadyCancel,	//클라이언트가 서버에게 대기 취소를 알림
    Msg_ConfirmCancel,	//서버가 클라이언트에게 대기 취소 받았음을 돌려줌
    Msg_PlayInGame,		//서버가 클라이언트에게 게임이 시작 됐음을 알려줌
    Msg_WaitGame		//서버가 클라이언트에게 아직 매칭이 되지 않았음을 알려줌
};

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE; //커서숨기는 함수에 사용되는 열거형

inline void gotoxy(int x, int y) { //gotoxy함수 
    COORD pos = { 2 * x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

inline void setcursortype(CURSOR_TYPE c) { //커서숨기는 함수 
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