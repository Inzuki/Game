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
		~Terrain();
		void draw(glm::mat4&, Lamp&, glm::vec4&);
		void updateRes(sf::Window&);
		void updateRes(int, int);

	private:
		int rnum;
		float *vertexData, *textureData, *normalData;
		std::vector<std::vector<float>> heights;
		GLuint texture, texture2, texture3, texture4, texture5,
			   vao, vertBuff, texBuff, normBuff,
			   shader;

		float avgPixel(sf::Color);
};

#endif