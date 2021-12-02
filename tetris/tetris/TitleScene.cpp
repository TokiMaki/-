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

}

void TitleScene::Paint(HDC hDC)
{
    int x = WINDOW_WIDTH / 4; //타이틀화면이 표시되는 x좌표
    int y = WINDOW_HEIGHT / 4; //타이틀화면이 표시되는 y좌표

    HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(rand() % 255, rand() % 255, rand() % 255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, myBrush);

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
    TextOut(hDC, x, y + 20 * 13, "  F4  : Quit", 12);
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

void TitleScene::KeyUp(unsigned char KEYCODE)
{
}

void TitleScene::InitScene() {

}

