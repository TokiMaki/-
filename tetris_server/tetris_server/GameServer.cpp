#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"
#include "socket_function.h"

DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	MatchSockets* match_sockets = (MatchSockets*)arg;
	match_sockets->client[0];

	srand((unsigned int)time(NULL));

	newRoomData.hupdate = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (newRoomData.hupdate == NULL)
	{
		return 1;
	}
	newRoomData.hcheckupdate = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (newRoomData.hcheckupdate == NULL)
	{
		return 1;
	}

	InitializeCriticalSection(&newRoomData.cs);
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		Player newplayerdata;
		//방 정보에 해당 클라이언트 소켓과 play데이터를 추가한다.
		newplayerdata.clientSocket = match_sockets->client[i];
		newplayerdata.m_GameClientNum = i;
		newplayerdata.hupdate = newRoomData.hupdate;
		newplayerdata.hcheckupdate = newRoomData.hcheckupdate;
		newplayerdata.cs = newRoomData.cs;
		newRoomData.pPlayers.push_back(newplayerdata);
	}
	newRoomData.reset();
	// 각 클라이언트의 소켓들과 소통할 커뮤쓰레드 생성
	newRoomData.CreateCommThread();
	newRoomData.m_GameTimer.Start();
	while (1)
	{
		//WaitForSingleObject(newRoomData.hupdate, INFINITE); // 쓰기 완료 기다리기
		//printf("Call GameThread\n");
		EnterCriticalSection(&newRoomData.cs);
		newRoomData.m_GameTimer.Tick(60.0f);
		newRoomData.check_key();
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int GameClientNum = newRoomData.pPlayers[i].m_GameClientNum;
			if (newRoomData.pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.gameover_flag == 0) {

				newRoomData.KeyUpdate(GameClientNum, newRoomData.m_GameTimer.GetTimeElapsed());

				if (newRoomData.pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.down_flag == 0)
					newRoomData.drop_block(i, newRoomData.m_GameTimer.GetTimeElapsed());

				newRoomData.check_game_over(GameClientNum);

				if (newRoomData.pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on == 1
					&& newRoomData.pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.gameover_flag == 0)
					// 뉴 블럭 m_gamestatus[m_pGameClient->m_ClientNum].flag가 있는 경우 새로운 블럭 생성

					newRoomData.new_block(GameClientNum);
			}

		}
		newRoomData.copy_another_map();
		//event사용?
		//받은 데이터들 모아서 업데이트 하기
		//SetEvent(newRoomData.hcheckupdate);
		LeaveCriticalSection(&newRoomData.cs);
	}
	delete match_sockets;
	DeleteCriticalSection(&newRoomData.cs);
	return 0;
}
DWORD WINAPI CommThread(LPVOID arg)
{
	Player* playdata = (Player*)arg;
	SOCKET client_sock = playdata->clientSocket;
	int retval;
	std::cout << client_sock << "commThread running\n" << std::endl;

	Player tempP;
	KeyInput tempKey;
	Gamestatus tempstatus[MAX_PLAYER];
	int tempClientNum = -1;
	int len = 0;

	tempClientNum = playdata->m_GameClientNum;
	// 자신이 몇번째 인지 보내주기
	len = sizeof(int);
	len = htonl(len);
	retval = send(client_sock, (char*)&len, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		return 0;
	}
	retval = send(client_sock, (char*)&tempClientNum, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display("send()");
		return 0;
	}

	while (1)
	{
		EnterCriticalSection(&playdata->cs);
		//WaitForSingleObject(playdata->hcheckupdate, INFINITE);
		//ResetEvent(hcheckupdate);
		// 클라이언트에 업데이트된 데이터 보내주기

		len = sizeof(Gamestatus) * MAX_PLAYER;
		len = htonl(len);
		retval = send(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			LeaveCriticalSection(&playdata->cs);
			break;
		}
		retval = send(client_sock, (char*)&playdata->m_gamestatus, sizeof(Gamestatus) * MAX_PLAYER, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			LeaveCriticalSection(&playdata->cs);
			break;
		}
		//키입력 데이터 주고 받기
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			LeaveCriticalSection(&playdata->cs);
			break;
		}
		len = ntohl(len);
		retval = recvn(client_sock, (char*)&tempKey, len, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			LeaveCriticalSection(&playdata->cs);
			break;
		}
		playdata->m_keys = tempKey;

		LeaveCriticalSection(&playdata->cs);
		//SetEvent(playdata->hupdate); // 쓰기 완료
		//WaitForSingleObject(playdata->hcheckupdate,INFINITE);
	}

	return 0;
}
void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread에 들어온 클라이언트들을 배열로 제작
	//소켓만 보내지 말고 Player struct를 보내기
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		std::cout << pPlayers[i].clientSocket << std::endl;
		HANDLE newCommThread = CreateThread(NULL, 0, CommThread, &pPlayers[i], 0, NULL);
	}

}

void GameServerThreadData::reset(void) {
	for (int i = 0; i < MAX_PLAYER; ++i) {
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].m_KeyFlag.crush_on = 0;
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].speed = 1;
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].b_type_next = rand() % 7; //다음번에 나올 블록 종류를 랜덤하게 생성
	}
	reset_main(); // m_gamestatus.board_org를 초기화
	for (int i = 0; i < MAX_PLAYER; ++i) {
		new_block(i); //새로운 블록을 하나 만듦
	}
	copy_another_map();
}

void GameServerThreadData::copy_another_map() {
	for (int i = 0; i < MAX_PLAYER; ++i) {
		for (int j = 0; j < MAX_PLAYER; j++) { // 게임판을 0으로 초기화
			if (pPlayers[i].m_GameClientNum != pPlayers[j].m_GameClientNum) {
				pPlayers[j].m_gamestatus[pPlayers[i].m_GameClientNum] = pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum];
			}
		}
	}
}

void GameServerThreadData::reset_main(void) { //게임판을 초기화  

	for (int i = 0; i < MAX_PLAYER; ++i) {
		for (int j = 0; j < BOARD_Y; j++) { // 게임판을 0으로 초기화
			for (int k = 0; k < BOARD_X; k++) {
				pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[j][k] = 0;
			}
		}
		for (int k = 1; k < BOARD_X; k++) { //y값이 3인 위치에 천장을 만듦
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[CEILLING_Y][k] = CEILLING;
		}
		for (int j = 1; j < BOARD_Y - 1; j++) { //좌우 벽을 만듦
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[j][0] = WALL;
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[j][BOARD_X - 1] = WALL;
		}
		for (int k = 0; k < BOARD_X; k++) { //바닥벽을 만듦 
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[BOARD_Y - 1][k] = WALL;
		}
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].m_KeyFlag.crush_on = 0;
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].speed = 1;
	}
}

void GameServerThreadData::new_block(int ClientNum) { //새로운 블록 생성
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	pPlayers[ClientNum].m_gamestatus[GameClientNum].bx = (BOARD_X / 2) - 1; //블록 생성 위치x좌표(게임판의 가운데)
	pPlayers[ClientNum].m_gamestatus[GameClientNum].by = 0;  //블록 생성위치 y좌표(제일 위)
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type_next; //다음블럭값을 가져옴 
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type_next = rand() % 7; //다음 블럭을 만듦
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation = 0;  //회전은 0번으로 가져옴

	pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on = 0; //new_block flag를 끔

	for (int i = 0; i < 4; i++) { //게임판 bx, by위치에 블럭생성
		int b_type = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type;
		int b_rotation = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation;
		int by = pPlayers[ClientNum].m_gamestatus[GameClientNum].by;
		int bx = pPlayers[ClientNum].m_gamestatus[GameClientNum].bx;

		for (int j = 0; j < 4; j++) {
			if (blocks[b_type][b_rotation][i][j] == 1)
				pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[by + i][bx + j] = ACTIVE_BLOCK;
		}
	}
}

void GameServerThreadData::check_key() {
	// 왼쪽키 트리거
	for (int i = 0; i < MAX_PLAYER; ++i) {
		int GameClientNum = pPlayers[i].m_GameClientNum;
		if (pPlayers[i].m_keys.left != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.left_flag = false;
		}

		// 오른쪽키 트리거
		if (pPlayers[i].m_keys.right != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.right_flag = false;
		}

		// 아래키 트리거
		if (pPlayers[i].m_keys.down != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.down_flag = false;
		}

		// 위키 트리거
		if (pPlayers[i].m_keys.up != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.up_flag = false;
		}

		// 스페이스키 트리거
		if (pPlayers[i].m_keys.space != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.space_flag = false;
		}

		// 쉬프트키 트리거
		if (pPlayers[i].m_keys.shift != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.shift_flag = false;
		}

		// 컨트롤키 트리거
		if (pPlayers[i].m_keys.ctrl != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.ctrl_flag = false;
		}
	}
}

void GameServerThreadData::KeyUpdate(int ClientNum, float fTimeElapsed) {
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	int by = pPlayers[ClientNum].m_gamestatus[GameClientNum].by;
	int bx = pPlayers[ClientNum].m_gamestatus[GameClientNum].bx;
	int b_rotation = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation;
	pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime += fTimeElapsed;

	for (int j = 1; j < BOARD_X - 1; j++) { //천장은 계속 새로운블럭이 지나가서 지워지면 새로 그려줌
		if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] == EMPTY)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}

	// 왼쪽키 눌렸을 때
	if (pPlayers[ClientNum].m_keys.left == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.left_flag == false) {
		if (check_crush(ClientNum, bx - 1, by, b_rotation) == true) {
			move_block(ClientNum, LEFT);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.left_flag = true;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	// 오른쪽키 눌렀을 때
	if (pPlayers[ClientNum].m_keys.right == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.right_flag == false) {
		if (check_crush(ClientNum, bx + 1, by, b_rotation) == true) {
			move_block(ClientNum, RIGHT);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.right_flag = true;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	// 아래키 눌렀을 때
	if (pPlayers[ClientNum].m_keys.down == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.down_flag == false) {
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == true) {
			drop_block(ClientNum, 100);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.down_flag = true;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	// 위키 눌렀을 때
	if (pPlayers[ClientNum].m_keys.up == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.up_flag == false) {
		if (check_crush(ClientNum, bx, by, (b_rotation + 1) % 4) == true) {
			move_block(ClientNum, UP);
		}
		//회전할 수 있는지 체크 후 가능하면 회전
		else if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on == true &&
			check_crush(ClientNum, bx, by - 1, (b_rotation + 1) % 4) == true)
			move_block(ClientNum, 100);
		//바닥에 닿은 경우 위쪽으로 한칸띄워서 회전이 가능하면 그렇게 함(특수동작)
		pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.up_flag = true;
	}

	// 각각의 키 꾹 눌렀을 때
	if (pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime >= pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed) {
		// 왼쪽 키 꾹 눌렀을 때
		if (pPlayers[ClientNum].m_keys.left == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.left_flag == 1) {
			if (check_crush(ClientNum, bx - 1, by, b_rotation) == true) {
				move_block(ClientNum, LEFT);
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
			}
		}
		// 오른쪽 키 꾹 눌렀을 때
		if (pPlayers[ClientNum].m_keys.right == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.right_flag == 1) {
			if (check_crush(ClientNum, bx + 1, by, b_rotation) == true) {
				move_block(ClientNum, RIGHT);
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
			}
		}
		// 아래키 꾹 눌렀을 때
		if (pPlayers[ClientNum].m_keys.down == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.down_flag == 1) {
			drop_block(ClientNum, 100);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}

	// 스페이스바 눌렀을 때
	if (pPlayers[ClientNum].m_keys.space == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.space_flag == 0) {
		hard_drop_block(ClientNum);
		pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.space_flag = 1;
	}

	// 쉬프트키 눌렀을 때
	TargetShift(GameClientNum, fTimeElapsed);

	ActiveItem(ClientNum, fTimeElapsed);

}

void GameServerThreadData::TargetShift(int clientNum, float fTimeElapsed) {
	int GameClientNum = pPlayers[clientNum].m_GameClientNum;
	if (pPlayers[clientNum].m_keys.shift == true && pPlayers[clientNum].m_gamestatus[GameClientNum].m_KeyFlag.shift_flag == 0) {
		if (pPlayers[clientNum].m_gamestatus[GameClientNum].target < MAX_PLAYER - 1) {
			pPlayers[clientNum].m_gamestatus[GameClientNum].target += 1;
		}
		else {
			pPlayers[clientNum].m_gamestatus[GameClientNum].target = 1;
		}
		pPlayers[clientNum].m_gamestatus[GameClientNum].m_KeyFlag.shift_flag = 1;
	}
}

void GameServerThreadData::drop_block(int ClientNum, float fTimeElapsed) {
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	int by = pPlayers[ClientNum].m_gamestatus[GameClientNum].by;
	int bx = pPlayers[ClientNum].m_gamestatus[GameClientNum].bx;
	int b_rotation = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation;

	pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime += fTimeElapsed;
	if (pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime >= pPlayers[ClientNum].m_gamestatus[GameClientNum].speed) {
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면
			for (int j = 0; j < BOARD_Y; j++) { //현재 조작중인 블럭을 굳힘
				for (int k = 0; k < BOARD_X; k++) {
					if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[j][k] == ACTIVE_BLOCK)
						pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[j][k] = INACTIVE_BLOCK;
				}
			}
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = false; //flag를 끔
			check_line(ClientNum); //라인체크를 함
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on = true; //새로운 블럭생성 flag를 켬
			new_block(ClientNum);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
		}
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == true)
			move_block(ClientNum, DOWN); //밑이 비어있으면 밑으로 한칸 이동

		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = true; //밑으로 이동이 안되면  crush flag를 켬

		pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
	}
}

void GameServerThreadData::hard_drop_block(int ClientNum) {
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;

	while (1) {
		int by = pPlayers[ClientNum].m_gamestatus[GameClientNum].by;
		int bx = pPlayers[ClientNum].m_gamestatus[GameClientNum].bx;
		int b_rotation = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation;

		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false) { //밑이 비어있지않고 crush flag가 켜저있으면
			for (int i = 0; i < BOARD_Y; i++) { //현재 조작중인 블럭을 굳힘
				for (int j = 0; j < BOARD_X; j++) {
					if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[i][j] == ACTIVE_BLOCK)
						pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = false; //flag를 끔
			check_line(ClientNum); //라인체크를 함
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on = true; //새로운 블럭생성 flag를 켬
			return;
		}
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == true)
			move_block(ClientNum, DOWN); //밑이 비어있으면 밑으로 한칸 이동
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = true; //밑으로 이동이 안되면  crush flag를 켬
		pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
	}
}

bool GameServerThreadData::check_crush(int ClientNum, int bx, int by, int b_rotation) { //지정된 좌표와 회전값으로 충돌이 있는지 검사
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) { //지정된 위치의 게임판과 블럭모양을 비교해서 겹치면 false를 리턴
			if (blocks[pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type][b_rotation][i][j] == 1 && pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[by + i][bx + j] > 0)
				return false;
		}
	}
	return true; //하나도 안겹치면 true리턴 
};

void GameServerThreadData::move_block(int ClientNum, int dir) { //블록을 이동시킴 
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	Gamestatus* m_gamestatus = &(pPlayers[ClientNum].m_gamestatus[GameClientNum]);

	switch (dir) {
	case LEFT: //왼쪽방향
		for (int i = 0; i < 4; i++) { //현재좌표의 블럭을 지움 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		for (int i = 0; i < 4; i++) { //왼쪽으로 한칸가서 active block을 찍음 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j - 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus->bx--; //좌표값 이동 
		break;

	case RIGHT:    //오른쪽 방향-> 왼쪽방향이랑 같은 원리로 동작 
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j + 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus->bx++;
		break;

	case DOWN:    //아래쪽 방향-> 왼쪽방향이랑 같은 원리로 동작
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i + 1][m_gamestatus->bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus->by++;
		break;

	case UP: //키보드 위쪽 눌렀을때 회전시킴-> 
		for (int i = 0; i < 4; i++) { //현재좌표의 블럭을 지움  
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		m_gamestatus->b_rotation = (m_gamestatus->b_rotation + 1) % 4; //회전값을 1증가시킴(3에서 4가 되는 경우는 0으로 되돌림) 
		for (int i = 0; i < 4; i++) { //회전된 블록을 찍음 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case 100: //블록이 바닥, 혹은 다른 블록과 닿은 상태에서 한칸위로 올려 회전이 가능한 경우 
			  //이를 동작시키는 특수동작 
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		m_gamestatus->b_rotation = (m_gamestatus->b_rotation + 1) % 4;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i - 1][m_gamestatus->bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus->by--;
		break;
	case 101: //블록을 위로 한칸 올리는 작업을 수행
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i - 1][m_gamestatus->bx + j] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus->by--;
		break;
	}
}

void GameServerThreadData::check_line(int ClientNum) {
	int block_amount; //한줄의 블록갯수를 저장하는 변수 
	int combo = 0; //콤보갯수 저장하는 변수 지정및 초기화
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	Gamestatus* m_gamestatus = &(pPlayers[ClientNum].m_gamestatus[GameClientNum]);
	for (int j = BOARD_Y - 2; j > CEILLING_Y;) { //i=MAIN_Y-2 : 밑쪽벽의 윗칸부터,  i>3 : 천장(3)아래까지 검사 
		block_amount = 0; //블록갯수 저장 변수 초기화
		for (int k = 1; k < BOARD_X - 1; k++) { //벽과 벽사이의 블록갯루를 셈 
			if (m_gamestatus->board_org[j][k] > 0) block_amount++;
		}
		if (block_amount == BOARD_X - 2) { //블록이 가득 찬 경우
			for (int k = j; k > 1; k--) { //윗줄을 한칸씩 모두 내림(윗줄이 천장이 아닌 경우에만) 
				for (int l = 1; l < BOARD_X - 1; l++) {
					if (m_gamestatus->board_org[k - 1][l] != CEILLING)
						m_gamestatus->board_org[k][l] = m_gamestatus->board_org[k - 1][l];

					if (m_gamestatus->board_org[k - 1][l] == CEILLING)
						m_gamestatus->board_org[k][l] = EMPTY;
					//윗줄이 천장인 경우에는 천장을 한칸 내리면 안되니까 빈칸을 넣음
				}
			}
			combo++;
			if (m_gamestatus->item == -1)
				m_gamestatus->item = rand() % 3;
		}
		else j--;
	}
	if (combo) {
		while (m_gamestatus->AttackedBlock > 0 && combo > 0) {
			m_gamestatus->AttackedBlock -= 1;
			combo -= 1;
		}
		attack(ClientNum, combo);
	}
	else {
		attacked(ClientNum);
	}
	//if (combo) { //줄 삭제가 있는 경우 점수와 레벨 목표를 새로 표시함  
	//	if (combo > 1) { //2콤보이상인 경우 경우 보너스및 메세지를 게임판에 띄웠다가 지움 
	//		gotoxy(BOARD_X_ADJ + (BOARD_X / 2) - 1, BOARD_Y_ADJ + m_gamestatus.by - 2); printf("%d COMBO!", combo);
	//		// Sleep(500);
	//		// score += (combo * m_gamestatus.level * 100);
	//		reset_main_cpy(); //텍스트를 지우기 위해 m_gamestatus.board_cpy을 초기화.
	//	//(m_gamestatus.board_cpy와 m_gamestatus.board_org가 전부 다르므로 다음번 draw()호출시 게임판 전체를 새로 그리게 됨) 
	//	}
	//	// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", (cnt <= 10) ? 10 - cnt : 0);
	//	// gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score);
	//}
}

// 공격함수
void GameServerThreadData::attack(int ClientNum, int Combo) {
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	int stack = 0;
	int Target = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (i != GameClientNum)
			stack++;
		if (stack >= pPlayers[ClientNum].m_gamestatus[GameClientNum].target) {
			Target = i;
			break;
		}
	}
	int TargetClientNum = pPlayers[Target].m_GameClientNum;
	pPlayers[Target].m_gamestatus[TargetClientNum].AttackedBlock += Combo;
}

//공격받음함수
void GameServerThreadData::attacked(int ClientNum) {
	int EmptyX = rand() % (BOARD_X - 2);
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	Gamestatus* m_gamestatus = &(pPlayers[ClientNum].m_gamestatus[GameClientNum]);
	// int Target = pPlayers[ClientNum].m_gamestatus[GameClientNum].target;
	for (int i = 0; i < m_gamestatus->AttackedBlock; ++i) {
		for (int j = CEILLING_Y; j < BOARD_Y - 2; j++) { //윗줄을 한칸씩 모두 내림(윗줄이 천장이 아닌 경우에만)
			for (int k = 1; k < BOARD_X - 1; k++) {
				if (m_gamestatus->board_org[j + 1][k] != CEILLING)
					m_gamestatus->board_org[j][k] = m_gamestatus->board_org[j + 1][k];

				if (m_gamestatus->board_org[j + 1][k] == CEILLING)
					m_gamestatus->board_org[j][k] = EMPTY;
				//윗줄이 천장인 경우에는 천장을 한칸 내리면 안되니까 빈칸을 넣음
			}
		}
		for (int j = 1; j < BOARD_X - 1; ++j) {
			m_gamestatus->board_org[BOARD_Y - 2][j] = EMPTY;
			if (j != EmptyX + 1) {
				m_gamestatus->board_org[BOARD_Y - 2][j] = INACTIVE_BLOCK;
			}
		}
	}
	m_gamestatus->AttackedBlock = 0;
}

void GameServerThreadData::check_level_up(float fTimeElapsed) {

	fLevelUpTime += fTimeElapsed;
	if (fLevelUpTime >= 15.f) {
		if (Level < 19) {
			Level++;
			fLevelUpTime = 0;
		}
	}
	for (int i = 0; i < MAX_PLAYER; ++i) {
		int GameClientNum = pPlayers[i].m_GameClientNum;
		pPlayers[i].m_gamestatus[GameClientNum].speed = 1 - Level * 0.5f;
	}
}

void GameServerThreadData::check_game_over(int ClinentNum) {
	int GameClientNum = pPlayers[ClinentNum].m_GameClientNum;
	for (int j = 1; j < BOARD_X - 2; j++) {
		if (pPlayers[ClinentNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] > 0) { //천장(위에서 세번째 줄)에 inactive가 생성되면 게임 오버
			pPlayers[ClinentNum].m_gamestatus[GameClientNum].m_KeyFlag.gameover_flag = 1;
		}
	}
}

void GameServerThreadData::ActiveItem(int ClientNum, float fTimeElapsed)
{
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	int stack = 0;
	int Target = 0;
	for (int i = 0; i < MAX_PLAYER; ++i) {
		if (i != GameClientNum)
			stack++;
		if (stack >= pPlayers[ClientNum].m_gamestatus[GameClientNum].target) {
			Target = i;
			break;
		}
	}
	int TargetClientNum = pPlayers[Target].m_GameClientNum;
	int item_block = pPlayers[ClientNum].m_gamestatus[GameClientNum].item;



	if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.screen_rotate_flag == 1) {
		if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fScreenRotateTime < 5.0f) {
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fScreenRotateTime += fTimeElapsed;
		}
		if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fScreenRotateTime >= 5.0f) {
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.screen_rotate_flag = 0;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fScreenRotateTime = 0;
		}
	}

	if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.speedup_flag == 1) {
		if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fSpeedUpTime < 10.0f) {
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fSpeedUpTime += fTimeElapsed;
		}
		if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fSpeedUpTime >= 10.0f) {
			pPlayers[ClientNum].m_gamestatus[GameClientNum].speed = 1 - Level * 0.5;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.speedup_flag = 0;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_GameFlag.fSpeedUpTime = 0;
		}
	}

	if (pPlayers[ClientNum].m_keys.ctrl == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.ctrl_flag == 0) {
		pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.ctrl_flag = 1;
		switch (item_block)
		{
		case 0:
			// 화면 돌리기
			pPlayers[Target].m_gamestatus[TargetClientNum].m_GameFlag.screen_rotate_flag = 1;
			break;
		case 1:
			//상대 스피드 업
			pPlayers[Target].m_gamestatus[TargetClientNum].m_GameFlag.speedup_flag = 1;
			pPlayers[Target].m_gamestatus[TargetClientNum].speed = pPlayers[Target].m_gamestatus[TargetClientNum].speed / 2.f;
			break;
		case 2:
			//현재 내려오는 블록 종류 변경
			int* bx = &pPlayers[Target].m_gamestatus[TargetClientNum].bx;
			int* by = &pPlayers[Target].m_gamestatus[TargetClientNum].by;
			int b_rotation = pPlayers[Target].m_gamestatus[TargetClientNum].b_rotation;
			for (int i = 0; i < 2; ++i) {
				if (*by < BOARD_Y) {
					move_block(Target, 101);		// 블록을 위로 2칸 올림
				}
			}

			//현재좌표의 블럭을 지움 
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (blocks[pPlayers[Target].m_gamestatus[TargetClientNum].b_type][b_rotation][i][j] == 1)
						pPlayers[Target].m_gamestatus[TargetClientNum].board_org[*bx + i][*by + j] = EMPTY;
				}
			}
			pPlayers[Target].m_gamestatus[TargetClientNum].b_type = rand() % 7;
			pPlayers[Target].m_gamestatus[TargetClientNum].b_rotation = rand() % 4;
			while (*bx <= 2) {
				if (check_crush(Target, *bx, *by, b_rotation))
					*bx++;
				else {
					break;
				}
			}
			while (*bx >= BOARD_X - 3) {
				if (check_crush(Target, *bx, *by, b_rotation))
					*bx--;
				else
					break;
			}
			while (*by > 0) {
				if(check_crush(Target, *bx, *by, b_rotation))
					*by--;
				else
					break;
			}
			for (int i = 0; i < 4; i++) { // 다시 찍음
				for (int j = 0; j < 4; j++) {
					if (blocks[pPlayers[Target].m_gamestatus[TargetClientNum].b_type][b_rotation][i][j] == 1)
						pPlayers[Target].m_gamestatus[TargetClientNum].board_org[*bx + i][*by + j] = ACTIVE_BLOCK;
				}
			}
			break;
		}
		pPlayers[ClientNum].m_gamestatus[GameClientNum].item = -1;
	}
}