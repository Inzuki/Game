#ifndef SHADOWS_H
#define SHADOWS_H

#include "INCLUDES.h"
#include "Shader.h"

namespace shadow {
	// (orthographic) light source that covers the whole scene.
	// light rays emitted from this source are all in parallel,
	// so this essentially creates a sun
	class Sun {
		public:
			Sun(glm::vec3);
			~Sun();
			void renderShadows(), debugDepthMap();
			GLuint getDepthShader(), getDepthMapFBO(), getDepthMap();
			GLuint getShadowWidth(), getShadowHeight();
			glm::vec3 getSunPosition();
			glm::mat4 getMatrix();

		private:
			const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
			const GLfloat near_plane = 1.f, far_plane = 150.f;

			glm::vec3 sun_position;
			glm::mat4 lightProjection, lightView, lightSpaceMatrix;

			GLuint depthMap, depthMapFBO;
			GLuint depthShader, debugShader;

			void createShadowMap();
	};

	// todo
	// omnidirectional light/point shadows
	class Light {

	};
};

#endif