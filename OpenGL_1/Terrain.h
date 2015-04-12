#ifndef TERRAIN_H
#define TERRAIN_H

#include "INCLUDES.h"
#include "Texture.h"
#include "Lamp.h"
#include "OBJ.h"

class Terrain {
	public:
		float getHeight(float, float);
		Terrain(const char *, const char *, sf::Window&);
		void draw(glm::mat4&, std::vector<Lamp>&),
			 deleteTerrain();
		void updateRes(sf::Window&);
	private:
		int rnum;
		float *vertexData, *textureData, *normalData;
		std::vector<std::vector<float>> heights;
		float avgPixel(sf::Color);
		GLuint texture, texture2, texture3, texture4, texture5,
			   vao, vertBuff, texBuff, normBuff,
			   shader;
};

#endif