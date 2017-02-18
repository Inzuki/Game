#define _CRT_SECURE_NO_WARNINGS

#ifndef SHADER_H
#define SHADER_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>

GLuint loadShaders(const char *, const char *); // vertex and fragment
GLuint loadShaders(const char *, const char *, const char *); // vertex, fragment, and geometry

#endif