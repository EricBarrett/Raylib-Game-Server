#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{
	
}

void EntityManager::update()
{
	EntityVec new_vec;
	std::unordered_map<std::string, EntityVec> new_map;
	for (auto e : m_entities)
	{
		if (e->isActive())
		{
			new_vec.push_back(e);
			new_map[e->tag()].push_back(e);
		}
	}
	if (!m_toAdd.empty())
	{
		for (auto a : m_toAdd)
		{
			new_vec.push_back(a);
			new_map[a->tag()].push_back(a);
		}
		m_toAdd.clear();
	}
	m_entities = new_vec;
	m_entityMap = new_map;
	m_totalEntities = new_vec.size();
}

void EntityManager::clearEntities()
{
	m_entities.clear();
	m_entityMap.clear();
}

void EntityManager::removeEntity(std::shared_ptr<Entity> entity)
{
	entity->destroy();
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string & tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_toAdd.push_back(e);
	return e;
}

EntityVec & EntityManager::getEntities()
{
	return m_entities;
}

EntityVec & EntityManager::getEntities(const std::string & tag)
{
	return m_entityMap[tag];
}


