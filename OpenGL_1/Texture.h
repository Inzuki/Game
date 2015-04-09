#define _CRT_SECURE_NO_WARNINGS

#ifndef TEXTURE_H
#define TEXTURE_H

#include "INCLUDES.h"

GLuint loadTexture(const char *);

class CubeMap {
	public:
		CubeMap(std::vector<const GLchar*>);
		void draw(GLuint);
		GLfloat **skyboxVerts;
	private:
		GLuint texture, vao, vbo;
};

#endif