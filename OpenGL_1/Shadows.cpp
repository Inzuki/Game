#include "Shadows.h"

shadow::Sun::Sun(glm::vec3 sun_position){
	this->sun_position = sun_position;
	createShadowMap();
}

GLuint shadow::Sun::getDepthShader(){ return depthShader; }
GLuint shadow::Sun::getDepthMapFBO(){ return depthMapFBO; }
GLuint shadow::Sun::getDepthMap(){    return depthMap;    }

GLuint shadow::Sun::getShadowWidth(){  return SHADOW_WIDTH;  }
GLuint shadow::Sun::getShadowHeight(){ return SHADOW_HEIGHT; }

glm::vec3 shadow::Sun::getSunPosition(){ return sun_position;     }
glm::mat4 shadow::Sun::getMatrix(){		 return lightSpaceMatrix; }

void shadow::Sun::renderShadows(){
	float space = 20.f;
	lightProjection = glm::ortho(-space, space, -space, space, near_plane, far_plane);
    lightView		= glm::lookAt(sun_position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;

	glUseProgram(depthShader);
	glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMat"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void shadow::Sun::debugDepthMap(){
	// todo
	/*
	glViewport(0, 0, 640, 480);
	glUseProgram(debugDepthQuad);
    glUniform1f(glGetUniformLocation(debugDepthQuad, "near_plane"), near_plane);
    glUniform1f(glGetUniformLocation(debugDepthQuad, "far_plane"),  far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    RenderQuad(); // uncomment this line to see depth map
	*/
}

void shadow::Sun::createShadowMap(){
	// generate depth map and frame buffer
    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// load depth shaders (second one is for debugging the depth map
	depthShader = loadShaders("simpleDepthShader.vert", "simpleDepthShader.frag");
	debugShader = loadShaders("debug_quad.vert", "debug_quad.frag");
}

shadow::Sun::~Sun(){
	glDeleteTextures(1, &depthMap);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteProgram(depthShader);
	glDeleteProgram(debugShader);
}