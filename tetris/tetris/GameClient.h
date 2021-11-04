#pragma once
#include "stdafx.h"

struct Gamestatus {
	int bx, by; //�̵����� ����� �����ǻ��� x,y��ǥ
	int b_type; //��� ����
	int b_rotation; //��� ȸ����
	int b_type_next; //���� ��ϰ�
	int level; //���� level
	float speed; //���� �������� �ӵ� 1�̸� 1�ʸ��� ��ĭ�� ������
	int board_org[BOARD_Y][BOARD_X]; //�������� ������ �����ϴ� �迭 ����Ϳ� ǥ���Ŀ� board_cpy�� �����
	int board_cpy[BOARD_Y][BOARD_X]; //maincpy�� �������� ����Ϳ� ǥ�õǱ� ���� ������ ������ ���� //main�� ��ü�� ��� ����Ϳ� ǥ������ �ʰ� board_cpy�� �迭�� ���ؼ� ���� �޶��� ���� ����Ϳ� ��ħ
	int item; // ������
	int target; // ���� ��� 
};

struct KeyInput {
	bool left = false;      //��
	bool right = false;     //��
	bool up = false;        //spin
	bool down = false;      //������ ������
	bool space = false;     //hard drop space(�ѹ��� �� ������ ������)
};

class GameClient
{
private:
	// WSADATA wsa;
	// SOCKET sock;
	KeyInput m_keys;                                //Ŭ���̾�Ʈ�� Ű�Է��� ����
	Gamestatus m_gamestatus[MAX_PLAYER];  	       //�� Ŭ���̾�Ʈ�� ���� ����
//private: //���� �߻� �� �ش� ���� ���
//	void err_quit(char* msg);
//	void err_display(char* msg);
//public:
//	GameClient();    //���� �� �����ʱ�ȭ, ���� ����, connect�� ����
//	~GameClient(); //�Ҹ� �� closesocket �� �������� ����
//public:
//	void TitleSceneSend(enum MSG_MathcingSystem);
//	//Msg_Ready �� Msg_ReadyCancel �� MatchingThread �� ����
//	int TitleSceneRecv();
//	//MatchMakingThread �κ��� ���� �޽����� ����
//	// Msg_ConfirmReadyCancel: Msg_ReadyCancel //���� ��, ���� �ʿ��� ������ Ȯ�ε����� Ȯ���ϰ� ���� ����
//	// Msg_PlayInGame : PlayScene ���� ��ȯ
//	// Msg_WaitGame : ���� Scene���� Msg_PlayInGame�� ���������� ���
//	void PlaySceneSend(); //KeyInput m_keys �� ���� Commthread �� ����
//	void PlaySceneRecv(); //GameServer ���� ����� ���� CommThread �κ��� 
//	// ���� - ���� ������ ���·� ����, ������ char�����̸�, ������GameStatus m_gamestatus�� ����
//	void UpdateData(Gamestatus* recvGSData);
//	//�������� ���� �����ͷ� ���� Ŭ���̾�Ʈ�� �����͸� ������Ʈ
//	void SetKeyInput(KeyInput* keyinput)
//	//������ �� Ŭ���̾�Ʈ�� Ű ���� �޾Ƽ� ����

};