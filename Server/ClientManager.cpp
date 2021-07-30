#include "ClientManager.h"
#include <iostream>

ClientManager::ClientManager()
{
	
}

void ClientManager::update()
{
	ClientVec new_vec;
	std::unordered_map< std::string, ClientVec > new_map;
	for (auto c : m_clients)
	{
		if (c->isActive())
		{
			new_vec.push_back(c);
			new_map[c->user()].push_back(c);
		}
	}
	if (!m_toAdd.empty())
	{
		for (auto a : m_toAdd)
		{
			new_vec.push_back(a);
			new_map[a->user()].push_back(a);
		}
		m_toAdd.clear();
	}
	m_clients = new_vec;
	m_clientMap = new_map;
}

void ClientManager::clearClients()
{
	m_clients.clear();
	m_clientMap.clear();
}

void ClientManager::removeClient(std::shared_ptr< Client > client)
{
	client->destroy();
	m_totalClients--;
}

std::shared_ptr< Client > ClientManager::addClient(ENetPeer* peer, const size_t& id, std::string username, Rank rank)
{
	m_totalClients++;
	auto e = std::shared_ptr< Client >(new Client(peer, id, username, rank));
	m_toAdd.push_back(e);
	
	return e;
}

ClientVec & ClientManager::getClients()
{
	
	return m_clients;
}

ClientVec & ClientManager::getClients(const std::string & username)
{
	
	return m_clientMap[username];
}

size_t ClientManager::size()
{
	
	return m_totalClients;
}