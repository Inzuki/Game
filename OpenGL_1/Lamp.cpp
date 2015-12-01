#include "Lamp.h"

Lamp::Lamp(glm::vec3 inPos, glm::vec3 color){
	this->color = color;

	std::vector<glm::vec3>    tempVertices;
	std::vector<glm::vec3>    vertices;
	std::vector<unsigned int> vertIndices;

	// open OBJ file and begin reading the OBJ's contents
	FILE *file = fopen("res/models/cube.obj", "r");

	while(true){
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break;

		if(strcmp(lineHeader, "v") == 0){
			glm::vec3 vertex;

			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);

			tempVertices.push_back(vertex);
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
		}
	}
	
	for(unsigned int i = 0; i < vertIndices.size(); i++){
		vertices.push_back(tempVertices[vertIndices[i] - 1]);
	}

	vertexCount = vertices.size();

	glGenBuffers(1, &vertBuff);
	glGenVertexArrays(1, &lamp);

	// create vertex array object
	glBindVertexArray(lamp);
		// vertices
		glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	model = glm::mat4();
	model = glm::translate(model, inPos);
	model = glm::scale(model, glm::vec3(0.1f));
}

void Lamp::draw(glm::mat4 &inModel, GLuint &shader, glm::mat4 &VP){
	model = inModel;
	
	glBindVertexArray(lamp);
		glUniform3f(glGetUniformLocation(shader, "inColor"), color.x, color.y, color.z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "VP"), 1, GL_FALSE, glm::value_ptr(VP));
		glUniformMatrix4fv(glGetUniformLocation(shader, "M"),  1, GL_FALSE, glm::value_ptr(inModel));
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	glBindVertexArray(0);
}

void Lamp::moveLamp(glm::vec3 translation){
	model = glm::translate(model, translation);
}

Lamp::~Lamp(){
	glDeleteBuffers(1,  &vertBuff);
	glDeleteVertexArrays(1, &lamp);
}

int Lamp::getVerts(){ return vertexCount; }

glm::vec3 Lamp::getLampPos(){ return glm::vec3(model[3]); }

glm::vec3 Lamp::getColor(){ return color; }