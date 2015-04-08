#include "Texture.h"

GLuint loadTexture(const char * file){
	sf::Image img;

	char texFile[128];
	sprintf(texFile, "res/textures/%s", file);

	img.loadFromFile(texFile);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // repeat texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // repeat texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		img.getSize().x,
		img.getSize().y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		img.getPixelsPtr()
	);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0); // unbind

	return texture;
}