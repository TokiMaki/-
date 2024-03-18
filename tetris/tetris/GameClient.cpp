#include "GameClient.h"
#include "TitleScene.h"
#include "WaitScene.h"
#include "GamePlayScene.h"

using namespace std;

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

void GameClient::ScreenRotate(HDC hDC, RECT rt)
{
	if (m_pScene->GetSceneNum() == Scene::SceneNum::GamePlay) {
		GamePlayScene* TempScene = (GamePlayScene*)m_pScene;
		TempScene->ScreenRotate(hDC, rt);
	}
}

void GameClient::SetSOCKET(SOCKET arg)
{
	LINGER optval;
	optval.l_onoff = 1;
	optval.l_linger = 1;
	sock = arg;
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&optval, sizeof(optval));
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
	int retval;
	string serverComponent[2];
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	ifstream readFile;
	readFile.open("Server.txt");
	if (readFile.is_open()) 
	{
		int cnt = 0;
		while (!readFile.eof()) 
		{
			string str;
			getline(readFile, str);
			serverComponent[cnt++] = str;
		}
		readFile.close();
	}
	char* serverIP = const_cast<char*>(serverComponent[0].c_str());
	int serverPort = stoi(serverComponent[1]);
	serveraddr.sin_addr.s_addr = inet_addr(serverIP);
	serveraddr.sin_port = htons(serverPort);
	retval = connect(GetSOCKET(), (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
}

void GameClient::TitleSceneSend(enum MSG_MatchMaking tag) {

}
