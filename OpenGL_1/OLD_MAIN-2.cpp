/*
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
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

	// load shaders
		GLuint lampShader     = loadShaders("lamp.vert",    "lamp.frag");
		GLuint lightingShader = loadShaders("default.vert", "default.frag");

	// initialize lamps
		Lamp lamp1(glm::vec3(30.f, 60.f, 30.f), glm::vec3(1.f, 1.f, 1.f));

	// load terrain
		Terrain terrain1("height.jpg", "res/textures/sandgrass.jpg", window);

	// load models
		OBJ stall("stall.obj",  "res/textures/stallTexture.png");
		OBJ rrerr("dragon.obj", "res/textures/blank.png", 10.f, 1.f);

	// light properties
		glUniform1i(glGetUniformLocation(lightingShader, "material.diffuse"),  0);

	bool running = true;
	while(running){
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
				terrain1.updateRes(window);
			}

			if(event.type == sf::Event::KeyReleased)
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();
		}

		glm::mat4 model, VP;
		glm::vec4 clipPlane;

		glEnable(GL_CLIP_DISTANCE0);

		#pragma region main_loop
			// compute matrices based on keyboard and mouse input
			computeMats(window, clk, deltaTime);

			// clear buffer
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// universal object properties (like size)
			VP = getProjectionMatrix() * getViewMatrix();
			glDisable(GL_CLIP_DISTANCE0);
			clipPlane = glm::vec4(0.f, -1.f, 0.f, 1000000);
		
			// draw terrain
			terrain1.updateRes(window);
			terrain1.draw(VP, lamp1, clipPlane);
			
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
			stall.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw dragon
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(30.f, 38.f, 5.f));
			model = glm::rotate(model, clk.getElapsedTime().asSeconds() / 1.5f, glm::vec3(0.f, 1.f, 0.f));
			rrerr.draw(model, getProjectionMatrix(), getViewMatrix(), lightingShader, clipPlane);

			// draw lamps
			glUseProgram(lampShader);
			model = glm::mat4();
			//lamp1.moveLamp(glm::vec3(clk.getElapsedTime().asSeconds() / -100.f,
			//						 0.f,
			//						 clk.getElapsedTime().asSeconds() / -100.f)
			//);
			model = glm::translate(model, lamp1.getLampPos());
			lamp1.draw(model, lampShader, VP);
		#pragma endregion
		
		// disable vertex stuff
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// display window
		window.display();

		lastTime = currentTime;
	}

	#pragma region delete
	// free shaders
	glDeleteProgram(lightingShader);
	glDeleteProgram(lampShader);

	return EXIT_SUCCESS; // qed
	#pragma endregion
}
//*/