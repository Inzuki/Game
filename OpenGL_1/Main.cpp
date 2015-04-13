#include "INCLUDES.h"

#include "OBJ.h"
#include "Lamp.h"
#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "Texture.h"

int main(){
	// initialize OpenGL window
	sf::Window window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Resize, sf::ContextSettings(64));
	glViewport(0, 0, window.getSize().x, window.getSize().y);
    window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	sf::Clock clk;
	char fps[32];

	// initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();
	if(err != GLEW_OK)
		std::cout << "glewInit() failed." << std::endl;

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

	// run window
	bool running = true;
	while(running){
		// phys testing
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
				int x_min = 0, z_min = 0, x_max = 0, z_max = 0;

				(int(getPos().x) - 25 < 0) ? x_min = 0 : x_min = int(getPos().x) - 25;
				(int(getPos().z) - 25 < 0) ? z_min = 0 : z_min = int(getPos().z) - 25;

				(int(getPos().x) + 25 > 399) ? x_max = 399 : x_max = int(getPos().x) + 25;
				(int(getPos().z) + 25 > 399) ? z_max = 399 : z_max = int(getPos().z) + 25;

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
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// draw lamps
		glUseProgram(lampShader);

		GLuint matrixLoc = glGetUniformLocation(lampShader, "VP");
		GLuint modelLoc  = glGetUniformLocation(lampShader, "M");
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(
			currentTime * 15.f - 50.f,
			65.f,
			currentTime * 15.f - 50.f)
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