#ifndef CAMERA_H
#define CAMERA_H

#include "INCLUDES.h"
#include "Terrain.h"

glm::mat4 getViewMatrix(), getProjectionMatrix();
glm::vec3 getPos(), getDir(), getRight(), getUp();
void setProj(glm::mat4&);
void moveY(float); void moveY_players(float&, float);
void computeMats(sf::Window&, sf::Clock, float);
void computeMats_reflection(sf::Window&, sf::Clock, float);
void setCursorLocked();
void setTypingStatus(bool);
bool typingStatus();

bool coll(glm::vec3, float, glm::vec3, glm::vec3);
void get3DRay(glm::vec3*, glm::vec3*, sf::Window&);

#endif