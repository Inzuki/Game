#include "ShadowMap.h"

#pragma region ShadowBox
ShadowBox::ShadowBox(){
	//lightView = in_lightView;
	//calculateWidthsAndHeights();
}

void ShadowBox::update(){
	//glm::mat4 rotation = calculateCameraRotation();
}

ShadowBox::~ShadowBox(){

}
#pragma endregion

#pragma region ShadowFrameBuffer
ShadowFrameBuffer::ShadowFrameBuffer(int width, int height, int window_width, int window_height){
	WIDTH  = width;
	HEIGHT = height;
	initFBO(window_width, window_height);
}

GLuint ShadowFrameBuffer::getShadowMap(){ return shadowMap; }

void ShadowFrameBuffer::initFBO(int window_width, int window_height){
	FBO = createFrameBuffer();
	shadowMap = createDepthBufferAttachment(WIDTH, HEIGHT);
	unbindFrameBuffer(window_width, window_height);
}

void ShadowFrameBuffer::bindFrameBuffer(){
	bindFrameBuffer(FBO, WIDTH, HEIGHT);
}

void ShadowFrameBuffer::bindFrameBuffer(GLuint frameBuffer, int width, int height){
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
}

void ShadowFrameBuffer::unbindFrameBuffer(int window_width, int window_height){
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);
}

GLuint ShadowFrameBuffer::createFrameBuffer(){
	GLuint frameBuffer;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	return frameBuffer;
}

GLuint ShadowFrameBuffer::createDepthBufferAttachment(int width, int height){
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	return texture;
}

ShadowFrameBuffer::~ShadowFrameBuffer(){
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &shadowMap);
}
#pragma endregion

#pragma region ShadowMap
ShadowMap::~ShadowMap(){}
#pragma endregion