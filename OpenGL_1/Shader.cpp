#include "Shader.h"

GLuint loadShaders(const char *vertexFile, const char *fragmentFile){
	// load shaders
	GLuint vertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	char vertFile[128], fragFile[128];
	sprintf(vertFile, "res/shaders/%s", vertexFile);
	sprintf(fragFile, "res/shaders/%s", fragmentFile);

	std::ifstream vertexShaderFile(vertFile);
	if(!vertexShaderFile.good())
		printf("%s not found\n", vertexFile);
	std::string   vertexShader;
	std::ifstream fragmentShaderFile(fragFile);
	if(!vertexShaderFile.good())
		printf("%s not found\n", fragmentFile);
	std::string   fragmentShader;

	// input data from the shader files to memory
	if(vertexShaderFile.is_open()){
		std::string line = "";
		while(getline(vertexShaderFile, line))
			vertexShader += line + "\n";

		vertexShaderFile.close();
	}
	if(fragmentShaderFile.is_open()){
		std::string line = "";
		while(getline(fragmentShaderFile, line))
			fragmentShader += line + "\n";

		fragmentShaderFile.close();
	}

	// compile shaders
	char const * vertexSourcePointer = vertexShader.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);
	char const * fragmentSourcePointer = fragmentShader.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderID);

	// link shaders
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	// return shaders
	return programID;
}