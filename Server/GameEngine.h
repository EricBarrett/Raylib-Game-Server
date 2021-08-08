#pragma once

#include "Scene.h"

class GameEngine
{
private:
	std::shared_ptr< Scene > scene;
public:
	GameEngine();
	void handleInput(const std::string& req, std::shared_ptr< Entity > entity);
	std::shared_ptr< Scene > currentScene();
};