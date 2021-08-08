#include "GameEngine.h"

GameEngine::GameEngine()
{
	std::shared_ptr<Level> level;
	level = std::make_shared< Level >();
	scene = level;
	scene->load();
}

void GameEngine::handleInput(const std::string& req, std::shared_ptr< Entity > entity)
{
	char cmd = req.at(req.size() - 2);
	
	// TODO: only send the changes in input
	std::shared_ptr< CTransform > transform = entity->getComponent< CTransform >(TRANSFORM);
	char up = 'w';
	char left = 'a';
	char down = 's';
	char right = 'd';
	if ( cmd == up )
	{
		transform->pos.y -= 1;
	}
	else if ( cmd == left )
	{
		transform->pos.x -= 1;
	}
	else if ( cmd == down )
	{
		transform->pos.y += 1;
	}
	else if ( cmd == right )
	{
		transform->pos.x += 1;
	}
}

std::shared_ptr< Scene > GameEngine::currentScene()
{
		
	return scene;
}