#include "Terrain.h"
#include "Camera.h"
#include "Shader.h"

SimpleTerrain::SimpleTerrain(int gridX, int gridZ, const char *file){
	x = gridX * SIZE;
	z = gridZ * SIZE;

	shineDamper  = 1.f;
	reflectivity = 0.f;

	texture = loadTexture(file);

	/*
	vertices[ptr++] = 0.f;
	vertices[ptr++] = 0.f;
	vertices[ptr++] = 0.f;

	vertices[ptr++] = 0.f;
	vertices[ptr++] = 0.f;
	vertices[ptr++] = 1.f;

	vertices[ptr++] = 1.f;
	vertices[ptr++] = 0.f;
	vertices[ptr++] = 0.f;

	vertices[ptr++] = 1.f;
	vertices[ptr++] = 0.f;
	vertices[ptr++] = 1.f;
	*/
	
	//int vertSize = 12, indxSize = 6; // default
	int vertSize = WIDTH * HEIGHT * 6 * 2;
		indxSize = (WIDTH * HEIGHT * 6) + (HEIGHT * 2);
	GLfloat *vertices = new GLfloat[vertSize],
			*normals  = new GLfloat[vertSize];
	GLuint  *indices  = new GLuint [indxSize];

	int ptr = 0,
		ptr2 = 0,
		counter = 0;

	for(int y = 0; y < HEIGHT; y++){
		for(int x = 0; x < WIDTH; x++){
			if(y > 0){
				if(x == 0){
					// Here we have to check if we begin indexing on a new line.
					// The reason for the special case is because the indexing has
					// to wrap around to the start of the new line (since we ended
					// on the right-most and have to begin again on the left-most).
					// This causes a degenerate triangle(?) because it stretches
					// from the right-most to the left-most, creating a wonky
					// triangle but to continue the indexing so it makes sense
					// to OpenGL.
					
					printf("%f\n\n", (float)ptr / 3.f);
					normals[ptr]    = -1.f;
					vertices[ptr++] = (float)x + 0.f;
					normals[ptr]    = -1.f;
					vertices[ptr++] = 0.f;
					normals[ptr]    = -1.f;
					vertices[ptr++] = (float)y + 1.f;
					printf("%f\n\n", (float)ptr / 3.f);
					
					indices[ptr2++] = counter + 1;
					indices[ptr2++] = counter - (WIDTH * 2 - 1);
					printf("%i, %i, %i ... ", counter + 1, counter - (WIDTH * 2 - 1), counter + 2);
					indices[ptr2++] = (counter++) + 2;
					printf("%i\n", counter);
					counter++;
					printf("%i\n", counter);
					
					/*indices[ptr2++] = counter + 1;
					indices[ptr2++] = counter - (WIDTH * 2 - 1);
					indices[ptr2++] = (counter++) + 2;*/
				}
				
				//indices[ptr2++] = counter;
				//indices[ptr2++] = counter - (WIDTH * 2 - 1);
				//indices[ptr2++] = counter - (WIDTH * 2 - 3);

				//vertices[ptr++] = (float)x + 0.f;
				//vertices[ptr++] = 0.f;
				//vertices[ptr++] = (float)y + 1.f;
			}else {
				if(x == 0){
					// add the first two vertices
					normals[ptr]    = 0.f;
					vertices[ptr++] = (float)x + 0.f;
					normals[ptr]    = 1.f;
					vertices[ptr++] = 0.f;
					normals[ptr]    = 0.f;
					vertices[ptr++] = (float)y + 0.f;
					
					normals[ptr]    = 0.f;
					vertices[ptr++] = (float)x + 0.f;
					normals[ptr]    = 1.f;
					vertices[ptr++] = 0.f;
					normals[ptr]    = 0.f;
					vertices[ptr++] = (float)y + 1.f;
				}
				
				normals[ptr]    = 0.f;
				vertices[ptr++] = (float)x + 1.f;
				normals[ptr]    = 1.f;
				vertices[ptr++] = 0.f;
				normals[ptr]    = 0.f;
				vertices[ptr++] = (float)y + 0.f;

				indices[ptr2++] = counter;
				indices[ptr2++] = counter + 1;
				indices[ptr2++] = (counter++) + 2;
				
				normals[ptr]    = 0.f;
				vertices[ptr++] = (float)x + 1.f;
				normals[ptr]    = 1.f;
				vertices[ptr++] = 0.f;
				normals[ptr]    = 0.f;
				vertices[ptr++] = (float)y + 1.f;

				indices[ptr2++] = counter;
				indices[ptr2++] = counter + 1;
				indices[ptr2++] = (counter++) + 2;

				printf("%f\n\n", (float)ptr / 3.f);
			}
		}
	}

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &texBuff);
	glGenBuffers(1, &normBuff);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indxSize, indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertSize, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, normBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * vertSize, normals, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		
		/*
		glBindBuffer(GL_ARRAY_BUFFER, texBuff);
		glBufferData(GL_ARRAY_BUFFER, , , GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		*/
	glBindVertexArray(0);
}

void SimpleTerrain::draw(glm::mat4 &viewMat, glm::mat4 &projMat, Lamp &lamp, GLuint &shader, glm::vec4 &clipPlane){
	glUniform3f(glGetUniformLocation(shader, "lightPos"),
				lamp.getLampPos().x,
				lamp.getLampPos().y,
				lamp.getLampPos().z
	);
	glUniform3f(glGetUniformLocation(shader, "lightColor"),
				lamp.getColor().x,
				lamp.getColor().y,
				lamp.getColor().z
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.f, 30.f, 0.f));
	model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
	glBindVertexArray(vao);
		glUniform1f(glGetUniformLocation(shader, "shineDamper"),  shineDamper);
		glUniform1f(glGetUniformLocation(shader, "reflectivity"), reflectivity);
		glUniform4fv(glGetUniformLocation(shader, "plane"), 1, glm::value_ptr(clipPlane));
		glUniform3f(glGetUniformLocation(shader, "viewPos"), getPos().x, getPos().y, getPos().z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"),  1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLE_STRIP, indxSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

/*SimpleTerrain::SimpleTerrain(int gridX, int gridZ, const char *file){
	x = gridX * SIZE;
	z = gridZ * SIZE;

	shineDamper  = 1.f;
	reflectivity = 0.f;

	texture = loadTexture(file);

	std::vector<glm::vec3> vecVertices, vecNormals;
	std::vector<glm::vec2> vecTextures;
	std::vector<int> vecIndices;

	for(int i = 0; i < VERTEX_COUNT; i++){
		for(int j = 0; j < VERTEX_COUNT; j++){
			vecVertices.push_back(
				glm::vec3(
					(float)j / ((float)VERTEX_COUNT - 1.f) * (float)SIZE,
					0.f,
					(float)i / ((float)VERTEX_COUNT - 1.f) * (float)SIZE
				)
			);
			vecNormals.push_back(
				glm::vec3(
					0.f,
					1.f,
					0.f
				)
			);
			vecTextures.push_back(
				glm::vec2(
					(float)j / ((float)VERTEX_COUNT - 1.f),
					(float)i / ((float)VERTEX_COUNT - 1.f)
				)
			);
		}
	}

	for(int gz = 0; gz < VERTEX_COUNT - 1; gz++){
		for(int gx = 0; gx < VERTEX_COUNT - 1; gx++){
			int topLeft  = (gz * VERTEX_COUNT) + gx;
			int topRight = topLeft + 1;
			int bottomLeft  = ((gz + 1) * VERTEX_COUNT) + gx;
			int bottomRight = bottomLeft + 1;

			vecIndices.push_back(topLeft);
			vecIndices.push_back(bottomLeft);
			vecIndices.push_back(topRight);
			vecIndices.push_back(topRight);
			vecIndices.push_back(bottomLeft);
			vecIndices.push_back(bottomRight);
		}
	}

	size = vecIndices.size();

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &texBuff);
	glGenBuffers(1, &normBuff);
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(glm::vec3), &vecVertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, texBuff);
		glBufferData(GL_ARRAY_BUFFER, vecTextures.size() * sizeof(glm::vec2), &vecTextures[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, normBuff);
		glBufferData(GL_ARRAY_BUFFER, vecNormals.size() * sizeof(glm::vec3), &vecNormals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, &vecIndices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void SimpleTerrain::draw(glm::mat4 &viewMat, glm::mat4 &projMat, Lamp &lamp, GLuint &shader, glm::vec4 &clipPlane){
	glUniform3f(glGetUniformLocation(shader, "lightPos"),
				lamp.getLampPos().x,
				lamp.getLampPos().y,
				lamp.getLampPos().z
	);
	glUniform3f(glGetUniformLocation(shader, "lightColor"),
				lamp.getColor().x,
				lamp.getColor().y,
				lamp.getColor().z
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glm::mat4 model;
	glBindVertexArray(vao);
		glUniform1f(glGetUniformLocation(shader, "shineDamper"),  shineDamper);
		glUniform1f(glGetUniformLocation(shader, "reflectivity"), reflectivity);
		glUniform4fv(glGetUniformLocation(shader, "plane"), 1, glm::value_ptr(clipPlane));
		glUniform3f(glGetUniformLocation(shader, "viewPos"), getPos().x, getPos().y, getPos().z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"),  1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_BYTE, 0);
	glBindVertexArray(0);
}*/



float Terrain::avgPixel(sf::Color c){
	return (c.r + c.g + c.b) / 3;
}

Terrain::Terrain(const char *file, sf::Window &window, GLuint &shader){
	shineDamper  = 1.f;
	reflectivity = 0.f;

	bgTexture = loadTexture("res/textures/sandgrass.jpg");
	rTexture  = loadTexture("res/textures/sand.jpg");
	gTexture  = loadTexture("res/textures/rock.jpg");
	bTexture  = loadTexture("res/textures/brick.png");
	blendMap  = loadTexture("res/textures/blend_map.png");

	glUseProgram(shader);
	glUniform1i(glGetUniformLocation(shader, "bgTex"),    0);
	glUniform1i(glGetUniformLocation(shader, "rTexture"), 1);
	glUniform1i(glGetUniformLocation(shader, "gTexture"), 2);
	glUniform1i(glGetUniformLocation(shader, "bTexture"), 3);
	glUniform1i(glGetUniformLocation(shader, "blendMap"), 4);
	glUniform2f(glGetUniformLocation(shader, "resolution"), window.getSize().x, window.getSize().y);
	glUniform3f(glGetUniformLocation(shader, "light.ambient"),  0.2f, 0.2f, 0.2f);
	glUniform3f(glGetUniformLocation(shader, "light.diffuse"),  0.5f, 0.5f, 0.5f);

	sf::Image img;
	char texFile[128];
	sprintf(texFile, "res/textures/%s", file);
	img.loadFromFile(texFile);

	// fill in the heights with empty spots
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

void Terrain::draw(glm::mat4 &viewMat, glm::mat4 &projMat, Lamp &lamp, GLuint &shader, glm::vec4 &clipPlane){
	glUniform3f(glGetUniformLocation(shader, "lightPos"),
				lamp.getLampPos().x,
				lamp.getLampPos().y,
				lamp.getLampPos().z
	);
	glUniform3f(glGetUniformLocation(shader, "lightColor"),
				lamp.getColor().x,
				lamp.getColor().y,
				lamp.getColor().z
	);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bgTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, blendMap);

	glm::mat4 model;
	glBindVertexArray(vao);
		glUniform1f(glGetUniformLocation(shader, "shineDamper"),  shineDamper);
		glUniform1f(glGetUniformLocation(shader, "reflectivity"), reflectivity);
		glUniform4fv(glGetUniformLocation(shader, "plane"), 1, glm::value_ptr(clipPlane));
		glUniform3f(glGetUniformLocation(shader, "viewPos"), getPos().x, getPos().y, getPos().z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"),  1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, rnum * 3);
	glBindVertexArray(0);
}

float Terrain::getHeight(float x, float z){
	float y = 0.f;
	
	y = heights[(int)z][(int)x];

	return y;
}

void Terrain::updateRes(sf::Window &window, GLuint &shader){
	glUseProgram(shader);
	glUniform2f(glGetUniformLocation(shader, "resolution"), window.getSize().x, window.getSize().y);
}

void Terrain::updateRes(int x, int y, GLuint &shader){
	glUseProgram(shader);
	glUniform2f(glGetUniformLocation(shader, "resolution"), x, y);
}

Terrain::~Terrain(){
	glDeleteTextures(1, &bgTexture);
	glDeleteTextures(1, &rTexture);
	glDeleteTextures(1, &gTexture);
	glDeleteTextures(1, &bTexture);
	glDeleteTextures(1, &blendMap);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertBuff);
	glDeleteBuffers(1, &normBuff);
	glDeleteBuffers(1, &texBuff);
}