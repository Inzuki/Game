#include "Camera.h"

#define RAY_RANGE 600
#define RECURSION_COUNT 200

glm::mat4 view, projection;
glm::vec3 position = glm::vec3(10, 5, 10);
glm::vec3 direction, right, up;

float horizontalAngle = 3.14f,
	  verticalAngle   = 45.f,
	  initFOV         = 45.f,
	  speed			  = 25.f,
	  mouseSpeed      = 0.01f;

bool cursorLocked = true;

glm::mat4 getViewMatrix(){ return view; }
glm::mat4 getProjectionMatrix(){ return projection; }
glm::vec3 getPos(){ return position; }
void setPosY(float y){ position.y = y; }

void setCursorLocked(){
	if(cursorLocked)
		cursorLocked = false;
	else
		cursorLocked = true;
}

void computeMats(sf::Window &window, sf::Clock clk, float deltaTime){
	double xpos = sf::Mouse::getPosition().x,
		   ypos = sf::Mouse::getPosition().y;

	if(cursorLocked){
		window.setMouseCursorVisible(false);
		sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2));

		horizontalAngle += mouseSpeed * float(window.getSize().x / 2 - xpos);
		verticalAngle   += mouseSpeed * float(window.getSize().y / 2 - ypos);

		direction = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.f),
			0,
			cos(horizontalAngle - 3.14f / 2.f)
		);

		// handle movement
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			position += direction * speed * deltaTime;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			position -= direction * speed * deltaTime;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			position += right * speed * deltaTime;
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			position -= right * speed * deltaTime;

		glm::vec3 up = glm::cross(right, direction);

		view = glm::lookAt(
			position,
			position + direction,
			up
		);
	}else
		window.setMouseCursorVisible(true);

	projection = glm::perspective(initFOV, 4.f / 3.f, .1f, 225.f);
}

bool coll(glm::vec3 pos, float r, glm::vec3 vA, glm::vec3 vB){
	glm::vec3 dirToSphere = pos - vA;
	glm::vec3 lineDir = glm::normalize(vB - vA);
	float lineLength = glm::distance(vA, vB);
	float t = glm::dot(dirToSphere, lineDir);
	glm::vec3 closestPoint;

	if(t <= 0.f)
		closestPoint = vA;
	else if(t >= lineLength)
		closestPoint = vB;
	else
		closestPoint = vA + lineDir * t;

	return glm::distance(pos, closestPoint) <= r;
}

void get3DRay(glm::vec3 *v1, glm::vec3 *v2, sf::Window &window){
	glm::vec4 viewport = glm::vec4(0.f, 0.f, window.getSize().x, window.getSize().y);

	*v1 = glm::unProject(glm::vec3(float(sf::Mouse::getPosition().x), float(sf::Mouse::getPosition().y), 0.f),
						 getViewMatrix(),
						 getProjectionMatrix(),
						 viewport
	);
	*v2 = glm::unProject(glm::vec3(float(sf::Mouse::getPosition().x), float(sf::Mouse::getPosition().y), 1.f),
						 getViewMatrix(),
						 getProjectionMatrix(),
						 viewport
	);
}