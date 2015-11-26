#ifndef WATER_H
#define WATER_H

#include "INCLUDES.h"
#include "Camera.h"
#include "Lamp.h"

#define REFLECTION_WIDTH    1600
#define REFLECTION_HEIGHT   1024
#define REFRACTION_WIDTH    1600
#define REFRACTION_HEIGHT   1024
#define WAVE_SPEED		  .00001

class Water {
	public:
		Water(int, int);
		~Water();
		void bindReflectionFrameBuffer();
		void bindRefractionFrameBuffer();
		void unbindCurrentFrameBuffer(int, int);
		void bindFrameBuffer(GLuint, int, int);

		void render(GLuint&, glm::mat4&, glm::mat4&, sf::Clock&, Lamp&);
		float getScale();

		GLuint getReflectionTexture();
		GLuint getRefractionTexture();
		GLuint getRefractionDepthTexture();

	private:
		GLuint reflectionFrameBuffer, reflectionTexture, reflectionDepthBuffer,
			   refractionFrameBuffer, refractionTexture, refractionDepthTexture,
			   dudvMap, normalMap;
		GLuint waterVAO, waterVBO;
		const float scale = 75.f;
		float moveFactor = 0.f;
		
		void initReflectionFrameBuffer(int, int);
		void initRefractionFrameBuffer(int, int);

		GLuint createFrameBuffer();
		GLuint createTextureAttachment(int, int);
		GLuint createDepthTextureAttachment(int, int);
		GLuint createDepthBufferAttachment(int, int);

		int sizeOfVerts;
};

#endif