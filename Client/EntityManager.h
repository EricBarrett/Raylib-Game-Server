#pragma once

#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;

class EntityManager
{
private:
	EntityVec m_entities;
	EntityVec m_toAdd;
	std::unordered_map<std::string, EntityVec> m_entityMap;
	size_t m_totalEntities = 0;
public:
	EntityManager();
	void clearEntities();
	void update();
	void removeEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> addEntity(const std::string & tag);
	EntityVec & getEntities();
	EntityVec & getEntities(const std::string & tag);
};
