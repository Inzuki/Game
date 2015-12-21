#ifndef TERRAIN_H
#define TERRAIN_H

#include "INCLUDES.h"
#include "Texture.h"
#include "Lamp.h"
#include "OBJ.h"

class SimpleTerrain {
	public:
		SimpleTerrain(const char*, GLuint&);
		void draw(glm::mat4&, glm::mat4&, std::vector<Lamp>&, GLuint&, glm::vec4&);
		float getHeight(float, float);

	private:
		int indxSize;

		float x, z, shineDamper, reflectivity;
		GLuint vao, ibo, vertBuff, texBuff, normBuff,
			   texture, rTexture, gTexture, bTexture, blendMap;
		GLuint heightMap;
		sf::Image heightMapData;

		glm::vec3 calcNormal(float, float);
};

#endif