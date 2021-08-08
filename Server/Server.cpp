#include "Server.h"

Server::Server(int port, const std::string& ip)
{
	addr.port = port;
	if ( ip.empty() )
	{
		addr.host = ENET_HOST_ANY;
	}
	else
	{
		enet_address_set_host(&addr, ip.c_str());
	}
	game = GameEngine();
}

int Server::initialize()
{
	host = enet_host_create(&addr, 4, 2, 0, 0);
	
	if ( host == NULL )
	{
		std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
		
		return 1;
	}
	state = ONLINE;
	const size_t dummyID = 0;
	
	return 0;
}

void Server::sendPacket(ENetPeer* peer, const char* payload)
{
	ENetPacket* packet = enet_packet_create(payload, strlen(payload) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void Server::broadcastPacket(ENetHost* host, const char* payload)
{
//	std::cout << payload << std::endl;
	ENetPacket* packet = enet_packet_create(payload, strlen(payload) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, 0, packet);
}

const char* Server::syncClients(EntityVec toParse)
{
	char data[20] = {'\0'};
	char msg[100] = {'\0'};
	std::shared_ptr< CTransform > transform = nullptr;
	
	for (auto e : game.currentScene()->entities.getEntities("client"))
	{
		transform = e->getComponent< CTransform >(TRANSFORM);
		if ( transform )
		{
			snprintf(data, sizeof data, "%f %f\n", transform->pos.x, transform->pos.y);
			strcat(msg, data);
		}
	}
	for (auto e : toParse)
	{
		if ( e->tag().compare("client") == 0 )
		{
			
			continue;
		}
		transform = e->getComponent< CTransform >(TRANSFORM);
		if ( transform )
		{
			snprintf(data, sizeof data, "%f %f\n", transform->pos.x, transform->pos.y);
			strcat(msg, data);
		}
	}
	if ( transform )
	{
		broadcastPacket(host, msg);
//		msg[0] = '\0';
//		data[0] = '\0';
		
		return msg;
	}
	
	return "";
}

const char* Server::syncClient(ENetPeer* peer, EntityVec toParse)
{
	char data[20] = {'\0'};
	char msg[100] = {'\0'};
	std::shared_ptr< CTransform > transform = nullptr;
	
	for (auto e : game.currentScene()->entities.getEntities("client"))
	{
		transform = e->getComponent< CTransform >(TRANSFORM);
		if ( transform )
		{
			snprintf(data, sizeof data, "%f %f\n", transform->pos.x, transform->pos.y);
			strcat(msg, data);
		}
	}
	for (auto e : toParse)
	{
		if ( e->tag().compare("client") == 0 )
		{
			
			continue;
		}
		transform = e->getComponent< CTransform >(TRANSFORM);
		if ( transform )
		{
			snprintf(data, sizeof data, "%f %f\n", transform->pos.x, transform->pos.y);
			strcat(msg, data);
		}
	}
	if ( transform )
	{
		sendPacket(peer, data);
		
		return msg;
	}
	
	return "";
}

void Server::resetServer()
{
	state = ONLINE;
	for (auto e : game.currentScene()->entities.getEntities())
	{
		std::shared_ptr< CTransform > transform = e->getComponent< CTransform >(TRANSFORM);
		transform->pos.x = 500;
		transform->pos.y = 700;
	}
}

void Server::poll()
{
	if ( enet_host_service(host, &event, 100) > 0 )
	{
		switch(event.type)
		{
			case ENET_EVENT_TYPE_CONNECT:
				onConnect();
			break;
			
			case ENET_EVENT_TYPE_RECEIVE:
				onReceive();
			break;
			
			case ENET_EVENT_TYPE_DISCONNECT:
				onDisconnect();
			break;
			
		}
	}
}

void Server::onConnect()
{
	switch(state)
	{
		case OFFLINE:
		{
			
			
			break;
		}
		
		case ONLINE:
		{
			std::cout << "A new client connected from: " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
			std::shared_ptr< Entity > e = game.currentScene()->entities.addEntity("client");
			Vector2 spawnPoint = (Vector2){200.0, 800.0};
			Vector2 initScale = (Vector2){1.0, 1.0};
			float initRotate = 0;
			std::shared_ptr< Client > newClient;
			
			e->addComponent(std::make_shared< CTransform >(spawnPoint, initScale, initRotate));
			if ( connected.getClients().size() <= 1 )
			{
				newClient = connected.addClient(event.peer, "new", HOST);
			}
			else
			{
				newClient = connected.addClient(event.peer, "new");
			}

//			Rectangle kickRect = { 250, 50 + 110 * kickButtons.size(), 225, 50 };
//			kickButtons.push_back(TextButton(kickRect, "kick"));
//			Rectangle sendRect = { 250, 100 + 110 * sendButtons.size(), 225, 50 };
//			sendButtons.push_back(TextButton(sendRect, "send"));
			char clientID[2];
			snprintf(clientID, 2, "%u", newClient->id());
			std::cout << clientID << std::endl;
					
			sendPacket(event.peer, clientID);
			
			break;
		}
		
		case RUNNING:
		{
			std::cout << "Cannot join a running game" << std::endl;
			enet_peer_reset(event.peer);
			
			break;
		}
	}
}

void Server::onReceive()
{
	switch(state)
	{
		case OFFLINE:
		{
			
			
			break;
		}
		
		case ONLINE:
		{
			unsigned char * payload = event.packet->data;
			int length = event.packet->dataLength;
			std::string req(reinterpret_cast<char*>(payload), length);
					
			if ( connected.getClients().back()->user().compare("new") == 0 )
			{
				// TODO: fix race condition
				connected.getClients().back()->user() = req;
			}
			else if ( connected.getClients().size() > 1 )
			{
				if ( isdigit(req.front()) && isalpha(req.at(req.size() - 2)) )
				{
					int cid = req.front() - '0';
					char cmd = req.at(req.size() - 2);
					if ( cid < game.currentScene()->entities.getEntities("client").size() )
					{
						// TODO: only send the changes in input
						std::shared_ptr< Entity > entity = game.currentScene()->entities.getEntities("client").at(cid);
						game.handleInput(req, entity);
					}
					else
					{
						std::cout << "bad input: " << cid << " is not a valid index" << std::endl;
					}
				}
				// host commands
				else if ( event.peer->address.host == connected.getClients()[1]->conn()->address.host )
				{
					std::cout << "executing host command" << std::endl;
					if ( req.compare("/run") == 0 )
					{
						std::cout << "running game" << std::endl;
						state = RUNNING;
					}
				}
			}
			
			break;
		}
		
		case RUNNING:
		{
			unsigned char * payload = event.packet->data;
			int length = event.packet->dataLength;
			std::string req(reinterpret_cast<char*>(payload), length);
					
			if ( connected.getClients().back()->user().compare("new") == 0 )
			{
				// TODO: fix race condition
				connected.getClients().back()->user() = req;
			}
			else if ( connected.getClients().size() > 1 )
			{
				if ( isdigit(req.front()) && isalpha(req.at(req.size() - 2)) )
				{
					int cid = req.front() - '0';
					char cmd = req.at(req.size() - 2);
					std::cout << cid << std::endl;
					if ( cid < game.currentScene()->entities.getEntities().size() )
					{
						// TODO: only send the changes in input
						std::shared_ptr< Entity > entity = game.currentScene()->entities.getEntities("client")[cid];
						game.handleInput(req, entity);
					}
					else
					{
						std::cout << "bad input: " << cid << " is not a valid index" << std::endl;
					}
				}
				// host commands
				else if ( event.peer->address.host == connected.getClients()[1]->conn()->address.host )
				{
					if( req.compare("/stop") == 0 )
					{
						resetServer();
					}
				}
			}
			
			break;
		}
	}
}

void Server::onDisconnect()
{
	switch(state)
	{
		case OFFLINE:
		{
			
			
			break;
		}
		
		case ONLINE:
		{
			std::cout << event.peer->address.host << ":" << event.peer->address.port << " has disconnected" << std::endl;
			event.peer->data = NULL;
			for (auto c : connected.getClients())
			{
				if ( c->conn() )
				{
					if ( c->conn()->address.host == event.peer->address.host )
					{
						connected.removeClient(c);
						game.currentScene()->entities.removeEntity(game.currentScene()->entities.getEntities("client")[c->id()]);
							
						break;
					}
				}
			}
			
			break;
		}
		
		case RUNNING:
		{
			std::cout << event.peer->address.host << ":" << event.peer->address.port << " has disconnected" << std::endl;
			event.peer->data = NULL;
			for (auto c : connected.getClients())
			{
				if ( c->conn() )
				{
					if ( c->conn()->address.host == event.peer->address.host )
					{
						connected.removeClient(c);
						game.currentScene()->entities.removeEntity(game.currentScene()->entities.getEntities()[c->id()]);
							
						break;
					}
				}
			}
			
			break;
		}
	}
}

EntityManager& Server::entities()
{
	
	return game.currentScene()->entities;
}

AppState Server::currentState()
{
	
	return state;
}

void Server::destroy()
{
	enet_host_destroy(host);
	state = OFFLINE;
}

AppState Server::toggleState()
{
	switch(state)
	{
		case ONLINE:
			state = RUNNING;
		break;
			
		case RUNNING:
			state = ONLINE;
		break;
	}
	
	return state;
}

void Server::kick(std::shared_ptr< Client > toKick)
{
	enet_peer_disconnect_now(toKick->conn(), 0);
	//enet_peer_reset(connected[index]);
	connected.removeClient(toKick);
	std::cout << game.currentScene()->entities.getEntities("client").size() << std::endl;
	std::cout << toKick->id() << std::endl;
	game.currentScene()->entities.removeEntity(game.currentScene()->entities.getEntities("client").at(toKick->id()));
}

void Server::banName(std::shared_ptr< Client > toBan)
{
	banNames.push_back(toBan->user());
	kick(toBan);
}

void Server::banName(const std::string& name)
{
	banNames.push_back(name);
}

void Server::banIP(std::shared_ptr< Client > toBan)
{
	ENetPeer* peer = toBan->conn();
	if ( peer )
	{	
		banIPs.push_back(peer->address);
	}
	kick(toBan);
}

void Server::update()
{
	connected.update();
	game.currentScene()->entities.update();
	syncClients(game.currentScene()->entities.getEntities());
}
