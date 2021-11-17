#include "Scene.h"
#include "Titlescene.h"
#include "GameClient.h"
#include "stdafx.h"

TitleScene::TitleScene() {}
TitleScene::TitleScene(SceneNum num, GameClient* const pGameClient) {
    m_SceneNum = num;
    m_pGameClient = pGameClient;
}
TitleScene::~TitleScene() {}

void TitleScene::Update(float fTimeElapsed) {
    int x = 5; //타이틀화면이 표시되는 x좌표
    int y = 4; //타이틀화면이 표시되는 y좌표
    int cnt; //타이틀 프레임을 세는 변수

    setcursortype(NOCURSOR);

    gotoxy(x, y + 0); printf("■□□□■■■□□■■□□■■");
    gotoxy(x, y + 1); printf("■■■□  ■□□    ■■□□■");
    gotoxy(x, y + 2); printf("□□□■              □■  ■");
    gotoxy(x, y + 3); printf("■■□■■  □  ■  □□■□□");
    gotoxy(x, y + 4); printf("■■  ■□□□■■■□■■□□");
    gotoxy(x + 5, y + 2); printf("T E T R I S");
    gotoxy(x, y + 7); printf("Enter Key to Start..");
    gotoxy(x, y + 9); printf("  △   : Shift");
    gotoxy(x, y + 10); printf("◁  ▷ : Left / Right");
    gotoxy(x, y + 11); printf("  ▽   : Soft Drop");
    gotoxy(x, y + 12); printf(" SPACE : Hard Drop");
    gotoxy(x, y + 13); printf("   P   : Pause");
    gotoxy(x, y + 14); printf("  ESC  : Quit");
    gotoxy(x, y + 16); printf("BONUS FOR HARD DROPS / COMBOS");

    if (GetAsyncKeyState(VK_RETURN)) {
        m_pGameClient->ChangeScene(Scene::SceneNum::Wait);
    }; //키입력이 게임 플레이 신으로
    while (kbhit()) getch(); //버퍼에 기록된 키값을 버림
}

void TitleScene::InitScene() {

}