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

    gotoxy(x, y + 0); printf("■□□□■■■□□■■□□■■");
    gotoxy(x, y + 1); printf("■■■□  ■□□    ■■□□■");
    gotoxy(x, y + 2); printf("□□□■              □■  ■");
    gotoxy(x, y + 3); printf("■■□■■  □  ■  □□■□□");
    gotoxy(x, y + 4); printf("■■  ■□□□■■■□■■□□");
    gotoxy(x + 5, y + 2); printf("T E T R I S");
    gotoxy(x, y + 7); printf("Please Enter Any Key to Start..");
    gotoxy(x, y + 9); printf("  △   : Shift");
    gotoxy(x, y + 10); printf("◁  ▷ : Left / Right");
    gotoxy(x, y + 11); printf("  ▽   : Soft Drop");
    gotoxy(x, y + 12); printf(" SPACE : Hard Drop");
    gotoxy(x, y + 13); printf("   P   : Pause");
    gotoxy(x, y + 14); printf("  ESC  : Quit");
    gotoxy(x, y + 16); printf("BONUS FOR HARD DROPS / COMBOS");

    //for (cnt = 0;; cnt++) { //cnt를 1씩 증가시키면서 계속 반복    //하나도 안중요한 별 반짝이는 애니메이션효과
    //    if (cnt % 200 == 0) { gotoxy(x + 4, y + 1); printf("★"); }       //cnt가 200으로 나누어 떨어질때 별을 표시
    //    if ((cnt % 200 - 100) == 0) { gotoxy(x + 4, y + 1); printf("  "); } //위 카운트에서 100카운트 간격으로 별을 지움
    //    if ((cnt % 350) == 0) { gotoxy(x + 13, y + 2); printf("☆"); } //윗별과 같지만 시간차를 뒀음
    //    if ((cnt % 350 - 100) == 0) { gotoxy(x + 13, y + 2); printf("  "); }
    //}

    if (kbhit()) {
        while (kbhit()) getch(); //버퍼에 기록된 키값을 버림
        m_pGameClient->ChangeScene(Scene::SceneNum::GamePlay);
    }; //키입력이 있으면 무한루프 종료
    while (kbhit()) getch(); //버퍼에 기록된 키값을 버림

}