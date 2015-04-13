#ifndef CAMERA_H
#define CAMERA_H

#include "INCLUDES.h"
#include "Terrain.h"

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getPos();
void setPosY(float);
void computeMats(sf::Window&, sf::Clock, float);
void setCursorLocked();

bool coll(glm::vec3, float, glm::vec3, glm::vec3);
void get3DRay(glm::vec3*, glm::vec3*, sf::Window&);
//glm::vec3 picker(sf::Window&);

#endif