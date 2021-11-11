#pragma once

class Scene;

class GamePlayScene : public Scene {

public:
    GamePlayScene();
    GamePlayScene(SceneNum num, GameClient* const pGameClient);
    ~GamePlayScene();

    void update();
    void reset();
    void reset_main();
    void reset_main_cpy();
    void draw_map();
    void new_block();
    void draw_main();
};