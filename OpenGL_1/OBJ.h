#define _CRT_SECURE_NO_WARNINGS

#ifndef OBJ_H
#define OBJ_H

#include "INCLUDES.h"

class OBJ {
	public:
		OBJ(const char *, const char *);
		OBJ(const char *, const char *, float, float);
		~OBJ();
		GLuint getTexture();
		void draw(glm::mat4&, glm::mat4&, glm::mat4&, GLuint&, glm::vec4&);
		int getVerts();

	private:
		int vertexCount;
		float shineDamper, reflectivity;
		GLuint obj, vertBuff, texBuff, normBuff, texture;
};

#endif