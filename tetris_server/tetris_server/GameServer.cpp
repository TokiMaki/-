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
		//�� ������ �ش� Ŭ���̾�Ʈ ���ϰ� play�����͸� �߰��Ѵ�.
		newplayerdata.clientSocket = match_sockets->client[i];
		newplayerdata.m_GameClientNum = i;
		newplayerdata.hupdate = newRoomData.hupdate;
		newplayerdata.hcheckupdate = newRoomData.hcheckupdate;
		newplayerdata.cs = newRoomData.cs;
		newRoomData.pPlayers.push_back(newplayerdata);
	}
	newRoomData.reset();
	// �� Ŭ���̾�Ʈ�� ���ϵ�� ������ Ŀ�¾����� ����
	newRoomData.CreateCommThread();
	newRoomData.m_GameTimer.Start();
	while (1)
	{
		//WaitForSingleObject(newRoomData.hupdate, INFINITE); // ���� �Ϸ� ��ٸ���
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
					// �� �� m_gamestatus[m_pGameClient->m_ClientNum].flag�� �ִ� ��� ���ο� �� ����

					newRoomData.new_block(GameClientNum);
			}

		}
		newRoomData.copy_another_map();
		//event���?
		//���� �����͵� ��Ƽ� ������Ʈ �ϱ�
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
	// �ڽ��� ���° ���� �����ֱ�
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
		// Ŭ���̾�Ʈ�� ������Ʈ�� ������ �����ֱ�

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
		//Ű�Է� ������ �ְ� �ޱ�
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
		//SetEvent(playdata->hupdate); // ���� �Ϸ�
		//WaitForSingleObject(playdata->hcheckupdate,INFINITE);
	}

	return 0;
}
void GameServerThreadData::CreateCommThread(void)
{
	//GameServerThread�� ���� Ŭ���̾�Ʈ���� �迭�� ����
	//���ϸ� ������ ���� Player struct�� ������
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
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].b_type_next = rand() % 7; //�������� ���� ��� ������ �����ϰ� ����
	}
	reset_main(); // m_gamestatus.board_org�� �ʱ�ȭ
	for (int i = 0; i < MAX_PLAYER; ++i) {
		new_block(i); //���ο� ����� �ϳ� ����
	}
	copy_another_map();
}

void GameServerThreadData::copy_another_map() {
	for (int i = 0; i < MAX_PLAYER; ++i) {
		for (int j = 0; j < MAX_PLAYER; j++) { // �������� 0���� �ʱ�ȭ
			if (pPlayers[i].m_GameClientNum != pPlayers[j].m_GameClientNum) {
				pPlayers[j].m_gamestatus[pPlayers[i].m_GameClientNum] = pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum];
			}
		}
	}
}

void GameServerThreadData::reset_main(void) { //�������� �ʱ�ȭ  

	for (int i = 0; i < MAX_PLAYER; ++i) {
		for (int j = 0; j < BOARD_Y; j++) { // �������� 0���� �ʱ�ȭ
			for (int k = 0; k < BOARD_X; k++) {
				pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[j][k] = 0;
			}
		}
		for (int k = 1; k < BOARD_X; k++) { //y���� 3�� ��ġ�� õ���� ����
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[CEILLING_Y][k] = CEILLING;
		}
		for (int j = 1; j < BOARD_Y - 1; j++) { //�¿� ���� ����
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[j][0] = WALL;
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[j][BOARD_X - 1] = WALL;
		}
		for (int k = 0; k < BOARD_X; k++) { //�ٴں��� ���� 
			pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_org[BOARD_Y - 1][k] = WALL;
		}
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].m_KeyFlag.crush_on = 0;
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].speed = 1;
	}
}

void GameServerThreadData::new_block(int ClientNum) { //���ο� ��� ����
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	pPlayers[ClientNum].m_gamestatus[GameClientNum].bx = (BOARD_X / 2) - 1; //��� ���� ��ġx��ǥ(�������� ���)
	pPlayers[ClientNum].m_gamestatus[GameClientNum].by = 0;  //��� ������ġ y��ǥ(���� ��)
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type_next; //���������� ������ 
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type_next = rand() % 7; //���� ���� ����
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation = 0;  //ȸ���� 0������ ������

	pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on = 0; //new_block flag�� ��

	for (int i = 0; i < 4; i++) { //������ bx, by��ġ�� ������
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
	// ����Ű Ʈ����
	for (int i = 0; i < MAX_PLAYER; ++i) {
		int GameClientNum = pPlayers[i].m_GameClientNum;
		if (pPlayers[i].m_keys.left != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.left_flag = false;
		}

		// ������Ű Ʈ����
		if (pPlayers[i].m_keys.right != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.right_flag = false;
		}

		// �Ʒ�Ű Ʈ����
		if (pPlayers[i].m_keys.down != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.down_flag = false;
		}

		// ��Ű Ʈ����
		if (pPlayers[i].m_keys.up != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.up_flag = false;
		}

		// �����̽�Ű Ʈ����
		if (pPlayers[i].m_keys.space != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.space_flag = false;
		}

		// ����ƮŰ Ʈ����
		if (pPlayers[i].m_keys.shift != true) {
			pPlayers[i].m_gamestatus[GameClientNum].m_KeyFlag.shift_flag = false;
		}

		// ��Ʈ��Ű Ʈ����
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

	for (int j = 1; j < BOARD_X - 1; j++) { //õ���� ��� ���ο���� �������� �������� ���� �׷���
		if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] == EMPTY)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] = CEILLING;
	}

	// ����Ű ������ ��
	if (pPlayers[ClientNum].m_keys.left == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.left_flag == false) {
		if (check_crush(ClientNum, bx - 1, by, b_rotation) == true) {
			move_block(ClientNum, LEFT);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.left_flag = true;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	// ������Ű ������ ��
	if (pPlayers[ClientNum].m_keys.right == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.right_flag == false) {
		if (check_crush(ClientNum, bx + 1, by, b_rotation) == true) {
			move_block(ClientNum, RIGHT);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.right_flag = true;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	// �Ʒ�Ű ������ ��
	if (pPlayers[ClientNum].m_keys.down == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.down_flag == false) {
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == true) {
			drop_block(ClientNum, 100);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.down_flag = true;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	// ��Ű ������ ��
	if (pPlayers[ClientNum].m_keys.up == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.up_flag == false) {
		if (check_crush(ClientNum, bx, by, (b_rotation + 1) % 4) == true) {
			move_block(ClientNum, UP);
		}
		//ȸ���� �� �ִ��� üũ �� �����ϸ� ȸ��
		else if (pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on == true &&
			check_crush(ClientNum, bx, by - 1, (b_rotation + 1) % 4) == true)
			move_block(ClientNum, 100);
		//�ٴڿ� ���� ��� �������� ��ĭ����� ȸ���� �����ϸ� �׷��� ��(Ư������)
		pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.up_flag = true;
	}

	// ������ Ű �� ������ ��
	if (pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime >= pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed) {
		// ���� Ű �� ������ ��
		if (pPlayers[ClientNum].m_keys.left == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.left_flag == 1) {
			if (check_crush(ClientNum, bx - 1, by, b_rotation) == true) {
				move_block(ClientNum, LEFT);
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
			}
		}
		// ������ Ű �� ������ ��
		if (pPlayers[ClientNum].m_keys.right == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.right_flag == 1) {
			if (check_crush(ClientNum, bx + 1, by, b_rotation) == true) {
				move_block(ClientNum, RIGHT);
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
				pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
			}
		}
		// �Ʒ�Ű �� ������ ��
		if (pPlayers[ClientNum].m_keys.down == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.down_flag == 1) {
			drop_block(ClientNum, 100);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}

	// �����̽��� ������ ��
	if (pPlayers[ClientNum].m_keys.space == true && pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.space_flag == 0) {
		hard_drop_block(ClientNum);
		pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.space_flag = 1;
	}

	// ����ƮŰ ������ ��
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
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false) { //���� ��������ʰ� crush flag�� ����������
			for (int j = 0; j < BOARD_Y; j++) { //���� �������� ���� ����
				for (int k = 0; k < BOARD_X; k++) {
					if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[j][k] == ACTIVE_BLOCK)
						pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[j][k] = INACTIVE_BLOCK;
				}
			}
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = false; //flag�� ��
			check_line(ClientNum); //����üũ�� ��
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on = true; //���ο� ������ flag�� ��
			new_block(ClientNum);
			pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
		}
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == true)
			move_block(ClientNum, DOWN); //���� ��������� ������ ��ĭ �̵�

		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = true; //������ �̵��� �ȵǸ�  crush flag�� ��

		pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
	}
}

void GameServerThreadData::hard_drop_block(int ClientNum) {
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;

	while (1) {
		int by = pPlayers[ClientNum].m_gamestatus[GameClientNum].by;
		int bx = pPlayers[ClientNum].m_gamestatus[GameClientNum].bx;
		int b_rotation = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation;

		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false) { //���� ��������ʰ� crush flag�� ����������
			for (int i = 0; i < BOARD_Y; i++) { //���� �������� ���� ����
				for (int j = 0; j < BOARD_X; j++) {
					if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[i][j] == ACTIVE_BLOCK)
						pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = false; //flag�� ��
			check_line(ClientNum); //����üũ�� ��
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.new_block_on = true; //���ο� ������ flag�� ��
			return;
		}
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == true)
			move_block(ClientNum, DOWN); //���� ��������� ������ ��ĭ �̵�
		if (check_crush(ClientNum, bx, by + 1, b_rotation) == false)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].m_KeyFlag.crush_on = true; //������ �̵��� �ȵǸ�  crush flag�� ��
		pPlayers[ClientNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
	}
}

bool GameServerThreadData::check_crush(int ClientNum, int bx, int by, int b_rotation) { //������ ��ǥ�� ȸ�������� �浹�� �ִ��� �˻�
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) { //������ ��ġ�� �����ǰ� ������� ���ؼ� ��ġ�� false�� ����
			if (blocks[pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type][b_rotation][i][j] == 1 && pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[by + i][bx + j] > 0)
				return false;
		}
	}
	return true; //�ϳ��� �Ȱ�ġ�� true���� 
};

void GameServerThreadData::move_block(int ClientNum, int dir) { //����� �̵���Ŵ 
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	Gamestatus* m_gamestatus = &(pPlayers[ClientNum].m_gamestatus[GameClientNum]);

	switch (dir) {
	case LEFT: //���ʹ���
		for (int i = 0; i < 4; i++) { //������ǥ�� ���� ���� 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		for (int i = 0; i < 4; i++) { //�������� ��ĭ���� active block�� ���� 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j - 1] = ACTIVE_BLOCK;
			}
		}
		m_gamestatus->bx--; //��ǥ�� �̵� 
		break;

	case RIGHT:    //������ ����-> ���ʹ����̶� ���� ������ ���� 
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

	case DOWN:    //�Ʒ��� ����-> ���ʹ����̶� ���� ������ ����
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

	case UP: //Ű���� ���� �������� ȸ����Ŵ-> 
		for (int i = 0; i < 4; i++) { //������ǥ�� ���� ����  
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = EMPTY;
			}
		}
		m_gamestatus->b_rotation = (m_gamestatus->b_rotation + 1) % 4; //ȸ������ 1������Ŵ(3���� 4�� �Ǵ� ���� 0���� �ǵ���) 
		for (int i = 0; i < 4; i++) { //ȸ���� ����� ���� 
			for (int j = 0; j < 4; j++) {
				if (blocks[m_gamestatus->b_type][m_gamestatus->b_rotation][i][j] == 1)
					m_gamestatus->board_org[m_gamestatus->by + i][m_gamestatus->bx + j] = ACTIVE_BLOCK;
			}
		}
		break;

	case 100: //����� �ٴ�, Ȥ�� �ٸ� ��ϰ� ���� ���¿��� ��ĭ���� �÷� ȸ���� ������ ��� 
			  //�̸� ���۽�Ű�� Ư������ 
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
	case 101: //����� ���� ��ĭ �ø��� �۾��� ����
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
	int block_amount; //������ ��ϰ����� �����ϴ� ���� 
	int combo = 0; //�޺����� �����ϴ� ���� ������ �ʱ�ȭ
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	Gamestatus* m_gamestatus = &(pPlayers[ClientNum].m_gamestatus[GameClientNum]);
	for (int j = BOARD_Y - 2; j > CEILLING_Y;) { //i=MAIN_Y-2 : ���ʺ��� ��ĭ����,  i>3 : õ��(3)�Ʒ����� �˻� 
		block_amount = 0; //��ϰ��� ���� ���� �ʱ�ȭ
		for (int k = 1; k < BOARD_X - 1; k++) { //���� �������� ��ϰ��縦 �� 
			if (m_gamestatus->board_org[j][k] > 0) block_amount++;
		}
		if (block_amount == BOARD_X - 2) { //����� ���� �� ���
			for (int k = j; k > 1; k--) { //������ ��ĭ�� ��� ����(������ õ���� �ƴ� ��쿡��) 
				for (int l = 1; l < BOARD_X - 1; l++) {
					if (m_gamestatus->board_org[k - 1][l] != CEILLING)
						m_gamestatus->board_org[k][l] = m_gamestatus->board_org[k - 1][l];

					if (m_gamestatus->board_org[k - 1][l] == CEILLING)
						m_gamestatus->board_org[k][l] = EMPTY;
					//������ õ���� ��쿡�� õ���� ��ĭ ������ �ȵǴϱ� ��ĭ�� ����
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
	//if (combo) { //�� ������ �ִ� ��� ������ ���� ��ǥ�� ���� ǥ����  
	//	if (combo > 1) { //2�޺��̻��� ��� ��� ���ʽ��� �޼����� �����ǿ� ����ٰ� ���� 
	//		gotoxy(BOARD_X_ADJ + (BOARD_X / 2) - 1, BOARD_Y_ADJ + m_gamestatus.by - 2); printf("%d COMBO!", combo);
	//		// Sleep(500);
	//		// score += (combo * m_gamestatus.level * 100);
	//		reset_main_cpy(); //�ؽ�Ʈ�� ����� ���� m_gamestatus.board_cpy�� �ʱ�ȭ.
	//	//(m_gamestatus.board_cpy�� m_gamestatus.board_org�� ���� �ٸ��Ƿ� ������ draw()ȣ��� ������ ��ü�� ���� �׸��� ��) 
	//	}
	//	// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", (cnt <= 10) ? 10 - cnt : 0);
	//	// gotoxy(STATUS_X_ADJ, STATUS_Y_SCORE); printf("        %6d", score);
	//}
}

// �����Լ�
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

//���ݹ����Լ�
void GameServerThreadData::attacked(int ClientNum) {
	int EmptyX = rand() % (BOARD_X - 2);
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	Gamestatus* m_gamestatus = &(pPlayers[ClientNum].m_gamestatus[GameClientNum]);
	// int Target = pPlayers[ClientNum].m_gamestatus[GameClientNum].target;
	for (int i = 0; i < m_gamestatus->AttackedBlock; ++i) {
		for (int j = CEILLING_Y; j < BOARD_Y - 2; j++) { //������ ��ĭ�� ��� ����(������ õ���� �ƴ� ��쿡��)
			for (int k = 1; k < BOARD_X - 1; k++) {
				if (m_gamestatus->board_org[j + 1][k] != CEILLING)
					m_gamestatus->board_org[j][k] = m_gamestatus->board_org[j + 1][k];

				if (m_gamestatus->board_org[j + 1][k] == CEILLING)
					m_gamestatus->board_org[j][k] = EMPTY;
				//������ õ���� ��쿡�� õ���� ��ĭ ������ �ȵǴϱ� ��ĭ�� ����
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
		if (pPlayers[ClinentNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] > 0) { //õ��(������ ����° ��)�� inactive�� �����Ǹ� ���� ����
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
			// ȭ�� ������
			pPlayers[Target].m_gamestatus[TargetClientNum].m_GameFlag.screen_rotate_flag = 1;
			break;
		case 1:
			//��� ���ǵ� ��
			pPlayers[Target].m_gamestatus[TargetClientNum].m_GameFlag.speedup_flag = 1;
			pPlayers[Target].m_gamestatus[TargetClientNum].speed = pPlayers[Target].m_gamestatus[TargetClientNum].speed / 2.f;
			break;
		case 2:
			//���� �������� ��� ���� ����
			int* bx = &pPlayers[Target].m_gamestatus[TargetClientNum].bx;
			int* by = &pPlayers[Target].m_gamestatus[TargetClientNum].by;
			int b_rotation = pPlayers[Target].m_gamestatus[TargetClientNum].b_rotation;
			for (int i = 0; i < 2; ++i) {
				if (*by < BOARD_Y) {
					move_block(Target, 101);		// ����� ���� 2ĭ �ø�
				}
			}

			//������ǥ�� ���� ���� 
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
			for (int i = 0; i < 4; i++) { // �ٽ� ����
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