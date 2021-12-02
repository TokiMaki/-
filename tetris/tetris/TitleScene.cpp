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
    int x = WINDOW_WIDTH / 2 - 3 * 20; //타이틀화면이 표시되는 x좌표
    int y = WINDOW_HEIGHT / 4; //타이틀화면이 표시되는 y좌표

    HDC UIDC;
    UIDC = CreateCompatibleDC(hDC);
    (HBITMAP)SelectObject(UIDC, m_pGameClient->UIBitmap);

    HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(2, 29, 106));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, myBrush);

    Rectangle(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    SelectObject(hDC, oldBrush);
    DeleteObject(myBrush);

    TransparentBlt(hDC, WINDOW_WIDTH / 2 - 128, y , 256, 177,
        UIDC, 256, 18, 256, 177, RGB(255, 0, 255));

    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    TextOut(hDC, x, y + 20 * 10, "Enter Key to Start..", 20);
    TextOut(hDC, x, y + 20 * 11, "  △   : Rotate", 15);
    TextOut(hDC, x, y + 20 * 12, "◁  ▷ : Left / Right", 21);
    TextOut(hDC, x, y + 20 * 13, "  ▽   : Soft Drop", 18);
    TextOut(hDC, x, y + 20 * 14, " SPACE : Hard Drop", 18);
    TextOut(hDC, x, y + 20 * 15, " Shift : Change Target", 22);
    TextOut(hDC, x, y + 20 * 16, " Ctrl : Use Item", 16);
    TextOut(hDC, x, y + 20 * 17, "  F4  : Quit", 12);

    DeleteDC(UIDC);
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

