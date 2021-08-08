#include "Scene.h"

void Level::load(const std::string &path)
{
	m_filePath = (path.empty()) ? m_filePath : path;
	entities = EntityManager(); // clear entities
//	std::ifstream input(m_filePath);
//	m_cam.target = (Vector2){ m_game->getWindowConfig().width / 2.0f, m_game->getWindowConfig().height / 2.0f};
//	m_cam.offset = (Vector2){ m_game->getWindowConfig().width / 2.0f, m_game->getWindowConfig().height / 2.0f};
//	m_cam.rotation = 0.0f;
//	m_cam.zoom = 1.0f;
}

void Level::update()
{
	
}

void Level::sMovement()
{
	
}

void Level::sRender()
{
	
}

// Only checks for player collision
void Level::sCollision(std::shared_ptr<Entity> entity)
{
	
}