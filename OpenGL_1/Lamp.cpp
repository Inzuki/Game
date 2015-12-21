#include "Lamp.h"

Lamp::Lamp(glm::vec3 inPos, glm::vec3 color){
	attenuation = glm::vec3(1.f, 0.f, 0.f);
	this->color = color;

	model = glm::mat4();
	model = glm::translate(model, inPos);
	model = glm::scale(model, glm::vec3(0.1f));
}

Lamp::Lamp(glm::vec3 inPos, glm::vec3 color, glm::vec3 attenuation){
	this->attenuation = attenuation;
	this->color = color;

	model = glm::mat4();
	model = glm::translate(model, inPos);
	model = glm::scale(model, glm::vec3(0.1f));
}

void Lamp::draw(OBJ &cube, glm::mat4 &inModel, GLuint &shader, glm::mat4 &viewMat, glm::mat4 &projMat){
	model = inModel;
	
	glUniform3f(glGetUniformLocation(shader, "inColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"),  1, GL_FALSE, glm::value_ptr(inModel));
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
	cube.draw(inModel, viewMat, projMat, shader, glm::vec4(0.f, 0.f, 0.f, 0.f));
}

void Lamp::moveLamp(glm::vec3 translation){
	model = glm::translate(model, translation);
}

Lamp::~Lamp(){}

int Lamp::getVerts(){ return vertexCount; }

glm::vec3 Lamp::getLampPos(){ return glm::vec3(model[3]); }

glm::vec3 Lamp::getColor(){ return color; }

glm::vec3 Lamp::getAttenuation(){ return attenuation; }