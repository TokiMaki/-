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


class WaitScene : public Scene {

    WaitScene();
    ~WaitScene();

    void update();
};

class PlayScene : public Scene {

    PlayScene();
    ~PlayScene();

    void update();
    void reset();
    void reset_main();
    void reset_main_cpy();
    void draw_map();
    void new_block();
    void draw_main();
};