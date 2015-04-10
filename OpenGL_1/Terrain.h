#ifndef TERRAIN_H
#define TERRAIN_H

#include "INCLUDES.h"
#include "Texture.h"
#include "OBJ.h"

class Terrain {
	public:
		Terrain(const char *, const char *);
		void draw(glm::mat4&, GLuint, GLuint, glm::mat4&);
		float x, z;
	private:
		float avgPixel(sf::Color);
		GLuint texture, texture2, texture3, vao, vertBuff, texBuff, normBuff;
		int rnum;
		float *vertexData, *textureData, *normalData;
};

#endif