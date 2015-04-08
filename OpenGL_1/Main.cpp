#include "INCLUDES.h"

#include "OBJ.h"
#include "Lamp.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

unsigned int ID;

int main(){
	// initialize OpenGL window
	sf::Window window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Titlebar, sf::ContextSettings(32));
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	sf::Clock clk;
    //window.setVerticalSyncEnabled(true);

	// connect to server
		const unsigned short port = 50001;
		std::string serverIp = "127.0.0.1";
		sf::IpAddress server = serverIp;

		printf("Connecting to %s...\n", serverIp.c_str());
	
	// tell the server the client connected
		char sendMsg[1024];
		sprintf(sendMsg, "0Inzuki");
	
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

		// socket.setBlocking(false);

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

	// lamps
	Lamp lamp(glm::vec3(1.2f, 1.0f, 2.0f));

	// load models
	OBJ stall("stall.obj", "stallTexture.png");

	glUseProgram(lightingShader);
	
	GLuint lampPosLoc = glGetUniformLocation(lightingShader, "light.position");
	GLuint viewPosLoc = glGetUniformLocation(lightingShader, "viewPos");
	GLuint matrixLoc  = glGetUniformLocation(lightingShader, "VP");
	GLuint modelLoc   = glGetUniformLocation(lightingShader, "M");
	glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);
	glUniform3f(glGetUniformLocation(lightingShader, "light.ambient"),  0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(lightingShader, "light.diffuse"),  0.5f, 0.5f, 0.5f);
	glUniform3f(glGetUniformLocation(lightingShader, "light.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(lightingShader, "material.shininess"), 32.0f);
	// use the below specular for when there's no specular texture
	// glUniform3i(glGetUniformLocation(lightingShader, "material.specular"),  0.5f, 0.5f, 0.5f);
	glUniform1i(glGetUniformLocation(lightingShader, "material.specular"),  0);

	bool wChecked = false;

	// run window
	bool running = true;
	while(running){
		// handle keyboard input
		sf::Event event;
        while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
				// tell the server the client disconnected
				sprintf(sendMsg, "-%i", ID);
				socket.send(sendMsg, sizeof(sendMsg), server, port);

				running = false;
				break;
			}

			if(event.type == sf::Event::KeyPressed){
				if(event.key.code == sf::Keyboard::W && !wChecked){
					wChecked = true;
					printf("go\n");
				}
			}
			
			if(event.type == sf::Event::KeyReleased){
				if(event.key.code == sf::Keyboard::W && wChecked){
					wChecked = false;
					printf("stop\n");
				}
			}
		}

		// compute matrices based on keyboard and mouse input
		computeMats(window, clk);
		
		// clear buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// setup light properties
		glUseProgram(lightingShader);

		glUniform3f(lampPosLoc, lamp.getLampPos().x, lamp.getLampPos().y, lamp.getLampPos().z);
		glUniform3f(viewPosLoc, getPos().x, getPos().y, getPos().z);

		glm::mat4 VP = getProjectionMatrix() * getViewMatrix();
		glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(VP));
		
		glm::mat4 model;
		// draw stall
		model = glm::scale(model, glm::vec3(0.25f));
		model = glm::rotate(model, 3.5f, glm::vec3(0.0, 1.0, 0.0));
		stall.draw(model, modelLoc);
		
		model = glm::translate(model, glm::vec3(9, 0, -9));
		model = glm::rotate(model, 1.5f, glm::vec3(0.0, 1.0, 0.0));
		stall.draw(model, modelLoc);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(1);

		// draw lamp
		glUseProgram(lampShader);

		matrixLoc = glGetUniformLocation(lampShader, "VP");
		modelLoc  = glGetUniformLocation(lampShader, "M");
		
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.1f));
		model = glm::translate(model, glm::vec3(1.2f, 10.0f, 20.0f));

		lamp.draw(model, modelLoc, matrixLoc, VP);

		window.display();
	}

	// free models
	stall.deleteObj();

	// free lamps
	lamp.deleteLamp();

	// free shaders
	glDeleteProgram(lightingShader);
	glDeleteProgram(lampShader);
	
	return 0;
}