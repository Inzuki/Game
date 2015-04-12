#include "Terrain.h"
#include "Camera.h"

float Terrain::avgPixel(sf::Color c){
	return (c.r + c.g + c.b) / 3;
}

Terrain::Terrain(const char *file, const char *texFilePath){

	texture = loadTexture(texFilePath);
	texture2 = loadTexture("sand.jpg");
	texture3 = loadTexture("rock.jpg");
	texture4 = loadTexture("path.png");
	texture5 = loadTexture("brick.png");

	sf::Image img;
	char texFile[128];
	sprintf(texFile, "res/textures/%s", file);
	img.loadFromFile(texFile);

	for(int i = 0; i < img.getSize().x - 1; i++)
		heights.push_back(std::vector<float>());
	
	for(int i = 0; i < img.getSize().x - 1; i++)
		for(int j = 0; j < img.getSize().y - 1; j++)
			heights[j].push_back(0.f);

	rnum = ((img.getSize().x - 1) * 2) * (img.getSize().y - 1);

	vertexData  = new float[rnum * 3 * 3];
	textureData = new float[rnum * 2 * 3];
	normalData  = new float[rnum * 3 * 3];

	glm::vec3 *tmpv = new glm::vec3[3], tmpn;
	glm::vec2 *tmpt = new glm::vec2[3];

	const float wmult = 1.f, hmult = .15f;

	int i = 0, ti = 0;

	for(int y = 0; y < img.getSize().y - 1; y++){
		for(int x = 0; x < img.getSize().x - 1; x++){
			float h[] = {
				avgPixel(img.getPixel(x + 1, y)),
				avgPixel(img.getPixel(x, y)),
				avgPixel(img.getPixel(x, y + 1))
			};

			tmpv[0] = glm::vec3((x + 1) * wmult, h[0] * hmult, y * wmult);
			tmpv[1] = glm::vec3(x * wmult, h[1] * hmult, y * wmult);
			tmpv[2] = glm::vec3(x * wmult, h[2] * hmult, (y + 1) * wmult);

			int a = 2;
			float b = 1.f / float(a);

			tmpt[0] = glm::vec2(b + b * (x % a), b + b * (y % a));
			tmpt[1] = glm::vec2(b * (x % a), b + b * (y % a));
			tmpt[2] = glm::vec2(b * (x % a), b * (y % a));

			tmpn = -glm::cross(tmpv[0] - tmpv[1], tmpv[2] - tmpv[1]);

			for(int w = 0; w < 3; w++){
				vertexData[i]   = tmpv[w].x;
				normalData[i]   = tmpn.x;
				textureData[ti] = tmpt[w].x;
				i++; ti++;

				vertexData[i]   = tmpv[w].y;
				normalData[i]   = tmpn.y;
				textureData[ti] = tmpt[w].y;
				i++; ti++;

				vertexData[i]   = tmpv[w].z;
				normalData[i]   = tmpn.z;
				i++;
			}
			
			h[0] = avgPixel(img.getPixel(x + 1, y));
			h[1] = avgPixel(img.getPixel(x, y + 1));
			h[2] = avgPixel(img.getPixel(x + 1, y + 1));

			tmpv[0] = glm::vec3((x + 1) * wmult, h[0] * hmult, y * wmult);
			tmpv[1] = glm::vec3(x * wmult, h[1] * hmult, (y + 1) * wmult);
			tmpv[2] = glm::vec3((x + 1) * wmult, h[2] * hmult, (y + 1) * wmult);

			tmpt[0] = glm::vec2(b + b * (x % a), b + b * (y % a));
			tmpt[1] = glm::vec2(b * (x % a), b * (y % a));
			tmpt[2] = glm::vec2(b + b * (x % a), b * (y % a));

			tmpn = glm::cross(tmpv[0] - tmpv[2], tmpv[1] - tmpv[2]);

			for(int w = 0; w < 3; w++){
				vertexData[i]   = tmpv[w].x;
				normalData[i]   = tmpn.x;
				textureData[ti] = tmpt[w].x;
				i++; ti++;

				vertexData[i]   = tmpv[w].y;
				heights[y][x]   = tmpv[w].y;
				normalData[i]   = tmpn.y;
				textureData[ti] = tmpt[w].y;
				i++; ti++;

				vertexData[i]   = tmpv[w].z;
				normalData[i]   = tmpn.z;
				i++;
			}
		}
	}

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &texBuff);
	glGenBuffers(1, &normBuff);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, (rnum * 3 * 3) * sizeof(float), vertexData, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
		
		glBindBuffer(GL_ARRAY_BUFFER, texBuff);
		glBufferData(GL_ARRAY_BUFFER, (rnum * 2 * 3) * sizeof(float), textureData, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, normBuff);
		glBufferData(GL_ARRAY_BUFFER, (rnum * 3 * 3) * sizeof(float), normalData, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void Terrain::draw(glm::mat4 &VP, GLuint &shader){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture3);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture4);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texture5);

	glm::mat4 model;
	glBindVertexArray(vao);
		glUniform3f(glGetUniformLocation(shader, "viewPos"), getPos().x, getPos().y, getPos().z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "VP"), 1, GL_FALSE, glm::value_ptr(VP));
		glUniformMatrix4fv(glGetUniformLocation(shader, "M"),  1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, rnum * 3);
	glBindVertexArray(0);
}

float Terrain::getHeight(float x, float z){
	float y = 0.f;
	
	y = heights[(int)z][(int)x];

	return y;
}

void Terrain::deleteTerrain(){
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &texture2);
	glDeleteTextures(1, &texture3);
	glDeleteTextures(1, &texture4);
	glDeleteTextures(1, &texture5);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertBuff);
	glDeleteBuffers(1, &normBuff);
	glDeleteBuffers(1, &texBuff);
}