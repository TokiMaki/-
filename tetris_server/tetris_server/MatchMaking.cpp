#include "stdafx.h"
#include "MatchMaking.h"


DWORD WINAPI MatchMakingThread(LPVOID arg)
{
	//std::queue<SOCKADDR>* MatchMakingQ = (std::queue<SOCKADDR>*)arg;
	std::queue<int>* test = (std::queue<int>*)arg;
	std::cout << "Now runnning Match Making Thread :)\n";

	while (!test->empty()) {
		std::cout << test->front();
		test->pop();
	}
	std::cout << "\nMatch Making Thread END :(\n";
	
	return 0;
}

bool isMatchMakingQFull()
{
	return false;
}

void CreateGameServerThread()
{
}

void MatchMakingQ_DeQ()
{
}
