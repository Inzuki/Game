#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MAX_LIGHTS 3

extern glm::vec3 position;