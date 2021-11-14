#include "GameClient.h"
#include "Timer.h"

GameClient gameClient;

int main() {
    while (1) {
        gameClient.Update();
    }
}