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

#define LEFT 75 //�·� �̵�    //Ű���尪�� 
#define RIGHT 77 //��� �̵� 
#define UP 72 //ȸ�� 
#define DOWN 80 //soft drop
#define SPACE 32 //hard drop
#define ESC 27 //�������� 

#define ACTIVE_BLOCK -2 // �����ǹ迭�� ����� ����� ���µ� 
#define CEILLING -1     // ����� �̵��� �� �ִ� ������ 0 �Ǵ� ���� ������ ǥ�� 
#define EMPTY 0         // ����� �̵��� �� ���� ������ ����� ǥ�� 
#define WALL 1
#define INACTIVE_BLOCK 2 // �̵��� �Ϸ�� ��ϰ� 

#define BOARD_X 11 //������ ����ũ��
#define BOARD_Y 23 //������ ����ũ��
#define BOARD_X_ADJ 3 //������ ��ġ���� 
#define BOARD_Y_ADJ 1 //������ ��ġ���� 

#define STATUS_X_ADJ BOARD_X_ADJ+BOARD_X+1 //��������ǥ�� ��ġ���� 
#define MAX_PLAYER 3 // �ִ� �ο���