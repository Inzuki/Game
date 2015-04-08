#ifndef CAMERA_H
#define CAMERA_H

#include "INCLUDES.h"

glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getPos();
void computeMats(sf::Window &, sf::Clock);

#endif