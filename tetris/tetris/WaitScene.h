#pragma once
#include "stdafx.h"
class Scene;

class WaitScene : public Scene {
public:
    WaitScene();
    WaitScene(SceneNum num, GameClient* const pGameClient);
    ~WaitScene();
    int Msg = 0;

    void InitScene() override;
    void Update(float fTimeElapsed) override;
    static DWORD WINAPI TestThread(LPVOID arg);
};