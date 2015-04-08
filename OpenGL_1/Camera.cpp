#include "Camera.h"

glm::mat4 view;
glm::mat4 projection;
glm::vec3 position = glm::vec3(0, 0, 5);

float horizontalAngle = 3.14f,
	  verticalAngle   = 0.f,
	  initFOV         = 45.f,
	  speed			  = 3.f,
	  mouseSpeed      = 0.01f;

glm::mat4 getViewMatrix(){ return view; }
glm::mat4 getProjectionMatrix(){ return projection; }
glm::vec3 getPos(){ return position; }

void computeMats(sf::Window &window, sf::Clock clk){
	static float lastTime = clk.restart().asSeconds();
	float currentTime = clk.restart().asSeconds();
	float deltaTime = float(currentTime - lastTime);

	double xpos = sf::Mouse::getPosition().x,
		   ypos = sf::Mouse::getPosition().y;

	sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2));

	horizontalAngle += mouseSpeed * float(window.getSize().x / 2 - xpos);
	verticalAngle   += mouseSpeed * float(window.getSize().y / 2 - ypos);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right(
		sin(horizontalAngle - 3.14f / 2.f),
		0,
		cos(horizontalAngle - 3.14f / 2.f)
	);

	glm::vec3 up = glm::cross(right, direction);

	// handle movement
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		position += direction * speed * deltaTime;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		position -= direction * speed * deltaTime;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		position += right * speed * deltaTime;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		position -= right * speed * deltaTime;

	projection = glm::perspective(initFOV, 4.f / 3.f, .1f, 100.f);

	view = glm::lookAt(
		position,
		position + direction,
		up
	);

	lastTime = currentTime;
}