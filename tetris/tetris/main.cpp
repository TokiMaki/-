#include "GameClient.h"
#include "Timer.h"

GameClient gameClient;

int main() {
    while (1) {
        gameClient.Update();
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    

    return 0;
}
