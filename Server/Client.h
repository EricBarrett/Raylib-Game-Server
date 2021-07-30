#pragma once

#include <enet/enet.h>

enum Rank { HOST, GUEST, MOD, ADMIN };

class Client
{
	friend class ClientManager;
private:
	ENetPeer* peer;
	Rank rank;
	const size_t eid = 0;
	bool active = true;
	std::string name = "new";
	
	void destroy()
	{
		
		active = false;
	}
public:
	Client()
		: peer(nullptr), name("all"), rank(ADMIN) {}
	
	Client(ENetPeer* p, const size_t id, std::string n, Rank r = GUEST)
		: peer(p), eid(id), name(n), rank(r) {}
	
	bool isActive() const
	{
		
		return active;
	}
	
	std::string & user()
	{
		
		return name;
	}
	
	const size_t & id() const
	{
		
		return eid;
	}
	
	ENetPeer* conn()
	{
	
		return peer;
	}
};