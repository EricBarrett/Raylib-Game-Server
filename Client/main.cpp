#include <iostream>
#include <cstring>
#include <string>
#define MAX_INPUT_CHARS 45

//#define NOGDICAPMASKS
//#define NOVIRTUALKEYCODES // VK_*
//#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
//#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
//#define NOSYSMETRICS      // SM_*
//#define NOMENUS           // MF_*
//#define NOICONS           // IDI_*
//#define NOKEYSTATES       // MK_*
//#define NOSYSCOMMANDS     // SC_*
//#define NORASTEROPS       // Binary and Tertiary raster ops
//#define NOSHOWWINDOW      // SW_*
//#define OEMRESOURCE       // OEM Resource values
//#define NOATOM            // Atom Manager routines
//#define NOCLIPBOARD       // Clipboard routines
//#define NOCOLOR           // Screen colors
//#define NOCTLMGR          // Control and Dialog routines
//#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
//#define NOKERNEL          // All KERNEL defines and routines
//#define NOUSER            // All USER defines and routines
//#define NONLS             // All NLS defines and routines
//#define NOMB              // MB_* and MessageBox()
//#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
//#define NOMETAFILE        // typedef METAFILEPICT
//#define NOMINMAX          // Macros min(a,b) and max(a,b)
//#define NOMSG             // typedef MSG and associated routines
//#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
//#define NOSCROLL          // SB_* and scrolling routines
//#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
//#define NOSOUND           // Sound driver routines
//#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
//#define NOWH              // SetWindowsHook and WH_*
//#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
//#define NOCOMM            // COMM driver routines
//#define NOKANJI           // Kanji support stuff.
//#define NOHELP            // Help engine interface.
//#define NOPROFILER        // Profiler interface.
//#define NODEFERWINDOWPOS  // DeferWindowPos routines
//#define NOMCX             // Modem Configuration Extensions
//#define Rectangle WinRectangle
#define CloseWindow CloseWinWindow
#define ShowCursor ShowWinCursor
#define LoadImage LoadWinImage
#define DrawText DrawWinText
#define DrawTextEx DrawWinTextEx
#define PlaySound PlayWinSound
#include <enet/enet.h>
#undef CloseWindow
#undef Rectangle
#undef ShowCursor
#undef LoadImage
#undef DrawText
#undef DrawTextEx
#undef PlaySound
#include "EntityManager.h"
#include "../GUI/gui.h"

#define PLAYER_SPD 1.0f


//#define PDELAY 100

const int CONN_TIME = 5000;
const int DISCONN_TIME = 3000;

enum AppState {OFFLINE, ONLINE, RUNNING};

void sendPacket(ENetPeer* peer, const char* payload)
{
	ENetPacket* packet = enet_packet_create(payload, strlen(payload) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

int main(int argc, char ** argv)
{
	const int screenWidth = 450;
    const int screenHeight = 700;
	EntityManager entities = EntityManager();
	std::shared_ptr< Entity > player = entities.addEntity("test");
	player->addComponent(std::make_shared< CTransform >());
	// initialize enet
	if ( enet_initialize() != 0 )
	{
		std::cerr << "An error occurred while initializing ENet!" << std::endl;
		
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	
	// initialize client
	ENetHost * client;
	client = enet_host_create(NULL, 1, 2, 0, 0);
	if ( client == NULL )
	{
		std::cerr << "An error occurred while trying to create an ENet client host." << std::endl;
		exit(EXIT_FAILURE);
	}

    InitWindow(screenWidth, screenHeight, "Client App");
	SetTargetFPS(60);
	
	AppState state = OFFLINE;
	Rectangle addrRect = { screenWidth / 2 - 100, 90, 225, 50 };
	Rectangle portRect = { screenWidth / 2 - 100, 190, 225, 50 };
	Rectangle nameRect = { screenWidth / 2 - 100, 290, 225, 50 };
	Rectangle msgRect = { screenWidth / 2 - 100,  390, 225, 50 };
	Rectangle sendRect = { screenWidth / 2 - 150,  390, 50, 50 };
	Rectangle connectRect = { 62, 500, 325, 100 };
	TextBox addrBox = TextBox(addrRect, "127.0.0.1", MAX_INPUT_CHARS);
	TextBox portBox = TextBox(portRect, "1234", MAX_INPUT_CHARS);
	TextBox nameBox = TextBox(nameRect, "anon", MAX_INPUT_CHARS);
	TextBox msgBox = TextBox(msgRect, "", MAX_INPUT_CHARS);
	TextButton connectButton = TextButton(connectRect, "Connect");
	TextButton sendButton = TextButton(sendRect, "Send", 20, GREEN);
	int framesCounter = 0;
	bool mouseOnAddr = false;
	bool mouseOnPort = false;
	bool mouseOnName = false;
	bool mouseOnMsg = false;
	ENetAddress addr;
	ENetEvent event;
	ENetPeer* peer;
	enet_address_set_host(&addr, addrBox.contents().c_str());
	addr.port = 1234;
	char id[1];
	size_t numid;
	
	// main loop
	while ( !WindowShouldClose() )    // Detect window close button or ESC key
    {
        // Update
		entities.update();
		if ( state == RUNNING || state == ONLINE)
		{
			// TODO: queue multiple movements together rather than using else ifs (diagonal movement)
			std::shared_ptr< CTransform > transform = player->getComponent< CTransform >(TRANSFORM);
			char msg[2];
			msg[0] = id[0];
			if ( IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A) )
			{
				transform->pos.x -= PLAYER_SPD;
				msg[1] = 'a';
				msg[2] = '\0';
				sendPacket(peer, msg);
				std::shared_ptr< CTransform > transform = player->getComponent< CTransform >(TRANSFORM);
				transform->pos.x += 1;
			}
			else if ( IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D) )
			{
				transform->pos.x += PLAYER_SPD;
				msg[1] = 'd';
				msg[2] = '\0';
				sendPacket(peer, msg);
				std::shared_ptr< CTransform > transform = player->getComponent< CTransform >(TRANSFORM);
				transform->pos.x -= 1;
			}
			else if ( IsKeyDown(KEY_UP) || IsKeyDown(KEY_W) )
			{
				transform->pos.y -= PLAYER_SPD;
				msg[1] = 'w';
				msg[2] = '\0';
				sendPacket(peer, msg);
				std::shared_ptr< CTransform > transform = player->getComponent< CTransform >(TRANSFORM);
				transform->pos.y -= 1;
			}
			else if ( IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S) )
			{
				transform->pos.y += PLAYER_SPD;
				msg[1] = 's';
				msg[2] = '\0';
				sendPacket(peer, msg);
				std::shared_ptr< CTransform > transform = player->getComponent< CTransform >(TRANSFORM);
				transform->pos.y += 1;
			}
		}
		mouseOnAddr = addrBox.isFocus();
		mouseOnPort = portBox.isFocus();
		mouseOnName = nameBox.isFocus();
		mouseOnMsg = msgBox.isFocus();
		
        addrBox.collect();
		portBox.collect();
		nameBox.collect();
		msgBox.collect();
		if ( !mouseOnAddr && !mouseOnPort && !mouseOnName  && !mouseOnMsg)
		{
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		}
		
		if ( mouseOnAddr || mouseOnPort || mouseOnName || mouseOnMsg )
		{
			framesCounter++;
		}
		else
		{
			framesCounter = 0;
		}
		
        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("IP Address:", 238, 60, 20, GRAY);
			addrBox.draw();
			DrawText("Port:", 300, 160, 20, GRAY);
			portBox.draw();
			DrawText("Username:", 250, 260, 20, GRAY);
			nameBox.draw();
			DrawText("Message:", 250, 360, 20, GRAY);
			msgBox.draw();
			sendButton.draw();
			connectButton.draw();
			if ( mouseOnAddr )
			{
				if ( addrBox.index() < MAX_INPUT_CHARS )
				{
					if ( ((framesCounter / 20) % 2) == 0 )
					{
						DrawText("|", addrBox.pos().x + 8 + MeasureText(addrBox.contents().c_str(), 40), addrBox.pos().y + 12, 40, MAROON);
					}
				}
			}
			else if ( mouseOnPort )
			{
				if ( portBox.index() < MAX_INPUT_CHARS )
				{
					if ( ((framesCounter / 20) % 2) == 0 )
					{
						DrawText("|", portBox.pos().x + 8 + MeasureText(portBox.contents().c_str(), 40), portBox.pos().y + 12, 40, MAROON);
					}
				}
			}
			else if ( mouseOnName )
			{
				if ( nameBox.index() < MAX_INPUT_CHARS )
				{
					if ( ((framesCounter / 20) % 2) == 0 )
					{
						DrawText("|", nameBox.pos().x + 8 + MeasureText(nameBox.contents().c_str(), 40), nameBox.pos().y + 12, 40, MAROON);
					}
				}
			}
			else if ( mouseOnMsg )
			{
				if ( msgBox.index() < MAX_INPUT_CHARS )
				{
					if ( ((framesCounter / 20) % 2) == 0 )
					{
						DrawText("|", msgBox.pos().x + 8 + MeasureText(msgBox.contents().c_str(), 40), msgBox.pos().y + 12, 40, MAROON);
					}
				}
			}
        EndDrawing();
		
		if ( connectButton.isFocus() )
		{
			switch(state)
			{
				case OFFLINE:
				{
					connectButton.text = "Connecting...";
					bool success = false;
					enet_address_set_host(&addr, addrBox.contents().c_str());
					addr.port = std::stoi(portBox.contents());
					// establish connection
					peer = enet_host_connect(client, &addr, 1, 0);
					if ( peer == NULL )
					{
						std::cerr << "Server is unavailable" << std::endl;
					}
					// poll for connection event
					if ( enet_host_service(client, &event, CONN_TIME) > 0 && event.type == ENET_EVENT_TYPE_CONNECT )
					{
						std::cout << "Connection to " << addrBox.contents() << " succeeded :)" << std::endl;
						sendPacket(peer, nameBox.contents().c_str());
						// poll for entity id
						if ( enet_host_service(client, &event, CONN_TIME) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE )
						{
							std::cout << event.packet->data << std::endl;
							//snprintf(id, sizeof id, "%u", event.packet->data);
							char * payload = reinterpret_cast<char*>(event.packet->data);
							id[0] = payload[0];
							id[1] = '\0';
							numid = std::stoi(id);
							
							if ( isdigit(id[0]) )
							{
								success = true;
								connectButton.text = "Disconnect";
								state = ONLINE;
							}
							std::cout << "connected with id: " << id << std::endl;
						}
						else
						{
							enet_peer_reset(peer);
							std::cout << "Server failed to respond" << std::endl;
						}
					}
					else
					{
						enet_peer_reset(peer);
						std::cout << "Connection to " << addrBox.contents() << " failed :(" << std::endl;
					}
					if ( !success )
					{
						connectButton.text = "Connect";
					}
					
					break;
				}
				
				case RUNNING:
				{
					connectButton.text = "Disconnecting...";
					// disconnect
					enet_peer_disconnect(peer, 0);
					while(enet_host_service(client, &event, DISCONN_TIME) > 0)
					{
						switch(event.type)
						{
							case ENET_EVENT_TYPE_RECEIVE:
								enet_packet_destroy(event.packet); // destroy any incoming packets when trying to disconnect
							break;
			
							case ENET_EVENT_TYPE_DISCONNECT:
								std::cout << "Disconnected from " << addrBox.contents() << std::endl; // success!
							break;
						}
					}
					connectButton.text = "Connect";
					state = OFFLINE;
					
					break;
				}
				
				case ONLINE:
				{
					connectButton.text = "Disconnecting...";
					// disconnect
					enet_peer_disconnect(peer, 0);
					while(enet_host_service(client, &event, DISCONN_TIME) > 0)
					{
						switch(event.type)
						{
							case ENET_EVENT_TYPE_RECEIVE:
								enet_packet_destroy(event.packet); // destroy any incoming packets when trying to disconnect
							break;
			
							case ENET_EVENT_TYPE_DISCONNECT:
								std::cout << "Disconnected from " << addrBox.contents() << std::endl; // success!
							break;
						}
					}
					connectButton.text = "Connect";
					state = OFFLINE;
					
					break;
				}
			}
		}
		
		if ( sendButton.isFocus() && (state == ONLINE || state == RUNNING) )
		{
			sendPacket(peer, msgBox.contents().c_str());
		}
		
        if ( enet_host_service(client, &event, 0) > 0 )
		{
			switch(event.type)
			{
				case ENET_EVENT_TYPE_RECEIVE:
				{
//					std::cout << "RECEIVED PACKET" << std::endl << "packet size: " << event.packet->dataLength << std::endl << "payload: " << event.packet->data << std::endl << "host: " << event.peer->address.host << std::endl << "channel: " << event.channelID << std::endl;
					//state = RUNNING;
					char * payload = reinterpret_cast<char*>(event.packet->data);
					
					std::cout << "server: " << payload << std::endl;
					std::shared_ptr< CTransform > transform = player->getComponent< CTransform >(TRANSFORM);
					float x;
					float y;
					char data[20];
//					numid = std::stoi(id);
					size_t sliceBegin = numid * 19;
					size_t sliceEnd = sliceBegin + 20;
					for (size_t index=sliceBegin; index < sliceEnd; index++)
					{
						data[index - sliceBegin] = payload[index];
					}
					std::cout << "client: " << data << std::endl;
					sscanf(data, "%f %f", &x, &y);
					if ( x != transform->pos.x || y != transform->pos.y )
					{
						std::cout << "Server-Client mismatch. Server coordinates: (" << x << ", " << y << "), Client coordinates: (" << transform->pos.x << ", " << transform->pos.y << ")" << std::endl;
						std::cout << "Syncing with Server" << std::endl;
					}
					transform->pos.x = x;
					transform->pos.y = y;
				
					break;
				}
				
				case ENET_EVENT_TYPE_DISCONNECT:
				{
					std::cout << "Disconnected from " << addrBox.contents() << std::endl; // success!
					connectButton.text = "Connect";
					state = OFFLINE;
					
					break;
				}
			}
		}
    }
	
	// deinitialize
	CloseWindow();
	enet_host_destroy(client);
	exit(EXIT_SUCCESS);

	return 0;
}