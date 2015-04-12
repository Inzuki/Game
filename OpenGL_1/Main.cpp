#include "INCLUDES.h"

#include "OBJ.h"
#include "Lamp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

int main(){
	// initialize OpenGL window
	sf::Window window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Resize, sf::ContextSettings(32));
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	sf::Clock clk;
    window.setVerticalSyncEnabled(true);
	char fps[16];

	// initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();
	if(err != GLEW_OK)
		std::cout << "glewInit() failed." << std::endl;

	// set up OpenGL window properties
	glViewport(0, 0, window.getSize().x, window.getSize().y);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	// load shaders
	GLuint lightingShader = loadShaders("vertex.txt",      "fragment.txt");
	GLuint lampShader     = loadShaders("vertex_lamp.txt", "fragment_lamp.txt");
	GLuint skyboxShader   = loadShaders("skybox.vert",     "skybox.frag");
	GLuint terrainShader  = loadShaders("terrain.vert",    "terrain.frag");

	// lamps
	std::vector<Lamp> lamps;
	lamps.push_back(Lamp(glm::vec3(1.2f, 1.0f, 2.0f)));

	// Lamp lamp(glm::vec3(1.2f, 1.0f, 2.0f));

	// load models
	OBJ stall("stall.obj", "stallTexture.png");

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
	Terrain terrain1("height.jpg", "sandgrass.jpg");

	// shader crap
	glUseProgram(terrainShader);
	glUniform1i(glGetUniformLocation(terrainShader, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(terrainShader, "material.two"),     1);
	glUniform1i(glGetUniformLocation(terrainShader, "material.thr"),     2);
	glUniform1i(glGetUniformLocation(terrainShader, "material.path"),    3);
	glUniform1i(glGetUniformLocation(terrainShader, "material.fou"),     4);
	glUniform2f(glGetUniformLocation(terrainShader, "resolution"), window.getSize().x, window.getSize().y);


	glUniform3f(glGetUniformLocation(terrainShader, "light.ambient"),  0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(terrainShader, "light.diffuse"),  0.5f, 0.5f, 0.5f);

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

	// run window
	bool running = true;
	while(running){
		// phys testing
		setPosY(terrain1.getHeight(getPos().x, getPos().z) + 5.f);

		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime = clk.getElapsedTime().asSeconds(),
			  deltaTime = float(currentTime - lastTime);

		sprintf(fps, "%f", 1.f / deltaTime);
		window.setTitle(fps);

		// handle keyboard input
		sf::Event event;
        while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
				running = false;
				break;
			}

			if(event.type == sf::Event::KeyReleased){
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();
			}

			if(event.type == sf::Event::Resized){
				glViewport(0, 0, window.getSize().x, window.getSize().y);
				glUseProgram(terrainShader);
				glUniform2f(glGetUniformLocation(terrainShader, "resolution"), window.getSize().x, window.getSize().y);
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
		
		GLuint lampPosLoc = glGetUniformLocation(terrainShader, "light.position");
		// draw terrain
		glUseProgram(terrainShader);
		
		glUniform3f(lampPosLoc, lamps[0].getLampPos().x, lamps[0].getLampPos().y, lamps[0].getLampPos().z);
		
		terrain1.draw(VP, terrainShader);

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
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// draw lamps
		glUseProgram(lampShader);

		GLuint matrixLoc = glGetUniformLocation(lampShader, "VP");
		GLuint modelLoc  = glGetUniformLocation(lampShader, "M");
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(
			currentTime * 9.f - 50.f,
			65.f,
			currentTime * 9.f - 50.f)
		);

		lamps[0].draw(model, modelLoc, matrixLoc, VP);

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
	glDeleteProgram(terrainShader);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(lampShader);
	
	return 0;
}

	// connect to server
	/*	const unsigned short port = 50001;
		std::string serverIp = "127.0.0.1";
		sf::IpAddress server = serverIp;

		printf("Connecting to %s...\n", serverIp.c_str());
	
	// tell the server the client connected
		char sendMsg[1024];
		sprintf(sendMsg, "+Inzuki");
	
		sf::UdpSocket socket;
		socket.send(sendMsg, sizeof(sendMsg), server, port);
		socket.setBlocking(false);

	// receive from the server if the client successfully connected
		char buffer[1024];
		std::size_t received;
		sf::IpAddress sender;
		unsigned short senderPort;
		socket.receive(buffer, sizeof(buffer), received, sender, senderPort);

		// if the client successfully connected, the server should send back a success signal
		if(buffer[0] == '0'){
			char buff[1024];
			for(int i = 0; i < sizeof(buffer); i++)
				buff[i] = buffer[i + 1];

			ID = atoi(buff);

			printf("Connected to %s successfully (given ID %i).\n", serverIp.c_str(), ID);
		}

		// socket.setBlocking(false);*/