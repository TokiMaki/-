#pragma once

#include "stdafx.h"

class Scene;

class GamePlayScene : public Scene {

public:
    GamePlayScene();
    GamePlayScene(SceneNum num, GameClient* const pGameClient);
    ~GamePlayScene();

    void Update(float fTimeElapsed) override;
    void reset();
    void reset_main();
    void reset_main_cpy();
    void new_block();
    void draw_map();
    void draw_main();
    void check_key();
    void check_key2(float fTimeElapsed);
    void KeyUpdate(float fTimeElapsed);
    void drop_block(float fTimeElapsed);
    void hard_drop_block();
    int check_crush(int bx, int by, int b_rotation);
    void move_block(int dir);
    void check_line(void);
    void check_level_up(void);
    void check_game_over(void);
private:
    KeyInput m_keys;                    //Ŭ���̾�Ʈ�� Ű�Է��� ����
    Gamestatus m_gamestatus;  	        //�� Ŭ���̾�Ʈ�� ���� ����
    Flag flag;

    int key; //Ű����� �Է¹��� Ű���� ����

    int level_goal; //���������� �Ѿ�� ���� ��ǥ����
    int cnt; //���� �������� ������ �� ���� ����
    int score; //���� ����

    //int STATUS_Y_GOAL; //GOAL ����ǥ����ġY ��ǥ ����
    int STATUS_Y_LEVEL; //LEVEL ����ǥ����ġY ��ǥ ����
    //int STATUS_Y_SCORE; //SCORE ����ǥ����ġY ��ǥ ����


    int blocks[7][4][4][4] = {
    {{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},           // 0 ����� ��
     {0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0},           // 1 1 ��� �� 
     {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},{0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0}},
    {{0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0},           // 2 ������� ��
     {0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,1,1,0,0,1,0,0}},
    {{0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0},           // 3 �� ������� ��
     {0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0},{0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0}},
    {{0,0,0,0,0,0,1,0,1,1,1,0,0,0,0,0},{0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0},           // 4 ������� ��
     {0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0}},
    {{0,0,0,0,1,0,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,0,0,1,1,0,0},           // 5 ����� ��
     {0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,0},{0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0}},
    {{0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0},{0,0,0,0,0,1,0,0,0,1,1,0,0,1,0,0},           // 6 �Ǹ�� ��
     {0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,0},{0,0,0,0,0,1,0,0,1,1,0,0,0,1,0,0}}
    }; //��ϸ�� ���� 4*4������ ����� ǥ�� blcoks[b_type][b_rotation][i][j]�� ���

};


