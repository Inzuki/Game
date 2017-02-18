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

// shaders with geometry
GLuint loadShaders(const char *vertexFile, const char *fragmentFile, const char *geometryFile){
	// load shaders
	GLuint vertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);

	char vertFile[128], fragFile[128], geoFile[128];
	sprintf(vertFile, "res/shaders/%s", vertexFile);
	sprintf(fragFile, "res/shaders/%s", fragmentFile);
	sprintf(geoFile,  "res/shaders/%s", geometryFile);

	std::ifstream vertexShaderFile(vertFile);
	if(!vertexShaderFile.good())
		printf("%s not found\n", vertexFile);
	std::string vertexShader;

	std::ifstream fragmentShaderFile(fragFile);
	if(!vertexShaderFile.good())
		printf("%s not found\n", fragmentFile);
	std::string fragmentShader;

	std::ifstream geometryShaderFile(geoFile);
	if(!vertexShaderFile.good())
		printf("%s not found\n", geometryFile);
	std::string geometryShader;

	// input data from the shader files to memory
	// vertex shader
	if(vertexShaderFile.is_open()){
		std::string line = "";
		while(getline(vertexShaderFile, line))
			vertexShader += line + "\n";

		vertexShaderFile.close();
	}
	// fragment shader
	if(fragmentShaderFile.is_open()){
		std::string line = "";
		while(getline(fragmentShaderFile, line))
			fragmentShader += line + "\n";

		fragmentShaderFile.close();
	}
	// geometry shader
	if(geometryShaderFile.is_open()){
		std::string line = "";
		while(getline(geometryShaderFile, line))
			geometryShader += line + "\n";

		geometryShaderFile.close();
	}

	// compile shaders
	// vertex shader
	char const * vertexSourcePointer = vertexShader.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);
	// fragment shader
	char const * fragmentSourcePointer = fragmentShader.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderID);
	// geometry shader
	char const * geometrySourcePointer = geometryShader.c_str();
	glShaderSource(geometryShaderID, 1, &geometrySourcePointer, NULL);
	glCompileShader(geometryShaderID);

	// link shaders
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glAttachShader(programID, geometryShaderID);
	glLinkProgram(programID);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(geometryShaderID);

	// return shaders
	return programID;
}