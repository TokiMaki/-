#include "GameClient.h"
#include "TitleScene.h"
#include "WaitScene.h"
#include "GamePlayScene.h"

GameClient::GameClient() {
    BuildScene();
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
}

void GameClient::Update(float fTimeElapsed) {
    m_GameTimer.Tick(0.0f);

}