﻿#include "INCLUDES.h"

#include "GUI.h"
#include "OBJ.h"
#include "Lamp.h"
#include "Water.h"
#include "Assimp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"
#include "MousePicker.h"

#define WATER_HEIGHT 25.0
#define FREE_CAM 1

int main(){
	// initialize SFML-OpenGL window
		char window_title[128] = "OpenGL";
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
		glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

	// load shaders
		GLuint lampShader     = loadShaders("lamp.vert",    "lamp.frag");
		GLuint lightingShader = loadShaders("default.vert", "default.frag");
		GLuint skyboxShader   = loadShaders("skybox.vert",  "skybox.frag");
		GLuint waterShader    = loadShaders("water.vert",   "water.frag");
		GLuint terrainShader  = loadShaders("terrain.vert", "terrain.frag");

	// initialize lamps
		//model = glm::translate(model, glm::vec3(25.f, -7.75f, 25.f));
		Lamp sun(glm::vec3(400.f, 33.f, 15.f), glm::vec3(0.9f, 0.9f, 0.2f));
		Lamp lamp2(glm::vec3(25.f, 2.75f, 25.f), glm::vec3(1.f, 1.0f, 0.0f), glm::vec3(0.5f, 0.01f, 0.001f));
		Lamp lamp3(glm::vec3(20.f, 2.f, 50.f), glm::vec3(0.f, 0.0f, 1.0f), glm::vec3(0.5f, 0.01f, 0.001f));
		std::vector<Lamp> lamps;
		lamps.push_back(sun);
		lamps.push_back(lamp2);
		lamps.push_back(lamp3);

	// load terrain
		SimpleTerrain sTerrain1("res/textures/height2.jpg", terrainShader);

	// load models
		OBJ stall("stall.obj", "res/textures/stallTexture.png"),
			lamp_post("lamp.obj", "res/textures/lamp_post.png"),
			lamp_cube("cube.obj", NULL);

	// load skyboxes
		#pragma region skybox
		// order of skyboxes (from top to bottom): right, left, top, bottom, back, front
			std::vector<const GLchar*> faces;
			faces.push_back("SunSetLeft2048.png");
			faces.push_back("SunSetRight2048.png");
			faces.push_back("SunSetUp2048.png");
			faces.push_back("SunSetDown2048.png");
			faces.push_back("SunSetFront2048.png");
			faces.push_back("SunSetBack2048.png");
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

	// initialize mouse picker
		MousePicker mousePicker(sTerrain1);

	// light properties
		glUseProgram(waterShader);
		glUniform3f(glGetUniformLocation(waterShader,    "sunColor"), sun.getColor().x, sun.getColor().y, sun.getColor().z);
		glUseProgram(terrainShader);
		glUniform3f(glGetUniformLocation(terrainShader,  "skyColor"), 0.2f, 0.3f, 0.3f);
		glUseProgram(lightingShader);
		glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);
		glUniform3f(glGetUniformLocation(lightingShader, "skyColor"), 0.2f, 0.3f, 0.3f);

	glm::vec3 ray_start = position, ray_finish(1.f);

	bool running = true;
	while(running){
		// set the player's Y coordinate based on the terrain's height at that position
		if(FREE_CAM == 0)
			moveY(-sTerrain1.getHeight(getPos().x, getPos().z) + 20.f);

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
			if(event.type == sf::Event::Resized)
				glViewport(0, 0, window.getSize().x, window.getSize().y);

			if(event.type == sf::Event::KeyReleased)
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();
		}
			
		mousePicker.update(window);
		if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
			ray_start  = position;
			ray_finish = ray_start + mousePicker.getCurrentRay() * 10.f;
		}

		glm::mat4 model;
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
			clipPlane = glm::vec4(0.f, 1.f, 0.f, -WATER_HEIGHT + 0.5f);

			// skybox
			glDepthMask(GL_FALSE);
				glUseProgram(skyboxShader);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
				skybox1.draw(skyboxShader);
			glDepthMask(GL_TRUE);
		
			// draw terrain
			glUseProgram(terrainShader);
			sTerrain1.draw(getProjectionMatrix(), getViewMatrix(), lamps, terrainShader, clipPlane);

			// setup light properties
			glUseProgram(lightingShader);
			for(int i = 0; i < MAX_LIGHTS; i++){
				char light_data[64];
				sprintf(light_data, "lightPos[%i]", i);
				glUniform3f(glGetUniformLocation(lightingShader, light_data),
							lamps[i].getLampPos().x,
							lamps[i].getLampPos().y,
							lamps[i].getLampPos().z
				);
				sprintf(light_data, "lightColor[%i]", i);
				glUniform3f(glGetUniformLocation(lightingShader, light_data),
							lamps[i].getColor().x,
							lamps[i].getColor().y,
							lamps[i].getColor().z
				);
				sprintf(light_data, "attenuation[%i]", i);
				glUniform3f(glGetUniformLocation(lightingShader, light_data),
							lamps[i].getAttenuation().x,
							lamps[i].getAttenuation().y,
							lamps[i].getAttenuation().z
				);
			}

			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(6.f, -3.5f, 6.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);
			
			// draw lamp post
			model = glm::mat4();
			//model = glm::translate(model, glm::vec3(25.f, -7.75f, 25.f));
			model = glm::translate(model, ray_finish);
			lamp_post.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, -3.5f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

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
			clipPlane = glm::vec4(0.f, -1.f, 0.f, WATER_HEIGHT + 0.5f);

			// skybox
			glDepthMask(GL_FALSE);
				glUseProgram(skyboxShader);
				glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()))));
				skybox1.draw(skyboxShader);
			glDepthMask(GL_TRUE);
		
			// draw terrain
			glUseProgram(terrainShader);
			sTerrain1.draw(getProjectionMatrix(), getViewMatrix(), lamps, terrainShader, clipPlane);
			
			glUseProgram(lightingShader);
			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(6.f, -3.5f, 6.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);
			
			// draw lamp post
			model = glm::mat4();
			//model = glm::translate(model, glm::vec3(25.f, -7.75f, 25.f));
			model = glm::translate(model, ray_finish);
			lamp_post.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, -3.5f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

			fbos.unbindCurrentFrameBuffer(window.getSize().x, window.getSize().y);
		#pragma endregion

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
			glUseProgram(terrainShader);
			sTerrain1.draw(getProjectionMatrix(), getViewMatrix(), lamps, terrainShader, clipPlane);
			
			glUseProgram(lightingShader);
			// draw stall
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(6.f, -3.5f, 6.f));
			model = glm::rotate(model, -2.5f, glm::vec3(0.f, 1.f, 0.f));
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);
			
			// draw lamp post
			model = glm::mat4();
			//model = glm::translate(model, glm::vec3(25.f, -7.75f, 25.f));
			model = glm::translate(model, ray_finish);
			lamp_post.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw guard
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(12.f, -3.5f, 5.f));
			model = glm::rotate(model, 4.725f, glm::vec3(1.f, 0.f, 0.f));
			model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
			mdl.tick(clk.getElapsedTime().asSeconds() / 2.f);
			mdl.render(deltaTime, model, getProjectionMatrix(), getViewMatrix(), clipPlane);

			// draw lamps
			glUseProgram(lampShader);
			for(int i = 0; i < MAX_LIGHTS; i++){
				model = glm::mat4();
				/*
				lamps[i].moveLamp(
					glm::vec3(
						clk.getElapsedTime().asSeconds() / -20.f,
						0.f,
						clk.getElapsedTime().asSeconds() / -20.f
					)
				);
				//*/

				if(i == 1)
					model = glm::translate(model, ray_finish + glm::vec3(0.f, 10.f, 0.f));
				else
					model = glm::translate(model, lamps[i].getLampPos());

				lamps[i].draw(lamp_cube, model, lampShader, getProjectionMatrix(), getViewMatrix());
			}

			// draw water quad
			glUseProgram(waterShader);
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(125.f, -20.f, 125.f));
			model = glm::scale(model, glm::vec3(fbos.getScale(), fbos.getScale(), fbos.getScale()));
			fbos.render(waterShader, model, getProjectionMatrix() * getViewMatrix(), clk, sun);

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