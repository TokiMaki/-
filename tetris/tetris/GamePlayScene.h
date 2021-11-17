#pragma once

#include "stdafx.h"

class Scene;

class GamePlayScene : public Scene {

public:
    GamePlayScene();
    GamePlayScene(SceneNum num, GameClient* const pGameClient);
    ~GamePlayScene();

    void InitScene();

    void Update(float fTimeElapsed) override;
    void reset();
    void reset_main();
    void reset_main_cpy();
    void new_block();
    void draw_map();
    void draw_main();
    void check_key();
    void KeyUpdate(float fTimeElapsed);
    void drop_block(float fTimeElapsed);
    void hard_drop_block();
    int check_crush(int bx, int by, int b_rotation);
    void move_block(int dir);
    void check_line(void);
    void check_level_up(void);
    void check_game_over(void);
private:
    KeyInput m_keys;                    //클라이언트의 키입력을 저장
    Gamestatus m_gamestatus;  	        //내 클라이언트의 게임 상태
    Flag flag;

    int key; //키보드로 입력받은 키값을 저장

    int level_goal; //다음레벨로 넘어가기 위한 목표점수
    int cnt; //현재 레벨에서 제거한 줄 수를 저장
    int score; //현재 점수

    //int STATUS_Y_GOAL; //GOAL 정보표시위치Y 좌표 저장
    int STATUS_Y_LEVEL; //LEVEL 정보표시위치Y 좌표 저장
    //int STATUS_Y_SCORE; //SCORE 정보표시위치Y 좌표 저장


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


