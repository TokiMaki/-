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
	check_key(); //키입력확인
	WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
	KeyUpdate(fTimeElapsed);

	//if (flag.crush_on && check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == false) {
	//	// 블록이 충돌했을 때 약간의 추가 시간을 부여해주는 부분인데 로직이 생각이 안나서 일딴 비워놓음
	//	// 조금더 충분히 생각해 보고 추가하거나 아예 삭제하는 쪽으로 할 예정
	//};
	if (flag.down_flag == 0) {
		drop_block(fTimeElapsed); // 블록을 한칸 내림
	}
	draw_main(); //화면을 그림
	check_level_up();  // 레벨업을 체크
	check_game_over(); // 게임오버를 체크
	if (flag.new_block_on == 1) new_block(); // 뉴 블럭 flag가 있는 경우 새로운 블럭 생성
}

void GamePlayScene::reset(void) {
	m_gamestatus.level = 1; //각종변수 초기화
	score = 0;
	level_goal = 1000;
	key = 0;
	flag.crush_on = 0;
	cnt = 0;
	m_gamestatus.speed = 1;

	system("cls"); //화면지움
	reset_main(); // m_gamestatus.board_org를 초기화
	draw_map(); // 게임화면을 그림
	draw_main(); // 게임판을 그림

	m_gamestatus.b_type_next = rand() % 7; //다음번에 나올 블록 종류를 랜덤하게 생성
	new_block(); //새로운 블록을 하나 만듦
}

void GamePlayScene::reset_main(void) { //게임판을 초기화  

	for (int i = 0; i < BOARD_Y; i++) { // 게임판을 0으로 초기화
		for (int j = 0; j < BOARD_X; j++) {
			m_gamestatus.board_org[i][j] = 0;
			m_gamestatus.board_cpy[i][j] = 100;
		}
	}
	for (int j = 1; j < BOARD_X; j++) { //y값이 3인 위치에 천장을 만듦
		m_gamestatus.board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 1; i < BOARD_Y - 1; i++) { //좌우 벽을 만듦
		m_gamestatus.board_org[i][0] = WALL;
		m_gamestatus.board_org[i][BOARD_X - 1] = WALL;
	}
	for (int j = 0; j < BOARD_X; j++) { //바닥벽을 만듦 
		m_gamestatus.board_org[BOARD_Y - 1][j] = WALL;
	}
}

void GamePlayScene::reset_main_cpy(void) { //m_gamestatus.board_cpy를 초기화 

	for (int i = 0; i < BOARD_Y; i++) {         //게임판에 게임에 사용되지 않는 숫자를 넣음 
		for (int j = 0; j < BOARD_X; j++) {  //이는 m_gamestatus.board_org와 같은 숫자가 없게 하기 위함 
			m_gamestatus.board_cpy[i][j] = 100;
		}
	}
}

void GamePlayScene::draw_map(void) { //게임 상태 표시를 나타내는 함수  
	int y = 3;           // m_gamestatus.level, goal, score만 게임중에 값이 바뀔수 도 있음 그 y값을 따로 저장해둠 
						 // 그래서 혹시 게임 상태 표시 위치가 바뀌어도 그 함수에서 안바꿔도 되게
	gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL = y); printf(" LEVEL : %5d", m_gamestatus.level);
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

	/*
	gotoxy(STATUS_X_ADJ, y + 8); printf(" YOUR SCORE :");
	gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE = y + 9); printf("        %6d", score);
	gotoxy(STATUS_X_ADJ, y + 10); printf(" LAST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 11); printf("        %6d", last_score);
	gotoxy(STATUS_X_ADJ, y + 12); printf(" BEST SCORE :");
	gotoxy(STATUS_X_ADJ, y + 13); printf("        %6d", best_score);
	gotoxy(STATUS_X_ADJ, y + 15); printf("  △   : Shift        SPACE : Hard Drop");
	gotoxy(STATUS_X_ADJ, y + 16); printf("◁  ▷ : Left / Right   P   : Pause");
	gotoxy(STATUS_X_ADJ, y + 17); printf("  ▽   : Soft Drop     ESC  : Quit");
	*/
}

void GamePlayScene::draw_main(void) { //게임판 그리는 함수 

	for (int j = 1; j < BOARD_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌 
		if (m_gamestatus.board_org[CEILLING_Y][j] == EMPTY) m_gamestatus.board_org[CEILLING_Y][j] = CEILLING;
	}
	for (int i = 0; i < BOARD_Y; i++) {
		for (int j = 0; j < BOARD_X; j++) {
			if (m_gamestatus.board_cpy[i][j] != m_gamestatus.board_org[i][j]) { //cpy랑 비교해서 값이 달라진 부분만 새로 그려줌.
												//이게 없으면 게임판전체를 계속 그려서 느려지고 반짝거림
				gotoxy(BOARD_X_ADJ + j, BOARD_Y_ADJ + i);
				switch (m_gamestatus.board_org[i][j]) {
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
	for (int i = 0; i < BOARD_Y; i++) { //게임판을 그린 후 m_gamestatus.board_cpy에 복사  
		for (int j = 0; j < BOARD_X; j++) {
			m_gamestatus.board_cpy[i][j] = m_gamestatus.board_org[i][j];
		}
	}
}

void GamePlayScene::new_block(void) { //새로운 블록 생성  
	int i, j;

	m_gamestatus.bx = (BOARD_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데) 
	m_gamestatus.by = 0;  //블록 생성위치 y좌표(제일 위) 
	m_gamestatus.b_type = m_gamestatus.b_type_next; //다음블럭값을 가져옴 
	m_gamestatus.b_type_next = rand() % 7; //다음 블럭을 만듦 
	m_gamestatus.b_rotation = 0;  //회전은 0번으로 가져옴 

	flag.new_block_on = 0; //new_block flag를 끔  

	for (i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성  
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = ACTIVE_BLOCK;
		}
	}
	for (i = 1; i < 3; i++) { //게임상태표시에 다음에 나올블럭을 그림 
		for (j = 0; j < 4; j++) {
			if (blocks[m_gamestatus.b_type_next][0][i][j] == 1) {
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
		flag.left_flag = false;
	}

	// 오른쪽키 트리거
	if (GetAsyncKeyState(VK_RIGHT)) {
		m_keys.right = true;
	}
	else {
		m_keys.right = false;
		flag.right_flag = false;
	}

	// 아래키 트리거
	if (GetAsyncKeyState(VK_DOWN)) {
		m_keys.down = true;
	}
	else {
		m_keys.down = false;
		flag.down_flag = false;
	}

	// 위키 트리거
	if (GetAsyncKeyState(VK_UP)) {
		m_keys.up = true;
	}
	else {
		m_keys.up = false;
		flag.up_flag = false;
	}

	// 스페이스키 트리거
	if (GetAsyncKeyState(VK_SPACE)) {
		m_keys.space = true;
	}
	else{
		m_keys.space = false;
		flag.space_flag = false;
	}

	if (GetAsyncKeyState(VK_ESCAPE)) {
		system("cls"); //화면을 지우고 
		exit(0); //게임종료 
	}
	while (kbhit()) getch(); //키버퍼를 비움 
	SetEvent(hWriteEvent);
}

void GamePlayScene::KeyUpdate(float fTimeElapsed) {
	m_gamestatus.fMoveBlockTime += fTimeElapsed;
	if (m_keys.left == true && flag.left_flag == false) {
		if (check_crush(m_gamestatus.bx - 1, m_gamestatus.by, m_gamestatus.b_rotation) == true) {
			move_block(LEFT);
			flag.left_flag = true;
			m_gamestatus.fKeyMoveSpeed = 0.2f;
			m_gamestatus.fMoveBlockTime = 0.0f;
		}
	}
	if (m_keys.right == true && flag.right_flag == false) {
		if (check_crush(m_gamestatus.bx + 1, m_gamestatus.by, m_gamestatus.b_rotation) == true) {
			move_block(RIGHT);
			flag.right_flag = true;
			m_gamestatus.fKeyMoveSpeed = 0.2f;
			m_gamestatus.fMoveBlockTime = 0.0f;
		}
	}
	if (m_keys.down == true && flag.down_flag == false) {
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == true) {
			drop_block(100);
			flag.down_flag = true;
			m_gamestatus.fKeyMoveSpeed = 0.2f;
			m_gamestatus.fMoveBlockTime = 0.0f;
		}
	}
	if (m_keys.up == true && flag.up_flag == false) {
		if (check_crush(m_gamestatus.bx, m_gamestatus.by, (m_gamestatus.b_rotation + 1) % 4) == true) { 
			move_block(UP);
		}
		//회전할 수 있는지 체크 후 가능하면 회전
		else if (flag.crush_on == true && check_crush(m_gamestatus.bx, m_gamestatus.by - 1, (m_gamestatus.b_rotation + 1) % 4) == true) 
			move_block(100);
		//바닥에 닿은 경우 위쪽으로 한칸띄워서 회전이 가능하면 그렇게 함(특수동작)
		flag.up_flag = true;
	}

	if (m_gamestatus.fMoveBlockTime >= m_gamestatus.fKeyMoveSpeed) {
		if (m_keys.left == true && flag.left_flag == 1) {
			if (check_crush(m_gamestatus.bx - 1, m_gamestatus.by, m_gamestatus.b_rotation) == true) {
				move_block(LEFT);
				m_gamestatus.fKeyMoveSpeed = 0.05f;
				m_gamestatus.fMoveBlockTime = 0.0f;
			}
		}
		if (m_keys.right == true && flag.right_flag == 1) {
			if (check_crush(m_gamestatus.bx + 1, m_gamestatus.by, m_gamestatus.b_rotation) == true) {
				move_block(RIGHT);
				m_gamestatus.fKeyMoveSpeed = 0.05f;
				m_gamestatus.fMoveBlockTime = 0.0f;
			}
		}
		if (m_keys.down == true && flag.down_flag == 1) {
			drop_block(100);
			m_gamestatus.fKeyMoveSpeed = 0.05f;
			m_gamestatus.fMoveBlockTime = 0.0f;
		}
	}

	if (m_keys.space == true && flag.space_flag == 0) {
		hard_drop_block();
		score += m_gamestatus.level; // hard drop 보너스
		flag.space_flag = 1;
	}
}

void GamePlayScene::drop_block(float fTimeElapsed) {
	m_gamestatus.fDropBlockTime += fTimeElapsed;
	if (m_gamestatus.fDropBlockTime >= m_gamestatus.speed) {
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면
			for (int i = 0; i < BOARD_Y; i++) { //현재 조작중인 블럭을 굳힘
				for (int j = 0; j < BOARD_X; j++) {
					if (m_gamestatus.board_org[i][j] == ACTIVE_BLOCK) 
						m_gamestatus.board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			flag.crush_on = false; //flag를 끔
			check_line(); //라인체크를 함
			flag.new_block_on = true; //새로운 블럭생성 flag를 켬
			return;
		}
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == true)
			move_block(DOWN); //밑이 비어있으면 밑으로 한칸 이동
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == false)
			flag.crush_on = true; //밑으로 이동이 안되면  crush flag를 켬
		m_gamestatus.fDropBlockTime = 0.0f;
	}
	return;
}

void GamePlayScene::hard_drop_block() {
	while (1) {
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면
			for (int i = 0; i < BOARD_Y; i++) { //현재 조작중인 블럭을 굳힘
				for (int j = 0; j < BOARD_X; j++) {
					if (m_gamestatus.board_org[i][j] == ACTIVE_BLOCK)
						m_gamestatus.board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			flag.crush_on = false; //flag를 끔
			check_line(); //라인체크를 함
			flag.new_block_on = true; //새로운 블럭생성 flag를 켬
			return;
		}
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == true)
			move_block(DOWN); //밑이 비어있으면 밑으로 한칸 이동
		if (check_crush(m_gamestatus.bx, m_gamestatus.by + 1, m_gamestatus.b_rotation) == false)
			flag.crush_on = true; //밑으로 이동이 안되면  crush flag를 켬
		m_gamestatus.fDropBlockTime = 0.0f;
	}
}

int GamePlayScene::check_crush(int bx, int by, int b_rotation) { //지정된 좌표와 회전값으로 충돌이 있는지 검사 
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) { //지정된 위치의 게임판과 블럭모양을 비교해서 겹치면 false를 리턴 
			if (blocks[m_gamestatus.b_type][b_rotation][i][j] == 1 && m_gamestatus.board_org[by + i][bx + j] > 0) return false;
		}
	}
	return true; //하나도 안겹치면 true리턴 
};

void GamePlayScene::move_block(int dir) { //블록을 이동시킴 
	int i, j;

	switch (dir) {
	case LEFT: //왼쪽방향 
		for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움 
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) { //왼쪽으로 한칸가서 active block을 찍음 
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j - 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus.bx--; //좌표값 이동 
		break;

	case RIGHT:    //오른쪽 방향. 왼쪽방향이랑 같은 원리로 동작 
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j + 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus.bx++;
		break;

	case DOWN:    //아래쪽 방향. 왼쪽방향이랑 같은 원리로 동작
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = EMPTY;
			}
		}
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i + 1][m_gamestatus.bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus.by++;
		break;

	case UP: //키보드 위쪽 눌렀을때 회전시킴. 
		for (i = 0; i < 4; i++) { //현재좌표의 블럭을 지움  
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = EMPTY;
			}
		}
		m_gamestatus.b_rotation = (m_gamestatus.b_rotation + 1) % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림) 
		for (i = 0; i < 4; i++) { //회전된 블록을 찍음 
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case 100: //블록이 바닥, 혹은 다른 블록과 닿은 상태에서 한칸위로 올려 회전이 가능한 경우 
			  //이를 동작시키는 특수동작 
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i][m_gamestatus.bx + j] = EMPTY;
			}
		}
		m_gamestatus.b_rotation = (m_gamestatus.b_rotation + 1) % 4;
		for (i = 0; i < 4; i++) {
			for (j = 0; j < 4; j++) {
				if (blocks[m_gamestatus.b_type][m_gamestatus.b_rotation][i][j] == 1) m_gamestatus.board_org[m_gamestatus.by + i - 1][m_gamestatus.bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus.by--;
		break;
	}
}

void GamePlayScene::check_line(void) {
	int i, j, k, l;

	int    block_amount; //한줄의 블록갯수를 저장하는 변수 
	int combo = 0; //콤보갯수 저장하는 변수 지정및 초기화 

	for (i = BOARD_Y - 2; i > 3;) { //i=MAIN_Y-2 : 밑쪽벽의 윗칸부터,  i>3 : 천장(3)아래까지 검사 
		block_amount = 0; //블록갯수 저장 변수 초기화 
		for (j = 1; j < BOARD_X - 1; j++) { //벽과 벽사이의 블록갯루를 셈 
			if (m_gamestatus.board_org[i][j] > 0) block_amount++;
		}
		if (block_amount == BOARD_X - 2) { //블록이 가득 찬 경우 
			if (flag.level_up_on == 0) { //레벨업상태가 아닌 경우에(레벨업이 되면 자동 줄삭제가 있음) 
				score += 100 * m_gamestatus.level; //점수추가 
				cnt++; //지운 줄 갯수 카운트 증가 
				combo++; //콤보수 증가  
			}
			for (k = i; k > 1; k--) { //윗줄을 한칸씩 모두 내림(윗줄이 천장이 아닌 경우에만) 
				for (l = 1; l < BOARD_X - 1; l++) {
					if (m_gamestatus.board_org[k - 1][l] != CEILLING) m_gamestatus.board_org[k][l] = m_gamestatus.board_org[k - 1][l];
					if (m_gamestatus.board_org[k - 1][l] == CEILLING) m_gamestatus.board_org[k][l] = EMPTY;
					//윗줄이 천장인 경우에는 천장을 한칸 내리면 안되니까 빈칸을 넣음 
				}
			}
		}
		else i--;
	}
	if (combo) { //줄 삭제가 있는 경우 점수와 레벨 목표를 새로 표시함  
		if (combo > 1) { //2콤보이상인 경우 경우 보너스및 메세지를 게임판에 띄웠다가 지움 
			gotoxy(BOARD_X_ADJ + (BOARD_X / 2) - 1, BOARD_Y_ADJ + m_gamestatus.by - 2); printf("%d COMBO!", combo);
			// Sleep(500);
			// score += (combo * m_gamestatus.level * 100);
			reset_main_cpy(); //텍스트를 지우기 위해 m_gamestatus.board_cpy을 초기화.
		//(m_gamestatus.board_cpy와 m_gamestatus.board_org가 전부 다르므로 다음번 draw()호출시 게임판 전체를 새로 그리게 됨) 
		}
		// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", (cnt <= 10) ? 10 - cnt : 0);
		// gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score);
	}
}

void GamePlayScene::check_level_up(void) {
	int i, j;

	if (cnt >= 10) { //레벨별로 10줄씩 없애야함. 10줄이상 없앤 경우 
		draw_main();
		flag.level_up_on = 1; //레벨업 flag를 띄움
		m_gamestatus.level += 1; //레벨을 1 올림
		cnt = 0; //지운 줄수 초기화  

		reset_main_cpy(); //텍스트를 지우기 위해 m_gamestatus.board_cpy을 초기화.
		//(m_gamestatus.board_cpy와 m_gamestatus.board_org가 전부 다르므로 다음번 draw()호출시 게임판 전체를 새로 그리게 됨) 

		//.check_line()함수 내부에서 m_gamestatus.level up flag가 켜져있는 경우 점수는 없음.         
		switch (m_gamestatus.level) { //레벨별로 속도를 조절해줌. 
		case 2:
			m_gamestatus.speed = 0.9;
			break;
		case 3:
			m_gamestatus.speed = 0.8;
			break;
		case 4:
			m_gamestatus.speed = 0.7;
			break;
		case 5:
			m_gamestatus.speed = 0.6;
			break;
		case 6:
			m_gamestatus.speed = 0.5;
			break;
		case 7:
			m_gamestatus.speed = 0.4;
			break;
		case 8:
			m_gamestatus.speed = 0.3;
			break;
		case 9:
			m_gamestatus.speed = 0.2;
			break;
		case 10:
			m_gamestatus.speed = 0.1;
			break;
		}
		flag.level_up_on = 0; //레벨업 flag꺼줌

		gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL); printf(" LEVEL : %5d", m_gamestatus.level); //레벨표시 
		// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", 10 - cnt); // 레벨목표 표시 

	}
}

void GamePlayScene::check_game_over(void) {
	int i;

	int x = 5;
	int y = 5;

	for (i = 1; i < BOARD_X - 2; i++) {
		if (m_gamestatus.board_org[CEILLING_Y][i] > 0) { //천장(위에서 세번째 줄)에 inactive가 생성되면 게임 오버 
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
	setcursortype(NOCURSOR); //커서 없앰
	reset(); //게임판 리셋
	int retval;

	int len = htonl(sizeof(Gamestatus));
	// 초기 게임 데이터 보내기
	retval = send(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	retval = send(m_pGameClient->GetSOCKET(), (char*)&m_gamestatus, sizeof(Gamestatus), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// 내가 몇번인지 확인
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}
	len = ntohf(len);
	retval = recvn(m_pGameClient->GetSOCKET(), (char*)&m_pGameClient->m_ClientNum, sizeof(int), 0);
	if (retval == SOCKET_ERROR) {
		err_display("send()");
	}

	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) {
		exit(1);
	}
	hWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hWriteEvent == NULL) {
		exit(1);
	}
	CreateThread(NULL, 0, GamePlayThread, (LPVOID)this, 0, NULL);
}

DWORD WINAPI GamePlayScene::GamePlayThread(LPVOID arg) {
	int retval;
	int len = 0;
	int Msg = 0;
	GamePlayScene* pGamePlayScene = (GamePlayScene*)arg;

	while (1) {
		WaitForSingleObject(hWriteEvent, INFINITE);
		KeyInput keys = pGamePlayScene->m_keys;

		int MSG_len = htonl(sizeof(KeyInput));

		retval = send(pGamePlayScene->m_pGameClient->GetSOCKET(), (char*)&MSG_len, sizeof(int), 0);
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