#pragma once
#define _WINSOCKAPI_
#include <Windows.h>

class GameClient;

class Scene
{
public:
    enum SceneNum {
        Title,
        Wait,
        GamePlay,
        Scene_Count
    };
    Scene();
    Scene(SceneNum num, GameClient* const pGameClient);
    ~Scene();
    const SceneNum GetSceneNum() { return m_SceneNum; };

    virtual void InitScene() = 0;
    virtual void Update(float fTimeElapsed) = 0;
    virtual void Paint(HDC hDC) = 0;
    virtual void KeyDown(unsigned char KEYCODE) = 0;
    virtual void KeyUp(unsigned char KEYCODE) = 0;

protected:
    SceneNum m_SceneNum;
    GameClient* m_pGameClient;
};

