#pragma once
#include "stdafx.h"

struct Gamestatus {
	int bx, by; //이동중인 블록의 게임판상의 x,y좌표
	int b_type; //블록 종류
	int b_rotation; //블록 회전값
	int b_type_next; //다음 블록값
	int level; //현재 level
	float speed; //블럭이 내려오는 속도 1이면 1초마다 한칸씩 내려옴
	int board_org[BOARD_Y][BOARD_X]; //게임판의 정보를 저장하는 배열 모니터에 표시후에 board_cpy로 복사됨
	int board_cpy[BOARD_Y][BOARD_X]; //maincpy는 게임판이 모니터에 표시되기 전의 정보를 가지고 있음 //main의 전체를 계속 모니터에 표시하지 않고 board_cpy와 배열을 비교해서 값이 달라진 곳만 모니터에 고침
	int item; // 아이템
	int target; // 공격 대상 
};

struct KeyInput {
	bool left = false;      //←
	bool right = false;     //→
	bool up = false;        //spin
	bool down = false;      //빠르게 내리기
	bool space = false;     //hard drop space(한번에 맨 밑으로 내리기)
};

class GameClient
{
private:
	// WSADATA wsa;
	// SOCKET sock;
	KeyInput m_keys;                                //클라이언트의 키입력을 저장
	Gamestatus m_gamestatus[MAX_PLAYER];  	       //내 클라이언트의 게임 상태
//private: //에러 발생 시 해당 내용 출력
//	void err_quit(char* msg);
//	void err_display(char* msg);
//public:
//	GameClient();    //생성 시 윈속초기화, 소켓 생성, connect를 수행
//	~GameClient(); //소멸 시 closesocket 과 윈속종료 수행
//public:
//	void TitleSceneSend(enum MSG_MathcingSystem);
//	//Msg_Ready 나 Msg_ReadyCancel 를 MatchingThread 에 전송
//	int TitleSceneRecv();
//	//MatchMakingThread 로부터 다음 메시지를 수령
//	// Msg_ConfirmReadyCancel: Msg_ReadyCancel //전송 후, 서버 쪽에서 수신이 확인됐음을 확인하고 연결 종료
//	// Msg_PlayInGame : PlayScene 으로 전환
//	// Msg_WaitGame : 현재 Scene에서 Msg_PlayInGame을 받을때까지 대기
//	void PlaySceneSend(); //KeyInput m_keys 의 값을 Commthread 에 전송
//	void PlaySceneRecv(); //GameServer 에서 연산된 값을 CommThread 로부터 
//	// 고정 - 가변 데이터 형태로 수신, 고정은 char형태이며, 가변은GameStatus m_gamestatus을 수신
//	void UpdateData(Gamestatus* recvGSData);
//	//서버에서 받은 데이터로 현재 클라이언트의 데이터를 업데이트
//	void SetKeyInput(KeyInput* keyinput)
//	//보내야 할 클라이언트의 키 값을 받아서 저장

};