#include "Scene.h"
#include "GamePlayScene.h"
#include "GameClient.h"
#include "stdafx.h"
#include "socket_function.h"

HANDLE hReadEvent, hWriteEvent; // �̺�Ʈ

GamePlayScene::GamePlayScene() {}
GamePlayScene::GamePlayScene(SceneNum num, GameClient* const pGameClient) {
	m_SceneNum = num;
	m_pGameClient = pGameClient;
}
GamePlayScene::~GamePlayScene() {}

void GamePlayScene::Update(float fTimeElapsed) {
	WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
	check_key(); //Ű�Է�Ȯ��
	//if (m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on && check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false) {
	//	// ����� �浹���� �� �ణ�� �߰� �ð��� �ο����ִ� �κ��ε� ������ ������ �ȳ��� �ϵ� �������
	//	// ���ݴ� ����� ������ ���� �߰��ϰų� �ƿ� �����ϴ� ������ �� ����
	//};
	SetEvent(hWriteEvent);
}

void GamePlayScene::Paint(HDC hDC)
{
	SetBkMode(hDC, TRANSPARENT);
	SetTextColor(hDC, RGB(0, 0, 0));
	draw_main(hDC);
	draw_map(hDC);
}

void GamePlayScene::KeyDown(unsigned char KEYCODE)
{
}

void GamePlayScene::reset(void) {
	m_gamestatus[m_pGameClient->m_ClientNum].level = 1; //�������� �ʱ�ȭ
	m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = 0;
	m_gamestatus[m_pGameClient->m_ClientNum].speed = 1;

	//system("cls"); //ȭ������
	reset_main(); // m_gamestatus[m_pGameClient->m_ClientNum].board_org�� �ʱ�ȭ
	//draw_map(); // ����ȭ���� �׸�
	//draw_main(); // �������� �׸�

	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //�������� ���� ��� ������ �����ϰ� ����
	new_block(); //���ο� ����� �ϳ� ����
}

void GamePlayScene::reset_main(void) { //�������� �ʱ�ȭ

	for (int i = 0; i < BOARD_Y; i++) { // �������� 0���� �ʱ�ȭ
		for (int j = 0; j < BOARD_X; j++) {
			m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] = 0;
			m_gamestatus[m_pGameClient->m_ClientNum].board_cpy[i][j] = 100;
		}
	}
	for (int j = 1; j < BOARD_X; j++) { //y���� 3�� ��ġ�� õ���� ����
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 1; i < BOARD_Y - 1; i++) { //�¿� ���� ����
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][0] = WALL;
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][BOARD_X - 1] = WALL;
	}
	for (int j = 0; j < BOARD_X; j++) { //�ٴں��� ���� 
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[BOARD_Y - 1][j] = WALL;
	}
}

void GamePlayScene::reset_main_cpy(void) { //m_gamestatus[m_pGameClient->m_ClientNum].board_cpy�� �ʱ�ȭ 

	for (int i = 0; i < BOARD_Y; i++) {         //�����ǿ� ���ӿ� ������ �ʴ� ���ڸ� ���� 
		for (int j = 0; j < BOARD_X; j++) {  //�̴� m_gamestatus[m_pGameClient->m_ClientNum].board_org�� ���� ���ڰ� ���� �ϱ� ���� 
			m_gamestatus[m_pGameClient->m_ClientNum].board_cpy[i][j] = 100;
		}
	}
}

void GamePlayScene::draw_map(HDC hDC) { //���� ���� ǥ�ø� ��Ÿ���� �Լ�
	int x = 360;
	int y = 130;

	TextOut(hDC, x + 48, y, "NEXT", 4);
	Rectangle(hDC, x+0, y+20, x + 130, y+120);

	if (m_pGameClient->m_ClientNum != -1) {
		for (int i = 1; i < 3; i++) { //���ӻ���ǥ�ÿ� ������ ���ú��� �׸� 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][0][i][j] == 1) {
					
					//gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
					//printf("��");
					TextOut(hDC, x+23 + 20 * i, y+40 + j * 20, "��", 2);
				}
				else {
					//gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
					//printf("  ");
					TextOut(hDC, x+23 + 20 * i, y+40 + 20 * j, "  ", 2);
				}
			}
		}
	}
	

	TextOut(hDC, x + 48, y+140, "ITEM", 4);
	Rectangle(hDC, x + 0, y + 160, x + 130, y + 260);
}

void GamePlayScene::draw_main(HDC hDC) { //������ �׸��� �Լ�
	// ���� ������ �ο� ���° �׸��������� ���� ����
	int DrawPlayers = 0;

	for (int j = 1; j < BOARD_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷���
		if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] == EMPTY)
			m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (i != m_pGameClient->m_ClientNum)
			DrawPlayers++;
		for (int j = 0; j < BOARD_Y; j++) {
			for (int k = 0; k < BOARD_X; k++) {
				if (i == m_pGameClient->m_ClientNum) {
					int x = BOARD_X_ADJ + k;
					int y = BOARD_Y_ADJ + j;
					switch (m_gamestatus[m_pGameClient->m_ClientNum].board_org[j][k]) {
					case EMPTY: //��ĭ��� 
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "  ", 2);
						break;
					case CEILLING: //õ���� 
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, ". ", 2);
						break;
					case WALL: //����� 
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
						break;
					case INACTIVE_BLOCK: //���� �� ���  
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
						break;
					case ACTIVE_BLOCK: //�����̰��ִ� �� ���
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
						break;
					}
				}
				else{
					int x = BOARD_X_ADJ + BOARD_X * DrawPlayers + 8 + k;
					int y = BOARD_Y_ADJ + j;
					switch (m_gamestatus[i].board_org[j][k]) {
					case EMPTY: //��ĭ��� 
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "  ", 2);
						break;
					case CEILLING: //õ���� 
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, ". ", 2);
						break;
					case WALL: //����� 
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
						break;
					case INACTIVE_BLOCK: //���� �� ���  
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
						break;
					case ACTIVE_BLOCK: //�����̰��ִ� �� ���
						TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
						break;
					}
				}
			}
		}
	}
}

void GamePlayScene::new_block(void) { //���ο� ��� ����  
	int i, j;

	m_gamestatus[m_pGameClient->m_ClientNum].bx = (BOARD_X / 2) - 1; //��� ���� ��ġx��ǥ(�������� ���) 
	m_gamestatus[m_pGameClient->m_ClientNum].by = 0;  //��� ������ġ y��ǥ(���� ��) 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type = m_gamestatus[m_pGameClient->m_ClientNum].b_type_next; //���������� ������ 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //���� ���� ���� 
	m_gamestatus[m_pGameClient->m_ClientNum].b_rotation = 0;  //ȸ���� 0������ ������ 

	m_gamestatus[m_pGameClient->m_ClientNum].flag.new_block_on = 0; //new_block m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��  

	for (i = 0; i < 4; i++) { //������ bx, by��ġ�� ������  
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
		}
	}
	for (i = 1; i < 3; i++) { //���ӻ���ǥ�ÿ� ������ ���ú��� �׸� 
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][0][i][j] == 1) {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("��");
			}
			else {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("  ");
			}
		}
	}
}

void GamePlayScene::check_key() {
	// ����Ű Ʈ����
	if (m_gamestatus[m_pGameClient->m_ClientNum].flag.gameover_flag == 0) {
		if (GetAsyncKeyState(VK_LEFT)) {
			m_keys.left = true;
		}
		else {
			m_keys.left = false;
			m_gamestatus[m_pGameClient->m_ClientNum].flag.left_flag = false;
		}

		// ������Ű Ʈ����
		if (GetAsyncKeyState(VK_RIGHT)) {
			m_keys.right = true;
		}
		else {
			m_keys.right = false;
			m_gamestatus[m_pGameClient->m_ClientNum].flag.right_flag = false;
		}

		// �Ʒ�Ű Ʈ����
		if (GetAsyncKeyState(VK_DOWN)) {
			m_keys.down = true;
		}
		else {
			m_keys.down = false;
			m_gamestatus[m_pGameClient->m_ClientNum].flag.down_flag = false;
		}

		// ��Ű Ʈ����
		if (GetAsyncKeyState(VK_UP)) {
			m_keys.up = true;
		}
		else {
			m_keys.up = false;
			m_gamestatus[m_pGameClient->m_ClientNum].flag.up_flag = false;
		}

		// �����̽�Ű Ʈ����
		if (GetAsyncKeyState(VK_SPACE)) {
			m_keys.space = true;
		}
		else {
			m_keys.space = false;
			m_gamestatus[m_pGameClient->m_ClientNum].flag.space_flag = false;
		}
		if (GetAsyncKeyState(VK_ESCAPE)) {
			system("cls"); //ȭ���� ����� 
			exit(0); //�������� 
		}
	}
}

void GamePlayScene::check_game_over(void) {
	int i;

	int x = 5;
	int y = 5;

	for (i = 1; i < BOARD_X - 2; i++) {
		if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][i] > 0) { //õ��(������ ����° ��)�� inactive�� �����Ǹ� ���� ���� 
			gotoxy(x, y + 0); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�"); //���ӿ��� �޼���
			gotoxy(x, y + 1); printf("��                              ��");
			gotoxy(x, y + 2); printf("��  +-----------------------+   ��");
			gotoxy(x, y + 3); printf("��  |  G A M E  O V E R..   |   ��");
			gotoxy(x, y + 4); printf("��  +-----------------------+   ��");
			gotoxy(x, y + 5); printf("��   YOUR SCORE: %6d         ��", score);
			gotoxy(x, y + 6); printf("��                              ��");
			gotoxy(x, y + 7); printf("��  Press any key to restart..  ��");
			gotoxy(x, y + 8); printf("��                              ��");
			gotoxy(x, y + 9); printf("�ǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢǢ�");

			while (kbhit()) getch();
			key = getch();
			reset();
		}
	}
}

void GamePlayScene::InitScene() {
	srand((unsigned)time(NULL)); //����ǥ����
	// reset(); //������ ����

	int retval;
	int len = 0;

	// ���� ������� Ȯ��
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_quit("��ȣ");
	}
	len = ntohl(len);
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&m_pGameClient->m_ClientNum, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_quit("��ȣ");
	}

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hReadEvent == NULL) {
		exit(1);
	}
	hWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hWriteEvent == NULL) {
		exit(1);
	}

	//setcursortype(NOCURSOR); //Ŀ�� ����
	//draw_map(); // ����ȭ���� �׸�

	CreateThread(NULL, 0, GamePlayThread, (LPVOID)this, 0, NULL);
}


DWORD WINAPI GamePlayScene::GamePlayThread(LPVOID arg) {
	int retval;
	int len = 0;
	int Msg = 0;
	GamePlayScene* pGamePlayScene = (GamePlayScene*)arg;

	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE);

		len = 0;
		retval = recvn(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_quit("status");
			break;
		}
		len = ntohl(len);
		retval = recvn(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&pGamePlayScene->m_gamestatus, sizeof(Gamestatus) * MAX_PLAYER, 0);
		if (retval == SOCKET_ERROR) {
			err_quit("status");
			break;
		}

		KeyInput keys = pGamePlayScene->m_keys;
		len = htonl(sizeof(KeyInput));
		retval = send(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_quit("send()");
			break;
		}
		retval = send(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&keys, sizeof(KeyInput), 0);
		if (retval == SOCKET_ERROR) {
			err_quit("send()");
			break;
		}

		SetEvent(hReadEvent); // �б� �Ϸ� �˸���
	}
	return 0;
}