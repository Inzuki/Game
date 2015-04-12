#include "Camera.h"

#define RAY_RANGE 600
#define RECURSION_COUNT 200

glm::mat4 view;
glm::mat4 projection;
glm::vec3 position = glm::vec3(10, 5, 10);

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
	}else
		window.setMouseCursorVisible(true);

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

	projection = glm::perspective(initFOV, 4.f / 3.f, .1f, 250.f);

	view = glm::lookAt(
		position,
		position + direction,
		up
	);
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

glm::vec3 picker(sf::Window &window){
	// get the viewport coordinates
	float x = (2.f * sf::Mouse::getPosition().x) / window.getSize().x - 1.f,
		  y = 1.f - (2.f * sf::Mouse::getPosition().y) / window.getSize().y;
	// get the normalized device coordinates
	glm::vec3 ray_nds(x, y, 1.f);

	// get the homogeneous clip coordinates
	glm::vec4 ray_clip(ray_nds.x, ray_nds.y, -1.f, 1.f);

	// get the eye coordinates
	glm::vec4 ray_eye = glm::inverse(getProjectionMatrix()) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.f, 0.f);

	// get the world coordinates
	glm::vec3 ray_world(
		(glm::inverse(getViewMatrix()) * ray_eye).x,
		(glm::inverse(getViewMatrix()) * ray_eye).y,
		(glm::inverse(getViewMatrix()) * ray_eye).z
	);
	
	// return the normalized vector
	return glm::normalize(ray_world);
}