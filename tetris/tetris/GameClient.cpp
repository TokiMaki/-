#include "GameClient.h"
#include "TitleScene.h"
#include "WaitScene.h"
#include "GamePlayScene.h"

GameClient::GameClient() {
    BuildScene();
    m_GameTimer.Start();
    ChangeScene(Scene::SceneNum::Title);
}

GameClient::~GameClient() {}

// 소켓 함수 오류 출력 후 종료
void GameClient::err_quit(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// 소켓 함수 오류 출력
void GameClient::err_display(char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

void GameClient::BuildScene() {
    m_arrScene[Scene::SceneNum::Title] = new TitleScene(Scene::SceneNum::Title, this);
    m_arrScene[Scene::SceneNum::Wait] = new WaitScene(Scene::SceneNum::Wait, this);
    m_arrScene[Scene::SceneNum::GamePlay] = new GamePlayScene(Scene::SceneNum::GamePlay, this);
}

void GameClient::ChangeScene(Scene::SceneNum tag) {
    m_pScene = m_arrScene[tag];
    m_pScene->InitScene();
}

void GameClient::Update() {
    m_GameTimer.Tick(60.0f);
    m_pScene->Update(m_GameTimer.GetTimeElapsed());
}

void GameClient::Paint(HDC hDC) {
    m_pScene->Paint(hDC);
}

void GameClient::KeyDown(unsigned char KEYCODE)
{
    m_pScene->KeyDown(KEYCODE);
}

void GameClient::KeyUp(unsigned char KEYCODE)
{
    m_pScene->KeyUp(KEYCODE);
}

void GameClient::SetSOCKET(SOCKET arg)
{
    sock = arg;
}

SOCKET GameClient::GetSOCKET()
{
    return sock;
}

bool GameClient::InitWSA()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;
    return true;
}

void GameClient::ConnetServer()
{
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(GetSOCKET(), (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");
}

void GameClient::TitleSceneSend(enum MSG_MatchMaking tag) {

}
