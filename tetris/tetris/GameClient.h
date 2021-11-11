#pragma once
#include "stdafx.h"
#include "Scene.h"
#include "Timer.h"

class GameClient
{
private:
	// WSADATA wsa;
	// SOCKET sock;
	KeyInput m_keys;                                //Ŭ���̾�Ʈ�� Ű�Է��� ����
	Gamestatus m_gamestatus[MAX_PLAYER];  	        //�� Ŭ���̾�Ʈ�� ���� ����

	CGameTimer m_GameTimer;

	void err_quit(char* msg);                       //���� �߻� �� �ش� ���� ���
	void err_display(char* msg);

public:
	GameClient();    //���� �� �����ʱ�ȭ, ���� ����, connect�� ����
	~GameClient(); //�Ҹ� �� closesocket �� �������� ����
	Scene* m_pScene;
	Scene* m_arrScene[Scene::SceneNum::Scene_Count];


	void ChangeScene(Scene::SceneNum tag);
	void BuildScene();
	void Update(float fTimeElapsed);
//public:
//	void TitleSceneSend(enum MSG_MathcingSystem);
//	//Msg_Ready �� Msg_ReadyCancel �� MatchingThread �� ����
//	int TitleSceneRecv();
//	//MatchMakingThread �κ��� ���� �޽����� ����
//	// Msg_ConfirmReadyCancel: Msg_ReadyCancel //���� ��, ���� �ʿ��� ������ Ȯ�ε����� Ȯ���ϰ� ���� ����
//	// Msg_PlayInGame : PlayScene ���� ��ȯ
//	// Msg_WaitGame : ���� Scene���� Msg_PlayInGame�� ���������� ���
//	void PlaySceneSend(); //KeyInput m_keys �� ���� Commthread �� ����
//	void PlaySceneRecv(); //GameServer ���� ����� ���� CommThread �κ���
//	// ���� - ���� ������ ���·� ����, ������ char�����̸�, ������GameStatus m_gamestatus�� ����
//	void UpdateData(Gamestatus* recvGSData);
//	//�������� ���� �����ͷ� ���� Ŭ���̾�Ʈ�� �����͸� ������Ʈ
//	void SetKeyInput(KeyInput* keyinput)
//	//������ �� Ŭ���̾�Ʈ�� Ű ���� �޾Ƽ� ����

};