#include "INCLUDES.h"

#include "GUI.h"
#include "OBJ.h"
#include "Lamp.h"
#include "Water.h"
#include "Assimp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

#define WATER_HEIGHT 25.0
#define FREE_CAM 1

int main(){
	// initialize SFML-OpenGL window
		char window_title[128] = "Horror Game";
		sf::ContextSettings contextSettings;
		contextSettings.antialiasingLevel = 4;
		contextSettings.depthBits = 64;
		sf::RenderWindow window(sf::VideoMode(1600, 1024),
								window_title,
								sf::Style::Resize,
								contextSettings
		);
		glViewport(0, 0, window.getSize().x, window.getSize().y);
		window.setVerticalSyncEnabled(true);
		window.setFramerateLimit(60);
		sf::Clock clk;
		char fps[64];

	// initialize GLEW (and other OpenGL settings)
		glewExperimental = true; glewInit();
		glEnable(GL_MULTISAMPLE);  
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// load shaders
		GLuint lampShader     = loadShaders("lamp.vert",    "lamp.frag");
		GLuint lightingShader = loadShaders("default.vert", "default.frag");
		GLuint skyboxShader   = loadShaders("skybox.vert",  "skybox.frag");
		GLuint waterShader    = loadShaders("water.vert",   "water.frag");
		GLuint terrainShader  = loadShaders("terrain.vert", "terrain.frag");

	// initialize lamps
		Lamp lamp1(glm::vec3(20.f, 75.f, 20.f), glm::vec3(1.f, 1.f, 1.f));

	// load terrain
		Terrain terrain1("height.jpg", window, terrainShader);
		SimpleTerrain sTerrain1(64, 64, "res/textures/sandgrass.jpg");

	// load models
		OBJ stall("stall.obj", "res/textures/stallTexture.png");

	// load skyboxes
		#pragma region skybox
		// order of skyboxes (from top to bottom): right, left, top, bottom, back, front
			std::vector<const GLchar*> faces;
			faces.push_back("redday_right.jpg");
			faces.push_back("redday_left.jpg");
			faces.push_back("redday_top.jpg");
			faces.push_back("redday_top.jpg");
			faces.push_back("redday_front.jpg");
			faces.push_back("redday_back.jpg");
			CubeMap skybox1(faces);
		#pragma endregion

	// load anims
		core::Model mdl;
		core::ModelLoader mdlLoad;
		mdlLoad.loadModel("res/models/boblampclean.md5mesh", &mdl);
		mdl.init();

	// initialize water
		Water fbos(window.getSize().x, window.getSize().y);

	// load GUI
		std::vector<GUI> guis;
		GUIRenderer guiRender;

	// light properties
		glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);

	bool running = true;
	while(running){
		// set the player's Y coordinate based on the terrain's height at that position
		if(FREE_CAM == 0)
			moveY(terrain1.getHeight(getPos().x, getPos().z) + 4.5f);

		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime     = clk.getElapsedTime().asSeconds(),
			  deltaTime       = float(currentTime - lastTime);
		// update window title to display framerate
		sprintf(fps, "[%s] - FPS: %i", window_title, (int)round(1.f / deltaTime)); window.setTitle(fps);

		// handle events
		sf::Event event;
        while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				running = false;

			// when the window is resized, fix the viewport
			if(event.type == sf::Event::Resized){
				glViewport(0, 0, window.getSize().x, window.getSize().y);
				terrain1.updateRes(window, terrainShader);
			}

			if(event.type == sf::Event::KeyReleased)
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();
		}

		glm::mat4 model, VP;
		glm::vec4 clipPlane;

		glEnable(GL_CLIP_DISTANCE0);

		#pragma region reflection_main_loop
			// compute matrices based on keyboard and mouse input
			computeMats_reflection(window, clk, deltaTime);

			fbos.bindReflectionFrameBuffer();
		
			// clear buffer
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// universal object properties (like size)
			VP = getProjectionMatrix() * getViewMatrix();
			clipPlane = glm::vec4(0.f, 1.f, 0.f, -WATER_HEIGHT + 0.5f);

			// skybox
			glDepthMask(GL_FALSE);
				glUseProgram(skyboxShader);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
				skybox1.draw(skyboxShader);
			glDepthMask(GL_TRUE);
		
			// draw terrain
			glUseProgram(terrainShader);
			terrain1.updateRes(REFLECTION_WIDTH, REFLECTION_HEIGHT, terrainShader);
			terrain1.draw(getProjectionMatrix(), getViewMatrix(), lamp1, terrainShader, clipPlane);

			// setup light properties
			glUseProgram(lightingShader);
			glUniform3f(glGetUniformLocation(
				lightingShader, "lightPos"),
				lamp1.getLampPos().x,
				lamp1.getLampPos().y,
				lamp1.getLampPos().z
			);
			glUniform3f(glGetUniformLocation(
				lightingShader, "lightColor"),
				lamp1.getColor().x,
				lamp1.getColor().y,
				lamp1.getColor().z
			);
			
			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(5.f, 38.f, 5.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, 38.f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

			// draw lamps
			glUseProgram(lampShader);
			model = glm::mat4();
			model = glm::translate(model, lamp1.getLampPos());
			lamp1.draw(model, lampShader, VP);

			//fbos.unbindCurrentFrameBuffer(window.getSize().x, window.getSize().y);
		#pragma endregion

		#pragma region refraction_main_loop
			// compute matrices based on keyboard and mouse input
			computeMats(window, clk, deltaTime);

			fbos.bindRefractionFrameBuffer();
		
			// clear buffer
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// universal object properties (like size)
			VP = getProjectionMatrix() * getViewMatrix();
			clipPlane = glm::vec4(0.f, -1.f, 0.f, WATER_HEIGHT + 0.5f);

			// skybox
			glDepthMask(GL_FALSE);
				glUseProgram(skyboxShader);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
				skybox1.draw(skyboxShader);
			glDepthMask(GL_TRUE);
		
			// draw terrain
			glUseProgram(terrainShader);
			terrain1.updateRes(REFRACTION_WIDTH, REFRACTION_HEIGHT, terrainShader);
			terrain1.draw(getProjectionMatrix(), getViewMatrix(), lamp1, terrainShader, clipPlane);

			glUseProgram(lightingShader);
			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(5.f, 38.f, 5.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, 38.f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

			// draw lamps
			glUseProgram(lampShader);
			model = glm::mat4();
			model = glm::translate(model, lamp1.getLampPos());
			lamp1.draw(model, lampShader, VP);

			fbos.unbindCurrentFrameBuffer(window.getSize().x, window.getSize().y);
		#pragma endregion

		#pragma region main_loop
			// clear buffer
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// universal object properties (like size)
			VP = getProjectionMatrix() * getViewMatrix();
			glDisable(GL_CLIP_DISTANCE0);
			clipPlane = glm::vec4(0.f, -1.f, 0.f, 1000000);

			// skybox
			glDepthMask(GL_FALSE);
				glUseProgram(skyboxShader);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
				skybox1.draw(skyboxShader);
			glDepthMask(GL_TRUE);
		
			// draw terrain
			glUseProgram(terrainShader);
			sTerrain1.draw(getProjectionMatrix(), getViewMatrix(), lamp1, terrainShader, clipPlane);
			//terrain1.updateRes(window, terrainShader);
			//terrain1.draw(getProjectionMatrix(), getViewMatrix(), lamp1, terrainShader, clipPlane);

			glUseProgram(lightingShader);
			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(5.f, 38.f, 5.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, 38.f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

			// draw lamps
			glUseProgram(lampShader);
			model = glm::mat4();
			//lamp1.moveLamp(glm::vec3(clk.getElapsedTime().asSeconds() / -20.f,
			//						 0.f,
			//						 clk.getElapsedTime().asSeconds() / -20.f)
			//);
			model = glm::translate(model, lamp1.getLampPos());
			lamp1.draw(model, lampShader, VP);

			// draw water quad
			/*glUseProgram(waterShader);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(100.f, 25.f, 100.f));
			model = glm::scale(model, glm::vec3(fbos.getScale(), fbos.getScale(), fbos.getScale()));
			fbos.render(waterShader, model, VP, clk, lamp1);*/

			// GUIs (must be rendered last)
			guiRender.render(guis);
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
	glDeleteProgram(waterShader);
	glDeleteProgram(lampShader);

	return EXIT_SUCCESS; // qed
	#pragma endregion
}