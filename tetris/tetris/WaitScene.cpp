#include "Scene.h"
#include "Waitscene.h"
#include "GameClient.h"
#include "stdafx.h"
#include "socket_function.h"

WaitScene::WaitScene() {}
WaitScene::WaitScene(SceneNum num, GameClient* const pGameClient) {
    m_SceneNum = num;
    m_pGameClient = pGameClient;
}
WaitScene::~WaitScene() {}

void WaitScene::Update(float fTimeElapsed) {
    int retval;

    // 윈속 초기화
    if (m_pGameClient->InitWSA() != 0) 
        return;

    // socket()
    m_pGameClient->SetSOCKET(socket(AF_INET, SOCK_STREAM, 0));
    if (m_pGameClient->GetSOCKET() == INVALID_SOCKET) err_quit("socket()");

    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(m_pGameClient->GetSOCKET(), (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    std::cout << "Waiting\n";
    CreateThread(NULL, 0, TestThread, (LPVOID)m_pGameClient->GetSOCKET(), 0, NULL);
    while (1) {
        
    }
}

DWORD __stdcall WaitScene::TestThread(LPVOID arg)
{
    int retval;
    int len = 0;
    int Msg;
    SOCKET client_sock = (SOCKET)arg;

    while (1) {
        retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
        if (retval == SOCKET_ERROR) {
            err_quit("recv()");
            break;
        }
        else if (retval == 0)
            break;
        len = ntohl(len);

        retval = recvn(client_sock, (char*)&Msg, len, 0);
        if (retval == SOCKET_ERROR) {
            err_quit("recv()");
            break;
        }
        else if (retval == 0)
            break;
        Msg = ntohl(Msg);
        printf("%d\n", Msg);
    }
    return 0;
}

