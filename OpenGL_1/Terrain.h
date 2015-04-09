#ifndef TERRAIN_H
#define TERRAIN_H

#include "INCLUDES.h"
#include "Texture.h"
#include "OBJ.h"

class Terrain {
	public:
		Terrain(const char *, int, int);
		void draw(glm::mat4&, GLuint, GLuint, glm::mat4&);
	private:
		void generateTerrain();
		GLuint texture, vao;
		const float SIZE = 800;
		const int VERTEX_COUNT = 128;
		float x, z;
		int size;
};

#endif