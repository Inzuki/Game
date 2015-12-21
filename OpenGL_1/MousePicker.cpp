#include "MousePicker.h"

#define RAY_RANGE 200
#define RECURSION_COUNT 150

MousePicker::MousePicker(SimpleTerrain &sT){ this->sT = &sT; }

void MousePicker::update(sf::Window &window){
	currentRay = calcMouseRay(window);

	if(intersectionInRange(0.f, (float)RAY_RANGE, currentRay))
		currentTerrainPoint = binarySearch(0, 0.f, (float)RAY_RANGE, currentRay);
	else
		currentTerrainPoint = glm::vec3(0.f, 5.f, 0.f);
}

glm::vec3 MousePicker::getPointOnRay(glm::vec3 ray, float distance){
	glm::vec3 scaledRay(
		(ray.x * distance) + position.x,
		(ray.y * distance) + position.y,
		(ray.z * distance) + position.z
	);

	return glm::vec3(scaledRay);
}

glm::vec3 MousePicker::binarySearch(int count, float start, float finish, glm::vec3 ray){
	float half = start + ((finish - start) / 2.f);

	if(count >= RECURSION_COUNT){
		glm::vec3 endPoint = getPointOnRay(ray, half);

		return endPoint;
	}

	if(intersectionInRange(start, half, ray))
		return binarySearch(count + 1, start, half, ray);
	else
		return binarySearch(count + 1, half, finish, ray);
}

bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray){
	glm::vec3 startPoint = getPointOnRay(ray, start),
			  endPoint   = getPointOnRay(ray, finish);

	if(!isUnderground(startPoint) && !isUnderground(endPoint))
		return true;
	else
		return false;
}

bool MousePicker::isUnderground(glm::vec3 testPoint){
	float height = 0.f;

	height = -sT->getHeight(testPoint.x, testPoint.z);

	if(testPoint.y < height)
		return true;
	else
		return false;
}



glm::vec3 MousePicker::calcMouseRay(sf::Window &window){
	/*
	float mouseX = sf::Mouse::getPosition().x / ((float)window.getSize().x * 0.5f) - 1.f,
		  mouseY = sf::Mouse::getPosition().y / ((float)window.getSize().y * 0.5f) - 1.f;

	glm::mat4 invertedVP = glm::inverse(getViewMatrix() * getProjectionMatrix());
	glm::vec4 screenPos  = glm::vec4(mouseX, -mouseY, 1.f, 1.f);
	glm::vec4 worldPos   = invertedVP * screenPos;

	glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

	return dir;
	//*/

	/*
	// viewport coordinates
	int mouseX = sf::Mouse::getPosition().x,
		mouseY = sf::Mouse::getPosition().y;

	// normalized device coordinates
	float normalizedX = ((2.f * mouseX) / ((float)window.getSize().x - 1.f) / 2.f),
		  normalizedY = -((2.f * mouseY) / ((float)window.getSize().y - 1.f) / 2.f);

	// clip space
	glm::vec4 clipCoords(normalizedX, normalizedY, -1.f, 1.f);

	// eye space
	glm::mat4 invertedProjMat = glm::inverse(getProjectionMatrix());
	glm::vec4 eyeCoords = invertedProjMat * clipCoords;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.f, 0.f);

	// world space
	glm::mat4 invertedViewMat = glm::inverse(getViewMatrix());
	glm::vec4 rayWorld = invertedViewMat * eyeCoords;
	glm::vec3 mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
	mouseRay = glm::normalize(mouseRay);

	return mouseRay;
	//*/

	/*
	float mouseX = (float)sf::Mouse::getPosition().x,
		  mouseY = (float)sf::Mouse::getPosition().y;

	glm::vec2 normalizedCoords = getNormalizedDeviceCoords(mouseX, mouseY, window);
	glm::vec4 clipCoords(normalizedCoords.x, normalizedCoords.y, -1.f, 1.f);
	glm::vec4 eyeCoords = toEyeCoords(clipCoords);
	glm::vec3 worldRay = toWorldCoords(eyeCoords);

	return worldRay;
	//*/

	/*
	int mouseX = sf::Mouse::getPosition().x,
		mouseY = sf::Mouse::getPosition().y;

	glm::vec4 viewport(0.f, 0.f, (float)window.getSize().x, (float)window.getSize().y);
	glm::vec3 ray = glm::unProject(
		glm::vec3(
			(float)mouseX,
			(float)mouseY,
			0.f
		),
		getViewMatrix(),
		getProjectionMatrix(),
		viewport
	);
	return ray;
	//return glm::normalize(ray);
	//*/

	glm::vec4 viewport = glm::vec4(0.f, 0.f, window.getSize().x, window.getSize().y);

	glm::vec3 v1 = glm::unProject(
		glm::vec3(
			float(sf::Mouse::getPosition().x),
			float(sf::Mouse::getPosition().y),
			0.f
		),
		getViewMatrix(),
		getProjectionMatrix(),
		viewport
	);
	glm::vec3 v2 = glm::unProject(
		glm::vec3(
			float(sf::Mouse::getPosition().x),
			float(sf::Mouse::getPosition().y),
			1.f
		),
		getViewMatrix(),
		getProjectionMatrix(),
		viewport
	);

	return glm::normalize(v2 - v1);
}

glm::vec3 MousePicker::toWorldCoords(glm::vec4 &eyeCoords){
	glm::mat4 invertedView = glm::inverse(getViewMatrix());
	glm::vec4 rayWorld = invertedView * eyeCoords;
	glm::vec3 mouseRay(rayWorld.x, rayWorld.y, rayWorld.z);
	return glm::normalize(mouseRay);
}

glm::vec4 MousePicker::toEyeCoords(glm::vec4 &clipCoords){
	return glm::inverse(getProjectionMatrix()) * clipCoords;
}

glm::vec2 MousePicker::getNormalizedDeviceCoords(float mouseX, float mouseY, sf::Window &window){
	float x = (2.f / mouseX) / window.getSize().x - 1.f,
		  y = (2.f / mouseY) / window.getSize().y - 1.f;
	return glm::vec2(mouseX, -mouseY);
}

glm::vec3 MousePicker::getCurrentRay(){ return currentRay; }

glm::vec3 MousePicker::getCurrentTerrainPoint(){ return currentTerrainPoint; }