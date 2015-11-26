#ifndef GUI_H
#define GUI_H

#include "INCLUDES.h"
#include "Texture.h"
#include "Shader.h"

class GUI {
	public:
		GUI(const char*, glm::vec2, glm::vec2);
		GUI(GLuint, glm::vec2, glm::vec2);
		~GUI();
		GLuint getTexture();
		glm::vec2 getPos();
		glm::vec2 getScale();

	private:
		GLuint texture, VAO;
		glm::vec2 pos, scale;
};

class GUIRenderer {
	public:
		GUIRenderer();
		~GUIRenderer();

		void render(std::vector<GUI> guis);

	private:
		GLuint quad, VBO, shader;
		GLint posAttrib;
};

#endif