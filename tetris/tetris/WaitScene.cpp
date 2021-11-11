#include "Scene.h"
#include "Waitscene.h"
#include "stdafx.h"

WaitScene::WaitScene() {}
WaitScene::WaitScene(SceneNum num, GameClient* const pGameClient) {
    m_SceneNum = num;
    m_pGameClient = pGameClient;
}
WaitScene::~WaitScene() {}