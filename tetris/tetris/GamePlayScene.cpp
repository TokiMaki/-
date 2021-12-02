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
	//WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
	//// check_key(); //Ű�Է�Ȯ��
	//SetEvent(hWriteEvent);
}

void GamePlayScene::ScreenRotate(HDC hDC, RECT rt)
{
	XFORM rotate;
	if (InitComplete && m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.screen_rotate_flag == 1) {
		rotate.eM11 = cos(180.0 / 180.0 * 3.141592);
		rotate.eM12 = sin(180.0 / 180.0 * 3.141592);
		rotate.eM21 = -sin(180.0 / 180.0 * 3.141592);
		rotate.eM22 = cos(180.0 / 180.0 * 3.141592);
		rotate.eDx = rt.right;
		rotate.eDy = rt.bottom;
		ModifyWorldTransform(hDC, &rotate, MWT_RIGHTMULTIPLY);
	}
	else
		ModifyWorldTransform(hDC, &rotate, MWT_IDENTITY);
}

void GamePlayScene::Paint(HDC hDC)
{
	// WaitForSingleObject(hReadEvent, INFINITE); // �б� �Ϸ� ��ٸ���
	// ȸ�� �����ϱ����� ��� ����ü


	if (InitComplete) {
		SetBkMode(hDC, TRANSPARENT);
		draw_main(hDC);
		draw_map(hDC);
	}
	// SetEvent(hWriteEvent);
}

void GamePlayScene::KeyDown(unsigned char KEYCODE)
{
	switch (KEYCODE) {
	case VK_LEFT:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.left = true;
		}
		break;
	case VK_RIGHT:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.right = true;
		}
		break;
	case VK_UP:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.up = true;
		}
		break;
	case VK_DOWN:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.down = true;
		}
		break;
	case VK_SHIFT:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.shift = true;
		}
		break;
	case VK_SPACE:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.space = true;
		}
		break;
	case VK_CONTROL:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 0) {
			m_keys.ctrl = true;
		}
		break;
	case VK_RETURN:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 1) {
			TerminateThread(hThread, 0);
			CloseHandle(hThread);
			CloseHandle(hReadEvent);
			hReadEvent = nullptr;
			CloseHandle(hWriteEvent);
			hWriteEvent = nullptr;
			closesocket(m_pGameClient->GetSOCKET());
			WSACleanup();
			m_pGameClient->ChangeScene(Scene::SceneNum::Title);
		}
		break;
	}
}

void GamePlayScene::KeyUp(unsigned char KEYCODE)
{
	switch (KEYCODE) {
	case VK_LEFT:
		m_keys.left = false;
		m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.left_flag = false;
		break;
	case VK_RIGHT:
		m_keys.right = false;
		m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.right_flag = false;
		break;
	case VK_UP:
		m_keys.up = false;
		m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.up_flag = false;
		break;
	case VK_DOWN:
		m_keys.down = false;
		m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.down_flag = false;
		break;
	case VK_SHIFT:
		m_keys.shift = false;
		break;
	case VK_SPACE:
		m_keys.space = false;
		m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.space_flag = false;
		break;
	}
}

void GamePlayScene::reset(void) {
	m_gamestatus[m_pGameClient->m_ClientNum].level = 1; //�������� �ʱ�ȭ
	m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.crush_on = 0;
	m_gamestatus[m_pGameClient->m_ClientNum].speed = 1;

	//system("cls"); //ȭ������
	reset_main(); // m_gamestatus[m_pGameClient->m_ClientNum].board_org�� �ʱ�ȭ
	//draw_map(); // ����ȭ���� �׸�
	//draw_main(); // �������� �׸�

	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //�������� ���� ���� ������ �����ϰ� ����
	new_block(); //���ο� ������ �ϳ� ����
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
	HDC blockDC, UIDC;
	blockDC = CreateCompatibleDC(hDC);
	UIDC = CreateCompatibleDC(hDC);
	(HBITMAP)SelectObject(blockDC, m_pGameClient->BlockBitmap);
	(HBITMAP)SelectObject(UIDC, m_pGameClient->UIBitmap);

	int x = WINDOW_WIDTH / 20 + 20 * (BOARD_X_ADJ + BOARD_X + 1);
	int y = 130;

	SetTextColor(hDC, RGB(255, 255, 255));
	TextOut(hDC, x + 48, y, "NEXT", 4);

	
	//Rectangle(hDC, x + 20, y + 20, x + 120, y + 140);
	TransparentBlt(hDC, x, y + 20, 120, 120,
		UIDC, 0, 0, 256, 256, RGB(255, 0, 255));

	if (m_pGameClient->m_ClientNum != -1) {
		for (int i = 0; i < 4; i++) { //���ӻ���ǥ�ÿ� ������ ���ú����� �׸� 
			for (int j = 0; j < 4; j++) {
				switch (m_gamestatus[m_pGameClient->m_ClientNum].b_type_next) {
				case 0:
					if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][0][i][j] == 1) {
						TransparentBlt(hDC, x + 20 + 20 * j, y + 40 + i * 20, 20, 20,
							blockDC, 32 * (m_gamestatus[m_pGameClient->m_ClientNum].b_type_next + 1), 0, 32, 32, RGB(255, 0, 255));
					}
					break;
				case 1:
					if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][1][i][j] == 1) {
						TransparentBlt(hDC, x + 30 + 20 * j, y + 40 + i * 20, 20, 20,
							blockDC, 32 * (m_gamestatus[m_pGameClient->m_ClientNum].b_type_next + 1), 0, 32, 32, RGB(255, 0, 255));
					}
					break;
				default:
					if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][0][i][j] == 1) {
						TransparentBlt(hDC, x + 30 + 20 * j, y + 40 + i * 20, 20, 20,
							blockDC, 32 * (m_gamestatus[m_pGameClient->m_ClientNum].b_type_next + 1), 0, 32, 32, RGB(255, 0, 255));
					}
					break;
				}
			}
		}
	}

	TextOut(hDC, x + 48, y + 160, "ITEM", 4);
	//Rectangle(hDC, x + 0, y + 180, x + 120, y + 300);
	TransparentBlt(hDC, x, y + 180, 120, 120,
		UIDC, 0, 0, 256, 256, RGB(255, 0, 255));

	char temp[15];
	wsprintf(temp, "TARGET : %d", m_gamestatus[m_pGameClient->m_ClientNum].target);
	TextOut(hDC, x + 30, y + 310, temp, strlen(temp));
	SetTextColor(hDC, RGB(0, 0, 0));

	DeleteDC(blockDC);
	DeleteDC(UIDC);
}

void GamePlayScene::draw_main(HDC hDC) { //������ �׸��� �Լ�
	// ���� ������ �ο� ���° �׸��������� ���� ����
	int DrawPlayers = 0;
	int x, y;
	
	HDC blockDC, UIDC;
	UIDC = CreateCompatibleDC(hDC);
	blockDC = CreateCompatibleDC(hDC);
	(HBITMAP)SelectObject(blockDC, m_pGameClient->BlockBitmap);
	(HBITMAP)SelectObject(UIDC, m_pGameClient->UIBitmap);

	HBRUSH myBrush = (HBRUSH)CreateSolidBrush(RGB(2, 29, 106));
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, myBrush);

	Rectangle(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	SelectObject(hDC, oldBrush);
	DeleteObject(myBrush);

	SetTextColor(hDC, RGB(255, 255, 255));
	// ���� �� �� �ٸ� ������� ���� �׸���
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (i != m_pGameClient->m_ClientNum)
			DrawPlayers++;
		for (int j = 0; j < BOARD_Y; j++) {
			for (int k = 0; k < BOARD_X; k++) {
				if (i == m_pGameClient->m_ClientNum) {
					x = BOARD_X_ADJ + k;
					y = BOARD_Y_ADJ + j;
				}
				else {
					x = BOARD_X_ADJ + BOARD_X * DrawPlayers + 8 + k;
					y = BOARD_Y_ADJ + j;
				}
				switch (m_gamestatus[i].board_org[j][k]) {
				case EMPTY: //��ĭ���
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 8, 0, 32, 32, RGB(255, 0, 255));
					break;
				case CEILLING: //õ����
					//TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, +WINDOW_HEIGHT / 15  + 20 * y, ". ", 2);
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 8, 0, 32, 32, RGB(255, 0, 255));
					break;
				case WALL: //����� 
					//TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + y + 20 * y, "��", 2);
	
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20, 
						blockDC, 32 * 0, 0, 32, 32, RGB(255, 0, 255));
					break;
				case INACTIVE_BLOCK: //���� ���� ���
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 9, 0, 32, 32, RGB(255, 0, 255));
					break;
				case ACTIVE_BLOCK: //�����̰��ִ� ���� ���
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * (m_gamestatus[i].b_type + 1), 0, 32, 32, RGB(255, 0, 255));
					break;
				}
			}
		}
		x = BOARD_X_ADJ + (BOARD_X / 2);
		y = BOARD_Y_ADJ + BOARD_Y + 1;
		TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - 18, WINDOW_HEIGHT / 15 + 20 * (y + 1), "My Board", 8);

		x = BOARD_X_ADJ + BOARD_X * i + 8 + (BOARD_X / 2);
		y = BOARD_Y_ADJ + BOARD_Y + 1;

		char temp[3];
		wsprintf(temp, "%d", i);
		TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * (y + 1), temp, 1);

		int tempAttackedBlock = m_gamestatus[i].AttackedBlock;
		for (int j = 0; j < tempAttackedBlock;) {
			if (i == m_pGameClient->m_ClientNum) {
				x = BOARD_X_ADJ + 1 + j;
				y = BOARD_Y_ADJ + 2;
			}
			else {
				x = BOARD_X_ADJ + BOARD_X * DrawPlayers + 1 + 8 + j;
				y = BOARD_Y_ADJ + 2;
			}
			if (tempAttackedBlock >= 10) {
				//TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, "��", 2);
				TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
					UIDC, 256 + 18, 0, 18, 18, RGB(255, 0, 255));
				j++;
				tempAttackedBlock -= 10;
			}
			else if (tempAttackedBlock < 10) {
				//TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, "��", 2);
				TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
					UIDC, 256, 0, 18, 18, RGB(255, 0, 255));
				j++;
			}
		}

		// ���� ���� Ȯ��
		if (m_gamestatus[i].m_KeyFlag.gameover_flag == 1) {
			if (i == m_pGameClient->m_ClientNum) {
				x = BOARD_X_ADJ + (BOARD_X / 2);
				y = BOARD_Y_ADJ + (BOARD_Y / 2);
			}
			else {
				x = BOARD_X_ADJ + BOARD_X * DrawPlayers + 8 + (BOARD_X / 2);
				y = BOARD_Y_ADJ + (BOARD_Y / 2);
			}
			SetTextColor(hDC, RGB(rand() % 55+200, rand() % 55+200, rand() % 55+200));
			TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - (9 * 2.7), WINDOW_HEIGHT / 15 + y + 20 * y, "Game Over", 9);
			SetTextColor(hDC, RGB(0, 0, 0));
		}
	}

	DeleteDC(blockDC);
	DeleteDC(UIDC);
}

void GamePlayScene::new_block(void) { //���ο� ���� ����  
	int i, j;

	m_gamestatus[m_pGameClient->m_ClientNum].bx = (BOARD_X / 2) - 1; //���� ���� ��ġx��ǥ(�������� ���) 
	m_gamestatus[m_pGameClient->m_ClientNum].by = 0;  //���� ������ġ y��ǥ(���� ��) 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type = m_gamestatus[m_pGameClient->m_ClientNum].b_type_next; //������������ ������ 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //���� ������ ���� 
	m_gamestatus[m_pGameClient->m_ClientNum].b_rotation = 0;  //ȸ���� 0������ ������ 

	m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.new_block_on = 0; //new_block m_gamestatus[m_pGameClient->m_ClientNum].flag�� ��  

	for (i = 0; i < 4; i++) { //������ bx, by��ġ�� ��������
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1)
				m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
		}
	}
}

void GamePlayScene::check_key() {

}

void GamePlayScene::InitScene() {
	// reset(); //������ ����
	InitComplete = false;
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

	hThread = CreateThread(NULL, 0, GamePlayThread, (LPVOID)this, 0, NULL);
}


DWORD WINAPI GamePlayScene::GamePlayThread(LPVOID arg) {
	int retval;
	int len = 0;
	int Msg = 0;
	GamePlayScene* pGamePlayScene = (GamePlayScene*)arg;

	while (1) {
		//WaitForSingleObject(hWriteEvent, INFINITE);
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

		pGamePlayScene->InitComplete = true;

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

		//pGamePlayScene->m_keys.shift = false;
		//SetEvent(hReadEvent); // �б� �Ϸ� �˸���
	}
	return 0;
}