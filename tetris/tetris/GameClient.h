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

	KeyInput m_keys;                    //클라이언트의 키입력을 저장
	Gamestatus m_gamestatus;  	        //내 클라이언트의 게임 상태
	CGameTimer m_GameTimer;

	void err_quit(char* msg);                       //에러 발생 시 해당 내용 출력
	void err_display(char* msg);

public:
	GameClient();    //생성 시 윈속초기화, 소켓 생성, connect를 수행
	~GameClient();   //소멸 시 closesocket 과 윈속종료 수행

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