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
	check_key(); //Ű�Է�Ȯ��
	WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
	// KeyUpdate(fTimeElapsed);

	//if (m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on && check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false) {
	//	// ����� �浹���� �� �ణ�� �߰� �ð��� �ο����ִ� �κ��ε� ������ ������ �ȳ��� �ϵ� �������
	//	// ���ݴ� ����� ������ ���� �߰��ϰų� �ƿ� �����ϴ� ������ �� ����
	//};
	if (m_gamestatus[m_pGameClient->m_ClientNum].flag.down_flag == 0) {
	//	drop_block(fTimeElapsed); // ����� ��ĭ ����
	}
	draw_main(); //ȭ���� �׸�
	// check_level_up();  // �������� üũ
	check_game_over(); // ���ӿ����� üũ
	// if (m_gamestatus[m_pGameClient->m_ClientNum].flag.new_block_on == 1) new_block(); // �� �� m_gamestatus[m_pGameClient->m_ClientNum].flag�� �ִ� ��� ���ο� �� ����
}

void GamePlayScene::Paint(HDC hDC)
{
	TextOut(hDC, 100, 500, "PLAY SCENE", 11);
}

void GamePlayScene::KeyDown(unsigned char KEYCODE)
{
}

void GamePlayScene::reset(void) {
	m_gamestatus[m_pGameClient->m_ClientNum].level = 1; //�������� �ʱ�ȭ
	m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = 0;
	m_gamestatus[m_pGameClient->m_ClientNum].speed = 1;

	system("cls"); //ȭ������
	reset_main(); // m_gamestatus[m_pGameClient->m_ClientNum].board_org�� �ʱ�ȭ
	draw_map(); // ����ȭ���� �׸�
	draw_main(); // �������� �׸�

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

void GamePlayScene::draw_map(void) { //���� ���� ǥ�ø� ��Ÿ���� �Լ�  
	int y = 3;           // m_gamestatus[m_pGameClient->m_ClientNum].level, goal, score�� �����߿� ���� �ٲ�� �� ���� �� y���� ���� �����ص� 
						 // �׷��� Ȥ�� ���� ���� ǥ�� ��ġ�� �ٲ� �� �Լ����� �ȹٲ㵵 �ǰ�
	gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL = y); printf(" LEVEL : %5d", m_gamestatus[m_pGameClient->m_ClientNum].level);
	// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL = y + 1); printf(" GOAL  : %5d", 10 - cnt);
	gotoxy(STATUS_X_ADJ, y + 2); printf("��    NEXT    ��");
	gotoxy(STATUS_X_ADJ, y + 3); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 4); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 5); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 6); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 7); printf("����������������������������");
	gotoxy(STATUS_X_ADJ, y + 9); printf("��    ITEM    ��");
	gotoxy(STATUS_X_ADJ, y + 10); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 11); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 12); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 13); printf("��            ��");
	gotoxy(STATUS_X_ADJ, y + 14); printf("����������������������������");

	/*
	gotoxy(STATUS_X_ADJ, y + 8); printf(" YOUR SCORE :");
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE = y + 9); printf("        %6d", score);
	gotoxy(STATUS_X_ADJ, y + 10); printf(" LAST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 11); printf("        %6d", last_score);
	gotoxy(STATUS_X_ADJ, y + 12); printf(" BEST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 13); printf("        %6d", best_score);
	gotoxy(STATUS_X_ADJ, y + 15); printf("  ��   : Shift        SPACE : Hard Drop");
	gotoxy(STATUS_X_ADJ, y + 16); printf("��  �� : Left / Right   P   : Pause");
	gotoxy(STATUS_X_ADJ, y + 17); printf("  ��   : Soft Drop     ESC  : Quit");
	*/
}

void GamePlayScene::draw_main(void) { //������ �׸��� �Լ� 

	for (int j = 1; j < BOARD_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷��� 
		if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] == EMPTY) m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 0; i < BOARD_Y; i++) {
		for (int j = 0; j < BOARD_X; j++) {
			if (m_gamestatus[m_pGameClient->m_ClientNum].board_cpy[i][j] != m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j]) { //cpy�� ���ؼ� ���� �޶��� �κи� ���� �׷���.
												//�̰� ������ ��������ü�� ��� �׷��� �������� ��¦�Ÿ�
				gotoxy(BOARD_X_ADJ + j, BOARD_Y_ADJ + i);
				switch (m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j]) {
				case EMPTY: //��ĭ��� 
					printf("  ");
					break;
				case CEILLING: //õ���� 
					printf(". ");
					break;
				case WALL: //����� 
					printf("��");
					break;
				case INACTIVE_BLOCK: //���� �� ���  
					printf("��");
					break;
				case ACTIVE_BLOCK: //�����̰��ִ� �� ���
					printf("��");
					break;
				}
			}
		}
	}
	for (int i = 0; i < BOARD_Y; i++) { //�������� �׸� �� m_gamestatus[m_pGameClient->m_ClientNum].board_cpy�� ����  
		for (int j = 0; j < BOARD_X; j++) {
			m_gamestatus[m_pGameClient->m_ClientNum].board_cpy[i][j] = m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j];
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
	else{
		m_keys.space = false;
		m_gamestatus[m_pGameClient->m_ClientNum].flag.space_flag = false;
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		system("cls"); //ȭ���� ����� 
		exit(0); //�������� 
	}
	while (kbhit()) getch(); //Ű���۸� ��� 
	SetEvent(hWriteEvent);
}

void GamePlayScene::KeyUpdate(float fTimeElapsed) {
	m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime += fTimeElapsed;
	if (m_keys.left == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.left_flag == false) {
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx - 1, m_gamestatus[m_pGameClient->m_ClientNum].by, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true) {
			move_block(LEFT);
			m_gamestatus[m_pGameClient->m_ClientNum].flag.left_flag = true;
			m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed = 0.2f;
			m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime = 0.0f;
		}
	}
	if (m_keys.right == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.right_flag == false) {
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx + 1, m_gamestatus[m_pGameClient->m_ClientNum].by, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true) {
			move_block(RIGHT);
			m_gamestatus[m_pGameClient->m_ClientNum].flag.right_flag = true;
			m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed = 0.2f;
			m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime = 0.0f;
		}
	}
	if (m_keys.down == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.down_flag == false) {
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true) {
			drop_block(100);
			m_gamestatus[m_pGameClient->m_ClientNum].flag.down_flag = true;
			m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed = 0.2f;
			m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime = 0.0f;
		}
	}
	if (m_keys.up == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.up_flag == false) {
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by, (m_gamestatus[m_pGameClient->m_ClientNum].b_rotation + 1) % 4) == true) { 
			move_block(UP);
		}
		//ȸ���� �� �ִ��� üũ �� �����ϸ� ȸ��
		else if (m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on == true && check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by - 1, (m_gamestatus[m_pGameClient->m_ClientNum].b_rotation + 1) % 4) == true) 
			move_block(100);
		//�ٴڿ� ���� ��� �������� ��ĭ����� ȸ���� �����ϸ� �׷��� ��(Ư������)
		m_gamestatus[m_pGameClient->m_ClientNum].flag.up_flag = true;
	}

	if (m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime >= m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed) {
		if (m_keys.left == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.left_flag == 1) {
			if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx - 1, m_gamestatus[m_pGameClient->m_ClientNum].by, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true) {
				move_block(LEFT);
				m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed = 0.05f;
				m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime = 0.0f;
			}
		}
		if (m_keys.right == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.right_flag == 1) {
			if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx + 1, m_gamestatus[m_pGameClient->m_ClientNum].by, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true) {
				move_block(RIGHT);
				m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed = 0.05f;
				m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime = 0.0f;
			}
		}
		if (m_keys.down == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.down_flag == 1) {
			drop_block(100);
			m_gamestatus[m_pGameClient->m_ClientNum].fKeyMoveSpeed = 0.05f;
			m_gamestatus[m_pGameClient->m_ClientNum].fMoveBlockTime = 0.0f;
		}
	}

	if (m_keys.space == true && m_gamestatus[m_pGameClient->m_ClientNum].flag.space_flag == 0) {
		hard_drop_block();
		score += m_gamestatus[m_pGameClient->m_ClientNum].level; // hard drop ���ʽ�
		m_gamestatus[m_pGameClient->m_ClientNum].flag.space_flag = 1;
	}
}

void GamePlayScene::drop_block(float fTimeElapsed) {
	m_gamestatus[m_pGameClient->m_ClientNum].fDropBlockTime += fTimeElapsed;
	if (m_gamestatus[m_pGameClient->m_ClientNum].fDropBlockTime >= m_gamestatus[m_pGameClient->m_ClientNum].speed) {
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false) { //���� ��������ʰ� crush m_gamestatus[m_pGameClient->m_ClientNum].flag�� ����������
			for (int i = 0; i < BOARD_Y; i++) { //���� �������� ���� ����
				for (int j = 0; j < BOARD_X; j++) {
					if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] == ACTIVE_BLOCK) 
						m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = false; //m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��
			check_line(); //����üũ�� ��
			m_gamestatus[m_pGameClient->m_ClientNum].flag.new_block_on = true; //���ο� ������ m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��
			return;
		}
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true)
			move_block(DOWN); //���� ��������� ������ ��ĭ �̵�
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false)
			m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = true; //������ �̵��� �ȵǸ�  crush m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��
		m_gamestatus[m_pGameClient->m_ClientNum].fDropBlockTime = 0.0f;
	}
	return;
}

void GamePlayScene::hard_drop_block() {
	while (1) {
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false) { //���� ��������ʰ� crush m_gamestatus[m_pGameClient->m_ClientNum].flag�� ����������
			for (int i = 0; i < BOARD_Y; i++) { //���� �������� ���� ����
				for (int j = 0; j < BOARD_X; j++) {
					if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] == ACTIVE_BLOCK)
						m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = false; //m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��
			check_line(); //����üũ�� ��
			m_gamestatus[m_pGameClient->m_ClientNum].flag.new_block_on = true; //���ο� ������ m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��
			return;
		}
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == true)
			move_block(DOWN); //���� ��������� ������ ��ĭ �̵�
		if (check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false)
			m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = true; //������ �̵��� �ȵǸ�  crush m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��
		m_gamestatus[m_pGameClient->m_ClientNum].fDropBlockTime = 0.0f;
	}
}

int GamePlayScene::check_crush(int bx, int by, int b_rotation) { //������ ��ǥ�� ȸ�������� �浹�� �ִ��� �˻� 
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) { //������ ��ġ�� �����ǰ� ������� ���ؼ� ��ġ�� false�� ���� 
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][b_rotation][i][j] == 1 && m_gamestatus[m_pGameClient->m_ClientNum].board_org[by + i][bx + j] > 0) return false;
		}
	}
	return true; //�ϳ��� �Ȱ�ġ�� true���� 
};

void GamePlayScene::move_block(int dir) { //����� �̵���Ŵ 
	int i, j;

	switch (dir) {
	case LEFT: //���ʹ��� 
		for (i = 0; i < 4; i++) { //������ǥ�� ���� ���� 
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) { //�������� ��ĭ���� active block�� ���� 
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j - 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus[m_pGameClient->m_ClientNum].bx--; //��ǥ�� �̵� 
		break;

	case RIGHT:    //������ ����. ���ʹ����̶� ���� ������ ���� 
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j + 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus[m_pGameClient->m_ClientNum].bx++;
		break;

	case DOWN:    //�Ʒ��� ����. ���ʹ����̶� ���� ������ ����
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i + 1][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus[m_pGameClient->m_ClientNum].by++;
		break;

	case UP: //Ű���� ���� �������� ȸ����Ŵ. 
		for (i = 0; i < 4; i++) { //������ǥ�� ���� ����  
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = EMPTY;
			}
		}
		m_gamestatus[m_pGameClient->m_ClientNum].b_rotation = (m_gamestatus[m_pGameClient->m_ClientNum].b_rotation + 1) % 4; //ȸ������ 1������Ŵ(3���� 4�� �Ǵ� ���� 0���� �ǵ���) 
		for (i = 0; i < 4; i++) { //ȸ���� ����� ���� 
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case 100: //����� �ٴ�, Ȥ�� �ٸ� ��ϰ� ���� ���¿��� ��ĭ���� �÷� ȸ���� ������ ��� 
			  //�̸� ���۽�Ű�� Ư������ 
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = EMPTY;
			}
		}
		m_gamestatus[m_pGameClient->m_ClientNum].b_rotation = (m_gamestatus[m_pGameClient->m_ClientNum].b_rotation + 1) % 4;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i - 1][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus[m_pGameClient->m_ClientNum].by--;
		break;
	}
}

void GamePlayScene::check_line(void) {
	int i, j, k, l;

	int    block_amount; //������ ��ϰ����� �����ϴ� ���� 
	int combo = 0; //�޺����� �����ϴ� ���� ������ �ʱ�ȭ 

	for (i = BOARD_Y - 2; i > 3;) { //i=MAIN_Y-2 : ���ʺ��� ��ĭ����,  i>3 : õ��(3)�Ʒ����� �˻� 
		block_amount = 0; //��ϰ��� ���� ���� �ʱ�ȭ 
		for (j = 1; j < BOARD_X - 1; j++) { //���� �������� ��ϰ��縦 �� 
			if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] > 0) block_amount++;
		}
		if (block_amount == BOARD_X - 2) { //����� ���� �� ��� 
			if (m_gamestatus[m_pGameClient->m_ClientNum].flag.level_up_on == 0) { //���������°� �ƴ� ��쿡(�������� �Ǹ� �ڵ� �ٻ����� ����) 
				score += 100 * m_gamestatus[m_pGameClient->m_ClientNum].level; //�����߰� 
				// cnt++; //���� �� ���� ī��Ʈ ���� 
				combo++; //�޺��� ����  
			}
			for (k = i; k > 1; k--) { //������ ��ĭ�� ��� ����(������ õ���� �ƴ� ��쿡��) 
				for (l = 1; l < BOARD_X - 1; l++) {
					if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[k - 1][l] != CEILLING) m_gamestatus[m_pGameClient->m_ClientNum].board_org[k][l] = m_gamestatus[m_pGameClient->m_ClientNum].board_org[k - 1][l];
					if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[k - 1][l] == CEILLING) m_gamestatus[m_pGameClient->m_ClientNum].board_org[k][l] = EMPTY;
					//������ õ���� ��쿡�� õ���� ��ĭ ������ �ȵǴϱ� ��ĭ�� ���� 
				}
			}
		}
		else i--;
	}
	if (combo) { //�� ������ �ִ� ��� ������ ���� ��ǥ�� ���� ǥ����  
		if (combo > 1) { //2�޺��̻��� ��� ��� ���ʽ��� �޼����� �����ǿ� ����ٰ� ���� 
			gotoxy(BOARD_X_ADJ + (BOARD_X / 2) - 1, BOARD_Y_ADJ + m_gamestatus[m_pGameClient->m_ClientNum].by - 2); printf("%d COMBO!", combo);
			// Sleep(500);
			// score += (combo * m_gamestatus[m_pGameClient->m_ClientNum].level * 100);
			reset_main_cpy(); //�ؽ�Ʈ�� ����� ���� m_gamestatus[m_pGameClient->m_ClientNum].board_cpy�� �ʱ�ȭ.
		//(m_gamestatus[m_pGameClient->m_ClientNum].board_cpy�� m_gamestatus[m_pGameClient->m_ClientNum].board_org�� ���� �ٸ��Ƿ� ������ draw()ȣ��� ������ ��ü�� ���� �׸��� ��) 
		}
		// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", (cnt <= 10) ? 10 - cnt : 0);
		// gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score);
	}
}

void GamePlayScene::check_level_up(void) {
	int i, j;

	if (cnt >= 10) { //�������� 10�پ� ���־���. 10���̻� ���� ��� 
		draw_main();
		m_gamestatus[m_pGameClient->m_ClientNum].flag.level_up_on = 1; //������ m_gamestatus[m_pGameClient->m_ClientNum].flag�� ���
		m_gamestatus[m_pGameClient->m_ClientNum].level += 1; //������ 1 �ø�
		cnt = 0; //���� �ټ� �ʱ�ȭ  

		reset_main_cpy(); //�ؽ�Ʈ�� ����� ���� m_gamestatus[m_pGameClient->m_ClientNum].board_cpy�� �ʱ�ȭ.
		//(m_gamestatus[m_pGameClient->m_ClientNum].board_cpy�� m_gamestatus[m_pGameClient->m_ClientNum].board_org�� ���� �ٸ��Ƿ� ������ draw()ȣ��� ������ ��ü�� ���� �׸��� ��) 

		//.check_line()�Լ� ���ο��� m_gamestatus[m_pGameClient->m_ClientNum].level up m_gamestatus[m_pGameClient->m_ClientNum].flag�� �����ִ� ��� ������ ����.         
		switch (m_gamestatus[m_pGameClient->m_ClientNum].level) { //�������� �ӵ��� ��������. 
		case 2:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.9;
			break;
		case 3:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.8;
			break;
		case 4:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.7;
			break;
		case 5:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.6;
			break;
		case 6:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.5;
			break;
		case 7:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.4;
			break;
		case 8:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.3;
			break;
		case 9:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.2;
			break;
		case 10:
			m_gamestatus[m_pGameClient->m_ClientNum].speed = 0.1;
			break;
		}
		m_gamestatus[m_pGameClient->m_ClientNum].flag.level_up_on = 0; //������ m_gamestatus[m_pGameClient->m_ClientNum].flag����

		gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL); printf(" LEVEL : %5d", m_gamestatus[m_pGameClient->m_ClientNum].level); //����ǥ�� 
		// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", 10 - cnt); // ������ǥ ǥ�� 

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
		err_display("recv()");
	}
	len = ntohl(len);
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&m_pGameClient->m_ClientNum, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}

	reset_main(); // m_gamestatus[m_pGameClient->m_ClientNum].board_org�� �ʱ�ȭ
	m_gamestatus[m_pGameClient->m_ClientNum].level = 1; //�������� �ʱ�ȭ
	m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = 0;
	m_gamestatus[m_pGameClient->m_ClientNum].speed = 1;
	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //�������� ���� ��� ������ �����ϰ� ����
	new_block(); //���ο� ����� �ϳ� ����

	len = htonl(sizeof(Gamestatus) * MAX_PLAYER);
	// �ʱ� ���� ������ ������
	retval = send(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	retval = send(m_pGameClient->GetSOCKET(), (char*)&m_gamestatus, sizeof(Gamestatus) * MAX_PLAYER, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// �̺�Ʈ ����
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) {
		exit(1);
	}
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) {
		exit(1);
	}

	setcursortype(NOCURSOR); //Ŀ�� ����
	draw_map(); // ����ȭ���� �׸�
	draw_main(); // �������� �׸�


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
			err_display("recv()");
			break;
		}
		len = ntohl(len);
		retval = recvn(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&pGamePlayScene->m_gamestatus, sizeof(Gamestatus) * MAX_PLAYER, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}

		KeyInput keys = pGamePlayScene->m_keys;
		len = htonl(sizeof(KeyInput));
		retval = send(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		retval = send(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&keys, sizeof(KeyInput), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		SetEvent(hReadEvent); // �б� �Ϸ� �˸���
	}
	return 0;
}

void GamePlayScene::PlaySceneSend() {
	int retval;
	int len = 0;
	int Msg = 0;

	int MSG_len = htonl(sizeof(KeyInput));

	retval = send(m_pGameClient->GetSOCKET(), (char*)&MSG_len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	retval = send(m_pGameClient->GetSOCKET(), (char*)&m_keys, sizeof(KeyInput), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
}