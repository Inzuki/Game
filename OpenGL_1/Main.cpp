#include "INCLUDES.h"

#include "OBJ.h"
#include "Lamp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

int ID = -1;

struct Player {
	int id;
	char name[1024];
	glm::vec3 pos, pos2, dir;
	glm::mat4 model;
	bool moving[4];
};

int main(){
	// initialize OpenGL window
	sf::Window window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Resize, sf::ContextSettings(64));
	glViewport(0, 0, window.getSize().x, window.getSize().y);
    window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	sf::Clock clk;
	char fps[64];

	// initialize GLEW
	glewExperimental = true; glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// load shaders
	GLuint lampShader     = loadShaders("vertex_lamp.txt", "fragment_lamp.txt");
	GLuint lightingShader = loadShaders("vertex.txt",      "fragment.txt");
	GLuint skyboxShader   = loadShaders("skybox.vert",     "skybox.frag");

	// lamps
	std::vector<Lamp> lamps;
	lamps.push_back(Lamp(glm::vec3(1.2f, 1.0f, 2.0f)));

	// load models
	OBJ stall("stall.obj", "stallTexture.png");
	OBJ cube("cube.obj",  NULL);

	// load skyboxes
	std::vector<const GLchar*> faces;
	faces.push_back("right.jpg");
	faces.push_back("left.jpg");
	faces.push_back("top.jpg");
	faces.push_back("bottom.jpg");
	faces.push_back("back.jpg");
	faces.push_back("front.jpg");
	CubeMap skybox1(faces);

	// load terrain
	Terrain terrain1("height.jpg", "sandgrass.jpg", window);

	// light properties
	glUseProgram(lightingShader);

	for(int i = 0; i < lamps.size(); i++){
		char strang[128], streng[128];
		sprintf(strang, "light[%i].ambient", i);
		sprintf(streng, "light[%i].diffuse", i);
		glUniform3f(glGetUniformLocation(lightingShader, strang), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(lightingShader, streng), 0.5f, 0.5f, 0.5f);
	}
	glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);
    glUniform1f(glGetUniformLocation(lightingShader, "material.shininess"), 32.0f);

	glm::mat4 boxModel;
	glm::vec3 pos(1.f, 5.f, 1.f);
	boxModel = glm::translate(boxModel, pos);

	#pragma region server
	// get name
	printf("Please enter a name: "); printf("Inzuki\n");
	char moniker[1024]; sprintf(moniker, "Inzuki");
	//gets(moniker);

	// connect to server
	const unsigned short port = 50001;
	//std::string serverIp = "134.129.55.96";
	std::string serverIp = "127.0.0.1";
	sf::IpAddress server = serverIp;

	printf("Connecting to %s...\n", serverIp.c_str());
	
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

		printf("Connected to %s successfully (given ID %i).\n", serverIp.c_str(), ID);
	}
	printf("Other connected players:\n");

	// insert this player into the list
	std::vector<Player> players;
	Player tempPlayer;
	tempPlayer.id = ID;
	sprintf(tempPlayer.name, "Inzuki");
	players.push_back(tempPlayer);

	socket.setBlocking(false);
	#pragma endregion

	bool running = true;
	bool moving[4];
	moving[0] = false;
	moving[1] = false;
	moving[2] = false;
	moving[3] = false;

	sf::Clock moveClk;
	while(running){
		#pragma region receiveFromServer
		// receive from the server
		if(socket.receive(buffer, sizeof(buffer), received, sender, senderPort) == sf::Socket::Done){
			char buff[1024];
			for(int i = 0; i < sizeof(buffer); i++)
				buff[i] = buffer[i + 1];

			switch(buffer[0]){
				// when a new player connects
				case 'a':{
					sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
					tempPlayer.pos = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.dir = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.pos2 = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.model = glm::mat4();
					tempPlayer.moving[0] = false;
					tempPlayer.moving[1] = false;
					tempPlayer.moving[2] = false;
					tempPlayer.moving[3] = false;
					players.push_back(tempPlayer);

					printf("%s has connected.\n", tempPlayer.name);
				}break;
				// inform the player of other connected players
				case 'A':{
					sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
					printf("- %s\n", tempPlayer.name);
					tempPlayer.pos = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.dir = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.pos2 = glm::vec3(0.f, 0.f, 0.f);
					tempPlayer.model = glm::mat4();
					tempPlayer.moving[0] = false;
					tempPlayer.moving[1] = false;
					tempPlayer.moving[2] = false;
					tempPlayer.moving[3] = false;
					players.push_back(tempPlayer);
				}break;
				// when a player disconnects
				case 'r':{
					printf("%s has disconnected.\n", players[atoi(buff)].name);

					for(int i = 0; i < players.size(); i++)
						if(players[i].id == atoi(buff))
							players.erase(players.begin() + i);
				}break;
				// when a player begins moving
				case 'm':{
					int id, key; float dirX, dirZ;
					sscanf(buff, "%i,%i,%f,%f", &id, &key, &dirX, &dirZ);

					for(int i = 0; i < players.size(); i++){
						if(players[i].id == id){
							players[i].moving[key] = true;

							players[i].dir = glm::vec3(dirX, 0.f, dirZ);
						}
					}
				}break;
				// when a player stops moving
				case 's':{
					int id, key; float x, z;
					sscanf(buff, "%i,%i,%f,%f", &id, &key, &x, &z);

					for(int i = 0; i < players.size(); i++){
						if(players[i].id == id){
							players[i].moving[key] = false;

							players[i].model = glm::translate(players[i].model,
															  glm::vec3(x - players[i].pos.x,
																		0.f,
																		z - players[i].pos.z));
							players[i].pos = glm::vec3(x, 0.f, z);
						}
					}
				}break;
			}
		}
		#pragma endregion

		// set the player's height to the terrain's height at that spot
		moveY(terrain1.getHeight(getPos().x, getPos().z) + 5.f);

		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime     = clk.getElapsedTime().asSeconds(),
			  deltaTime       = float(currentTime - lastTime);
		// update window title to display framerate
		sprintf(fps, "[OpenGL] - FPS: %f", (1.f / deltaTime));
		window.setTitle(fps);

		// handle keyboard input
		sf::Event event;
        while(window.pollEvent(event)){
			// close the window and tell the server the client disconnected
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
				sprintf(sendMsg, "-%i", ID);
				socket.send(sendMsg, sizeof(sendMsg), server, port);

				running = false;
				break;
			}

			if(event.type == sf::Event::KeyPressed){
				// when the player moves
				if(event.key.code == sf::Keyboard::W && !moving[0]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 0, getDir().x, getDir().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[0] = true;
				}
				if(event.key.code == sf::Keyboard::A && !moving[1]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 1, getDir().x, getDir().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[1] = true;
				}
				if(event.key.code == sf::Keyboard::S && !moving[2]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 2, getDir().x, getDir().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[2] = true;
				}
				if(event.key.code == sf::Keyboard::D && !moving[3]){
					sprintf(sendMsg, "m%i,%i,%f,%f", ID, 3, getDir().x, getDir().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[3] = true;
				}
			}

			if(event.type == sf::Event::KeyReleased){
				// lock/unlock the cursor
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();

				// when the player stops moving
				if(event.key.code == sf::Keyboard::W){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 0, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[0] = false;
				}
				if(event.key.code == sf::Keyboard::A){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 1, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[1] = false;
				}
				if(event.key.code == sf::Keyboard::S){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 2, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[2] = false;
				}
				if(event.key.code == sf::Keyboard::D){
					sprintf(sendMsg, "s%i,%i,%f,%f", ID, 3, getPos().x, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
					moving[3] = false;
				}
			}
			// when the window is resized, fix the viewport
			if(event.type == sf::Event::Resized){
				glViewport(0, 0, window.getSize().x, window.getSize().y);
				terrain1.updateRes(window);
			}
			// mouse picking information
			if(event.type == sf::Event::MouseButtonPressed){
				glm::vec3 v1, v2;
				get3DRay(&v1, &v2, window);

				// set a restriction on checking the area around the player
				// so it doesn't check over the entire map every call
				float range = 20.f;

				int x_min = 0, z_min = 0, x_max = 0, z_max = 0;

				(int(getPos().x) - range < 0) ? x_min = 0 : x_min = int(getPos().x) - range;
				(int(getPos().z) - range < 0) ? z_min = 0 : z_min = int(getPos().z) - range;

				(int(getPos().x) + range > 399) ? x_max = 399 : x_max = int(getPos().x) + range;
				(int(getPos().z) + range > 399) ? z_max = 399 : z_max = int(getPos().z) + range;

				// set the position of the box
				for(int z = z_min; z < z_max; z++){
					for(int x = x_min; x < x_max; x++){
						if(coll(glm::vec3(x, terrain1.getHeight(x, z), z), 2, v1, v2)){
							// set the box to the position the cursor is
							// pointing at, rather than translating it by that much
							boxModel = glm::translate(
								boxModel, glm::vec3(
									float(x) - pos.x,
									terrain1.getHeight(x, z) + 1.f - pos.y,
									float(z) - pos.z
								)
							);
							// set the position to the new position
							pos = glm::vec3(float(x), terrain1.getHeight(x, z) + 1.f, float(z));

							break; // quit running the for loop when the spot is found
						}
					}
				}
			}
		}
		// compute matrices based on keyboard and mouse input
		computeMats(window, clk, deltaTime);
		
		// clear buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 VP = getProjectionMatrix() * getViewMatrix();

		// skybox
		glDepthMask(GL_FALSE);
			glUseProgram(skyboxShader);
			glm::mat4 VP2 = getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(VP2));

			skybox1.draw(skyboxShader);
		glDepthMask(GL_TRUE);
		
		// draw terrain
		terrain1.draw(VP, lamps);

		// setup light properties
		glUseProgram(lightingShader);
		
		for(int i = 0; i < lamps.size(); i++){
			char strang[128];
			sprintf(strang, "light[%i].position", i);
			glUniform3f(glGetUniformLocation(lightingShader, strang),
				lamps[i].getLampPos().x,
				lamps[i].getLampPos().y,
				lamps[i].getLampPos().z
			);
		}
		
		// draw stall
		glm::mat4 model;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(25.f, 0.f, 25.f));
		stall.draw(model, VP, lightingShader);

		// draw cube
		cube.draw(boxModel, VP, lightingShader);

		// draw players
		for(int i = 0; i < players.size(); i++){
			if(players[i].id != ID){
				if(players[i].moving[0]){
					players[i].pos2.x += players[i].dir.x * deltaTime;
					players[i].pos2.z += players[i].dir.z * deltaTime;

					players[i].model = glm::translate(players[i].model,
													  glm::vec3(players[i].pos2.x,
																0.f,
																players[i].pos2.z));
				}

				cube.draw(players[i].model, VP, lightingShader);
			}
		}
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// draw lamps
		glUseProgram(lampShader);
		GLuint matrixLoc = glGetUniformLocation(lampShader, "VP");
		GLuint modelLoc  = glGetUniformLocation(lampShader, "M");
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-50.f, 65.f, -50.f));
		for(int i = 0; i < lamps.size(); i++)
			lamps[i].draw(model, modelLoc, matrixLoc, VP);

		window.display();

		lastTime = currentTime;
	}

	// free terrain
	terrain1.deleteTerrain();
	// free models
	stall.deleteObj();
	// free lamps
	for(int i = 0; i < lamps.size(); i++)
		lamps[i].deleteLamp();
	// free shaders
	glDeleteProgram(lightingShader);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(lampShader);
	
	return EXIT_SUCCESS; // qed
}