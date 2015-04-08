#ifndef LAMP_H
#define LAMP_H

#include "INCLUDES.h"

class Lamp {
	public:
		Lamp(glm::vec3);
		void draw(glm::mat4&, GLuint, GLuint, glm::mat4&), deleteLamp();
		glm::vec3 getLampPos();
		int getVerts();
	private:
		int vertexCount;
		GLuint lamp, vertBuff;
		glm::mat4 model;
};

#endif