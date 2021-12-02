#pragma once
#include "stdafx.h"
#include "Scene.h"
#include "Timer.h"

class GameClient
{
private:
	int retval;
	WSADATA wsa;
	SOCKET sock;
	SOCKADDR_IN serveraddr;

	KeyInput m_keys;                    //Ŭ���̾�Ʈ�� Ű�Է��� ����
	Gamestatus m_gamestatus;  	        //�� Ŭ���̾�Ʈ�� ���� ����
	CGameTimer m_GameTimer;

	void err_quit(char* msg);                       //���� �߻� �� �ش� ���� ���
	void err_display(char* msg);

public:
	GameClient();    //���� �� �����ʱ�ȭ, ���� ����, connect�� ����
	~GameClient();   //�Ҹ� �� closesocket �� �������� ����

	void ChangeScene(Scene::SceneNum tag);
	void BuildScene();
	void Update();
	void Paint(HDC hDC);
	void KeyDown(unsigned char KEYCODE);
	void KeyUp(unsigned char KEYCODE);
	void ScreenRotate(HDC hDC);

	void SetSOCKET(SOCKET);
	SOCKET GetSOCKET();

	bool InitWSA();

	void ConnetServer();

	Scene* m_pScene;
	Scene* m_arrScene[Scene::SceneNum::Scene_Count];

	int m_ClientNum = -1;

	void TitleSceneSend(enum MSG_MatchMaking tag);
//	//Msg_Ready �� Msg_ReadyCancel �� MatchingThread �� ����
	// int TitleSceneRecv();
//	//MatchMakingThread �κ��� ���� �޽����� ����
//	// Msg_ConfirmReadyCancel: Msg_ReadyCancel //���� ��, ���� �ʿ��� ������ Ȯ�ε����� Ȯ���ϰ� ���� ����
//	// Msg_PlayInGame : PlayScene ���� ��ȯ
//	// Msg_WaitGame : ���� Scene���� Msg_PlayInGame�� ���������� ���
	// void PlaySceneSend(); //KeyInput m_keys �� ���� Commthread �� ����
	// void PlaySceneRecv(); //GameServer ���� ����� ���� CommThread �κ���
//	// ���� - ���� ������ ���·� ����, ������ char�����̸�, ������GameStatus m_gamestatus�� ����
//	void UpdateData(Gamestatus* recvGSData);
//	//�������� ���� �����ͷ� ���� Ŭ���̾�Ʈ�� �����͸� ������Ʈ
//	void SetKeyInput(KeyInput* keyinput)
//	//������ �� Ŭ���̾�Ʈ�� Ű ���� �޾Ƽ� ����
};