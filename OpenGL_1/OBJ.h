#define _CRT_SECURE_NO_WARNINGS

#ifndef OBJ_H
#define OBJ_H

#include "INCLUDES.h"

class OBJ {
	public:
		OBJ(const char *, const char *);
		GLuint getTexture();
		void draw(glm::mat4&, GLuint, GLuint, glm::mat4&), deleteObj();
		int getVerts();
	private:
		int vertexCount;
		GLuint obj, vertBuff, texBuff, normBuff, texture;
};

#endif