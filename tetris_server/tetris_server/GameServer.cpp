#include "stdafx.h"
#include "GameServer.h"
#include "MatchMaking.h"
#include "socket_function.h"

DWORD WINAPI GameServerThread(LPVOID arg)
{
	GameServerThreadData newRoomData;
	MatchSockets* match_sockets = (MatchSockets*)arg;


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
	for (int i = 0; i < MAX_PLAYER; ++i)
	{
		Player newplayerdata;
		//�� ������ �ش� Ŭ���̾�Ʈ ���ϰ� play�����͸� �߰��Ѵ�.
		newplayerdata.clientSocket = match_sockets->client[i];
		newplayerdata.m_GameClientNum = i;
		newplayerdata.hupdate = newRoomData.hupdate;
		newplayerdata.hcheckupdate = newRoomData.hcheckupdate;
		newRoomData.pPlayers.push_back(newplayerdata);
	}
	newRoomData.reset();
	// �� Ŭ���̾�Ʈ�� ���ϵ�� ������ Ŀ�¾����� ����
	newRoomData.CreateCommThread();
	newRoomData.m_GameTimer.Start();
	while (1)
	{
		newRoomData.m_GameTimer.Tick(60.0f);
		WaitForSingleObject(newRoomData.hupdate, INFINITE); // ���� �Ϸ� ��ٸ���
		//printf("Call GameThread\n");
		newRoomData.check_key();
		for (int i = 0; i < MAX_PLAYER; ++i) {
			int GameClientNum = newRoomData.pPlayers[i].m_GameClientNum;
			if (newRoomData.pPlayers[i].m_gamestatus[GameClientNum].flag.gameover_flag == 0) {
				newRoomData.KeyUpdate(GameClientNum, newRoomData.m_GameTimer.GetTimeElapsed());
				if (newRoomData.pPlayers[i].m_gamestatus[GameClientNum].flag.down_flag == 0)
					newRoomData.drop_block(i, newRoomData.m_GameTimer.GetTimeElapsed());
				newRoomData.check_game_over(GameClientNum);
				if (newRoomData.pPlayers[i].m_gamestatus[GameClientNum].flag.new_block_on == 1
					&& newRoomData.pPlayers[i].m_gamestatus[GameClientNum].flag.gameover_flag == 0)
					// �� �� m_gamestatus[m_pGameClient->m_ClientNum].flag�� �ִ� ��� ���ο� �� ����
					newRoomData.new_block(GameClientNum);
			}

		}
		newRoomData.copy_another_map();
		//event���?
		//���� �����͵� ��Ƽ� ������Ʈ �ϱ�
		SetEvent(newRoomData.hcheckupdate);
	}
	delete match_sockets;
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
		WaitForSingleObject(playdata->hcheckupdate, INFINITE);
		//ResetEvent(hcheckupdate);
		// Ŭ���̾�Ʈ�� ������Ʈ�� ������ �����ֱ�

		len = sizeof(Gamestatus) * MAX_PLAYER;
		len = htonl(len);
		retval = send(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			SetEvent(playdata->hupdate); // ���� �Ϸ�
			break;
		}
		len = ntohl(len);
		retval = send(client_sock, (char*)&playdata->m_gamestatus, len, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send()");
			SetEvent(playdata->hupdate); // ���� �Ϸ�
			break;
		}
		//Ű�Է� ������ �ְ� �ޱ�
		retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			SetEvent(playdata->hupdate); // ���� �Ϸ�
			break;
		}
		len = ntohl(len);
		retval = recvn(client_sock, (char*)&tempKey, len, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recv()");
			SetEvent(playdata->hupdate); // ���� �Ϸ�
			break;
		}
		playdata->m_keys = tempKey;

		SetEvent(playdata->hupdate); // ���� �Ϸ�
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
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].flag.crush_on = 0;
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
				pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_cpy[j][k] = 100;
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
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].level = 1; //�������� �ʱ�ȭ
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].flag.crush_on = 0;
		pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].speed = 1;
	}
}

void GameServerThreadData::reset_main_cpy(void) { //m_gamestatus.board_cpy�� �ʱ�ȭ 

	for (int i = 0; i < MAX_PLAYER; ++i) {
		for (int j = 0; j < BOARD_Y; j++) {         //�����ǿ� ���ӿ� ������ �ʴ� ���ڸ� ����
			for (int k = 0; k < BOARD_X; k++) {  //�̴� m_gamestatus.board_org�� ���� ���ڰ� ���� �ϱ� ����
				pPlayers[i].m_gamestatus[pPlayers[i].m_GameClientNum].board_cpy[j][k] = 100;
			}
		}
	}
}

void GameServerThreadData::new_block(int ClientNum) { //���ο� ��� ����
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;
	pPlayers[ClientNum].m_gamestatus[GameClientNum].bx = (BOARD_X / 2) - 1; //��� ���� ��ġx��ǥ(�������� ���)
	pPlayers[ClientNum].m_gamestatus[GameClientNum].by = 0;  //��� ������ġ y��ǥ(���� ��)
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type_next; //���������� ������ 
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_type_next = rand() % 7; //���� ���� ����
	pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation = 0;  //ȸ���� 0������ ������

	pPlayers[ClientNum].m_gamestatus[GameClientNum].flag.new_block_on = 0; //new_block flag�� ��

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
			pPlayers[i].m_gamestatus[GameClientNum].flag.left_flag = false;
		}

		// ������Ű Ʈ����
		if (pPlayers[i].m_keys.right != true) {
			pPlayers[i].m_gamestatus[GameClientNum].flag.right_flag = false;
		}

		// �Ʒ�Ű Ʈ����
		if (pPlayers[i].m_keys.down != true) {
			pPlayers[i].m_gamestatus[GameClientNum].flag.down_flag = false;
		}

		// ��Ű Ʈ����
		if (pPlayers[i].m_keys.up != true) {
			pPlayers[i].m_gamestatus[GameClientNum].flag.up_flag = false;
		}

		// �����̽�Ű Ʈ����
		if (pPlayers[i].m_keys.space != true) {
			pPlayers[i].m_gamestatus[GameClientNum].flag.space_flag = false;
		}
	}
}

void GameServerThreadData::KeyUpdate(int clientNum, float fTimeElapsed) {
	int GameClientNum = pPlayers[clientNum].m_GameClientNum;
	int by = pPlayers[clientNum].m_gamestatus[GameClientNum].by;
	int bx = pPlayers[clientNum].m_gamestatus[GameClientNum].bx;
	int b_rotation = pPlayers[clientNum].m_gamestatus[GameClientNum].b_rotation;
	pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime += fTimeElapsed;

	if (pPlayers[clientNum].m_keys.left == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.left_flag == false) {
		if (check_crush(clientNum, bx - 1, by, b_rotation) == true) {
			move_block(clientNum, LEFT);
			pPlayers[clientNum].m_gamestatus[GameClientNum].flag.left_flag = true;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	if (pPlayers[clientNum].m_keys.right == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.right_flag == false) {
		if (check_crush(clientNum, bx + 1, by, b_rotation) == true) {
			move_block(clientNum, RIGHT);
			pPlayers[clientNum].m_gamestatus[GameClientNum].flag.right_flag = true;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	if (pPlayers[clientNum].m_keys.down == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.down_flag == false) {
		if (check_crush(clientNum, bx, by + 1, b_rotation) == true) {
			drop_block(clientNum, 100);
			pPlayers[clientNum].m_gamestatus[GameClientNum].flag.down_flag = true;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.2f;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}
	if (pPlayers[clientNum].m_keys.up == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.up_flag == false) {
		if (check_crush(clientNum, bx, by, (b_rotation + 1) % 4) == true) {
			move_block(clientNum, UP);
		}
		//ȸ���� �� �ִ��� üũ �� �����ϸ� ȸ��
		else if (pPlayers[clientNum].m_gamestatus[GameClientNum].flag.crush_on == true &&
			check_crush(clientNum, bx, by - 1, (b_rotation + 1) % 4) == true)
			move_block(clientNum, 100);
		//�ٴڿ� ���� ��� �������� ��ĭ����� ȸ���� �����ϸ� �׷��� ��(Ư������)
		pPlayers[clientNum].m_gamestatus[GameClientNum].flag.up_flag = true;
	}

	if (pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime >= pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed) {
		if (pPlayers[clientNum].m_keys.left == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.left_flag == 1) {
			if (check_crush(clientNum, bx - 1, by, b_rotation) == true) {
				move_block(clientNum, LEFT);
				pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
				pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
			}
		}
		if (pPlayers[clientNum].m_keys.right == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.right_flag == 1) {
			if (check_crush(clientNum, bx + 1, by, b_rotation) == true) {
				move_block(clientNum, RIGHT);
				pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
				pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
			}
		}
		if (pPlayers[clientNum].m_keys.down == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.down_flag == 1) {
			drop_block(clientNum, 100);
			pPlayers[clientNum].m_gamestatus[GameClientNum].fKeyMoveSpeed = 0.05f;
			pPlayers[clientNum].m_gamestatus[GameClientNum].fMoveBlockTime = 0.0f;
		}
	}

	if (pPlayers[clientNum].m_keys.space == true && pPlayers[clientNum].m_gamestatus[GameClientNum].flag.space_flag == 0) {
		hard_drop_block(clientNum);
		pPlayers[clientNum].m_gamestatus[GameClientNum].flag.space_flag = 1;
	}
}

void GameServerThreadData::drop_block(int PlayerNum, float fTimeElapsed) {
	int GameClientNum = pPlayers[PlayerNum].m_GameClientNum;
	int by = pPlayers[PlayerNum].m_gamestatus[GameClientNum].by;
	int bx = pPlayers[PlayerNum].m_gamestatus[GameClientNum].bx;
	int b_rotation = pPlayers[PlayerNum].m_gamestatus[GameClientNum].b_rotation;

	pPlayers[PlayerNum].m_gamestatus[GameClientNum].fDropBlockTime += fTimeElapsed;
	if (pPlayers[PlayerNum].m_gamestatus[GameClientNum].fDropBlockTime >= pPlayers[PlayerNum].m_gamestatus[GameClientNum].speed) {
		if (check_crush(PlayerNum, bx, by + 1, b_rotation) == false) { //���� ��������ʰ� crush flag�� ����������
			for (int j = 0; j < BOARD_Y; j++) { //���� �������� ���� ����
				for (int k = 0; k < BOARD_X; k++) {
					if (pPlayers[PlayerNum].m_gamestatus[GameClientNum].board_org[j][k] == ACTIVE_BLOCK)
						pPlayers[PlayerNum].m_gamestatus[GameClientNum].board_org[j][k] = INACTIVE_BLOCK;
				}
			}
			pPlayers[PlayerNum].m_gamestatus[GameClientNum].flag.crush_on = false; //flag�� ��
			check_line(); //����üũ�� ��
			pPlayers[PlayerNum].m_gamestatus[GameClientNum].flag.new_block_on = true; //���ο� ������ flag�� ��
			new_block(PlayerNum);
			pPlayers[PlayerNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
		}
		if (check_crush(PlayerNum, bx, by + 1, b_rotation) == true)
			move_block(PlayerNum, DOWN); //���� ��������� ������ ��ĭ �̵�

		if (check_crush(PlayerNum, bx, by + 1, b_rotation) == false)
			pPlayers[PlayerNum].m_gamestatus[GameClientNum].flag.crush_on = true; //������ �̵��� �ȵǸ�  crush flag�� ��

		pPlayers[PlayerNum].m_gamestatus[GameClientNum].fDropBlockTime = 0.0f;
	}
}

void GameServerThreadData::hard_drop_block(int ClientNum) {
	int GameClientNum = pPlayers[ClientNum].m_GameClientNum;

	while (1) {
		int by = pPlayers[ClientNum].m_gamestatus[GameClientNum].by;
		int bx = pPlayers[ClientNum].m_gamestatus[GameClientNum].bx;
		int b_rotation = pPlayers[ClientNum].m_gamestatus[GameClientNum].b_rotation;

		if (check_crush(GameClientNum, bx, by + 1, b_rotation) == false) { //���� ��������ʰ� crush flag�� ����������
			for (int i = 0; i < BOARD_Y; i++) { //���� �������� ���� ����
				for (int j = 0; j < BOARD_X; j++) {
					if (pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[i][j] == ACTIVE_BLOCK)
						pPlayers[ClientNum].m_gamestatus[GameClientNum].board_org[i][j] = INACTIVE_BLOCK;
				}
			}
			pPlayers[ClientNum].m_gamestatus[GameClientNum].flag.crush_on = false; //flag�� ��
			check_line(); //����üũ�� ��
			pPlayers[ClientNum].m_gamestatus[GameClientNum].flag.new_block_on = true; //���ο� ������ flag�� ��
			return;
		}
		if (check_crush(GameClientNum, bx, by + 1, b_rotation) == true)
			move_block(ClientNum, DOWN); //���� ��������� ������ ��ĭ �̵�
		if (check_crush(GameClientNum, bx, by + 1, b_rotation) == false)
			pPlayers[ClientNum].m_gamestatus[GameClientNum].flag.crush_on = true; //������ �̵��� �ȵǸ�  crush flag�� ��
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
	}
}

void GameServerThreadData::check_line(void) {
	int block_amount; //������ ��ϰ����� �����ϴ� ���� 
	int combo = 0; //�޺����� �����ϴ� ���� ������ �ʱ�ȭ 
	for (int i = 0; i < MAX_PLAYER; ++i) {
		int GameClientNum = pPlayers[i].m_GameClientNum;
		Gamestatus* m_gamestatus = &(pPlayers[i].m_gamestatus[GameClientNum]);
		for (int j = BOARD_Y - 2; j > 3;) { //i=MAIN_Y-2 : ���ʺ��� ��ĭ����,  i>3 : õ��(3)�Ʒ����� �˻� 
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
			}
			else j--;
		}
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
//
//void GameServerThreadData::check_level_up(void) {
//	int i, j;
//
//	if (cnt >= 10) { //�������� 10�پ� ���־���. 10���̻� ���� ��� 
//		draw_main();
//		flag.level_up_on = 1; //������ flag�� ���
//		m_gamestatus.level += 1; //������ 1 �ø�
//		cnt = 0; //���� �ټ� �ʱ�ȭ  
//
//		reset_main_cpy(); //�ؽ�Ʈ�� ����� ���� m_gamestatus.board_cpy�� �ʱ�ȭ.
//		//(m_gamestatus.board_cpy�� m_gamestatus.board_org�� ���� �ٸ��Ƿ� ������ draw()ȣ��� ������ ��ü�� ���� �׸��� ��) 
//
//		//.check_line()�Լ� ���ο��� m_gamestatus.level up flag�� �����ִ� ��� ������ ����.         
//		switch (m_gamestatus.level) { //�������� �ӵ��� ��������. 
//		case 2:
//			m_gamestatus.speed = 0.9;
//			break;
//		case 3:
//			m_gamestatus.speed = 0.8;
//			break;
//		case 4:
//			m_gamestatus.speed = 0.7;
//			break;
//		case 5:
//			m_gamestatus.speed = 0.6;
//			break;
//		case 6:
//			m_gamestatus.speed = 0.5;
//			break;
//		case 7:
//			m_gamestatus.speed = 0.4;
//			break;
//		case 8:
//			m_gamestatus.speed = 0.3;
//			break;
//		case 9:
//			m_gamestatus.speed = 0.2;
//			break;
//		case 10:
//			m_gamestatus.speed = 0.1;
//			break;
//		}
//		flag.level_up_on = 0; //������ flag����
//
//		gotoxy(STATUS_X_ADJ, STATUS_Y_LEVEL); printf(" LEVEL : %5d", m_gamestatus.level); //����ǥ�� 
//		// gotoxy(STATUS_X_ADJ, STATUS_Y_GOAL); printf(" GOAL  : %5d", 10 - cnt); // ������ǥ ǥ�� 
//
//	}
//}

void GameServerThreadData::check_game_over(int ClinentNum) {
	int GameClientNum = pPlayers[ClinentNum].m_GameClientNum;
	for (int j = 1; j < BOARD_X - 2; j++) {
		if (pPlayers[ClinentNum].m_gamestatus[GameClientNum].board_org[CEILLING_Y][j] > 0) { //õ��(������ ����° ��)�� inactive�� �����Ǹ� ���� ����
			pPlayers[ClinentNum].m_gamestatus[GameClientNum].flag.gameover_flag = 1;
		}
	}
}