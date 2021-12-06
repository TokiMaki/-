#include "Scene.h"
#include "GamePlayScene.h"
#include "GameClient.h"
#include "stdafx.h"
#include "socket_function.h"


GamePlayScene::GamePlayScene() {}
GamePlayScene::GamePlayScene(SceneNum num, GameClient* const pGameClient) {
	m_SceneNum = num;
	m_pGameClient = pGameClient;
}
GamePlayScene::~GamePlayScene() {}

void GamePlayScene::Update(float fTimeElapsed) {
	//WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
	//// check_key(); //키입력확인
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
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.left = true;
		}
		break;
	case VK_RIGHT:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.right = true;
		}
		break;
	case VK_UP:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.up = true;
		}
		break;
	case VK_DOWN:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.down = true;
		}
		break;
	case VK_SHIFT:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.shift = true;
		}
		break;
	case VK_SPACE:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.space = true;
		}
		break;
	case VK_CONTROL:
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 0) {
			m_keys.ctrl = true;
		}
		break;
	case VK_RETURN:
		/*if (m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.gameover_flag == 1) {
			TerminateThread(hThread, 0);
			CloseHandle(hThread);
			CloseHandle(hReadEvent);
			hReadEvent = nullptr;
			CloseHandle(hWriteEvent);
			hWriteEvent = nullptr;
			closesocket(m_pGameClient->GetSOCKET());
			WSACleanup();
			m_pGameClient->ChangeScene(Scene::SceneNum::Title);
		}*/
		if (m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.gameover_flag == 1 || m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.win_flag == 1) {
			m_keys.enter = true;
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
	case VK_CONTROL:
		m_keys.ctrl = false;
		break;
	case VK_SPACE:
		m_keys.space = false;
		m_gamestatus[m_pGameClient->m_ClientNum].m_KeyFlag.space_flag = false;
		break;
	}
}

void GamePlayScene::reset(void) {
	m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.crush_on = 0;
	m_gamestatus[m_pGameClient->m_ClientNum].speed = 1;

	//system("cls"); //화면지움
	reset_main(); // m_gamestatus[m_pGameClient->m_ClientNum].board_org를 초기화
	//draw_map(); // 게임화면을 그림
	//draw_main(); // 게임판을 그림

	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //다음번에 나올 블록 종류를 랜덤하게 생성
	new_block(); //새로운 블록을 하나 만듦
}

void GamePlayScene::reset_main(void) { //게임판을 초기화

	for (int i = 0; i < BOARD_Y; i++) { // 게임판을 0으로 초기화
		for (int j = 0; j < BOARD_X; j++) {
			m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] = 0;
		}
	}
	for (int j = 1; j < BOARD_X; j++) { //y값이 3인 위치에 천장을 만듦
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 1; i < BOARD_Y; i++) { //좌우 벽을 만듦
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][0] = WALL;
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][BOARD_X - 1] = WALL;
	}
	for (int j = 0; j < BOARD_X; j++) { //바닥벽을 만듦 
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[BOARD_Y - 1][j] = WALL;
	}
}

void GamePlayScene::draw_map(HDC hDC) { //게임 상태 표시를 나타내는 함수
	HDC blockDC, UIDC;
	blockDC = CreateCompatibleDC(hDC);
	UIDC = CreateCompatibleDC(hDC);
	(HBITMAP)SelectObject(blockDC, m_pGameClient->BlockBitmap);
	(HBITMAP)SelectObject(UIDC, m_pGameClient->UIBitmap);

	int x = WINDOW_WIDTH / 20 + 20 * (BOARD_X_ADJ + BOARD_X + 1);
	int y = 130;

	SetTextColor(hDC, RGB(255, 255, 255));
	// 다음 블록
	TextOut(hDC, x + 48, y, "NEXT", 4);
	TransparentBlt(hDC, x, y + 20, 120, 120,
		UIDC, 0, 0, 256, 256, RGB(255, 0, 255));
	if (m_pGameClient->m_ClientNum != -1) {
		for (int i = 0; i < 4; i++) { //게임상태표시에 다음에 나올블럭을 그림 
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

	// 아이템 창
	TextOut(hDC, x + 48, y + 160, "ITEM", 4);
	TransparentBlt(hDC, x, y + 180, 120, 120,
		UIDC, 0, 0, 256, 256, RGB(255, 0, 255));
	switch (m_gamestatus[m_pGameClient->m_ClientNum].item) {
	case 0:
		TransparentBlt(hDC, x + 12, y + 190, 100, 100,
			UIDC, 512, 0, 256, 256, RGB(255, 0, 255));
		break;
	case 1:
		TransparentBlt(hDC, x + 10, y + 190, 100, 100,
			UIDC, 512 + 256 + 256, 0, 256, 256, RGB(255, 0, 255));
		break;
	case 2:
		TransparentBlt(hDC, x + 10, y + 190, 100, 100,
			UIDC, 512 + 256, 0, 256, 256, RGB(255, 0, 255));
		break;
	}
	
	// 타겟 표시
	char temp[15];
	wsprintf(temp, "TARGET : %d", m_gamestatus[m_pGameClient->m_ClientNum].target);
	TextOut(hDC, x + 30, y + 310, temp, strlen(temp));

	DeleteDC(blockDC);
	DeleteDC(UIDC);
}

void GamePlayScene::draw_main(HDC hDC) {
	// 게임판 그리는 함수
	// 나를 제외한 인원 몇명째 그릴것인지에 대한 변수
	int DrawPlayers = 0;
	int x, y;
	int danger_background = 0;

	HDC blockDC, UIDC;
	UIDC = CreateCompatibleDC(hDC);
	blockDC = CreateCompatibleDC(hDC);
	(HBITMAP)SelectObject(blockDC, m_pGameClient->BlockBitmap);
	(HBITMAP)SelectObject(UIDC, m_pGameClient->UIBitmap);

	for (int j = 0; j < BOARD_Y; j++) {
		for (int k = 0; k < BOARD_X; k++) {
			if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[j][k] == INACTIVE_BLOCK) {
				danger_background++;
				break;
			}
		}
	}
	HBRUSH myBrush;
	if (danger_background >= 13) {
		myBrush = (HBRUSH)CreateSolidBrush(RGB(12.5 * danger_background + 2, 0, 106 - 5 * danger_background));
	}
	else {
		myBrush = (HBRUSH)CreateSolidBrush(RGB(2, 29, 106));
	}
	HBRUSH oldBrush = (HBRUSH)SelectObject(hDC, myBrush);

	Rectangle(hDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	SelectObject(hDC, oldBrush);
	DeleteObject(myBrush);

	HPEN myPen = (HPEN)CreatePen(PS_DOT, 1, RGB(255, 255, 255));
	HPEN oldPen = (HPEN)SelectObject(hDC, myPen);

	SetTextColor(hDC, RGB(255, 255, 255));
	// 나와 내 옆 다른 사람들의 보드 그리기
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
				case EMPTY: //빈칸모양
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 8, 0, 32, 32, RGB(255, 0, 255));
					break;
				case CEILLING: //천장모양
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 8, 0, 32, 32, RGB(255, 0, 255));
					MoveToEx(hDC, WINDOW_WIDTH / 20 + 20 * x,
						WINDOW_HEIGHT / 15 + 20 * y + 10, NULL);
					LineTo(hDC, WINDOW_WIDTH / 20 + 20 * x + 20,
						WINDOW_HEIGHT / 15 + 20 * y + 10);
					break;
				case WALL: //벽모양 
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 0, 0, 32, 32, RGB(255, 0, 255));

					break;
				case INACTIVE_BLOCK: //굳은 블럭 모양
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * 9, 0, 32, 32, RGB(255, 0, 255));
					break;
				case ACTIVE_BLOCK: //움직이고있는 블럭 모양
					TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
						blockDC, 32 * (m_gamestatus[i].b_type + 1), 0, 32, 32, RGB(255, 0, 255));
					break;
				}
			}
		}

		x = BOARD_X_ADJ + (BOARD_X / 2);
		y = BOARD_Y_ADJ + BOARD_Y + 1;
		TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - 18, WINDOW_HEIGHT / 15 + 20 * (y + 1), "My Board", 8);

		if (i != 0) {
			x = BOARD_X_ADJ + BOARD_X * i + 8 + (BOARD_X / 2);
			y = BOARD_Y_ADJ + BOARD_Y + 1;
		}
		
		if (m_gamestatus[m_pGameClient->m_ClientNum].target == i) {
			TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * (x-1), WINDOW_HEIGHT / 15 + 20 * (y-1), 60, 100,
				UIDC, 264, 197, 68, 56, RGB(255, 0, 255));
		}

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
				TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
					UIDC, 256 + 18, 0, 18, 18, RGB(255, 0, 255));
				j++;
				tempAttackedBlock -= 10;
			}
			else if (tempAttackedBlock < 10) {
				TransparentBlt(hDC, WINDOW_WIDTH / 20 + 20 * x, WINDOW_HEIGHT / 15 + 20 * y, 20, 20,
					UIDC, 256, 0, 18, 18, RGB(255, 0, 255));
				j++;
			}
		}

		// 게임 오버 확인
		if (m_gamestatus[i].m_GameFlag.gameover_flag == 1) {
			if (i == m_pGameClient->m_ClientNum) {
				x = BOARD_X_ADJ + (BOARD_X / 2);
				y = BOARD_Y_ADJ + (BOARD_Y / 2);
			}
			else {
				x = BOARD_X_ADJ + BOARD_X * DrawPlayers + 8 + (BOARD_X / 2);
				y = BOARD_Y_ADJ + (BOARD_Y / 2);
			}
			SetTextColor(hDC, RGB(rand() % 55 + 200, rand() % 55 + 200, rand() % 55 + 200));
			TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - (9 * 2.7), WINDOW_HEIGHT / 15 + y + 20 * y, "Game Over", 9);
			TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - (27 * 2.8), WINDOW_HEIGHT / 15 + y + 21 * y, "Press Enter To Return Title", 27);
			SetTextColor(hDC, RGB(0, 0, 0));
		}

		// 게임 오버 확인
		if (m_gamestatus[i].m_GameFlag.win_flag == 1) {
			if (i == m_pGameClient->m_ClientNum) {
				x = BOARD_X_ADJ + (BOARD_X / 2);
				y = BOARD_Y_ADJ + (BOARD_Y / 2);
			}
			else {
				x = BOARD_X_ADJ + BOARD_X * DrawPlayers + 8 + (BOARD_X / 2);
				y = BOARD_Y_ADJ + (BOARD_Y / 2);
			}
			SetTextColor(hDC, RGB(rand() % 55 + 200, rand() % 55 + 200, rand() % 55 + 200));
			TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - (3 * 2.7), WINDOW_HEIGHT / 15 + y + 20 * y, "Win", 3);
			TextOut(hDC, WINDOW_WIDTH / 20 + 20 * x - (27 * 2.8), WINDOW_HEIGHT / 15 + y + 21 * y, "Press Enter To Return Title", 27);
			SetTextColor(hDC, RGB(0, 0, 0));
		}
	}

	SelectObject(hDC, oldPen);
	DeleteObject(myPen);

	DeleteDC(blockDC);
	DeleteDC(UIDC);
}

void GamePlayScene::new_block(void) { //새로운 블록 생성  
	int i, j;

	m_gamestatus[m_pGameClient->m_ClientNum].bx = (BOARD_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데) 
	m_gamestatus[m_pGameClient->m_ClientNum].by = 0;  //블록 생성위치 y좌표(제일 위) 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type = m_gamestatus[m_pGameClient->m_ClientNum].b_type_next; //다음블럭값을 가져옴 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //다음 블럭을 만듦 
	m_gamestatus[m_pGameClient->m_ClientNum].b_rotation = 0;  //회전은 0번으로 가져옴 

	m_gamestatus[m_pGameClient->m_ClientNum].m_GameFlag.new_block_on = 0; //new_block m_gamestatus[m_pGameClient->m_ClientNum].flag를 끔  

	for (i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1)
				m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
		}
	}
}

void GamePlayScene::check_key() {

}

void GamePlayScene::InitScene() {
	// reset(); //게임판 리셋
	InitComplete = false;
	m_keys = { false, false, false, false, false, false, false, false };


	int retval;
	int len = 0;

	// 내가 몇번인지 확인
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_quit("번호");
	}
	len = ntohl(len);
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&m_pGameClient->m_ClientNum, len, 0);
	if (retval == SOCKET_ERROR) {
		err_quit("번호");
	}

	//// 이벤트 생성
	//hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//if (hReadEvent == NULL) {
	//	exit(1);
	//}
	//hWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	//if (hWriteEvent == NULL) {
	//	exit(1);
	//}

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
		retval = recvn(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&pGamePlayScene->m_gamestatus, len, 0);
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

		if (keys.enter) {
			pGamePlayScene->m_pGameClient->ChangeScene(Scene::SceneNum::Title);
			break;
		}
		//pGamePlayScene->m_keys.shift = false;
		//SetEvent(hReadEvent); // 읽기 완료 알리기
	}
	closesocket(pGamePlayScene->m_pGameClient->GetSOCKET());
	WSACleanup();
	return 0;
}