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
    /*
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
    */
}

void TitleScene::Paint(HDC hDC)
{
    int x = WINDOW_WIDTH / 4; //타이틀화면이 표시되는 x좌표
    int y = WINDOW_HEIGHT / 4; //타이틀화면이 표시되는 y좌표

    HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, myBrush);

    RECT rt = { x,y,x + 250,y + 100 };
    //FillRect(hDC, &rt, CreateSolidBrush(RGB(rand()%255, rand() % 255, rand() % 255)));
    Rectangle(hDC, x, y, x+250, y+100);
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(rand() % 255, rand() % 255, rand() % 255));
    TextOut(hDC, x + 20 * 5, y + 20 * 2, "T E T R I S", 11);
    SetTextColor(hDC, RGB(0, 0, 0));
    TextOut(hDC, x, y + 20 * 7, "Enter Key to Start..", 20);
    TextOut(hDC, x, y + 20 * 9, "  △   : Shift", 14);
    TextOut(hDC, x, y + 20 * 10, "◁  ▷ : Left / Right", 21);
    TextOut(hDC, x, y + 20 * 11, "  ▽   : Soft Drop", 18);
    TextOut(hDC, x, y + 20 * 12, " SPACE : Hard Drop", 18);
    TextOut(hDC, x, y + 20 * 13, "  ESC  : Quit", 13);
    TextOut(hDC, x, y + 20 * 15, "BONUS FOR HARD DROPS / COMBOS", 29);

    SelectObject(hDC, oldBrush);
    DeleteObject(myBrush);
}

void TitleScene::KeyDown(unsigned char KEYCODE)
{
    switch (KEYCODE) {
    case VK_RETURN:
        m_pGameClient->ChangeScene(Scene::SceneNum::Wait);
        break;
    }
}

void TitleScene::InitScene() {

}

