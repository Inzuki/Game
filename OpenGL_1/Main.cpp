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
	float x, y, z;
};

int main(){
	// initialize OpenGL window
	sf::Window window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Resize, sf::ContextSettings(64));
	glViewport(0, 0, window.getSize().x, window.getSize().y);
    window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	sf::Clock clk;
	char fps[32];

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
	printf("Please enter a name: ");
	char moniker[1024];
	gets(moniker);

	// connect to server
	const unsigned short port = 50001;
	std::string serverIp = "134.129.55.96";
	//std::string serverIp = "127.0.0.1";
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
	#pragma endregion server

	bool running = true;
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
					players.push_back(tempPlayer);

					printf("%s has connected.", tempPlayer.name);
				}break;
				// inform the player of other connected players
				case 'A':{
					sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
					printf("- %s\n", tempPlayer.name);
					players.push_back(tempPlayer);
				}break;
				// when a player disconnects
				case 'r':{
					printf("%s has disconnected.", players[atoi(buff)].name);

					for(int i = 0; i < players.size(); i++)
						if(players[i].id == atoi(buff))
							players.erase(players.begin() + i);
				}break;
			}
		}
		#pragma endregion receiveFromServer

		// set the player's height to the terrain's height at that spot
		setPosY(terrain1.getHeight(getPos().x, getPos().z) + 5.f);

		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime = clk.getElapsedTime().asSeconds(),
			  deltaTime = float(currentTime - lastTime);
		// update window title to display framerate
		sprintf(fps, "Game - FPS: %f", 1.f / deltaTime);
		window.setTitle(fps);

		// handle keyboard input
		sf::Event event;
        while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
				char sendMsg[1024];
				sprintf(sendMsg, "-%i", ID);
				socket.send(sendMsg, sizeof(sendMsg), server, port);

				running = false;
				break;
			}

			if(event.type == sf::Event::KeyReleased)
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();

			if(event.type == sf::Event::Resized){
				glViewport(0, 0, window.getSize().x, window.getSize().y);
				terrain1.updateRes(window);
			}

			if(event.type == sf::Event::MouseButtonPressed){
				glm::vec3 v1, v2;
				get3DRay(&v1, &v2, window);

				// set a restriction on checking the area around the player
				// so it doesn't check over the entire map every call
				float range = 25.f;

				int x_min = 0, z_min = 0, x_max = 0, z_max = 0;

				(int(getPos().x) - range < 0) ? x_min = 0 : x_min = int(getPos().x) - range;
				(int(getPos().z) - range < 0) ? z_min = 0 : z_min = int(getPos().z) - range;

				(int(getPos().x) + range > 399) ? x_max = 399 : x_max = int(getPos().x) + range;
				(int(getPos().z) + range > 399) ? z_max = 399 : z_max = int(getPos().z) + range;

				for(int z = z_min; z < z_max; z++){
					for(int x = x_min; x < x_max; x++){
						if(coll(glm::vec3(x, terrain1.getHeight(x, z), z), 2, v1, v2)){
							boxModel = glm::translate(
								boxModel, glm::vec3(
									float(x) - pos.x,
									terrain1.getHeight(x, z) + 1.f - pos.y,
									float(z) - pos.z
								)
							);

							pos = glm::vec3(float(x), terrain1.getHeight(x, z) + 1.f, float(z));

							break;
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
		
		glm::mat4 model;
		// draw stall
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(25.f, 0.f, 25.f));
		stall.draw(model, VP, lightingShader);

		// draw cube
		cube.draw(boxModel, VP, lightingShader);

		// draw players
		for(int i = 0; i < players.size(); i++){
			if(players[i].id != ID){
				model = glm::mat4();
				model = glm::translate(model, glm::vec3(players[i].x,
														players[i].y,
														players[i].z));

				cube.draw(model, VP, lightingShader);
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
		model = glm::translate(model, glm::vec3(
			currentTime * 5.f - 50.f,
			65.f,
			currentTime * 5.f - 50.f)
		);

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
	
	return EXIT_SUCCESS;
}