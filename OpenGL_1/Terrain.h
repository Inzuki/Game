#ifndef TERRAIN_H
#define TERRAIN_H

#include "INCLUDES.h"
#include "Texture.h"
#include "OBJ.h"

class Terrain {
	public:
		float getHeight(float, float);
		Terrain(const char *, const char *);
		void draw(glm::mat4&, GLuint&),
			 deleteTerrain();
	private:
		int rnum;
		float *vertexData, *textureData, *normalData;
		std::vector<std::vector<float>> heights;
		float avgPixel(sf::Color);
		GLuint texture, texture2, texture3, texture4, texture5,
			   vao, vertBuff, texBuff, normBuff;
};

#endif