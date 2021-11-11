#include "Scene.h"
#include "GamePlayScene.h"
#include "stdafx.h"

GamePlayScene::GamePlayScene() {}
GamePlayScene::GamePlayScene(SceneNum num, GameClient* const pGameClient) {
	m_SceneNum = num;
	m_pGameClient = pGameClient;
}
GamePlayScene::~GamePlayScene() {}

void GamePlayScene::draw_main() { //게임판 그리는 함수

}