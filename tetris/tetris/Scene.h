#pragma once

class Scene
{
public:
    Scene();
    ~Scene();
};

class TitleScene : public Scene {

    TitleScene();
    ~TitleScene();

    void update();
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
};