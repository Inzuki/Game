#ifndef LAMP_H
#define LAMP_H

#include "INCLUDES.h"

class Lamp {
	public:
		Lamp(glm::vec3, glm::vec3);
		~Lamp();

		void draw(glm::mat4&, GLuint&, glm::mat4&);
		glm::vec3 getLampPos(), getColor();
		int getVerts();
		void moveLamp(glm::vec3);

	private:
		int vertexCount;
		GLuint lamp, vertBuff;
		glm::mat4 model;
		glm::vec3 color;
};

#endif