#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#include "stdafx.h"
#include "socket_function.h"
#include "MatchMaking.h"
#include "GameServer.h"
using namespace std;

vector<SOCKET> MatchMakingQ; //대기 중인 클라이언트 소켓을 저장

int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    string serverComponent;
    ifstream readFile;
    readFile.open("Server.txt");
    if (readFile.is_open())
    {
        while (!readFile.eof())
        {
            getline(readFile, serverComponent);
        }
        readFile.close();
    }
    int serverPort = stoi(serverComponent);

    serveraddr.sin_port = htons(serverPort);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    HANDLE MatchMaking = CreateThread(NULL, 0, MatchMakingThread, &MatchMakingQ, 0, NULL);

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen;

    //std::cout << sizeof(ClientGameData) << std::endl;
    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }
        MatchMakingQ.push_back(client_sock);
        printf("[TCP 서버] 클라이언트 접속: 소켓번호=%d IP 주소=%s, 포트 번호=%d\n",
            client_sock, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    }

    // closesocket()
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}