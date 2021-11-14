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
    int x = 5; //Ÿ��Ʋȭ���� ǥ�õǴ� x��ǥ
    int y = 4; //Ÿ��Ʋȭ���� ǥ�õǴ� y��ǥ
    int cnt; //Ÿ��Ʋ �������� ���� ����

    setcursortype(NOCURSOR);

    gotoxy(x, y + 0); printf("����������������");
    gotoxy(x, y + 1); printf("�����  ����    ������");
    gotoxy(x, y + 2); printf("�����              ���  ��");
    gotoxy(x, y + 3); printf("������  ��  ��  ������");
    gotoxy(x, y + 4); printf("���  �������������");
    gotoxy(x + 5, y + 2); printf("T E T R I S");
    gotoxy(x, y + 7); printf("Please Enter Any Key to Start..");
    gotoxy(x, y + 9); printf("  ��   : Shift");
    gotoxy(x, y + 10); printf("��  �� : Left / Right");
    gotoxy(x, y + 11); printf("  ��   : Soft Drop");
    gotoxy(x, y + 12); printf(" SPACE : Hard Drop");
    gotoxy(x, y + 13); printf("   P   : Pause");
    gotoxy(x, y + 14); printf("  ESC  : Quit");
    gotoxy(x, y + 16); printf("BONUS FOR HARD DROPS / COMBOS");

    //for (cnt = 0;; cnt++) { //cnt�� 1�� ������Ű�鼭 ��� �ݺ�    //�ϳ��� ���߿��� �� ��¦�̴� �ִϸ��̼�ȿ��
    //    if (cnt % 200 == 0) { gotoxy(x + 4, y + 1); printf("��"); }       //cnt�� 200���� ������ �������� ���� ǥ��
    //    if ((cnt % 200 - 100) == 0) { gotoxy(x + 4, y + 1); printf("  "); } //�� ī��Ʈ���� 100ī��Ʈ �������� ���� ����
    //    if ((cnt % 350) == 0) { gotoxy(x + 13, y + 2); printf("��"); } //������ ������ �ð����� ����
    //    if ((cnt % 350 - 100) == 0) { gotoxy(x + 13, y + 2); printf("  "); }
    //}

    if (kbhit()) {
        while (kbhit()) getch(); //���ۿ� ��ϵ� Ű���� ����
        m_pGameClient->ChangeScene(Scene::SceneNum::GamePlay);
    }; //Ű�Է��� ���� �÷��� ������
    while (kbhit()) getch(); //���ۿ� ��ϵ� Ű���� ����

}