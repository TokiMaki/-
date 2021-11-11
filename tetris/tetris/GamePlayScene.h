#pragma once

class Scene;

class GamePlayScene : public Scene {

    GamePlayScene();
    ~GamePlayScene();

    void update();
    void reset();
    void reset_main();
    void reset_main_cpy();
    void draw_map();
    void new_block();
    void draw_main();
};