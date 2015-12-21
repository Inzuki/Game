#ifndef LAMP_H
#define LAMP_H

#include "INCLUDES.h"
#include "OBJ.h"

class Lamp {
	public:
		Lamp(glm::vec3, glm::vec3);
		Lamp(glm::vec3, glm::vec3, glm::vec3);
		~Lamp();

		void draw(OBJ&, glm::mat4&, GLuint&, glm::mat4&, glm::mat4&);
		glm::vec3 getLampPos(), getColor(), getAttenuation();
		int getVerts();
		void moveLamp(glm::vec3);

	private:
		int vertexCount;
		glm::mat4 model;
		glm::vec3 color, attenuation;
};

#endif