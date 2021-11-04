#pragma once

#include<stdio.h>
#include<winsock2.h>
#include<windows.h>
#include<conio.h>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include<queue>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "ws2_32")

#define LEFT 75 //좌로 이동    //키보드값들 
#define RIGHT 77 //우로 이동 
#define UP 72 //회전 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
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