#include "Scene.h"
#include "stdafx.h"

Scene::Scene() {}
Scene::Scene(SceneNum num, GameClient* const pGameClient) {
	m_SceneNum = num;
	m_pGameClient = pGameClient;
}
Scene::~Scene() {
	if (m_pGameClient != nullptr)
		delete m_pGameClient;
}

WaitScene::WaitScene() {}
WaitScene::~WaitScene() {}

PlayScene::PlayScene() {}
PlayScene::~PlayScene() {}

void PlayScene::draw_main() { //게임판 그리는 함수
    
}