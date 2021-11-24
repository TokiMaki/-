#include "Scene.h"
#include "GamePlayScene.h"
#include "GameClient.h"
#include "stdafx.h"
#include "socket_function.h"

HANDLE hReadEvent, hWriteEvent; // 이벤트

GamePlayScene::GamePlayScene() {}
GamePlayScene::GamePlayScene(SceneNum num, GameClient* const pGameClient) {
	m_SceneNum = num;
	m_pGameClient = pGameClient;
}
GamePlayScene::~GamePlayScene() {}

void GamePlayScene::Update(float fTimeElapsed) {
	WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
	check_key(); //키입력확인
	//if (m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on && check_crush(m_gamestatus[m_pGameClient->m_ClientNum].bx, m_gamestatus[m_pGameClient->m_ClientNum].by + 1, m_gamestatus[m_pGameClient->m_ClientNum].b_rotation) == false) {
	//	// 블록이 충돌했을 때 약간의 추가 시간을 부여해주는 부분인데 로직이 생각이 안나서 일딴 비워놓음
	//	// 조금더 충분히 생각해 보고 추가하거나 아예 삭제하는 쪽으로 할 예정
	//};
	draw_main(); //화면을 그림
	SetEvent(hWriteEvent);
}

void GamePlayScene::reset(void) {
	m_gamestatus[m_pGameClient->m_ClientNum].level = 1; //각종변수 초기화
	m_gamestatus[m_pGameClient->m_ClientNum].flag.crush_on = 0;
	m_gamestatus[m_pGameClient->m_ClientNum].speed = 1;

	system("cls"); //화면지움
	reset_main(); // m_gamestatus[m_pGameClient->m_ClientNum].board_org를 초기화
	draw_map(); // 게임화면을 그림
	draw_main(); // 게임판을 그림

	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //다음번에 나올 블록 종류를 랜덤하게 생성
	new_block(); //새로운 블록을 하나 만듦
}

void GamePlayScene::reset_main(void) { //게임판을 초기화

	for (int i = 0; i < BOARD_Y; i++) { // 게임판을 0으로 초기화
		for (int j = 0; j < BOARD_X; j++) {
			m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][j] = 0;
			m_gamestatus[m_pGameClient->m_ClientNum].board_cpy[i][j] = 100;
		}
	}
	for (int j = 1; j < BOARD_X; j++) { //y값이 3인 위치에 천장을 만듦
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 1; i < BOARD_Y - 1; i++) { //좌우 벽을 만듦
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][0] = WALL;
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[i][BOARD_X - 1] = WALL;
	}
	for (int j = 0; j < BOARD_X; j++) { //바닥벽을 만듦 
		m_gamestatus[m_pGameClient->m_ClientNum].board_org[BOARD_Y - 1][j] = WALL;
	}
}

void GamePlayScene::reset_main_cpy(void) { //m_gamestatus[m_pGameClient->m_ClientNum].board_cpy를 초기화 

	for (int i = 0; i < BOARD_Y; i++) {         //게임판에 게임에 사용되지 않는 숫자를 넣음 
		for (int j = 0; j < BOARD_X; j++) {  //이는 m_gamestatus[m_pGameClient->m_ClientNum].board_org와 같은 숫자가 없게 하기 위함 
			m_gamestatus[m_pGameClient->m_ClientNum].board_cpy[i][j] = 100;
		}
	}
}

void GamePlayScene::draw_map(void) { //게임 상태 표시를 나타내는 함수  
	int y = 3;           // m_gamestatus[m_pGameClient->m_ClientNum].level, goal, score만 게임중에 값이 바뀔수 도 있음 그 y값을 따로 저장해둠 
						 // 그래서 혹시 게임 상태 표시 위치가 바뀌어도 그 함수에서 안바꿔도 되게
	// gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL = y); printf(" LEVEL : %5d", m_gamestatus[m_pGameClient->m_ClientNum].level);
	// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL = y + 1); printf(" GOAL  : %5d", 10 - cnt);
	gotoxy(STATUS_X_ADJ, y + 2); printf("┌    NEXT    ┐");
	gotoxy(STATUS_X_ADJ, y + 3); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 4); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 5); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 6); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 7); printf("└────────────┘");
	gotoxy(STATUS_X_ADJ, y + 9); printf("┌    ITEM    ┐");
	gotoxy(STATUS_X_ADJ, y + 10); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 11); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 12); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 13); printf("│            │");
	gotoxy(STATUS_X_ADJ, y + 14); printf("└────────────┘");
	
}

void GamePlayScene::draw_main(void) { //게임판 그리는 함수
	// 나를 제외한 인원 몇명째 그릴것인지에 대한 변수
	int DrawPlayers = 0;

	for (int j = 1; j < BOARD_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
		if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] == EMPTY)
			m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (i != m_pGameClient->m_ClientNum)
			DrawPlayers++;
		for (int j = 0; j < BOARD_Y; j++) {
			for (int k = 0; k < BOARD_X; k++) {
				if (i == m_pGameClient->m_ClientNum) {
					if (m_gamestatus[i].board_cpy[j][k] != m_gamestatus[i].board_org[j][k]) { //cpy랑 비교해서 값이 달라진 부분만 새로 그려줌.
						//이게 없으면 게임판전체를 계속 그려서 느려지고 반짝거림
						gotoxy(BOARD_X_ADJ + k, BOARD_Y_ADJ + j);
						switch (m_gamestatus[m_pGameClient->m_ClientNum].board_org[j][k]) {
						case EMPTY: //빈칸모양 
							printf("  ");
							break;
						case CEILLING: //천장모양 
							printf(". ");
							break;
						case WALL: //벽모양 
							printf("▩");
							break;
						case INACTIVE_BLOCK: //굳은 블럭 모양  
							printf("□");
							break;
						case ACTIVE_BLOCK: //움직이고있는 블럭 모양
							printf("■");
							break;
						}
					}
				}
				else if (m_gamestatus[i].board_cpy[j][k] != m_gamestatus[i].board_org[j][k]) { //cpy랑 비교해서 값이 달라진 부분만 새로 그려줌.
						//이게 없으면 게임판전체를 계속 그려서 느려지고 반짝거림
						gotoxy(BOARD_X_ADJ + BOARD_X * DrawPlayers + 8 + k, BOARD_Y_ADJ + j);
						switch (m_gamestatus[i].board_org[j][k]) {
						case EMPTY: //빈칸모양 
							printf("  ");
							break;
						case CEILLING: //천장모양 
							printf(". ");
							break;
						case WALL: //벽모양 
							printf("▩");
							break;
						case INACTIVE_BLOCK: //굳은 블럭 모양  
							printf("□");
							break;
						case ACTIVE_BLOCK: //움직이고있는 블럭 모양
							printf("■");
							break;
						}
					}
				
			}
		}
	}
	for (int i = 1; i < 3; i++) { //게임상태표시에 다음에 나올블럭을 그림 
		for (int j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][0][i][j] == 1) {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("■");
			}
			else {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("  ");
			}
		}
	}
	for (int i = 0; i < MAX_PLAYER; ++i) {
		for (int j = 0; j < BOARD_Y; j++) { //게임판을 그린 후 m_gamestatus[m_pGameClient->m_ClientNum].board_cpy에 복사
			for (int k = 0; k < BOARD_X; k++) {
				m_gamestatus[i].board_cpy[j][k] = m_gamestatus[i].board_org[j][k];
			}
		}
	}
}

void GamePlayScene::new_block(void) { //새로운 블록 생성  
	int i, j;

	m_gamestatus[m_pGameClient->m_ClientNum].bx = (BOARD_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데) 
	m_gamestatus[m_pGameClient->m_ClientNum].by = 0;  //블록 생성위치 y좌표(제일 위) 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type = m_gamestatus[m_pGameClient->m_ClientNum].b_type_next; //다음블럭값을 가져옴 
	m_gamestatus[m_pGameClient->m_ClientNum].b_type_next = rand() % 7; //다음 블럭을 만듦 
	m_gamestatus[m_pGameClient->m_ClientNum].b_rotation = 0;  //회전은 0번으로 가져옴 

	m_gamestatus[m_pGameClient->m_ClientNum].flag.new_block_on = 0; //new_block m_gamestatus[m_pGameClient->m_ClientNum].flag를 끔  

	for (i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성  
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type][m_gamestatus[m_pGameClient->m_ClientNum].b_rotation][i][j] == 1) m_gamestatus[m_pGameClient->m_ClientNum].board_org[m_gamestatus[m_pGameClient->m_ClientNum].by + i][m_gamestatus[m_pGameClient->m_ClientNum].bx + j] = ACTIVE_BLOCK;
		}
	}
	for (i = 1; i < 3; i++) { //게임상태표시에 다음에 나올블럭을 그림 
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus[m_pGameClient->m_ClientNum].b_type_next][0][i][j] == 1) {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("■");
			}
			else {
				gotoxy(STATUS_X_ADJ + 2 + j, i + 6);
				printf("  ");
			}
		}
	}
}

void GamePlayScene::check_key() {
	// 왼쪽키 트리거
	if (GetAsyncKeyState(VK_LEFT)) {
		m_keys.left = true;
	}
	else {
		m_keys.left = false;
		m_gamestatus[m_pGameClient->m_ClientNum].flag.left_flag = false;
	}

	// 오른쪽키 트리거
	if (GetAsyncKeyState(VK_RIGHT)) {
		m_keys.right = true;
	}
	else {
		m_keys.right = false;
		m_gamestatus[m_pGameClient->m_ClientNum].flag.right_flag = false;
	}

	// 아래키 트리거
	if (GetAsyncKeyState(VK_DOWN)) {
		m_keys.down = true;
	}
	else {
		m_keys.down = false;
		m_gamestatus[m_pGameClient->m_ClientNum].flag.down_flag = false;
	}

	// 위키 트리거
	if (GetAsyncKeyState(VK_UP)) {
		m_keys.up = true;
	}
	else {
		m_keys.up = false;
		m_gamestatus[m_pGameClient->m_ClientNum].flag.up_flag = false;
	}

	// 스페이스키 트리거
	if (GetAsyncKeyState(VK_SPACE)) {
		m_keys.space = true;
	}
	else{
		m_keys.space = false;
		m_gamestatus[m_pGameClient->m_ClientNum].flag.space_flag = false;
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		system("cls"); //화면을 지우고 
		exit(0); //게임종료 
	}
	while (kbhit()) getch(); //키버퍼를 비움 
}

void GamePlayScene::check_game_over(void) {
	int i;

	int x = 5;
	int y = 5;

	for (i = 1; i < BOARD_X - 2; i++) {
		if (m_gamestatus[m_pGameClient->m_ClientNum].board_org[CEILLING_Y][i] > 0) { //천장(위에서 세번째 줄)에 inactive가 생성되면 게임 오버 
			gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤"); //게임오버 메세지
			gotoxy(x, y + 1); printf("▤                              ▤");
			gotoxy(x, y + 2); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 3); printf("▤  |  G A M E  O V E R..   |   ▤");
			gotoxy(x, y + 4); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 5); printf("▤   YOUR SCORE: %6d         ▤", score);
			gotoxy(x, y + 6); printf("▤                              ▤");
			gotoxy(x, y + 7); printf("▤  Press any key to restart..  ▤");
			gotoxy(x, y + 8); printf("▤                              ▤");
			gotoxy(x, y + 9); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");

			while (kbhit()) getch();
			key = getch();
			reset();
		}
	}
}

void GamePlayScene::InitScene() {
	srand((unsigned)time(NULL)); //난수표생성
	// reset(); //게임판 리셋

	int retval;
	int len = 0;

	// 내가 몇번인지 확인
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}
	len = ntohl(len);
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&m_pGameClient->m_ClientNum, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
	}

	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hReadEvent == NULL) {
		exit(1);
	}
	hWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hWriteEvent == NULL) {
		exit(1);
	}

	setcursortype(NOCURSOR); //커서 없앰
	draw_map(); // 게임화면을 그림

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

		SetEvent(hReadEvent); // 읽기 완료 알리기
	}
	return 0;
}

//void GamePlayScene::PlaySceneSend() {
//	int retval;
//	int len = 0;
//	int Msg = 0;
//
//	int MSG_len = htonl(sizeof(KeyInput));
//
//	retval = send(m_pGameClient->GetSOCKET(), (char*)&MSG_len, sizeof(int), 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("send()");
//	}
//	retval = send(m_pGameClient->GetSOCKET(), (char*)&m_keys, sizeof(KeyInput), 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("send()");
//	}
//}