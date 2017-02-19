#ifndef SHADOWS_H
#define SHADOWS_H

#include "INCLUDES.h"

namespace shadow {
	// (orthographic) light source that covers the whole scene.
	// light rays emitted from this source are all in parallel,
	// so this essentially creates a sun
	class Sun {
		public:
			Sun(glm::vec3);

		private:
			glm::vec3 position;
			GLuint depthMap, depthMapFBO;

			void createShadowMap();
	};
};

#endif