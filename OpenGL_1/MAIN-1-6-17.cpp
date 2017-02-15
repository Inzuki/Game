/*#include "GUI.h"
#include "OBJ.h"
#include "Lamp.h"
#include "Water.h"
#include "Assimp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

// #define WATER_HEIGHT 25.0

int ID = -1;
unsigned short MAX_PLAYERS = 8;

struct Player {
	int id;
	bool moving[4];
	char name[1024];
	glm::vec3 pos, dir, right;
	bool noclipping;
};

struct ChatMessage {
	std::string text;
	int r, g, b;
};

// mousepicker ray
glm::vec3 createRay(sf::Window &window){
	float mouseX = sf::Mouse::getPosition().x / (window.getSize().x * 0.5f) - 1.f;
	float mouseY = sf::Mouse::getPosition().y / (window.getSize().y * 0.5f) - 1.f;

	glm::mat4 proj = glm::perspective(45.f, 4.f / 3.f, 0.1f, 333.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0.f), getDir(), getUp());

	glm::mat4 invVP = glm::inverse(proj * view);
	glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.f, 1.f);
	glm::vec4 worldPos = invVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	return dir;
}

int main(){
	#pragma region server
		// let the client enter a name
		printf("Please enter a name: "); //printf("Inzuki\n");
		char moniker[1024]; //sprintf(moniker, "Inzuki");
		//gets(moniker);
		std::cin.getline(moniker, sizeof(moniker));

		// let the client enter an IP address to connect to
		printf("Please enter a server IP: "); //printf("***\n");
		char someIP[128];
		gets(someIP);

		// shortcutting so you don't have to type the IP every time
		// if the shortcut is localhost
		if(strcmp(someIP, "lh") == 0)
			sprintf(someIP, "127.0.0.1");
		// if the shortcut is my IP over Hamachi
		else if(strcmp(someIP, "inz") == 0)
			sprintf(someIP, "25.32.10.114");

		// server setup
		const unsigned short port = 50001;
		std::string serverIP = someIP; // have the user enter an IP
		sf::IpAddress server = serverIP;

		// attempt to connect
		printf("Connecting to %s...\n", serverIP.c_str());
		
		// tell the server the client is attempting to connect
		char sendMsg[1024];
		sprintf(sendMsg, "+%s", moniker);
	
		sf::UdpSocket socket;
		socket.send(sendMsg, sizeof(sendMsg), server, port);

		// receive from the server if the client successfully connected
		char buffer[1024];
		std::size_t received;
		sf::IpAddress sender;
		unsigned short senderPort;
		socket.receive(buffer, sizeof(buffer), received, sender, senderPort);

		// if the client successfully connected, the server should send back a success signal
		if(buffer[0] == 'y'){
			char buff[1024];
			for(int i = 0; i < sizeof(buffer); i++)
				buff[i] = buffer[i + 1];

			ID = atoi(buff);

			printf("Connected to %s successfully (given ID %i).\n", serverIP.c_str(), ID);
		}
		printf("Other connected players:\n");

		// create a list for player information
		// and insert them into said list
		std::vector<Player> players;
		Player tempPlayer;
		tempPlayer.id = ID;
		sprintf(tempPlayer.name, moniker);
		players.push_back(tempPlayer);

		socket.setBlocking(false);
	#pragma endregion

	#pragma region window/OpenGL and GLEW init
		// initialize SFML-OpenGL window
		char window_title[128] = "OpenGL";
		sf::ContextSettings contextSettings;
		contextSettings.antialiasingLevel = 4;
		contextSettings.depthBits = 64;
		sf::RenderWindow window(sf::VideoMode(1440, 900), window_title, sf::Style::Resize | sf::Style::Close, contextSettings);
		window.setVerticalSyncEnabled(true);
		window.setFramerateLimit(60);
		sf::Clock clk;
		char fps[64];

		// initialize GLEW (and other OpenGL settings)
		glewExperimental = GL_TRUE;
		glewInit();
		glViewport(0, 0, window.getSize().x, window.getSize().y);

		glEnable(GL_MULTISAMPLE);  
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
	#pragma endregion

	#pragma region chat init
		// load font
		sf::Font font; font.loadFromFile("res/fonts/trebuc.ttf");
		sf::Text chat_msgs_text;
		chat_msgs_text.setScale(0.5f, 0.5f);
		chat_msgs_text.setFont(font);

		// load typing box
		sf::RectangleShape typing_box;
		typing_box.setSize(sf::Vector2f(555.f, 30.f));
		typing_box.setFillColor(sf::Color(125, 125, 245, 70));

		sf::RectangleShape chatmessages_box;
		chatmessages_box.setSize(sf::Vector2f(555.f, 300.f));
		chatmessages_box.setFillColor(sf::Color(125, 125, 245, 60));

		// initialize vector that will hold the messages, and a var to hold the text being typed to send
		//std::vector<std::string> chat_messages;
		std::vector<ChatMessage> chat_messages;
		float fade_time = 9.f; // time for the chat box and messages to fade away (in seconds)

		char typingText[1024];
		sprintf(typingText, "");
		sf::Text text;
		text.setScale(0.5f, 0.5f);
		text.setFont(font);
		text.setString(typingText);
	#pragma endregion

	#pragma region load shaders
		GLuint lampShader     = loadShaders("lamp.vert",    "lamp.frag");
		GLuint lightingShader = loadShaders("default.vert", "default.frag");
		GLuint skyboxShader   = loadShaders("skybox.vert",  "skybox.frag");
		GLuint terrainShader  = loadShaders("terrain.vert", "terrain.frag");
	#pragma endregion

	#pragma region lamps (lights)
		//model = glm::translate(model, glm::vec3(25.f, -7.75f, 25.f));
		Lamp sun(glm::vec3(125.f, 75.f, 125.f), glm::vec3(0.9f, 0.9f, 0.2f)); // overall "environment" lighting
		Lamp lamp2(glm::vec3(25.f, 2.75f, 25.f), glm::vec3(1.f, 1.0f, 0.0f), glm::vec3(0.5f, 0.01f, 0.001f)); // yellow lamp (follows lamp post)
		Lamp lamp3(glm::vec3(20.f, 5.f, 50.f), glm::vec3(0.f, 0.0f, 1.0f), glm::vec3(0.3f, 0.001f, 0.001f)); // blue lamp
		std::vector<Lamp> lamps;
		lamps.push_back(sun);
		lamps.push_back(lamp2);
		lamps.push_back(lamp3);
	#pragma endregion

	#pragma region terrains
		//SimpleTerrain sTerrain1("res/textures/height2.jpg", terrainShader);
	#pragma endregion

	#pragma region model loading
		OBJ stall("stall.obj", "res/textures/stallTexture.png"),
			lamp_post("lamp.obj", "res/textures/lamp_post.png"),
			lamp_cube("cube.obj", "res/textures/blank.png");
	#pragma endregion

	#pragma region skyboxes
		// order of skyboxes (from top to bottom): right, left, top, bottom, back, front
		std::vector<const GLchar*> faces;
		faces.push_back("skyrender0001.bmp"); //
		faces.push_back("skyrender0004.bmp"); //
		faces.push_back("skyrender0003.bmp"); //
		faces.push_back("skyrender0003.bmp"); // bottom (which oddly isn't working well)
		faces.push_back("skyrender0005.bmp"); //
		faces.push_back("skyrender0002.bmp"); //
		CubeMap skybox1(faces);
	#pragma endregion

	#pragma region animations
		core::Model mdl;
		core::ModelLoader mdlLoad;
		mdlLoad.loadModel("res/models/boblampclean.md5mesh", &mdl);
		mdl.init();
	#pragma endregion

	#pragma region GUI
		std::vector<GUI> guis;
		GUIRenderer guiRender;
	#pragma endregion

	#pragma region light/shader properties
		glUseProgram(terrainShader);
		glUniform3f(glGetUniformLocation(terrainShader,  "skyColor"), 0.2f, 0.3f, 0.3f);
		glUseProgram(lightingShader);
		glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);
		glUniform3f(glGetUniformLocation(lightingShader, "skyColor"), 0.2f, 0.3f, 0.3f);
	#pragma endregion

	// mousepicking setup + noclip setting
	glm::vec3 ray_start = position, ray_finish(1.f);
	bool _FREE_CAM = 1;

	// networking stuff:
	// boolean values that help prevent DDoS'ing the server.
	// tell the server once that the player is moving once they press the key
	// and tell the server when they stop moving when they let go of the key
	bool moving[4];
		moving[0] = false; moving[1] = false; moving[2] = false; moving[3] = false;
	sf::Clock dir_change, pos_update,
			  message_fade;		   // this one is for when a message is received.
	bool ignore_msg_start = false; // a box is displayed and other messages are
								   // too when a message is received to display
								   // all recent chat messages.

	bool running = true;
	while(running){
		#pragma region receiveFromServer
			if(socket.receive(buffer, sizeof(buffer), received, sender, senderPort) == sf::Socket::Done){
				char buff[1024];
				for(int i = 0; i < sizeof(buffer); i++)
					buff[i] = buffer[i + 1];

				switch(buffer[0]){
					// when a new player connects
					case 'a':{
						sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
						tempPlayer.pos   = glm::vec3(20.f, 10.f, 20.f);
						tempPlayer.dir   = glm::vec3(0.f, 0.f, 0.f);
						tempPlayer.right = glm::vec3(0.f, 0.f, 0.f);
						tempPlayer.noclipping = false;
						for(int i = 0; i < 4; i++)
							tempPlayer.moving[i] = false;

						players.push_back(tempPlayer);

						printf("%s has connected.\n", tempPlayer.name);
					}break;
					// inform the player of other connected players
					case 'A':{
						sscanf(buff, "%i,%f,%f,%f,%i,%s", &tempPlayer.id, &tempPlayer.pos.x, &tempPlayer.pos.y, &tempPlayer.pos.z, &tempPlayer.noclipping, &tempPlayer.name);
						printf("- %s\n", tempPlayer.name);
						tempPlayer.dir   = glm::vec3(0.f, 0.f, 0.f);
						tempPlayer.right = glm::vec3(0.f, 0.f, 0.f);
						for(int i = 0; i < 4; i++)
							tempPlayer.moving[i] = false;

						players.push_back(tempPlayer);
					}break;
					// when a player disconnects
					case 'r':{
						for(int i = 0; i < players.size(); i++){
							if(players[i].id == atoi(buff)){
								printf("%s has disconnected.\n", players[i].name);
								players.erase(players.begin() + i);
								break;
							}
						}
					}break;
					// when a player is moving, set values that will be processed in a later loop
					case 'm':{
						int tID, key;
						float dir_x, dir_y, dir_z, right_x, right_z;
						sscanf(buff, "%i,%i,%f,%f,%f,%f,%f", &tID, &key, &dir_x, &dir_y, &dir_z, &right_x, &right_z);

						for(int i = 0; i < players.size(); i++){
							if(tID == players[i].id){
								players[i].dir.x   = dir_x;
								players[i].dir.y   = dir_y;
								players[i].dir.z   = dir_z;
								players[i].right.x = right_x;
								players[i].right.z = right_z;
								players[i].moving[key] = true;
							}
						}
					}break;
					// when a player stops moving
					case 's':{
						int tID, key;
						float pos_x, pos_y, pos_z;
						sscanf(buff, "%i,%i,%f,%f,%f", &tID, &key, &pos_x, &pos_y, &pos_z);

						for(int i = 0; i < players.size(); i++){
							if(tID == players[i].id){
								players[i].moving[key] = false;
								players[i].pos.x = pos_x;
								players[i].pos.y = pos_y;
								players[i].pos.z = pos_z;
							}
						}
					}break;
					// when a player's direction changes
					case 'd':{
						int tID;
						float dir_x, dir_y, dir_z, right_x, right_z;
						sscanf(buff, "%i,%f,%f,%f,%f,%f", &tID, &dir_x, &dir_y, &dir_z, &right_x, &right_z);

						for(int i = 0; i < players.size(); i++){
							if(tID == players[i].id){
								players[i].dir.x   = dir_x;
								players[i].dir.y   = dir_y;
								players[i].dir.z   = dir_z;
								players[i].right.x = right_x;
								players[i].right.z = right_z;
							}
						}
					}break;
					// regular position update
					case 'p':{
						int tID;
						float pos_x, pos_y, pos_z;
						sscanf(buff, "%i,%f,%f,%f", &tID, &pos_x, &pos_y, &pos_z);

						for(int i = 0; i < players.size(); i++){
							if(tID == players[i].id){
								players[i].pos.x = pos_x;
								players[i].pos.y = pos_y;
								players[i].pos.z = pos_z;
							}
						}
					}break;
					// when a player sends a specific key press
					case 'k':{
						int tID; char key;
						sscanf(buff, "%i,%c", &tID, &key);

						if(key == 'v')
							for(int i = 0; i < players.size(); i++)
								if(tID == players[i].id)
									players[tID].noclipping = !players[tID].noclipping;
					}break;
					// when a player sends a chat message
					case 'c':{
						int tID; char key, chatmsg[1024];
						sscanf(buff, "%c,%i,%s", &key, &tID, &chatmsg);

						// since the chat message box starts off on, we need this check in place
						// so it doesn't start off on. it'll be re-enabled after the fade time is over
						// (defined later above in the 'chat init' region. however, if the client
						// receives a message while this is in place, then the window won't show up.
						//
						// this helps fix that:
						// so, the chat box is on a timer that resets when a message is received.
						// the chat box will disappear once it passes the fade time. since the clock
						// is already restarted (technically) at the start of the application, the
						// chat box will be already turned on. this boolean, along with another timer,
						// will work together to ensure the above doesn't happen, but if the client
						// DOES receive a message in this time frame, to skip all this work and
						// display the chat box again. phew.
						if(!ignore_msg_start)
							ignore_msg_start = true;

						// replace all smiley faces with spaces, since sending
						// hard spaces just breaks the message and stops it there
						for(int i = 0; i < sizeof(chatmsg); i++){
							if(chatmsg[i] != NULL){
								if(chatmsg[i] == '☺')
									chatmsg[i] = ' ';
							}else
								break;
						}
						
						char msg_with_name[1024];
						for(int i = 0; i < players.size(); i++)
							if(tID == players[i].id)
								sprintf(msg_with_name, "%s", players[i].name);

						// limit the chat to 14 messages at any given time
						if(chat_messages.size() > 13)
							chat_messages.erase(chat_messages.begin());
						
						ChatMessage temp_msg;

						// figure out what kind of message it is
						// normal chat message
						if(key == 'n'){
							sprintf(msg_with_name, "%s: %s", msg_with_name, chatmsg);
							temp_msg.text = msg_with_name;
							temp_msg.r    = 255;
							temp_msg.g    = 255;
							temp_msg.b    = 255;
						}
						// user connects
						else if(key == '+'){
							temp_msg.text = chatmsg;
							temp_msg.r    = 50;
							temp_msg.g    = 255;
							temp_msg.b    = 50;
						}
						// user disconnects
						else if(key == '-'){
							temp_msg.text = chatmsg;
							temp_msg.r    = 255;
							temp_msg.g    = 50;
							temp_msg.b    = 50;
						}
						// unknown message
						else
							printf("\nERR: Unknown chat message received.\n\n");

						chat_messages.push_back(temp_msg);

						chatmessages_box.setFillColor(sf::Color(125, 125, 245, 60.f));
						message_fade.restart();
					}break;
				}
			}
		#pragma endregion

		// set the player's Y coordinate based on the terrain's height at that position
		if(_FREE_CAM)
			//moveY(-sTerrain1.getHeight(getPos().x, getPos().z) + 20.f);
			position.y = 5.f;

		// calculate timestamp
		#pragma region timestamp calculations
			static float lastTime = clk.getElapsedTime().asSeconds();
			float currentTime     = clk.getElapsedTime().asSeconds(),
				  deltaTime       = float(currentTime - lastTime);
		#pragma endregion
		// update window title to display current framerate
		sprintf(fps, "[%s] - FPS: %i", window_title, (int)round(1.f / deltaTime)); window.setTitle(fps);

		#pragma region server updating
			// let the server know the player moved the mouse to update the direction.
			// also use a clock so we don't spam the server with constant direction changing and crash it
			if(dir_change.getElapsedTime().asMilliseconds() > 5 && sf::Mouse::getPosition().x != window.getSize().x / 2 && sf::Mouse::getPosition().y != window.getSize().y / 2){
				if(moving[0] || moving[1] || moving[2] || moving[3]){
					sprintf(sendMsg, "d%i,%f,%f,%f,%f,%f", ID, getDir().x, getDir().y, getDir().z, getRight().x, getRight().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					dir_change.restart();
				}
			}
			// also update the player's position
			// (NOTE: this is unsafe as the player can spoof their position.
			//     this will be edited later, but for now this will do.)
			// addendum: only necessary to check when they aren't moving,
			// since checking will screw up the position really bad
			if(pos_update.getElapsedTime().asSeconds() > 10){
				if(!moving[0] && !moving[1] && !moving[2] && !moving[3]){
					sprintf(sendMsg, "p%i,%f,%f,%f", ID, getPos().x, getPos().y, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					pos_update.restart();
				}
			}
		#pragma endregion

		#pragma region event handling
			sf::Event event;
			while(window.pollEvent(event)){
				// close the window
				if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
					// tell the server a client is disconnecting
					sprintf(sendMsg, "-%i", ID);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					// then close the window and application
					running = false;
					break;
				}

				// when the window is resized, fix the viewport
				if(event.type == sf::Event::Resized)
					glViewport(0, 0, window.getSize().x, window.getSize().y);

				// print coordinates of the lamp
				if(event.type == sf::Event::MouseButtonReleased)
					if(event.mouseButton.button == sf::Mouse::Right)
						printf("- Lamp Position: (%f, %f, %f)\n", ray_finish.x, ray_finish.y, ray_finish.z);

				if(event.type == sf::Event::TextEntered)
					if(typingStatus())
						if(event.text.unicode > 31 && event.text.unicode < 128)
							sprintf(typingText, "%s%c", typingText, static_cast<char>(event.text.unicode));

				// handle networking so the server doesn't get DDoS'ed
				// send the server what key the user is pressing
				// and the direction they're currently facing
				if(event.type == sf::Event::KeyPressed){
					// chatbox stuff
					if(event.key.code == sf::Keyboard::Return){
						// if the client is currently typing and presses
						// the enter/return key then send the message
						if(typingStatus()){
							// make sure the first character isn't empty, no point in sending empty messages
							if(typingText[0] != NULL){
								// when sending the message buffer, it cuts off at the first space.
								// not sure if there's an easier way, but here I'm just replacing
								// all spaces with an actual character (that the user can't input)
								// and then every instance of that character will be replaced with
								// actual spaces later. very strange but it works!
								// also, it breaks out of the loop once it hits the first part
								// of the array that's empty, so it doesn't loop through the whole
								// array pointlessly to save processing time
								for(int i = 0; i < sizeof(typingText); i++){
									if(typingText[i] != NULL){
										if(typingText[i] == ' ')
											typingText[i] = '☺';
									}else
										break;
								}

								sprintf(sendMsg, "c%i,%s", ID, typingText);
								socket.send(sendMsg, sizeof(sendMsg), server, port);
								memset(typingText, 0, sizeof(typingText));
							}

							setTypingStatus(false);
						}
						// otherwise, if they aren't typing and press the
						// enter/return key, stop all movement handling and commence typing
						else
							setTypingStatus(true);
					}

					// for W
					if(!moving[0] && event.key.code == sf::Keyboard::W){
						moving[0] = true;
						sprintf(sendMsg, "m%i,%i,%f,%f,%f,%f,%f", ID, 0, getDir().x, getDir().y, getDir().z, getRight().x, getRight().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
					// for A
					if(!moving[1] && event.key.code == sf::Keyboard::A){
						moving[1] = true;
						sprintf(sendMsg, "m%i,%i,%f,%f,%f,%f,%f", ID, 1, getDir().x, getDir().y, getDir().z, getRight().x, getRight().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
					// for S
					if(!moving[2] && event.key.code == sf::Keyboard::S){
						moving[2] = true;
						sprintf(sendMsg, "m%i,%i,%f,%f,%f,%f,%f", ID, 2, getDir().x, getDir().y, getDir().z, getRight().x, getRight().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
					// for D
					if(!moving[3] && event.key.code == sf::Keyboard::D){
						moving[3] = true;
						sprintf(sendMsg, "m%i,%i,%f,%f,%f,%f,%f", ID, 3, getDir().x, getDir().y, getDir().z, getRight().x, getRight().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
				}
				if(event.type == sf::Event::KeyReleased){
					// remove letters from the text being typed when backspace is pressed
					if(event.key.code == sf::Keyboard::BackSpace){
						if(typingStatus()){
							for(int i = 0; i < sizeof(typingText); i++){
								// make sure you're not on the first character, otherwise you'll go out-of-bounds of the array
								if(i != 0 && typingText[i] == NULL){
									typingText[i - 1] = NULL;
									break;
								}
							}
						}
					}

					// lock/unlock cursor
					if(event.key.code == sf::Keyboard::Q)
						setCursorLocked();

					// "noclip"
					if(event.key.code == sf::Keyboard::V){
						_FREE_CAM = !_FREE_CAM;
						// tell the server and clients the player is noclipping (debugging purposes I guess)
						sprintf(sendMsg, "k%i,%c", ID, 'v');
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}

					// for W
					if(moving[0] && event.key.code == sf::Keyboard::W){
						moving[0] = false;
						sprintf(sendMsg, "s%i,%i,%f,%f,%f", ID, 0, getPos().x, getPos().y, getPos().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
					// for A
					if(moving[1] && event.key.code == sf::Keyboard::A){
						moving[1] = false;
						sprintf(sendMsg, "s%i,%i,%f,%f,%f", ID, 1, getPos().x, getPos().y, getPos().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
					// for S
					if(moving[2] && event.key.code == sf::Keyboard::S){
						moving[2] = false;
						sprintf(sendMsg, "s%i,%i,%f,%f,%f", ID, 2, getPos().x, getPos().y, getPos().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
					// for D
					if(moving[3] && event.key.code == sf::Keyboard::D){
						moving[3] = false;
						sprintf(sendMsg, "s%i,%i,%f,%f,%f", ID, 3, getPos().x, getPos().y, getPos().z);
						socket.send(sendMsg, sizeof(sendMsg), server, port);
					}
				}
			}
		#pragma endregion
		
		// mousepicker stuff
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			ray_start  = position;
			ray_finish = ray_start + createRay(window) * 15.f;
		}

		glm::mat4 model;
		glm::vec4 clipPlane;

		glEnable(GL_CLIP_DISTANCE0);

		// compute matrices based on keyboard and mouse input
		computeMats(window, clk, deltaTime);

		// setup light properties //
		// send light information to the shader
		glUseProgram(lightingShader);
		for(int i = 0; i < lamps.size(); i++){
			// buffer to hold the current light's info to be sent to the shader
			char light_data[64];
			sprintf(light_data, "lightPos[%i]", i);
			// send the position of the light
			glUniform3f(glGetUniformLocation(lightingShader, light_data),
						lamps[i].getLampPos().x,
						lamps[i].getLampPos().y,
						lamps[i].getLampPos().z
			);
			// send the light's color
			sprintf(light_data, "lightColor[%i]", i);
			glUniform3f(glGetUniformLocation(lightingShader, light_data),
						lamps[i].getColor().x,
						lamps[i].getColor().y,
						lamps[i].getColor().z
			);
			// send the attentuation of the light (how bright the light is)
			sprintf(light_data, "attenuation[%i]", i);
			glUniform3f(glGetUniformLocation(lightingShader, light_data),
						lamps[i].getAttenuation().x,
						lamps[i].getAttenuation().y,
						lamps[i].getAttenuation().z
			);
		}

		#pragma region main_loop
			// clear buffer
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// universal object properties (like size)
			glDisable(GL_CLIP_DISTANCE0);
			clipPlane = glm::vec4(0.f, -1.f, 0.f, 1000000);

			// skybox
			glDepthMask(GL_FALSE);
				glUseProgram(skyboxShader);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
				skybox1.draw(skyboxShader);
			glDepthMask(GL_TRUE);
		
			// draw terrain
			//glUseProgram(terrainShader);
			//sTerrain1.draw(getProjectionMatrix(), getViewMatrix(), lamps, terrainShader, clipPlane);
			
			glUseProgram(lightingShader);
			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(6.f, 0.f, 6.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);
			
			// draw lamp post
			model = glm::mat4();
			//model = glm::translate(model, ray_finish);
			lamp_post.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw (other) players
			for(int i = 0; i < players.size(); i++){
				// don't display this player's model and such - no point other than creating
				// confusion, it'll just put a model in the position of the camera and
				// a. waste memory AND b. get in the way of the camera and difficult to see
				if(players[i].id != ID){
					if(!players[i].noclipping)
					//	moveY_players(players[i].pos.y, -sTerrain1.getHeight(players[i].pos.x, players[i].pos.z) + 20.f);
						players[i].pos.y = 5.f;

					if(players[i].moving[0]){
						players[i].pos.x += players[i].dir.x * 20.f * deltaTime;
						players[i].pos.y += players[i].dir.y * 20.f * deltaTime;
						players[i].pos.z += players[i].dir.z * 20.f * deltaTime;
					}
					if(players[i].moving[2]){
						players[i].pos.x -= players[i].dir.x * 20.f * deltaTime;
						players[i].pos.y -= players[i].dir.y * 20.f * deltaTime;
						players[i].pos.z -= players[i].dir.z * 20.f * deltaTime;
					}
					if(players[i].moving[3]){
						players[i].pos.x += players[i].right.x * 20.f * deltaTime;
						players[i].pos.z += players[i].right.z * 20.f * deltaTime;
					}
					if(players[i].moving[1]){
						players[i].pos.x -= players[i].right.x * 20.f * deltaTime;
						players[i].pos.z -= players[i].right.z * 20.f * deltaTime;
					}

					model = glm::mat4();
					// lower the model because the camera is being placed there for some reason
					model = glm::translate(model, glm::vec3(players[i].pos.x, players[i].pos.y - 5.f, players[i].pos.z));
					model = glm::scale(model, glm::vec3(0.75f));
					lamp_post.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);
				}
			}

			// draw animated models (must be done after traditional models)
			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, 0.f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

			// draw lamps
			glUseProgram(lampShader);
			for(int i = 0; i < lamps.size(); i++){
				model = glm::mat4();
				
				if(i == 1) // draw the light from the lamp wherever it is placed from the ray
					model = glm::translate(model, glm::vec3(0.f, 10.f, 0.f));
					//model = glm::translate(model, ray_finish + glm::vec3(0.f, 10.f, 0.f));
				else
					model = glm::translate(model, lamps[i].getLampPos());

				lamps[i].draw(lamp_cube, model, lampShader, getProjectionMatrix(), getViewMatrix());
			}
			
			// GUIs (must be rendered last, with text below)
			guiRender.render(guis);

			// draw text
			// save OpenGL state
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			window.pushGLStates();
				#pragma region chat box
					// display chat messages when
					// a. when the player opens the typing text box, or
					// b. when a player sends a message.
					// the messages and window will eventually fade away after
					// a certain time, and reappear when there's a new message
					if(ignore_msg_start){
						if(typingStatus() || message_fade.getElapsedTime().asSeconds() < fade_time){
							// draw the box that will hold all the other messages
							chatmessages_box.setPosition(15.f, window.getSize().y - chatmessages_box.getSize().y - 55.f);
							window.draw(chatmessages_box);

							// begin fading objects if the player isn't typing anymore
							if(!typingStatus()){
								if(message_fade.getElapsedTime().asSeconds() >= fade_time - 2.f){
									// fade chatbox
									chatmessages_box.setFillColor(sf::Color(125, 125, 245, 60.f - (message_fade.getElapsedTime().asSeconds() - (fade_time - 2.f)) * 30.f));

									// fade text
									for(int i = 0; i < chat_messages.size(); i++){
										chat_msgs_text.setColor(
											sf::Color(
												chat_messages[i].r,
												chat_messages[i].g,
												chat_messages[i].b,
												255 - (message_fade.getElapsedTime().asSeconds() - (fade_time - 2.f)) * 123.f
											)
										);

										chat_msgs_text.setPosition(25.f, chatmessages_box.getPosition().y + 10.f + (i * 20.f));
										chat_msgs_text.setString(chat_messages[i].text);
										window.draw(chat_msgs_text);
									}
								}
							}

							if(message_fade.getElapsedTime().asSeconds() <= fade_time - 2.f){
								for(int i = 0; i < chat_messages.size(); i++){
									chat_msgs_text.setColor(sf::Color(chat_messages[i].r, chat_messages[i].g, chat_messages[i].b, 255));

									chat_msgs_text.setPosition(25.f, chatmessages_box.getPosition().y + 10.f + (i * 20.f));
									chat_msgs_text.setString(chat_messages[i].text);
									window.draw(chat_msgs_text);
								}
							}
						}
					}
				
					// display the text the user is currently typing
					if(typingStatus()){
						// draw the box that will "hold" the text being typed
						typing_box.setPosition(15.f, window.getSize().y - 45.f);
						window.draw(typing_box);

						// draw the text that the user is typing
						text.setPosition(25.f, window.getSize().y - 40.f);
						text.setString(typingText);
						window.draw(text);
					}
				#pragma endregion
			window.popGLStates();
		#pragma endregion
		
		// disable vertex stuff
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
		glDisableVertexAttribArray(4);

		// display window
		window.display();

		lastTime = currentTime;
	}

	#pragma region delete
		// free shaders
		glDeleteProgram(lightingShader);
		glDeleteProgram(skyboxShader);
		glDeleteProgram(lampShader);

		return EXIT_SUCCESS; // qed
	#pragma endregion
}*/