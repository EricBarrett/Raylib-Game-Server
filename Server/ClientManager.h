#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "Client.h"

typedef std::vector<std::shared_ptr<Client>> ClientVec;

class ClientManager
{
private:
	ClientVec m_clients;
	ClientVec m_toAdd;
	std::unordered_map< std::string, ClientVec > m_clientMap;
	size_t m_totalClients = 0;
public:
	ClientManager();
	void clearClients();
	void update();
	void removeClient(std::shared_ptr< Client > client);
	std::shared_ptr< Client > addClient(ENetPeer* peer, std::string username, Rank rank =  GUEST);
	ClientVec & getClients();
	ClientVec & getClients(const std::string & tag);
	size_t size();
};