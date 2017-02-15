#include "Camera.h"

glm::mat4 view, projection;
//glm::vec3 position = glm::vec3(25.f, 55.f, 25.f);
glm::vec3 direction = glm::vec3(0.f, 0.f,  3.f),
			  right = glm::vec3(0.f, 0.f, -1.f),
			  up    = glm::vec3(0.f, 1.f,  0.f);

float yaw = 3.14f,
	  pitch   = 45.f,
	  initFOV         = 45.f,
	  speed			  = 20.f,
	  mouseSpeed      = 0.01f;

bool cursorLocked = true, isTyping = false;

glm::mat4 getViewMatrix(){ return view; }
glm::mat4 getProjectionMatrix(){ return projection; }
glm::vec3 getPos(){ return position;  }
glm::vec3 getDir(){ return direction; }
glm::vec3 getRight(){ return right;   }
glm::vec3 getUp(){    return up;      }

void setProj(glm::mat4 &proj){ projection = proj; }

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
void moveY_players(float &height, float y){
	// the speed at which the player changes height
	float changeSpeed = 0.15f,
	// some range
		  range = 0.05f;

	// if the height is greater than the new height
	if(height > (y + range)){
		// lower the height to the new height
		height -= (changeSpeed * abs(height - y)) / 0.5f;
	}else
	// else if the height is less than the new height
	if(height < (y - range)){
		// rise the height to the new height
		height += (changeSpeed * abs(height - y)) / 0.5f;
	}else
		height = y;
}

void setCursorLocked(){ if(cursorLocked) cursorLocked = false; else cursorLocked = true; }
void setTypingStatus(bool status){ isTyping = status; }
bool typingStatus(){ return isTyping; }
sf::Clock clik;

// compute matricies
void computeMats(sf::Window &window, sf::Clock clk, float deltaTime){
	if(cursorLocked && !isTyping){
		double xpos = sf::Mouse::getPosition().x,
			   ypos = sf::Mouse::getPosition().y;

		window.setMouseCursorVisible(false);

		sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2));

		yaw   += mouseSpeed * float(window.getSize().x / 2.f - xpos);
		pitch += mouseSpeed * float(window.getSize().y / 2.f - ypos);

		if(pitch > 45.55f)
			pitch = 45.55f;
		if(pitch < 42.4f)
			pitch = 42.4f;

		direction = glm::vec3(
			cos(pitch) * sin(yaw),
			sin(pitch),
			cos(pitch) * cos(yaw)
		);

		right = glm::vec3(
			sin(yaw - 3.14f / 2.f),
			0,
			cos(yaw - 3.14f / 2.f)
		);

		// handle movement
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			position.x += direction.x * speed * deltaTime;
			position.y += direction.y * speed * deltaTime;
			position.z += direction.z * speed * deltaTime;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			position.x -= direction.x * speed * deltaTime;
			position.y -= direction.y * speed * deltaTime;
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

		glm::vec3 up = glm::cross(right, direction);

		///*
		view = glm::lookAt(
			position,
			position + direction,
			up
		);
		projection = glm::perspective(initFOV, 4.f / 3.f, 0.1f, 300.0f);
		//*/
		
		/*
		float ck = clik.getElapsedTime().asSeconds();
		glm::vec3 lightInvDir(glm::vec3(-7.f, 5.f, -6.f));

		projection = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
		view = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		//*/
	}else {
		if(isTyping)
			sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2));
		else
			window.setMouseCursorVisible(true);
	}
}

// compute matricies for water reflection (everything is upside down)
void computeMats_reflection(sf::Window &window, sf::Clock clk, float deltaTime){
	if(cursorLocked){
		float distance = 2.f * (position.y - 25.f);
		position.y -= distance;

		double xpos = sf::Mouse::getPosition().x,
			   ypos = sf::Mouse::getPosition().y;

		window.setMouseCursorVisible(false);
		sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2));

		yaw   += mouseSpeed * float(window.getSize().x / 2.f - xpos);
		pitch += mouseSpeed * float(window.getSize().y / 2.f - ypos);

		if(pitch > 45.55f)
			pitch = 45.55f;
		if(pitch < 42.4f)
			pitch = 42.4f;

		direction = glm::vec3(
			 cos(pitch) * sin(yaw),
			-sin(pitch),
			 cos(pitch) * cos(yaw)
		);

		right = glm::vec3(
			sin(yaw - 3.14f / 2.f),
			0,
			cos(yaw - 3.14f / 2.f)
		);

		// handle movement
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
			position.x += direction.x * speed * deltaTime;
			position.y -= direction.y * speed * deltaTime;
			position.z += direction.z * speed * deltaTime;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
			position.x -= direction.x * speed * deltaTime;
			position.z -= direction.z * speed * deltaTime;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
			position.x += right.x * speed * deltaTime;
			position.y -= direction.y * speed * deltaTime;
			position.z += right.z * speed * deltaTime;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
			position.x -= right.x * speed * deltaTime;
			position.z -= right.z * speed * deltaTime;
		}

		glm::vec3 up = glm::cross(right, direction);

		view = glm::lookAt(
			position,
			position + direction,
			up
		);

		projection = glm::perspective(initFOV, 4.f / 3.f, 0.1f, 1000.0f);

		position.y += distance;
	}else
		window.setMouseCursorVisible(true);
}