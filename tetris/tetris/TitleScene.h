#pragma once

class Scene;

class TitleScene : public Scene {

public:
    TitleScene();
    TitleScene(SceneNum num, GameClient* const pGameClient);
    ~TitleScene();

    void draw();
    void update();
};
