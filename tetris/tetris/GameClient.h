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
	void ScreenRotate(HDC hDC);

	void SetSOCKET(SOCKET);
	SOCKET GetSOCKET();

	bool InitWSA();

	void ConnetServer();

	Scene* m_pScene;
	Scene* m_arrScene[Scene::SceneNum::Scene_Count];

	int m_ClientNum = -1;

	void TitleSceneSend(enum MSG_MatchMaking tag);
//	//Msg_Ready 나 Msg_ReadyCancel 를 MatchingThread 에 전송
	// int TitleSceneRecv();
//	//MatchMakingThread 로부터 다음 메시지를 수령
//	// Msg_ConfirmReadyCancel: Msg_ReadyCancel //전송 후, 서버 쪽에서 수신이 확인됐음을 확인하고 연결 종료
//	// Msg_PlayInGame : PlayScene 으로 전환
//	// Msg_WaitGame : 현재 Scene에서 Msg_PlayInGame을 받을때까지 대기
	// void PlaySceneSend(); //KeyInput m_keys 의 값을 Commthread 에 전송
	// void PlaySceneRecv(); //GameServer 에서 연산된 값을 CommThread 로부터
//	// 고정 - 가변 데이터 형태로 수신, 고정은 char형태이며, 가변은GameStatus m_gamestatus을 수신
//	void UpdateData(Gamestatus* recvGSData);
//	//서버에서 받은 데이터로 현재 클라이언트의 데이터를 업데이트
//	void SetKeyInput(KeyInput* keyinput)
//	//보내야 할 클라이언트의 키 값을 받아서 저장
};