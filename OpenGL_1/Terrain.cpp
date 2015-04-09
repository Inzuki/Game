#include "Terrain.h"

Terrain::Terrain(const char *file, int gridX, int gridZ){
	texture = loadTexture(file);

	sf::Image img;
	char texFile[128];
	sprintf(texFile, "res/textures/%s", file);
	img.loadFromFile(texFile);

	x = gridX * SIZE;
	z = gridZ * SIZE;

	generateTerrain();
}

void Terrain::generateTerrain(){
	int count = VERTEX_COUNT * VERTEX_COUNT, vertexPointer = 0, pointer = 0;

	float *vertices = new float[count * 3];
	float *textures = new float[count * 2];
	float *normals  = new float[count * 3];
	int   *indices  = new int[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT)];

	for(int i = 0; i < VERTEX_COUNT; i++){
		for(int j = 0; j < VERTEX_COUNT; j++){
			vertices[vertexPointer * 3]     = float(j) / (float(VERTEX_COUNT) - 1 * SIZE);
			vertices[vertexPointer * 3 + 1] = 0;
			vertices[vertexPointer * 3 + 2] = float(i) / (float(VERTEX_COUNT) - 1 * SIZE);

			textures[vertexPointer * 2]     = float(j) / (float(VERTEX_COUNT) - 1);
			textures[vertexPointer * 2 + 1] = float(i) / (float(VERTEX_COUNT) - 1);

			normals[vertexPointer * 3]      = 0;
			normals[vertexPointer * 3 + 1]  = 1;
			normals[vertexPointer * 3 + 2]  = 0;

			vertexPointer++;
		}
	}
	
	for(int i = 0; i < VERTEX_COUNT - 1; i++){
		for(int j = 0; j < VERTEX_COUNT - 1; j++){
			int topLeft = (i * VERTEX_COUNT) + j,
				topRight = topLeft + 1,
				bottomLeft = ((i + 1) * VERTEX_COUNT) + j,
				bottomRight = bottomLeft + 1;

			indices[pointer++] = topLeft;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = topRight;
			indices[pointer++] = topRight;
			indices[pointer++] = bottomLeft;
			indices[pointer++] = bottomRight;
		}
	}

	GLuint vertBuff, texBuff, normBuff;
	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &texBuff);
	glGenBuffers(1, &normBuff);
	glGenVertexArrays(1, &vao);

	size = sizeof(vertices);

	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, texBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, normBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void Terrain::draw(glm::mat4 &model, GLuint modelLoc, GLuint matrixLoc, glm::mat4 &VP){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vao);
		glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(VP));
		glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
	glBindVertexArray(0);
}