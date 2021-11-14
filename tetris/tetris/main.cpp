#include "GameClient.h"
#include "socket_err.h"
#include "Timer.h"

GameClient gameClient;

int main() {
    while (1) {
        gameClient.Update();
    }
}