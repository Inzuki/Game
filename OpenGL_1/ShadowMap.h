#define _CRT_SECURE_NO_WARNINGS

#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "INCLUDES.h"

class ShadowBox {
	public:
		ShadowBox();
		~ShadowBox();
		void update();

	private:
		const float OFFSET = 10.f, SHADOW_DISTANCE = 100.f;
		glm::vec4 UP = glm::vec4(0.f, 1.f, 0.f, 0.f),
				  FORWARd = glm::vec4(0.f, 0.f, -1.f, 0.f);

		float minX, maxX, minY, maxY, minZ, maxZ,
			  farWidth, farHeight, nearWidth, nearHeight;
		glm::mat4 lightView;
};

class ShadowFrameBuffer {
	public:
		ShadowFrameBuffer(int, int, int, int);
		~ShadowFrameBuffer();
		GLuint getShadowMap();

	private:
		void initFBO(int, int),
			 bindFrameBuffer(), bindFrameBuffer(GLuint, int, int), unbindFrameBuffer(int, int);
		GLuint createFrameBuffer(), createDepthBufferAttachment(int, int);

		int WIDTH, HEIGHT;
		GLuint FBO, shadowMap;
};

class ShadowMap {
	public:
		ShadowMap();
		~ShadowMap();

	private:
		const int SHADOW_MAP_SIZE = 1024;

		ShadowFrameBuffer shadowFBO;
		//ShadowShader shader;
		ShadowBox shadowBox;

		glm::mat4 proj = glm::mat4(),
				  lightView = glm::mat4(),
				  projView = glm::mat4();
				  //offset = createOffset();

		// ShadowMapEntRenderer entRenderer;
};

#endif