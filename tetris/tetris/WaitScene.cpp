#include "Scene.h"
#include "Waitscene.h"
#include "GameClient.h"
#include "stdafx.h"
#include "socket_function.h"

HANDLE hWaitReadEvent, hWaitWriteEvent; // 이벤트

WaitScene::WaitScene() {}
WaitScene::WaitScene(SceneNum num, GameClient* const pGameClient) {
	m_SceneNum = num;
	m_pGameClient = pGameClient;
}
WaitScene::~WaitScene() {}

void WaitScene::InitScene() {
	system("cls");

	int retval;

	// 윈속 초기화
	if (m_pGameClient->InitWSA() != true)
		exit(1);

	// socket()
	m_pGameClient->SetSOCKET(socket(AF_INET, SOCK_STREAM, 0));
	if (m_pGameClient->GetSOCKET() == INVALID_SOCKET) err_quit("socket()");

	// connect()
	m_pGameClient->ConnetServer();

	hWaitReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWaitReadEvent == NULL) {
		exit(1);
	}
	hWaitWriteEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hWaitWriteEvent == NULL) {
		exit(1);
	}

	CreateThread(NULL, 0, TestThread, (LPVOID)this, 0, NULL);
}

void WaitScene::Update(float fTimeElapsed) {
	static float WaitTimer = 0.f;
	WaitTimer += fTimeElapsed;

	int retval;
	int len = 0;

	//while (1) {

	std::cout << "Waiting";
	for (int i = 0; i < 3; ++i) {
		if (WaitTimer >= i * 0.5) {
			std::cout << ".";
		}
	}
	if (WaitTimer >= 1.5) {
		system("cls");
		WaitTimer = 0;
	}
	std::cout << "\r";
	WaitForSingleObject(hWaitReadEvent, INFINITE);
	if (Msg == MSG_MatchMaking::Msg_PlayInGame) {
		m_pGameClient->ChangeScene(Scene::SceneNum::GamePlay);
	}
	SetEvent(hWaitWriteEvent);

	//    retval = recvn(m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
	//    if (retval == SOCKET_ERROR) {
	//        err_quit("recv()");
	//        break;
	//    }
	//    else if (retval == 0)
	//        break;
	//    len = ntohl(len);

	//    retval = recvn(m_pGameClient->GetSOCKET(), (char*)&Msg, len, 0);
	//    if (retval == SOCKET_ERROR) {
	//        err_quit("recv()");
	//        break;
	//    }
	//    else if (retval == 0)
	//        break;
	//    Msg = ntohl(Msg);
	//    // printf("%d\n", Msg);

	//    if (Msg == MSG_MatchMaking::Msg_PlayInGame) {
	//        m_pGameClient->ChangeScene(Scene::SceneNum::GamePlay);
	//        break;
	//    }

	//    Msg = 0;
	//    int sendMsg = htonl(Msg);

	//    int MSG_len = htonl(sizeof(int));
	//    retval = send(m_pGameClient->GetSOCKET(), (char*)&MSG_len, sizeof(int), 0);
	//    if (retval == SOCKET_ERROR) {
	//        err_display("send()");
	//        break;
	//    }

	//    retval = send(m_pGameClient->GetSOCKET(), (char*)&sendMsg, sizeof(int), 0);
	//    if (retval == SOCKET_ERROR) {
	//        err_display("send()");
	//        break;
	//    }
	//}

}

void WaitScene::Paint(HDC hDC) {
	TextOut(hDC, 100, 300, "WAIT SCENE", 11);
}

DWORD __stdcall WaitScene::TestThread(LPVOID arg)
{
	int retval;
	int len = 0;
	WaitScene* pWaitScene = (WaitScene*)arg;

	while (1) {
		WaitForSingleObject(hWaitWriteEvent, INFINITE);
		retval = recvn(pWaitScene->m_pGameClient->GetSOCKET(), (char*)&len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_quit("recv()");
			break;
		}
		else if (retval == 0)
			break;
		len = ntohl(len);

		retval = recvn(pWaitScene->m_pGameClient->GetSOCKET(), (char*)&pWaitScene->Msg, len, 0);
		if (retval == SOCKET_ERROR) {
			err_quit("recv()");
			break;
		}
		else if (retval == 0)
			break;
		pWaitScene->Msg = ntohl(pWaitScene->Msg);
		printf("%d\n", pWaitScene->Msg);

		if (pWaitScene->Msg == MSG_MatchMaking::Msg_PlayInGame) {
			SetEvent(hWaitReadEvent);
			break;
		}
		pWaitScene->Msg = 0;
		int sendMsg = htonl(pWaitScene->Msg);

		int MSG_len = htonl(sizeof(int));
		retval = send(pWaitScene->m_pGameClient->GetSOCKET(), (char*)&MSG_len, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		retval = send(pWaitScene->m_pGameClient->GetSOCKET(), (char*)&sendMsg, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}
		SetEvent(hWaitReadEvent);
	}
	return 0;
}

