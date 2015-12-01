#ifndef TERRAIN_H
#define TERRAIN_H

#include "INCLUDES.h"
#include "Texture.h"
#include "Lamp.h"
#include "OBJ.h"

#define SIZE 100
#define VERTEX_COUNT 64
#define WIDTH  2
#define HEIGHT 2

class SimpleTerrain {
	public:
		SimpleTerrain(int, int, const char*);
		void draw(glm::mat4&, glm::mat4&, Lamp&, GLuint&, glm::vec4&);

	private:
		int size, indxSize;

		float x, z, shineDamper, reflectivity;
		GLuint texture, vao, ibo, vertBuff, texBuff, normBuff;
		//std::vector<int> vecIndices;
		//int *indices;
};

class Terrain {
	public:
		float getHeight(float, float);

		Terrain(const char *, sf::Window&, GLuint&);
		~Terrain();
		void draw(glm::mat4&, glm::mat4&, Lamp&, GLuint&, glm::vec4&);
		void updateRes(sf::Window&, GLuint &);
		void updateRes(int, int, GLuint &);

	private:
		int rnum;
		float *vertexData, *textureData, *normalData, shineDamper, reflectivity;
		std::vector<std::vector<float>> heights;
		GLuint bgTexture, rTexture, gTexture, bTexture, blendMap,
			   vao, vertBuff, texBuff, normBuff;

		float avgPixel(sf::Color);
};

#endif