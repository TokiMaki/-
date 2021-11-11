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