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

bool cursorLocked = true, isTyping = false;

glm::mat4 getViewMatrix(){ return view; }
glm::mat4 getProjectionMatrix(){ return projection; }
glm::vec3 getPos(){ return position;  }
glm::vec3 getDir(){ return direction; }
glm::vec3 getRight(){ return right;   }
bool typing(){ return isTyping; }
void setTyping(bool trigger){
	isTyping = trigger;
}

void moveY(float y){
	// the speed at which the player changes height
	float changeSpeed = 0.15f,
	// some range
		  range = 0.05f;

	// if the height is greater than the new height
	if(position.y > (y + range)){
		// lower the height to the new height
		position.y -= (changeSpeed * abs(position.y - y)) / 0.5f;
	}else
	// else if the height is less than the new height
	if(position.y < (y - range)){
		// rise the height to the new height
		position.y += (changeSpeed * abs(position.y - y)) / 0.5f;
	}else
		position.y = y;
}

void setCursorLocked(){ if(cursorLocked) cursorLocked = false; else cursorLocked = true; }

void computeMats(sf::Window &window, sf::Clock clk, float deltaTime){
	double xpos = sf::Mouse::getPosition().x,
		   ypos = sf::Mouse::getPosition().y;

	if(cursorLocked){
		window.setMouseCursorVisible(false);
		sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2.f, window.getSize().y / 2.f));

		horizontalAngle += mouseSpeed * float(window.getSize().x / 2.f - xpos);
		verticalAngle   += mouseSpeed * float(window.getSize().y / 2.f - ypos);

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
		if(!isTyping){
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
				position.x += direction.x * speed * deltaTime;
				position.z += direction.z * speed * deltaTime;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
				position.x -= direction.x * speed * deltaTime;
				position.z -= direction.z * speed * deltaTime;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
				position.x += right.x * speed * deltaTime;
				position.z += right.z * speed * deltaTime;
			}
			if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
				position.x -= right.x * speed * deltaTime;
				position.z -= right.z * speed * deltaTime;
			}
		}

		glm::vec3 up = glm::cross(right, direction);

		view = glm::lookAt(
			position,
			position + direction,
			up
		);
	}else
		window.setMouseCursorVisible(true);

	projection = glm::perspective(initFOV, 4.f / 3.f, .1f, 200.f);
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