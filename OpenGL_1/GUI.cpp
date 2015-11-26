#include "GUI.h"

/* --- *\
|* GUI *|
\* --- */
GUI::GUI(const char *file, glm::vec2 pos, glm::vec2 scale){
	glGenVertexArrays(1, &VAO);
	texture = loadTexture(file);
	this->pos   = pos;
	this->scale = scale;
}

GUI::GUI(GLuint texture, glm::vec2 pos, glm::vec2 scale){
	glGenVertexArrays(1, &VAO);
	this->pos      = pos;
	this->texture  = texture;
	this->scale    = scale;
}

GLuint GUI::getTexture(){  return texture; }
glm::vec2 GUI::getPos(){   return pos;     }
glm::vec2 GUI::getScale(){ return scale;   }

GUI::~GUI(){ /*glDeleteTextures(1, &texture);*/ }



/* ------------ *\
|* GUI RENDERER *|
\* ------------ */
GUIRenderer::GUIRenderer(){
	GLfloat positions[] = { -1, 1, -1, -1, 1, 1, 1, -1 };
	
	glGenVertexArrays(1, &quad);

	shader = loadShaders("gui.vert", "gui.frag");

	posAttrib = glGetAttribLocation(shader, "position");

	glBindVertexArray(quad);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GUIRenderer::render(std::vector<GUI> guis){
	glUseProgram(shader);

	glBindVertexArray(quad);
	glEnableVertexAttribArray(posAttrib);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	
	for(int i = 0; i < guis.size(); i++){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, guis[i].getTexture());
		glUniform1i(glGetUniformLocation(shader, "guiTexture"), 0);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(guis[i].getPos().x, guis[i].getPos().y, 0.f));
		model = glm::scale(model, glm::vec3(guis[i].getScale().x, guis[i].getScale().y, 0.f));
		glUniformMatrix4fv(glGetUniformLocation(shader, "trans"), 1, GL_FALSE, (GLfloat*)&model[0][0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisableVertexAttribArray(posAttrib);
	glBindVertexArray(0);
}

GUIRenderer::~GUIRenderer(){
	glDeleteProgram(shader);
}