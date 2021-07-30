#pragma once

#include "Component.h"

#include <iostream>

typedef std::variant< std::shared_ptr<CBBox>, std::shared_ptr<CTransform>, std::shared_ptr<CAsset>, std::shared_ptr<CGravity>, std::shared_ptr<CController>, std::shared_ptr<CDuration>, std::shared_ptr<CAnim>, std::shared_ptr<CEmitter> > Cent;

class Entity
{
	friend class EntityManager;
	std::unordered_map< ComponentKey, Cent > m_components;
	const size_t m_id = 0;
	const std::string m_tag = "default";
	bool m_active = true;
	
	void destroy()
	{
		m_active = false;
	}
	
public:
	Entity(const size_t & id, const std::string & tag, bool active = true)
		: m_id(id), m_tag(tag), m_active(active) {}
	
	template <class C>
	std::shared_ptr<C> getComponent(ComponentKey key)
	{
		//std::cout << "fetching: " << key << std::endl;
		Cent val = m_components[key];
		
		if (auto pval = std::get_if<std::shared_ptr< C > >(&val))
		{
			return *pval;
		}
		else
		{
//			std::cerr << "failed to get value at index " << key << std::endl;
//			std::cerr << "required index: " << val.index() << std::endl;
			
			return nullptr;
		}
		
//		return std::get<std::shared_ptr< C > >(val); // pretty dangerous
	}
	
	template <class C>
	void addComponent(std::shared_ptr<C> component)
	{
		if (component)
		{
			m_components[component->type] = component; // also pretty dangerous
		}
	}
	
	void removeComponent(ComponentKey key)
	{
		m_components.erase(key);
	}
	
	void clearComponents()
	{
		m_components.clear();
	}
	
	bool isActive() const
	{
		return m_active;
	}
	
	const std::string & tag() const
	{
		return m_tag;
	}
	
	const size_t & id() const
	{
		return m_id;
	}
};