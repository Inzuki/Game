#include "OBJ.h"
#include "Texture.h"

OBJ::OBJ(const char *objName, const char *textureFile){
	// load the texture, ez
	texture = loadTexture(textureFile);
	
	// get setup to read OBJ
	std::vector<glm::vec2>				   tempTextures;
	std::vector<glm::vec3>    tempVertices,			   tempNormals;
	std::vector<glm::vec2>				   textures;
	std::vector<glm::vec3>    vertices,                normals;
	std::vector<unsigned int> vertIndices, texIndices, normIndices;

	// open OBJ file and begin reading the OBJ's contents
	char objFile[128];
	sprintf(objFile, "res/models/%s", objName);
	FILE *file = fopen(objFile, "r");

	while(true){
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break;

		if(strcmp(lineHeader, "v") == 0){
			glm::vec3 vertex;

			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			tempVertices.push_back(vertex);
		}else if(strcmp(lineHeader, "vt") == 0){
			glm::vec2 texture;

			fscanf(file, "%f %f\n", &texture.x, &texture.y);

			tempTextures.push_back(texture);
		}else if(strcmp(lineHeader, "vn") == 0){
			glm::vec3 normal;

			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);

			tempNormals.push_back(normal);
		}else if(strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], textureIndex[3], normalIndex[3];

			fscanf(file,
				   "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				   &vertexIndex[0],
				   &textureIndex[0],
				   &normalIndex[0],
				   &vertexIndex[1],
				   &textureIndex[1],
				   &normalIndex[1],
				   &vertexIndex[2],
				   &textureIndex[2],
				   &normalIndex[2]
			);
			
			vertIndices.push_back(vertexIndex[0]);
			vertIndices.push_back(vertexIndex[1]);
			vertIndices.push_back(vertexIndex[2]);
			 texIndices.push_back(textureIndex[0]);
			 texIndices.push_back(textureIndex[1]);
			 texIndices.push_back(textureIndex[2]);
			normIndices.push_back(normalIndex[0]);
			normIndices.push_back(normalIndex[1]);
			normIndices.push_back(normalIndex[2]);
		}
	}

	fclose(file);
	
	for(unsigned int i = 0; i < vertIndices.size(); i++){
		vertices.push_back(tempVertices[vertIndices[i] - 1]);
		 textures.push_back(tempTextures[texIndices[i] - 1]);
		  normals.push_back(tempNormals[normIndices[i] - 1]);
	}

	vertexCount = vertices.size();

	glGenBuffers(1, &vertBuff);
	glGenBuffers(1, &texBuff);
	glGenBuffers(1, &normBuff);
	glGenVertexArrays(1, &obj);

	// create vertex array object
	glBindVertexArray(obj);
		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
		// texture coordinates
		glBindBuffer(GL_ARRAY_BUFFER, texBuff);
		glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(glm::vec2), &textures[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		// normals
		glBindBuffer(GL_ARRAY_BUFFER, normBuff);
		glBufferData(GL_ARRAY_BUFFER,  normals.size() * sizeof(glm::vec2), &normals[0],  GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void OBJ::draw(glm::mat4 &model, GLuint modelLoc, GLuint matrixLoc, glm::mat4 &VP){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(obj);
		glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(VP));
		glUniformMatrix4fv(modelLoc,  1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
}

void OBJ::deleteObj(){
	glDeleteBuffers(1, &vertBuff);
	glDeleteBuffers(1,  &texBuff);
	glDeleteBuffers(1, &normBuff);
	glDeleteVertexArrays(1, &obj);
}

GLuint OBJ::getTexture(){ return texture; }

int OBJ::getVerts(){ return vertexCount; }