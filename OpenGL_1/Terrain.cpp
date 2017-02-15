#include "Terrain.h"
#include "Camera.h"
#include "Shader.h"

#define WIDTH  256
#define HEIGHT 256
#define MAX_HEIGHT 15

SimpleTerrain::SimpleTerrain(const char *heightMapFile, GLuint &shader){
	shineDamper  = 1.f;
	reflectivity = 0.f;

	heightMap = loadTexture(heightMapFile);
	heightMapData.loadFromFile(heightMapFile);
	texture  = loadTexture("res/textures/grass2.png");
	rTexture = loadTexture("res/textures/sand.jpg");
	gTexture = loadTexture("res/textures/grass.png");
	bTexture = loadTexture("res/textures/bricky.png");
	blendMap = loadTexture("res/textures/blend_map.png");
	
	glUseProgram(shader);
	
	std::vector<GLuint>indices;
	std::vector<GLfloat>vertices, normals, textures;

	int counter = 0;

	// create a grid of vertices
	for(int y = 0; y < HEIGHT; y++){
		for(int x = 0; x < WIDTH; x++){
			vertices.push_back((GLfloat)x);
			vertices.push_back((GLfloat)getHeight((float)x, (float)y));
			vertices.push_back(-(float)y);

			glm::vec3 normal = calcNormal((float)x, (float)y);
			normals.push_back(-normal.x);
			normals.push_back(-normal.y);
			normals.push_back(normal.z);

			textures.push_back((float)x / WIDTH  - 1);
			textures.push_back((float)y / HEIGHT - 1);
		}
	}

	// create triangles based on vertices above
	for(int y = 0; y < HEIGHT - 1; y++){
		for(int x = 0; x < WIDTH - 1; x++){
			if(x == 0){
				if(y == 0){
					// create first two triangles
					indices.push_back(counter);
					indices.push_back(counter + WIDTH);
					indices.push_back(++counter);
					indices.push_back(counter + WIDTH);
				}else {
					// create degenerate triangle
					indices.push_back(counter + WIDTH);
					indices.push_back(++counter);

					// create the next triangles to fix the order
					indices.push_back(counter);
					indices.push_back(counter + WIDTH);
					indices.push_back(++counter);
					indices.push_back(counter + WIDTH);
				}
			}else {
				// add the rest of the row
				indices.push_back(++counter);
				indices.push_back(counter + WIDTH);
			}
		}
	}

	indxSize = indices.size();

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &texBuff);
	glGenBuffers(1, &normBuff);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normBuff);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		
		glBindBuffer(GL_ARRAY_BUFFER, texBuff);
		glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), &textures[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void SimpleTerrain::draw(glm::mat4 &viewMat, glm::mat4 &projMat, std::vector<Lamp>&lamps, GLuint &shader, glm::vec4 &clipPlane){
	glUniform1f(glGetUniformLocation(shader, "shineDamper"),  shineDamper);
	glUniform1f(glGetUniformLocation(shader, "reflectivity"), reflectivity);
	glUniform1i(glGetUniformLocation(shader, "bgTex"),    0);
	glUniform1i(glGetUniformLocation(shader, "rTex"),     1);
	glUniform1i(glGetUniformLocation(shader, "gTex"),     2);
	glUniform1i(glGetUniformLocation(shader, "bTex"),     3);
	glUniform1i(glGetUniformLocation(shader, "blendMap"), 4);

	for(int i = 0; i < (int)lamps.size(); i++){
		char light_data[64];
		sprintf(light_data, "lightPos[%i]", i);
		glUniform3f(glGetUniformLocation(shader, light_data),
					lamps[i].getLampPos().x,
					lamps[i].getLampPos().y,
					lamps[i].getLampPos().z
		);
		sprintf(light_data, "lightColor[%i]", i);
		glUniform3f(glGetUniformLocation(shader, light_data),
					lamps[i].getColor().x,
					lamps[i].getColor().y,
					lamps[i].getColor().z
		);
		sprintf(light_data, "attenuation[%i]", i);
		glUniform3f(glGetUniformLocation(shader, light_data),
					lamps[i].getAttenuation().x,
					lamps[i].getAttenuation().y,
					lamps[i].getAttenuation().z
		);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, blendMap);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.f, (float)MAX_HEIGHT, 0.f));
	model = glm::rotate(model, 3.15f, glm::vec3(1.f, 0.f, 0.f));
	glBindVertexArray(vao);
		glUniform4fv(glGetUniformLocation(shader, "plane"), 1, glm::value_ptr(clipPlane));
		glUniform3f(glGetUniformLocation(shader, "viewPos"), getPos().x, getPos().y, getPos().z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"),  1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLE_STRIP, indxSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

float SimpleTerrain::getHeight(float x, float z){
	float height = 0.f;

	if(x < 0.f || x >= (float)WIDTH || z < 0.f || z >= (float)HEIGHT)
		height = 0.f;
	else {
		height =
			heightMapData.getPixel((int)x, (int)z).r +
			heightMapData.getPixel((int)x, (int)z).g +
			heightMapData.getPixel((int)x, (int)z).b /
			3.f
		;
		height += 256.f / 2.f;
		height /= 256.f / 2.f;
		height *= (float)MAX_HEIGHT;
	}

	return height;
}

glm::vec3 SimpleTerrain::calcNormal(float x, float z){
	float heightL = getHeight(x - 1, z),
		  heightR = getHeight(x + 1, z),
		  heightD = getHeight(x, z - 1),
		  heightU = getHeight(x, z + 1);

	glm::vec3 normal(heightL - heightR, 2.f, heightD - heightU);
	normal = glm::normalize(normal);

	return normal;
}