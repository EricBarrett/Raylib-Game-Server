#include <iostream>
#include <array>
#include <string>

// handle global namespace mess
#define NOGDI
#define CloseWindow CloseWinWindow
#define ShowCursor ShowWinCursor
#define LoadImage LoadWinImage
#define DrawText DrawWinText
#define DrawTextEx DrawWinTextEx
#define PlaySound PlayWinSound

#include "ClientManager.h"
#undef CloseWindow
#undef Rectangle
#undef ShowCursor
#undef LoadImage
#undef DrawText
#undef DrawTextEx
#undef PlaySound

#include "EntityManager.h"
#include "../GUI/gui.h"

// TODO: config file
#define PORT 1234

const std::string HOST_IP = "";
enum AppState { OFFLINE, ONLINE, RUNNING };
std::array< std::string, 2 > offlineText = { { "OFFLINE", "Boot Up" } };
std::array< std::string, 2 > onlineText = { { "ONLINE", "Shutdown" } };
std::array< std::string, 2 > runText = { { "RUNNING", "Stop" } };
std::array< Vector2, 4 > initPos = { { (Vector2){ 200, 800 }, (Vector2){ 400, 800 }, (Vector2){ 600, 800 }, (Vector2){ 200, 800 } } }; 
Color offlineCol = RED;
Color onlineCol = GREEN;
std::unordered_map< AppState, std::array< std::string, 2 > > stateStr = { { OFFLINE, offlineText }, { ONLINE, onlineText }, { RUNNING, runText } };
std::unordered_map< AppState, Color > stateCol = { { OFFLINE, offlineCol }, { ONLINE, onlineCol }, {RUNNING, onlineCol} };
Vector2 initVel = { 1, 0 };
Vector2 initScale = { 1, 1 };
float initRotate = 0;

std::vector< std::string > banNames;
std::vector< ENetAddress > banIPs;
std::vector< TextButton > kickButtons;
std::vector< TextButton > sendButtons;

void sendPacket(ENetPeer* peer, const char* payload)
{
	ENetPacket* packet = enet_packet_create(payload, strlen(payload) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void broadcastPacket(ENetHost* host, const char* payload)
{
	std::cout << payload << std::endl;
	ENetPacket* packet = enet_packet_create(payload, strlen(payload) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, 0, packet);
}

int main(int argc, char ** argv)
{
	const int screenWidth = 800;
	const int screenHeight = 500;
	char msg[20];
	ClientManager connected;
	AppState state = OFFLINE;
	if ( enet_initialize() != 0 )
	{
		std::cerr << "An error occurred while initializing ENet!" << std::endl;
		
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	
	InitWindow(screenWidth, screenHeight, "Server Interface");
	SetTargetFPS(60);
	Rectangle stateRect = { screenWidth - 325, 0, 325, 50 };
	TextButton stateButton = TextButton(stateRect, stateStr[state][1].c_str());
	Rectangle kickRect = { 250, 50, 225, 50 };
	kickButtons.push_back(TextButton(kickRect, "kick"));
	Rectangle sendRect = { 250, 100, 225, 50 };
	sendButtons.push_back(TextButton(sendRect, "send"));
	
	ENetAddress addr;
	ENetEvent event;
	ENetHost * server;
	if (HOST_IP.size() > 0)
	{
		enet_address_set_host(&addr, HOST_IP.c_str());
	}
	else
	{
		addr.host = ENET_HOST_ANY;
	}
	addr.port = PORT;
	server = enet_host_create(&addr, 4, 2, 0, 0);
	
	if ( server == NULL )
	{
		std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
		exit(EXIT_FAILURE);
	}
	state = ONLINE;
	const size_t dummyID = 0;
	connected.addClient(nullptr, dummyID, "all", ADMIN);
	EntityManager entities;
	std::shared_ptr< Entity > teste = entities.addEntity("test");
	teste->addComponent(std::make_shared< CTransform >((Vector2) { 500, 700 }, initScale, initRotate));
	entities.update();
	
	// main loop
	while( !WindowShouldClose() )
	{
		// entities
		if ( state == RUNNING || state == ONLINE)
		{
			connected.update();
			entities.update();
			std::shared_ptr< CTransform > transform = nullptr;
			// server objects
			for (auto e : entities.getEntities("client"))
			{
				transform = e->getComponent< CTransform >(TRANSFORM);
			}
			if ( state == RUNNING )
			{
				for (auto e : entities.getEntities("test"))
				{
					transform = e->getComponent< CTransform >(TRANSFORM);
					transform->pos.x += initVel.x;
					transform->pos.y += initVel.y;
				}
//				std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
//				snprintf(msg, sizeof msg, "%f %f", transform->pos.x, transform->pos.y);
//				broadcastPacket(server, msg);
			}
			if ( transform )
			{
				snprintf(msg, sizeof msg, "%f %f", transform->pos.x, transform->pos.y);
				broadcastPacket(server, msg);
			}
			
		}
		// GUI
		if ( stateButton.isFocus() )
		{
			switch(state)
			{
				case OFFLINE:
				{
					std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
					transform->pos.x = initPos[2].x + 100;
					transform->pos.y = initPos[2].y - 300;
					server = enet_host_create(&addr, 4, 2, 0, 0);
	
					if ( server == NULL )
					{
						std::cerr << "An error occurred while trying to create an ENet server host." << std::endl;
					}
					else
					{
						state = ONLINE;
					}
				
					break;
				}
				
				case ONLINE:
				{
					enet_host_destroy(server);
					state = OFFLINE;
				
					break;
				}
				
				case RUNNING:
				{
					state = ONLINE;
					std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
					transform->pos.x = initPos[2].x + 100;
					transform->pos.y = initPos[2].y - 300;
					
					break;
				}
			}
		}
		stateButton.text = stateStr[state][1];
		stateButton.textCol = stateCol[state];
		
		int index = -1;
		int buttonType = -1;
		for (int i=0; i < kickButtons.size(); i++)
		{
			if ( kickButtons[i].isFocus() )
			{
				index = i;
				buttonType = 0;
				
				break;
			}
			else if ( sendButtons[i].isFocus() )
			{
				index = i;
				buttonType = 1;
				
				break;
			}
		}
		if ( index > 0 )
		{
			switch(buttonType)
			{
				case 0:
				{
					std::shared_ptr< Client > kicked = connected.getClients()[index];
					enet_peer_disconnect_now(kicked->conn(), 0);
					//enet_peer_reset(connected[index]);
					connected.removeClient(kicked);
				
					break;
				}
				
				case 1:
				{
//					std::cout << teste->tag() << std::endl;
					std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
					snprintf(msg, sizeof msg, "%f %f", transform->pos.x, transform->pos.y);
					
					sendPacket(connected.getClients()[index]->conn(), msg);
					
					break;
				}
			}
			
		}
		else if ( index == 0 )
		{
			switch(buttonType)
			{
				case 0:
				{
					for (auto it=connected.getClients().begin() + 1; it != connected.getClients().end(); it++)
					{
						//std::cout << i << std::endl;
						//std::shared_ptr< Client > kicked = connected.getClients()[i];
						enet_peer_disconnect_now((*it)->conn(), 0);
						//enet_peer_reset(connected[i]);
						connected.removeClient((*it));
						//connected.update();
					}
					
					break;
				}
				
				case 1:
				{
//					std::cout << teste->tag() << std::endl;
					std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
					snprintf(msg, sizeof msg, "%f %f", transform->pos.x, transform->pos.y);
					broadcastPacket(server, msg);
				
					break;
				}
			}
		}
		
		// draw
		BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText(stateStr[state][0].c_str(), 5, 5, 40, stateCol[state]);
			stateButton.draw();
			for (int i=0;  i < connected.getClients().size(); i++)
			{
				DrawRectangle(5, 50 + 110 * i, 225, 100, LIGHTGRAY);
				DrawText(connected.getClients()[i]->user().c_str(), 10, 80 + 110 * i, 40, MAROON);
				kickButtons.at(i).draw();
				sendButtons.at(i).draw();
			}
		EndDrawing();
		
		// poll
		if( enet_host_service(server, &event, 100) > 0 )
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					std::cout << "A new client connected from: " << event.peer->address.host << ":" << event.peer->address.port << std::endl;
					std::shared_ptr< Entity > e = entities.addEntity("client");
					e->addComponent(std::make_shared< CTransform >(initPos[connected.size()], initScale, initRotate));
					if ( connected.getClients().size() <= 1 )
					{
						connected.addClient(event.peer, e->id(), "new", HOST);
					}
					else
					{
						connected.addClient(event.peer, e->id(), "new");
					}
					
					Rectangle kickRect = { 250, 50 + 110 * kickButtons.size(), 225, 50 };
					kickButtons.push_back(TextButton(kickRect, "kick"));
					Rectangle sendRect = { 250, 100 + 110 * sendButtons.size(), 225, 50 };
					sendButtons.push_back(TextButton(sendRect, "send"));
					char clientID[2];
					snprintf(clientID, 2, "%u", e->id());
					std::cout << clientID << std::endl;
					//itoa(e->id(), clientID, 10);
					
					sendPacket(event.peer, clientID);
				
					break;
				}
				
				case ENET_EVENT_TYPE_RECEIVE:
				{
					std::cout << "RECEIVED PACKET" << std::endl << "packet size: " << event.packet->dataLength << std::endl << "payload: " << event.packet->data << std::endl << "host: " << event.peer->address.host << std::endl << "channel: " << event.channelID << std::endl;
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
							if ( cid < entities.getEntities().size() )
							{
								std::shared_ptr< Entity > entity = entities.getEntities()[cid];
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
							else
							{
								std::cout << "bad input: " << cid << " is not a valid index" << std::endl;
							}
						}
						else if ( event.peer->address.host == connected.getClients()[1]->conn()->address.host )
						{
							if ( req.compare("/run") == 0 && state == ONLINE )
							{
								std::cout << "running game" << std::endl;
								state = RUNNING;
							}
							else if( req.compare("/stop") == 0 && state == RUNNING )
							{
								state = ONLINE;
								std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
								transform->pos.x = initPos[2].x + 100;
								transform->pos.y = initPos[2].y - 300;
							}
						}
					}
					
					break;
				}
				
				case ENET_EVENT_TYPE_DISCONNECT:
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
							
								break;
							}
						}
					}
					
					break;
				}
			}
		}
	}
	
	CloseWindow();
	enet_host_destroy(server);
	
	exit(EXIT_SUCCESS);
	
	return 0;
}