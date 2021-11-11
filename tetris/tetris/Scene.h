#pragma once

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

protected:
    SceneNum m_SceneNum;
    GameClient* m_pGameClient;
};

