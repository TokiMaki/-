#pragma once

// ���� �Լ� ���� ��� �� ����
void err_quit(char* msg);

// ���� �Լ� ���� ���
void err_display(char* msg);

// ����� ���� ������ ���� �Լ�
int recvn(SOCKET s, char* buf, int len, int flags);