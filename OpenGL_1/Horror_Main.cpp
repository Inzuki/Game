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
	glm::vec3 pos, dir, right;
	glm::mat4 model;
	bool moving[4];
};

int main(){
	// initialize OpenGL window
	sf::RenderWindow window(sf::VideoMode(1600, 1024), "OpenGL", sf::Style::Resize, sf::ContextSettings(64));
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
	OBJ stall("stall.obj", "res/textures/stallTexture.png");

	// load skybox(es)
	std::vector<const GLchar*> faces;
	faces.push_back("right.jpg");
	faces.push_back("left.jpg");
	faces.push_back("top.jpg");
	faces.push_back("bottom.jpg");
	faces.push_back("back.jpg");
	faces.push_back("front.jpg");
	CubeMap skybox1(faces);

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
	glm::vec3 boxPos(1.f, 5.f, 1.f);
	boxModel = glm::translate(boxModel, boxPos);

	// stuff
	bool running = true;

	while(running){
		float sx = 2.f / window.getSize().x,
			  sy = 2.f / window.getSize().y;

		// set the player's height to the terrain's height at that spot
		moveY(5.f);

		// calculate timestamp
		static float lastTime = clk.getElapsedTime().asSeconds();
		float currentTime     = clk.getElapsedTime().asSeconds(),
			  deltaTime       = float(currentTime - lastTime);
		// update window title to display framerate
		sprintf(fps, "[OpenGL] - FPS: %i", (int)round(1.f / deltaTime));
		window.setTitle(fps);

		// handle keyboard input
		sf::Event event;
        while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				running = false;

			if(event.type == sf::Event::KeyReleased){
				// lock/unlock the cursor
				if(event.key.code == sf::Keyboard::Q)
					setCursorLocked();
			}

			// when the window is resized, fix the viewport
			if(event.type == sf::Event::Resized)
				glViewport(0, 0, window.getSize().x, window.getSize().y);
		}
		// compute matrices based on keyboard and mouse input
		computeMats(window, clk, deltaTime);
		
		// clear buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 VP = getProjectionMatrix() * getViewMatrix();
		glm::mat4 model;

		// skybox
		glDepthMask(GL_FALSE);
			glUseProgram(skyboxShader);
			glm::mat4 VP2 = getProjectionMatrix() * glm::mat4(glm::mat3(getViewMatrix()));
			glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "VP"), 1, GL_FALSE, glm::value_ptr(VP2));

			skybox1.draw(skyboxShader);
		glDepthMask(GL_TRUE);

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
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(25.f, 0.f, 25.f));
		stall.draw(model, VP, lightingShader);

		// draw lamps
		glUseProgram(lampShader);
		GLuint matrixLoc = glGetUniformLocation(lampShader, "VP"), modelLoc  = glGetUniformLocation(lampShader, "M");
		
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-25.f, 60.f, -25.f));
		for(int i = 0; i < lamps.size(); i++)
			lamps[i].draw(model, modelLoc, matrixLoc, VP);
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// display window
		window.display();

		lastTime = currentTime;
	}

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