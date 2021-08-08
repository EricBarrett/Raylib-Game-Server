#include <iostream>
#include <array>
#include <string>

// handle global namespace mess

// TODO: config file
#define PORT 1234

#include "Server.h"
#include "../GUI/gui.h"

std::array< std::string, 2 > offlineText = { { "OFFLINE", "Boot Up" } };
std::array< std::string, 2 > onlineText = { { "ONLINE", "Shutdown" } };
std::array< std::string, 2 > runText = { { "RUNNING", "Stop" } };
Color offlineCol = RED;
Color onlineCol = GREEN;
std::unordered_map< AppState, std::array< std::string, 2 > > stateStr = { { OFFLINE, offlineText }, { ONLINE, onlineText }, { RUNNING, runText } };
std::unordered_map< AppState, Color > stateCol = { { OFFLINE, offlineCol }, { ONLINE, onlineCol }, {RUNNING, onlineCol} };
Vector2 initVel = { 1, 0 };

std::vector< TextButton > kickButtons;
std::vector< TextButton > sendButtons;

int main(int argc, char ** argv)
{
	const int screenWidth = 800;
	const int screenHeight = 500;
	if ( enet_initialize() != 0 )
	{
		std::cerr << "An error occurred while initializing ENet!" << std::endl;
		
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	
	InitWindow(screenWidth, screenHeight, "Server Interface");
	SetTargetFPS(60);
	Server server = Server(PORT);
	Rectangle stateRect = { screenWidth - 325, 0, 325, 50 };
	TextButton stateButton = TextButton(stateRect, stateStr[server.currentState()][1].c_str());
	Rectangle kickRect = { 250, 50, 225, 50 };
	Rectangle sendRect = { 250, 100, 225, 50 };
	
	kickButtons.push_back(TextButton(kickRect, "kick"));
	sendButtons.push_back(TextButton(sendRect, "send"));
	int ret = server.initialize();
	if ( ret > 0 )
	{
		exit(EXIT_FAILURE);
	}
	
	const size_t dummyID = 0;
	server.connected.addClient(nullptr, "all", ADMIN);
	int numClients = 1;
	EntityManager& entities = server.entities();
	std::shared_ptr< Entity > adminEntity = entities.addEntity("client");
	std::shared_ptr< Entity > teste = entities.addEntity("test");
	teste->addComponent(std::make_shared< CTransform >((Vector2) { 500, 700 }, (Vector2) { 1, 1 }, 0.0f));
	entities.update();
	
	// main loop
	while( !WindowShouldClose() )
	{
		// update
		server.update();
		server.poll();
		// GUI
		if ( stateButton.isFocus() )
		{
			switch(server.currentState())
			{
				case OFFLINE:
				{
					std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
					transform->pos.x = 500;
					transform->pos.y = 700;
					server.initialize();
				
					break;
				}
				
				case ONLINE:
				{
					server.destroy();
				
					break;
				}
				
				case RUNNING:
				{
					std::shared_ptr< CTransform > transform = teste->getComponent< CTransform >(TRANSFORM);
					transform->pos.x = 500;
					transform->pos.y = 700;
					server.toggleState();
					
					break;
				}
			}
		}
		stateButton.text = stateStr[server.currentState()][1];
		stateButton.textCol = stateCol[server.currentState()];
		
		if ( server.connected.size() > numClients )
		{
			for (int i=numClients; i < server.connected.size(); i++)
			{
				Rectangle kickRect = { 250, 50 + 110 * i, 225, 50 };
				kickButtons.push_back(TextButton(kickRect, "kick"));
				Rectangle sendRect = { 250, 100 + 110 * i, 225, 50 };
				sendButtons.push_back(TextButton(sendRect, "send"));
			}
		}
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
					server.kick(server.connected.getClients()[index]);
				
					break;
				}
				
				case 1:
				{
					std::shared_ptr< Client > client = server.connected.getClients()[index];
					server.syncClient(client->conn(), entities.getEntities());
					
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
					for (auto it=server.connected.getClients().begin() + 1; it != server.connected.getClients().end(); it++)
					{
						server.kick((*it));
					}
					
					break;
				}
				
				case 1:
				{
					server.syncClients(entities.getEntities());
				
					break;
				}
			}
		}
		
		// draw
		BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText(stateStr[server.currentState()][0].c_str(), 5, 5, 40, stateCol[server.currentState()]);
			stateButton.draw();
			for (int i=0;  i < server.connected.getClients().size(); i++)
			{
				DrawRectangle(5, 50 + 110 * i, 225, 100, LIGHTGRAY);
				DrawText(server.connected.getClients()[i]->user().c_str(), 10, 80 + 110 * i, 40, MAROON);
				kickButtons.at(i).draw();
				sendButtons.at(i).draw();
			}
		EndDrawing();
	}
	
	std::cout << "exiting" << std::endl;
	CloseWindow();
	server.destroy();
	
	exit(EXIT_SUCCESS);
	
	return 0;
}