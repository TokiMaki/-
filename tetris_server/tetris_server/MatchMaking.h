#pragma once
DWORD WINAPI MatchMakingThread(LPVOID arg); //��ġ����ŷ ���� �������, ���� ���μ��� ���� �� ������ �Բ� ����

bool isMatchMakingQFull();
//���� MatchMakingQ���� ����ϴ� Ŭ���̾�Ʈ�� ������ �ľ��� MAX_PLAYER �̸��� ��� MatchMakingQ�� �߰��Ǿ� �ִ� Ŭ���̾�Ʈ�鿡�� Msg_wait�� ���� 
//MAX_PLAYER �̻��� ��� MatchMakingQ�� �߰��Ǿ� �ִ� ������� MAX_PLAYER���� Ŭ���̾�Ʈ���� Msg_PlayInGame�� �����ְ� CreateGameServerThread�� MatchMakingQ_DeQ�� ����

void CreateGameServerThread();
//GameServerThread�� �����ϸ�, MatchMakingQ�� ���ϵ��� ����

void MatchMakingQ_DeQ();
//GameServerThread�� �Ѿ MatchMakingQ�� Ŭ���̾�Ʈ ������ ����

enum MSG_MatchMaking //��ġ����ŷ �ý��ۿ��� ����� �޽���
{
	Msg_Ready,			//Ŭ���̾�Ʈ�� �������� ��������� �˸�
	Msg_ReadyCancel,	//Ŭ���̾�Ʈ�� �������� ��� ��Ҹ� �˸�
	Msg_ConfirmCancel,	//������ Ŭ���̾�Ʈ���� ��� ��� �޾����� ������
	Msg_PlayInGame,		//������ Ŭ���̾�Ʈ���� ������ ���� ������ �˷���
	Msg_WaitGame		//������ Ŭ���̾�Ʈ���� ���� ��Ī�� ���� �ʾ����� �˷���
};