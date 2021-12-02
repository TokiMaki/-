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
	void ScreenRotate(HDC hDC, RECT rt);

	void SetSOCKET(SOCKET);
	SOCKET GetSOCKET();

	bool InitWSA();

	void ConnetServer();

	Scene* m_pScene;
	Scene* m_arrScene[Scene::SceneNum::Scene_Count];

	int m_ClientNum = -1;

	void TitleSceneSend(enum MSG_MatchMaking tag);

	HBITMAP BlockBitmap;
	HBITMAP UIBitmap;
};