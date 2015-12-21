#ifndef MOUSEPICKER_H
#define MOUSEPICKER_H

#include "INCLUDES.h"
#include "Terrain.h"
#include "Camera.h"

class MousePicker {
	public:
		MousePicker(SimpleTerrain&);
		void update(sf::Window&);
		glm::vec3 getCurrentRay(), getCurrentTerrainPoint(), calcMouseRay(sf::Window&);

	private:
		glm::vec3 currentRay, currentTerrainPoint;
		SimpleTerrain *sT;

		glm::vec2 getNormalizedDeviceCoords(float, float, sf::Window&);
		glm::vec4 toEyeCoords(glm::vec4&);
		glm::vec3 toWorldCoords(glm::vec4&);

		glm::vec3 getPointOnRay(glm::vec3, float);
		glm::vec3 binarySearch(int, float, float, glm::vec3);
		bool intersectionInRange(float, float, glm::vec3);
		bool isUnderground(glm::vec3);
};

#endif