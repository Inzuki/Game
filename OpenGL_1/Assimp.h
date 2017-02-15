#ifndef ASSIMP_H
#define ASSIMP_H

#include "INCLUDES.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Camera.h"
#include "Shader.h"

#define MAX_BONES 64

// model loading
struct Vertex {
	glm::vec3 pos, normal;
	glm::vec2 uvs;
};

struct Texture {
	GLuint id;
	std::string type;
};

class Mesh_MDL {
	public:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		Mesh_MDL(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
		void draw(GLuint shader);
	private:
		GLuint VAO, VBO, EBO;
		void setupMesh();
};

// animations
namespace core {
	struct Model {
		struct Mesh {
			std::vector<glm::vec2> uvs;
			std::vector<glm::vec3> vertices, normals;
			std::vector<glm::vec4> weights, boneID;
			std::vector<unsigned int>indices;

			glm::mat4 baseModelMatrix;

			GLuint vao, vbo, nbo, ebo, uvb, tex, wbo, idbo;
			GLint posAttrib, normAttrib, texAttrib, weightAttrib, boneAttrib;
			GLuint modelID, viewID, projectionID, modelTransID;

			sf::Image image;
		};
		
		struct Animation {
			std::string name;
			double duration, ticksPerSec;
			std::vector<glm::mat4> boneTrans;
			std::map<std::string, glm::mat4> boneOffset;

			struct Channel {
				std::string name;
				glm::mat4 offset;
				std::vector<aiVectorKey> mPositionKeys, mScalingKeys;
				std::vector<aiQuatKey> mRotationKeys;
			};
			std::vector<Channel> channels;

			struct BoneNode {
				std::string name;
				BoneNode *parent;
				std::vector<BoneNode> children;
				glm::mat4 nodeTransform, boneTransform;
			};
			BoneNode root;

			void buildBoneTree(const aiScene* scene, aiNode *node, BoneNode *bNode, Model *m);
		};

		std::vector<Animation> animations;
		unsigned int currentAnim;
		void setAnimation(std::string name);
		std::vector<std::string> animNames;
		std::map<std::string, unsigned int> boneID;
		void tick(double time);
		void updateBoneTree(double time, Animation::BoneNode *node, glm::mat4 &transform);
		glm::mat4 modelTrans;
		void setModelTrans(glm::mat4 &transform);
		std::string rootPath;
		std::vector<Mesh> meshes;
		GLuint shader;
		bool modelLoaded;
		Model();
		void init();
		void render(float dt, glm::mat4 &model, glm::mat4 &viewMat, glm::mat4 &projMat, glm::vec4 &clipPlane);
	};

	class ModelLoader {
		private:
			void processNode(const aiScene *scene, aiNode *node, Model *m);
			void processMesh(const aiScene *scene, aiNode *node, aiMesh *mesh, Model *m);
			void processAnimations(const aiScene *scene, Model *m);
		public:
			bool loadModel(const char *fp, Model *m);
	};

	glm::mat4 toMat4(aiMatrix4x4 *ai);
}

#endif