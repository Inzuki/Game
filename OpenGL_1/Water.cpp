#include "Water.h"

Water::Water(int window_width, int window_height){
	initReflectionFrameBuffer(window_width, window_height);
	initRefractionFrameBuffer(window_width, window_height);
	dudvMap   = loadTexture("res/textures/water.png");
	normalMap = loadTexture("res/textures/water_normal.png");

	float vertices[] = { 1.f, 1.f, 1.f, -1.f, -1.f, 1.f, 1.f, -1.f, -1.f, -1.f, -1.f, 1.f };
	sizeOfVerts = sizeof(vertices);
	
	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);

	glBindVertexArray(waterVAO);
		glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeOfVerts, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Water::render(GLuint &shader, glm::mat4 &model, glm::mat4 &VP, sf::Clock &clk, Lamp &lamp){
	moveFactor += (float)WAVE_SPEED * clk.getElapsedTime().asSeconds();
	fmod(moveFactor, 1.f);

	glUniform3f(glGetUniformLocation(
				shader, "lightPos"),
				lamp.getLampPos().x,
				lamp.getLampPos().y,
				lamp.getLampPos().z
	);

	glBindVertexArray(waterVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectionTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractionTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, dudvMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, getRefractionDepthTexture());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUniform1i(glGetUniformLocation(shader, "reflectionTex"), 0);
		glUniform1i(glGetUniformLocation(shader, "refractionTex"), 1);
		glUniform1i(glGetUniformLocation(shader, "dudvMap"), 2);
		glUniform1i(glGetUniformLocation(shader, "normalMap"), 3);
		glUniform1i(glGetUniformLocation(shader, "depthMap"), 4);

		glUniform1f(glGetUniformLocation(shader, "moveFactor"), moveFactor);

		glUniform3f(glGetUniformLocation(shader, "camPos"), getPos().x, getPos().y, getPos().z);
		glUniformMatrix4fv(glGetUniformLocation(shader, "VP"), 1, GL_FALSE, glm::value_ptr(VP));
		glUniformMatrix4fv(glGetUniformLocation(shader, "M"),  1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, sizeOfVerts);

		glDisable(GL_BLEND);
	glBindVertexArray(0);
}

float Water::getScale(){ return scale; }

void Water::initReflectionFrameBuffer(int window_width, int window_height){
	reflectionFrameBuffer = createFrameBuffer();
	reflectionTexture     = createTextureAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	reflectionDepthBuffer = createDepthBufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
	unbindCurrentFrameBuffer(window_width, window_height);
}

void Water::initRefractionFrameBuffer(int window_width, int window_height){
	refractionFrameBuffer  = createFrameBuffer();
	refractionTexture      = createTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	refractionDepthTexture = createDepthTextureAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
	unbindCurrentFrameBuffer(window_width, window_height);
}

void Water::bindReflectionFrameBuffer(){
	bindFrameBuffer(reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void Water::bindRefractionFrameBuffer(){
	bindFrameBuffer(refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

void Water::unbindCurrentFrameBuffer(int window_width, int window_height){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
}

void Water::bindFrameBuffer(GLuint frameBuffer, int width, int height){
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}

GLuint Water::createFrameBuffer(){
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	return frameBuffer;
}

GLuint Water::createTextureAttachment(int width, int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);
	return texture;
}

GLuint Water::createDepthTextureAttachment(int width, int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	return texture;
}

GLuint Water::createDepthBufferAttachment(int width, int height){
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	return depthBuffer;
}

GLuint Water::getReflectionTexture(){ return reflectionTexture; }

GLuint Water::getRefractionTexture(){ return refractionTexture; }

GLuint Water::getRefractionDepthTexture(){ return refractionDepthTexture; }

Water::~Water(){
	glDeleteFramebuffers(1, &reflectionFrameBuffer);
	glDeleteTextures(1, &reflectionTexture);
	glDeleteRenderbuffers(1, &reflectionDepthBuffer);
	glDeleteFramebuffers(1, &refractionFrameBuffer);
	glDeleteTextures(1, &refractionTexture);
	glDeleteTextures(1, &refractionDepthTexture);

	glDeleteTextures(1, &dudvMap);
	glDeleteTextures(1, &normalMap);
}