/*#include "GUI.h"
#include "OBJ.h"
#include "Lamp.h"
//#include "Water.h"
#include "Assimp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

int ID = -1;

struct Player {
	int id;
	char name[1024];
	glm::vec3 pos, dir, right;
	bool moving[4];
};

struct ChatMessage {
	std::string text;
	int r, g, b;
};

// for rendering a quad to the screen
GLuint quadVAO = 0, quadVBO;
void RenderQuad(){
	if(quadVAO == 0){
		GLfloat quadVerts[] = {
			// position      // texture coords
			-1.f,  1.f, 0.f, 0.f, 1.f,
			-1.f, -1.f, 0.f, 0.f, 0.f,
			 1.f,  1.f, 0.f, 1.f, 1.f,
			 1.f, -1.f, 0.f, 1.f, 0.f
		};

		glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), &quadVerts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}

	glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
void RenderCube(){
    if(cubeVAO == 0){
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
             0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
             0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
             0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
             0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
             0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
             0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
             0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
             0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
             0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
             0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
             0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
             0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void RenderScene(
	float deltaTime,
	OBJ &stall, OBJ &lamp_post, OBJ &lamp_cube,
	glm::vec4 &clipPlane,
	GLuint &lightingShader, GLuint &skyboxShader, GLuint &lampShader,
	GLuint &woodTexture,
	GLuint &planeVAO,
	CubeMap skybox,
	std::vector<Player> &players, std::vector<Lamp> &lamps
){
	// skybox
	glDepthMask(GL_FALSE);
		glUseProgram(skyboxShader);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
		skybox.draw(skyboxShader);
	glDepthMask(GL_TRUE);
		
	glUseProgram(lightingShader);

	glm::mat4 model;
	// draw stall
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(5.f, 0.f, 15.f));
	model = glm::rotate(model, -.5f, glm::vec3(0.f, 1.f, 0.f));
	stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);
			
	// draw ground
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.f, 1.f, 0.f));
	model = glm::scale(model, glm::vec3(2.f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	glBindVertexArray(planeVAO);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
			
	// draw lamp post
	lamp_post.draw(glm::mat4(), getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

	// draw other players
	for(int i = 0; i < (int)players.size(); i++){
		// no need to display ourselves - the client relative to the program,
		// we'll never see ourselves (barring mirrors or third person).
		// plus, it'd just get in the way of the camera and waste resources
		if(players[i].id != ID){
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

	// draw lamps
	glUseProgram(lampShader);
	for(int i = 0; i < (int)lamps.size(); i++){
		model = glm::mat4();
				
		model = glm::translate(model, lamps[i].getLampPos());
		//if(i == 1) // draw the light from the lamp wherever it is placed from the ray
			//model = glm::translate(model, ray_finish + glm::vec3(0.f, 10.f, 0.f)); // for mousepicking
			//model = glm::translate(model, glm::vec3(0.f, 10.f, 0.f));
		//else
			//model = glm::translate(model, lamps[i].getLampPos());

		lamps[i].draw(lamp_cube, model, lampShader, getProjectionMatrix(), getViewMatrix());
	}
}

int main(){
	#pragma region server
	// let the client enter a name
	printf("Please enter a name: "); printf("Inzuki\n");
	char moniker[1024]; sprintf(moniker, "Inzuki");
	//std::cin.getline(moniker, sizeof(moniker));

	// let the client enter an IP address to connect to
	printf("Please enter a server IP: "); printf("127.0.0.1\n");
	char someIP[128]; sprintf(someIP, "127.0.0.1");
	//gets(someIP);

	// shortcutting so you don't have to type the IP every time
	// if the shortcut is localhost
	if(strcmp(someIP, "lh") == 0)
		sprintf(someIP, "127.0.0.1");

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

		printf("Connected to %s successfully (given ID %i).\nOther connected players:\n", serverIP.c_str(), ID);
	}else if(buffer[0] == 'n'){
		char buff[1024];
		for(int i = 0; i < sizeof(buffer); i++)
			buff[i] = buffer[i + 1];

		printf("Failed to connect to server - Reason: %s\n", buff);
		system("PAUSE");
		return EXIT_FAILURE;
	}

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
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
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

	sf::Clock message_fade;	// this one is for when a message is received.
	bool ignore_msg_start = false;
	#pragma endregion

	#pragma region load shaders and set up properties
	GLuint lampShader     = loadShaders("lamp.vert",    "lamp.frag");
	GLuint lightingShader = loadShaders("default.vert", "default.frag");
	GLuint skyboxShader   = loadShaders("skybox.vert",  "skybox.frag");
	//GLuint terrainShader  = loadShaders("terrain.vert", "terrain.frag");

	GLuint debugDepthQuad = loadShaders("debug_quad.vert", "debug_quad.frag");
	GLuint simpleDepthShader = loadShaders("simpleDepthShader.vert", "simpleDepthShader.frag");
	
	// set up shader properties
	//glUseProgram(terrainShader);
	//glUniform3f(glGetUniformLocation(terrainShader,  "skyColor"), 0.2f, 0.3f, 0.3f);
	glUseProgram(lightingShader);
	glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader, "shadowMap"), 1);
	glUniform3f(glGetUniformLocation(lightingShader, "skyColor"), 0.2f, 0.3f, 0.3f);
	#pragma endregion

	#pragma region lamps (lights)
	// REMEMBER: when adding/removing lights, make sure to update the number of lights in the default.vert/frag files
	//model = glm::translate(model, glm::vec3(25.f, -7.75f, 25.f));
	Lamp sun(glm::vec3(125.f, 75.f, 125.f), glm::vec3(0.9f, 0.9f, 0.2f)); // overall "environment" lighting
	Lamp lamp2(glm::vec3(-5.f, 10.f, -5.f), glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.5f, 0.01f, 0.001f)); // yellow lamp (follows lamp post)
	std::vector<Lamp> lamps;
	lamps.push_back(sun);
	lamps.push_back(lamp2);
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

	#pragma region ground
	GLfloat planeVertices[] = {
        // Positions           // Normals        // Texture Coords
         25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
        -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,
        -25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f,

         25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
         25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f,
        -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f
    };
    // Setup plane VAO
    GLuint planeVAO, planeVBO, woodTexture;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glBindVertexArray(0);

	woodTexture = loadTexture("res/textures/wood.png");
	#pragma endregion

	#pragma region shadowmap depthbuffer
	glm::vec3 lightPos(glm::vec3(-2.f, 4.f, -1.f));

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	#pragma endregion

	// noclip setting
	bool _FREE_CAM = 0;

	// settings for sending information to server
		// these settings will help prevent DDOS'ing the server and only
		// telling the server when the client starts and stops moving once
		bool moving[4];
		for(int i = 0; i < 4; i++)
			moving[i] = false;
		// we'll be updating the client's direction to the server every so often, so we need a clock to track it
		sf::Clock dir_change;
		
		//GLfloat near_plane = 1.0f, far_plane = 7.5f;
	bool running = true;
	while(running){
		#pragma region receive from server
		if(socket.receive(buffer, sizeof(buffer), received, sender, senderPort) == sf::Socket::Done){
			char buff[1024];
			for(int i = 0; i < sizeof(buffer); i++)
				buff[i] = buffer[i + 1];

			switch(buffer[0]){
				// when a new player connects
				case 'a':{
					sscanf(buff, "%i,%s", &tempPlayer.id, &tempPlayer.name);
					tempPlayer.pos   = glm::vec3(20.f, 10.f, 20.f);
					tempPlayer.dir   = glm::vec3(0.f);
					tempPlayer.right = glm::vec3(0.f);
					for(int i = 0; i < 4; i++)
						tempPlayer.moving[i] = false;

					players.push_back(tempPlayer);

					printf("%s has connected.\n", tempPlayer.name);
				}break;
				// inform the player of other connected players
				case 'A':{
					sscanf(buff,
						   "%i,%f,%f,%f,%s",
						   &tempPlayer.id,
						   &tempPlayer.pos.x,
						   &tempPlayer.pos.y,
						   &tempPlayer.pos.z,
						   &tempPlayer.name
					);
					tempPlayer.dir   = glm::vec3(0.f);
					tempPlayer.right = glm::vec3(0.f);
					for(int i = 0; i < 4; i++)
						tempPlayer.moving[i] = false;
					printf("- %s\n", tempPlayer.name);

					players.push_back(tempPlayer);
				}break;
				// when a player disconnects
				case 'r':{
					for(int i = 0; i < (int)players.size(); i++){
						if(players[i].id == atoi(buff)){
							printf("%s has disconnected.\n", players[i].name);
							players.erase(players.begin() + i);
							break;
						}
					}
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
					for(int i = 0; i < (int)players.size(); i++)
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
					else {
						printf("\nERR: Unknown chat message received.\n\n");
						break;
					}

					chat_messages.push_back(temp_msg);

					chatmessages_box.setFillColor(sf::Color(125, 125, 245, 60));
					message_fade.restart();
				}break;
				// when a player is moving
				case '>':{
					int tID, key;
					float dirx, diry, dirz, rx, ry, rz;
					sscanf(buff, "%i,%i,%f,%f,%f,%f,%f,%f", &tID, &key, &dirx, &diry, &dirz, &rx, &ry, &rz);

					for(int i = 0; i < (int)players.size(); i++){
						if(tID == players[i].id){
							players[i].dir   = glm::vec3(dirx, diry, dirz);
							players[i].right = glm::vec3(rx, ry, rz);
							players[i].moving[key] = true;
						}
					}
				}break;
				// when a player stops moving
				case '<':{
					int tID, key;
					float x, y, z;
					sscanf(buff, "%i,%i,%f,%f,%f", &tID, &key, &x, &y, &z);

					for(int i = 0; i < (int)players.size(); i++){
						if(tID == players[i].id){
							players[i].pos = glm::vec3(x, y, z);
							players[i].moving[key] = false;
						}
					}
				}break;
				// updated direction information from other players
				case 'd':{
					int tID;
					float dirx, diry, dirz, rx, ry, rz;
					sscanf(buff, "%i,%f,%f,%f,%f,%f,%f", &tID, &dirx, &diry, &dirz, &rx, &ry, &rz);

					for(int i = 0; i < (int)players.size(); i++){
						if(tID == players[i].id){
							players[i].dir   = glm::vec3(dirx, diry, dirz);
							players[i].right = glm::vec3(rx, ry, rz);
						}
					}
				}break;
			}
		}
		#pragma endregion

		// set the player's Y coordinate based on the terrain's height at that position
		if(_FREE_CAM) position.y = 5.f;

		#pragma region timestamp calculations
		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime     = clk.getElapsedTime().asSeconds(),
				deltaTime       = float(currentTime - lastTime);
		// update window title to display current framerate
		sprintf(fps, "[%s] - FPS: %i", window_title, (int)round(1.f / deltaTime)); window.setTitle(fps);
		#pragma endregion

		#pragma region server updating
		// let the server know the player moved the mouse to update the direction.
		// also use a clock so we don't spam the server with constant direction changing and crash it
		if(dir_change.getElapsedTime().asMilliseconds() >= 5 && sf::Mouse::getPosition().x != window.getSize().x / 2 && sf::Mouse::getPosition().y != window.getSize().y / 2){
			if(moving[0] || moving[1] || moving[2] || moving[3]){
				sprintf(sendMsg, "d%i,%f,%f,%f,%f,%f", ID, getDir().x, getDir().y, getDir().z, getRight().x, getRight().z);
				socket.send(sendMsg, sizeof(sendMsg), server, port);
				dir_change.restart();
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

			if(event.type == sf::Event::TextEntered)
				if(typingStatus())
					if(event.text.unicode > 31 && event.text.unicode < 128)
						sprintf(typingText, "%s%c", typingText, static_cast<char>(event.text.unicode));

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

				// here we handle sending the position of the player to the server.
				// we inform the server that we have started moving and on KeyReleased,
				// we inform the server that we have stopped moving. reason being is to
				// prevent us spamming the server every millisecond that we're moving,
				// which would DDOS the server and client connected.
				// also, we don't need to send the position, as it'll be sent and
				// received when the client stops moving
				if(!moving[0] && event.key.code == sf::Keyboard::W){
					moving[0] = true;
					sprintf(sendMsg,
							">%i,%i,%f,%f,%f,%f,%f,%f",
							ID, 0,
							getDir().x, getDir().y, getDir().z,
							getRight().x, getRight().y, getRight().z
					);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
				if(!moving[1] && event.key.code == sf::Keyboard::A){
					moving[1] = true;
					sprintf(sendMsg,
							">%i,%i,%f,%f,%f,%f,%f,%f",
							ID, 1,
							getDir().x, getDir().y, getDir().z,
							getRight().x, getRight().y, getRight().z
					);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
				if(!moving[2] && event.key.code == sf::Keyboard::S){
					moving[2] = true;
					sprintf(sendMsg,
							">%i,%i,%f,%f,%f,%f,%f,%f",
							ID, 2,
							getDir().x, getDir().y, getDir().z,
							getRight().x, getRight().y, getRight().z
					);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
				if(!moving[3] && event.key.code == sf::Keyboard::D){
					moving[3] = true;
					sprintf(sendMsg,
							">%i,%i,%f,%f,%f,%f,%f,%f",
							ID, 3,
							getDir().x, getDir().y, getDir().z,
							getRight().x, getRight().y, getRight().z
					);
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
				//if(event.key.code == sf::Keyboard::V)
				//	_FREE_CAM = !_FREE_CAM;

				// informing the server the client stopped moving while also preventing DDOS'ing the server
				if(moving[0] && event.key.code == sf::Keyboard::W){
					moving[0] = false;
					sprintf(sendMsg, "<%i,%i,%f,%f,%f", ID, 0, getPos().x, getPos().y, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
				if(moving[1] && event.key.code == sf::Keyboard::A){
					moving[1] = false;
					sprintf(sendMsg, "<%i,%i,%f,%f,%f", ID, 1, getPos().x, getPos().y, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
				if(moving[2] && event.key.code == sf::Keyboard::S){
					moving[2] = false;
					sprintf(sendMsg, "<%i,%i,%f,%f,%f", ID, 2, getPos().x, getPos().y, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
				if(moving[3] && event.key.code == sf::Keyboard::D){
					moving[3] = false;
					sprintf(sendMsg, "<%i,%i,%f,%f,%f", ID, 3, getPos().x, getPos().y, getPos().z);
					socket.send(sendMsg, sizeof(sendMsg), server, port);
				}
			}
		}
		#pragma endregion

		glm::vec4 clipPlane;
		//glEnable(GL_CLIP_DISTANCE0);

		// compute matrices based on keyboard and mouse input
		computeMats(window, clk, deltaTime);

		// setup light properties - send light information to the shader
		glUseProgram(lightingShader);
		for(int i = 0; i < (int)lamps.size(); i++){
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
		// universal object properties (like size)
		glDisable(GL_CLIP_DISTANCE0);
		// TODO: explain the ridiculously large number for the W component
		//clipPlane = glm::vec4(0.f, -1.f, 0.f, 1000000);

		// move light source
		//lightPos.x = sin(clk.getElapsedTime().asSeconds() * 2.f) * 10.f;
		//lightPos.z = cos(clk.getElapsedTime().asSeconds() * 2.f) * 10.f;

		// depth and shadow stuff
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 1.f, far_plane = 7.5f;

		lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
		lightView       = glm::lookAt(lightPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
		lightSpaceMatrix = lightProjection * lightView;

		glUseProgram(simpleDepthShader);
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "lightSpaceMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
			RenderScene(
				deltaTime,
				stall, lamp_post, lamp_cube,
				clipPlane,
				simpleDepthShader, skyboxShader, lampShader,
				woodTexture, planeVAO,
				skybox1,
				players, lamps
			);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport, and display the scene as normal
		glViewport(0, 0, window.getSize().x, window.getSize().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set up properties for shadows
		glUseProgram(lightingShader);
		setProj(glm::perspective(45.f, 4.f / 3.f, 0.1f, 333.0f));
		glUniform3fv(glGetUniformLocation(lightingShader, "lightPos_shade"), 1, &lightPos[0]);
		glUniform3fv(glGetUniformLocation(lightingShader, "viewPos"), 1, &getPos()[0]);
		glUniformMatrix4fv(glGetUniformLocation(lightingShader, "lightSpaceMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

		RenderScene(
			deltaTime,
			stall, lamp_post, lamp_cube,
			clipPlane,
			lightingShader, skyboxShader, lampShader,
			woodTexture, planeVAO,
			skybox1,
			players, lamps
		);

		// for debugging the depth map
		glUseProgram(debugDepthQuad);
		glUniform1f(glGetUniformLocation(debugDepthQuad, "near_plane"), near_plane);
        glUniform1f(glGetUniformLocation(debugDepthQuad, "far_plane"), far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        RenderQuad();
		
		#pragma region chat box
		// save OpenGL state
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		window.pushGLStates();
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
						chatmessages_box.setFillColor(sf::Color(125, 125, 245, (sf::Uint8)(60.f - (message_fade.getElapsedTime().asSeconds() - (fade_time - 2.f)) * 30.f)));

						// fade text
						for(int i = 0; i < (int)chat_messages.size(); i++){
							chat_msgs_text.setColor(
								sf::Color(
									chat_messages[i].r,
									chat_messages[i].g,
									chat_messages[i].b,
									(sf::Uint8)(255.f - (message_fade.getElapsedTime().asSeconds() - (fade_time - 2.f)) * 123.f)
								)
							);

							chat_msgs_text.setPosition(25.f, chatmessages_box.getPosition().y + 10.f + (i * 20.f));
							chat_msgs_text.setString(chat_messages[i].text);
							window.draw(chat_msgs_text);
						}
					}
				}

				if(message_fade.getElapsedTime().asSeconds() <= fade_time - 2.f){
					for(int i = 0; i < (int)chat_messages.size(); i++){
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
		#pragma endregion
	}

	#pragma region cleanup
	// free shaders
	glDeleteProgram(lightingShader);
	glDeleteProgram(skyboxShader);
	glDeleteProgram(lampShader);

	glDeleteProgram(simpleDepthShader);
	glDeleteProgram(debugDepthQuad);

	// free depthmap
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);

	// free ground
	glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
	glDeleteTextures(1, &woodTexture);

	return EXIT_SUCCESS; // qed
	#pragma endregion
}*/