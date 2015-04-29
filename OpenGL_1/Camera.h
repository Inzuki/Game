#ifndef CAMERA_H
#define CAMERA_H

#include "INCLUDES.h"
#include "Terrain.h"

glm::mat4 getViewMatrix(), getProjectionMatrix();
glm::vec3 getPos(), getDir(), getRight();
bool typing(); void setTyping(bool);
void moveY(float);
void computeMats(sf::Window&, sf::Clock, float);
void setCursorLocked();

bool coll(glm::vec3, float, glm::vec3, glm::vec3);
void get3DRay(glm::vec3*, glm::vec3*, sf::Window&);

#endif