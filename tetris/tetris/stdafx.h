#pragma once

#include<winsock2.h>
#include<windows.h>
#include<stdio.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32")

#define LEFT 75 //좌로 이동    //키보드값들 
#define RIGHT 77 //우로 이동 
#define UP 72 //회전 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define p 112 //일시정지 
#define P 80 //일시정지
#define ESC 27 //게임종료 

#define ACTIVE_BLOCK -2 // 게임판배열에 저장될 블록의 상태들 
#define CEILLING -1     // 블록이 이동할 수 있는 공간은 0 또는 음의 정수료 표현 
#define EMPTY 0         // 블록이 이동할 수 없는 공간은 양수로 표현 
#define WALL 1
#define INACTIVE_BLOCK 2 // 이동이 완료된 블록값 

#define BOARD_X 11 //게임판 가로크기
#define BOARD_Y 23 //게임판 세로크기
#define BOARD_X_ADJ 3 //게임판 위치조정 
#define BOARD_Y_ADJ 1 //게임판 위치조정 

#define STATUS_X_ADJ BOARD_X_ADJ+BOARD_X+1 //게임정보표시 위치조정 
#define MAX_PLAYER 3 // 최대 인원수



struct KeyInput {
    bool left = false;      //←
    bool right = false;     //→
    bool up = false;        //↑
    bool down = false;      //←
    bool space = false;     //hard drop space(한번에 맨 밑으로 내리기)
};

struct Gamestatus {
    int bx, by; //이동중인 블록의 게임판상의 x,y좌표
    int b_type; //블록 종류
    int b_rotation; //블록 회전값
    int b_type_next; //다음 블록값
    int level; //현재 level
    float speed; //블럭이 내려오는 속도 1이면 1초마다 한칸씩 내려옴
    float fDropBlockTime = 0.0f;
    int board_org[BOARD_Y][BOARD_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 main_cpy로 복사됨 
    int board_cpy[BOARD_Y][BOARD_X]; //maincpy는 게임판이 모니터에 표시되기 전의 정보를 가지고 있음 
                                  //main의 전체를 계속 모니터에 표시하지 않고(이렇게 하면 모니터가 깜빡거림) 
                                  //main_cpy와 배열을 비교해서 값이 달라진 곳만 모니터에 고침
    int item;       // 0 키 반전
                    // 1 상대 일시적 스피드 업
                    // 2 내려오고 있는 블록 모양 바꾸기
    int target;
};