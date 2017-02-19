#include "OBJ.h"
#include "Lamp.h"
#include "Assimp.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

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

void RenderScene(
	sf::Clock &clk, float deltaTime,
	OBJ &stall, OBJ &lamp_post, OBJ &lamp_cube,
	glm::vec4 &clipPlane,
	GLuint &lightingShader, GLuint &skyboxShader, GLuint &lampShader,
	GLuint &woodTexture,
	GLuint &planeVAO,
	CubeMap skybox,
	std::vector<Lamp> &lamps
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
	model = glm::mat4();
	model = glm::scale(model, glm::vec3(5.f));
	model = glm::rotate(model, clk.getElapsedTime().asSeconds(), glm::vec3(0.f, 1.f, 0.f));
	lamp_post.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

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
		lamp_post2("tree.obj", "res/textures/tree.png"),
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
        // Positions            // Normals         // Texture Coords
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f,  25.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 0.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f,  25.0f
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

	#pragma region shadow mapping
	glm::vec3 lightPos(glm::vec3(-45.f, 75.f, -40.f));

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // - Create depth texture
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

	bool running = true;
	while(running){
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

		#pragma region event handling
		sf::Event event;
		while(window.pollEvent(event)){
			// close the window
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				running = false;

			// when the window is resized, fix the viewport
			if(event.type == sf::Event::Resized)
				glViewport(0, 0, window.getSize().x, window.getSize().y);

			if(event.type == sf::Event::KeyReleased){
				// lock/unlock cursor
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();

				// "noclip"
				//if(event.key.code == sf::Keyboard::V)
				//	_FREE_CAM = !_FREE_CAM;
			}
		}
		#pragma endregion

		glm::vec4 clipPlane;

		// compute matrices based on keyboard and mouse input
		computeMats(window, clk, deltaTime);

		#pragma region main_loop
		// render shadows
		glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
		GLfloat near_plane = 1.f, far_plane = 150.f;
		float space = 20.f;
		lightProjection = glm::ortho(-space, space, -space, space, near_plane, far_plane);
        lightView		= glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;

		glUseProgram(simpleDepthShader);
		glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader, "lightSpaceMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		//glCullFace(GL_FRONT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            RenderScene(
				clk, deltaTime,
				stall, lamp_post2, lamp_cube,
				clipPlane,
				simpleDepthShader, skyboxShader, lampShader,
				woodTexture, planeVAO,
				skybox1,
				lamps
			);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK);

		// reset viewport, and display the scene as normal
		glViewport(0, 0, window.getSize().x, window.getSize().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
		
		glUniform3fv(glGetUniformLocation(lightingShader, "lightPos_shade"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(lightingShader, "viewPos"), 1, &getPos()[0]);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader, "lightSpaceMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

		RenderScene(
			clk, deltaTime,
			stall, lamp_post2, lamp_cube,
			clipPlane,
			lightingShader, skyboxShader, lampShader,
			woodTexture, planeVAO,
			skybox1,
			lamps
		);
		
		glViewport(0, 0, 640, 480);
		glUseProgram(debugDepthQuad);
        glUniform1f(glGetUniformLocation(debugDepthQuad, "near_plane"), near_plane);
        glUniform1f(glGetUniformLocation(debugDepthQuad, "far_plane"),  far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        RenderQuad(); // uncomment this line to see depth map

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

	// free ground
	glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
	glDeleteTextures(1, &woodTexture);

	return EXIT_SUCCESS; // qed
	#pragma endregion
}