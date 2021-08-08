#pragma once

#include "ClientManager.h"
#include "GameEngine.h"

enum AppState {OFFLINE, ONLINE, RUNNING};

class Server
{
private:
	GameEngine game;
	const int CONN_TIME = 5000;
	const int DISCONN_TIME = 3000;
	AppState state = OFFLINE;
	std::vector< std::string > banNames;
	std::vector< ENetAddress > banIPs;
	ENetAddress addr;
	ENetEvent event;
	ENetHost * host;

public:
	ClientManager connected;

	Server(int port = 1234, const std::string& ip = "");
	int initialize();
	void sendPacket(ENetPeer* peer, const char* payload);
	void broadcastPacket(ENetHost* host, const char* payload);
	const char* syncClients(EntityVec toParse);
	const char* syncClient(ENetPeer* peer, EntityVec toParse);
	void resetServer();
	void poll();
	void onConnect();
	void onReceive();
	void onDisconnect();
	EntityManager& entities();
	AppState currentState();
	void destroy();
	AppState toggleState();
	void kick(std::shared_ptr< Client >);
	void banName(std::shared_ptr< Client >);
	void banName(const std::string& name);
	void banIP(std::shared_ptr< Client >);
	void update();
};