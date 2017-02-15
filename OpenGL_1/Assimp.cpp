#include "Assimp.h"

#pragma region model loading
Mesh_MDL::Mesh_MDL(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures){
	this->vertices = vertices;
	this->indices  = indices;
	this->textures = textures;

	this->setupMesh();
}

void Mesh_MDL::setupMesh(){

}
#pragma endregion model loading

#pragma region animations
bool core::ModelLoader::loadModel(const char *fp, Model *m){
	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile(fp, aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

	if(!scene)
		return false;

	m->rootPath = (std::string)fp;
	for(int x = m->rootPath.size() - 1; x >= 0; x--){
		if(m->rootPath[x] == '/' || m->rootPath[x] == '\\'){
			m->rootPath = m->rootPath.substr(0, x + 1);
			x = -1;
		}
	}

	processAnimations(scene, m);
	processNode(scene, scene->mRootNode, m);

	if(scene->HasAnimations())
		m->animations[m->currentAnim].buildBoneTree(scene, scene->mRootNode, &m->animations[m->currentAnim].root, m);

	m->modelTrans = glm::mat4(1.f);
	m->modelLoaded = true;

	return true;
};

void core::ModelLoader::processAnimations(const aiScene* scene, Model *m){
	if(scene->HasAnimations()){
		for(std::size_t x = 0; x < scene->mNumAnimations; x++){
			Model::Animation tempAnim;
			tempAnim.name = scene->mAnimations[x]->mName.data;
			tempAnim.duration = scene->mAnimations[x]->mDuration;
			tempAnim.ticksPerSec = scene->mAnimations[x]->mTicksPerSecond;

			for(std::size_t y = 0; y < scene->mAnimations[x]->mNumChannels; y++){
				Model::Animation::Channel tempChan;
				tempChan.name = scene->mAnimations[x]->mChannels[y]->mNodeName.data;

				for(std::size_t z = 0; z < scene->mAnimations[x]->mChannels[y]->mNumPositionKeys; z++)
					tempChan.mPositionKeys.push_back(scene->mAnimations[x]->mChannels[y]->mPositionKeys[z]);
				for(std::size_t z = 0; z < scene->mAnimations[x]->mChannels[y]->mNumRotationKeys; z++)
					tempChan.mRotationKeys.push_back(scene->mAnimations[x]->mChannels[y]->mRotationKeys[z]);
				for(std::size_t z = 0; z < scene->mAnimations[x]->mChannels[y]->mNumScalingKeys; z++)
					tempChan.mScalingKeys.push_back(scene->mAnimations[x]->mChannels[y]->mScalingKeys[z]);

				tempAnim.channels.push_back(tempChan);
			}

			m->currentAnim = 0;

			for(int z = 0; z < MAX_BONES; z++)
				tempAnim.boneTrans.push_back(glm::mat4(1.f));

			m->animations.push_back(tempAnim);
		}

		m->animations[m->currentAnim].root.name = "rootBoneTreeNode";
	}
};

void core::Model::Animation::buildBoneTree(const aiScene *scene, aiNode *node, BoneNode *bNode, Model *m){
	if(scene->HasAnimations()){
		if(m->boneID.find(node->mName.data) != m->boneID.end()){
			BoneNode tempNode;
			tempNode.name = node->mName.data;
			tempNode.parent = bNode;
			tempNode.nodeTransform = toMat4(&node->mTransformation);
			tempNode.boneTransform = boneOffset[tempNode.name];
			bNode->children.push_back(tempNode);
		}

		if(node->mNumChildren > 0){
			for(unsigned int x = 0; x < node->mNumChildren; x++){
				if(m->boneID.find(node->mName.data) != m->boneID.end())
					buildBoneTree(scene, node->mChildren[x], &bNode->children[bNode->children.size() - 1], m);
				else
					buildBoneTree(scene, node->mChildren[x], bNode, m);
			}
		}
	}
};

void core::ModelLoader::processNode(const aiScene *scene, aiNode *node, Model *m){
	if(node->mNumMeshes > 0)
		for(unsigned int x = 0; x < node->mNumMeshes; x++)
			processMesh(scene, node, scene->mMeshes[node->mMeshes[x]], m);

	if(node->mNumChildren > 0)
		for(unsigned int x = 0; x < node->mNumChildren; x++)
			processNode(scene, node->mChildren[x], m);
};

void core::ModelLoader::processMesh(const aiScene *scene, aiNode *node, aiMesh *mesh, Model *m){
	Model::Mesh tempMesh;

	tempMesh.weights.resize(mesh->mNumVertices);
	std::fill(tempMesh.weights.begin(), tempMesh.weights.end(), glm::vec4(1.f));
	tempMesh.boneID.resize(mesh->mNumVertices);
	std::fill(tempMesh.boneID.begin(), tempMesh.boneID.end(), glm::vec4(-333.f));

	tempMesh.baseModelMatrix = toMat4(&node->mTransformation);

	if(node->mParent != NULL)
		tempMesh.baseModelMatrix *= toMat4(&node->mParent->mTransformation);

	for(unsigned x = 0; x < mesh->mNumVertices; x++){
		glm::vec3 tempV;
		tempV.x = mesh->mVertices[x].x;
		tempV.y = mesh->mVertices[x].y;
		tempV.z = mesh->mVertices[x].z;
		tempMesh.vertices.push_back(tempV);

		glm::vec2 tempUV;
		tempUV.x = mesh->mTextureCoords[0][x].x;
		tempUV.y = mesh->mTextureCoords[0][x].y;
		tempMesh.uvs.push_back(tempUV);

		if(mesh->HasNormals()){
			glm::vec3 tempN;
			tempN.x = mesh->mNormals[x].x;
			tempN.y = mesh->mNormals[x].y;
			tempN.z = mesh->mNormals[x].z;
		}
	}

	for(unsigned int x = 0; x < mesh->mNumFaces; x++){
		tempMesh.indices.push_back(mesh->mFaces[x].mIndices[0]);
		tempMesh.indices.push_back(mesh->mFaces[x].mIndices[1]);
		tempMesh.indices.push_back(mesh->mFaces[x].mIndices[2]);
	}

	if(scene->HasMaterials()){
		aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];

		if(mat->GetTextureCount(aiTextureType_DIFFUSE) > 0){
			aiString path;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string finalFP = m->rootPath + path.C_Str();
			tempMesh.image.loadFromFile(finalFP);
		}
	}

	if(mesh->HasBones()){
		for(std::size_t x = 0; x < mesh->mNumBones; x++){
			unsigned int index = 0;

			if(m->boneID.find(mesh->mBones[x]->mName.data) == m->boneID.end())
				index = m->boneID.size();
			else
				index = m->boneID[mesh->mBones[x]->mName.data];

			m->boneID[mesh->mBones[x]->mName.data] = index;

			for(std::size_t y = 0; y < m->animations[m->currentAnim].channels.size(); y++)
				if(m->animations[m->currentAnim].channels[y].name == mesh->mBones[x]->mName.data)
					m->animations[m->currentAnim].boneOffset[mesh->mBones[x]->mName.data] = toMat4(&mesh->mBones[x]->mOffsetMatrix);

			for(std::size_t y = 0; y < mesh->mBones[x]->mNumWeights; y++){
				unsigned int vertexID = mesh->mBones[x]->mWeights[y].mVertexId;

				if(tempMesh.boneID[vertexID].x == -333){
					tempMesh.boneID[vertexID].x = (float)index;
					tempMesh.weights[vertexID].x = mesh->mBones[x]->mWeights[y].mWeight;
				}else if(tempMesh.boneID[vertexID].y == -333){
					tempMesh.boneID[vertexID].y = (float)index;
					tempMesh.weights[vertexID].y = mesh->mBones[x]->mWeights[y].mWeight;
				}else if(tempMesh.boneID[vertexID].z == -333){
					tempMesh.boneID[vertexID].z = (float)index;
					tempMesh.weights[vertexID].z = mesh->mBones[x]->mWeights[y].mWeight;
				}else if(tempMesh.boneID[vertexID].w == -333){
					tempMesh.boneID[vertexID].w = (float)index;
					tempMesh.weights[vertexID].w = mesh->mBones[x]->mWeights[y].mWeight;
				}
			}
		}
	}

	m->meshes.push_back(tempMesh);
};

void core::Model::tick(double time){
	double timeInTicks = time * animations[currentAnim].ticksPerSec;
	updateBoneTree(timeInTicks, &animations[currentAnim].root, glm::mat4(1.f));
};

void core::Model::updateBoneTree(double timeInTicks, Model::Animation::BoneNode *node, glm::mat4 &parentTransform){
	int chanIndex = 0;

	for(std::size_t x = 0; x < animations[currentAnim].channels.size(); x++)
		if(node->name == animations[currentAnim].channels[x].name)
			chanIndex = x;

	double animTime = std::fmod(timeInTicks, animations[currentAnim].duration);

	aiQuaternion aiRotation(animations[currentAnim].channels[chanIndex].mRotationKeys[0].mValue);
	aiVector3D   aiTranslation(animations[currentAnim].channels[chanIndex].mPositionKeys[0].mValue);
	aiVector3D   aiScale(animations[currentAnim].channels[chanIndex].mScalingKeys[0].mValue);

	Assimp::Interpolator<aiQuaternion> slerp;
	Assimp::Interpolator<aiVector3D> lerp;

	int key1, key2;
	if(std::round(animTime) < animTime){
		key1 = (int)std::round(animTime);
		key2 = key1 + 1;
	}else {
		key1 = (int)std::round(animTime) - 1;
		key2 = (int)std::round(animTime);
	}

	if(animations[currentAnim].channels[chanIndex].mPositionKeys.size() > 1)
		lerp(aiTranslation, animations[currentAnim].channels[chanIndex].mPositionKeys[key1].mValue, animations[currentAnim].channels[chanIndex].mPositionKeys[key2].mValue, float(animTime - key1));
	if(animations[currentAnim].channels[chanIndex].mScalingKeys.size() > 1)
		lerp(aiScale, animations[currentAnim].channels[chanIndex].mScalingKeys[key1].mValue, animations[currentAnim].channels[chanIndex].mScalingKeys[key2].mValue, float(animTime - key1));
	if(animations[currentAnim].channels[chanIndex].mRotationKeys.size() > 1)
		slerp(aiRotation, animations[currentAnim].channels[chanIndex].mRotationKeys[key1].mValue, animations[currentAnim].channels[chanIndex].mRotationKeys[key2].mValue, float(animTime - key1));

	glm::vec3 translation((GLfloat)aiTranslation.x, (GLfloat)aiTranslation.y, (GLfloat)aiTranslation.z);
	glm::vec3 scaling((GLfloat)aiScale.x, (GLfloat)aiScale.y, (GLfloat)aiScale.z);
	glm::quat rotation((GLfloat)aiRotation.w, (GLfloat)aiRotation.x, (GLfloat)aiRotation.y, (GLfloat)aiRotation.z);

	glm::mat4 finalModel =
		parentTransform *
		glm::translate(glm::mat4(1.f), translation) *
		glm::mat4_cast(rotation) *
		glm::scale(glm::mat4(1.f), scaling);

	animations[currentAnim].boneTrans[boneID[node->name]] = finalModel * animations[currentAnim].boneOffset[node->name];

	for(std::size_t x = 0; x < node->children.size(); x++)
		updateBoneTree(timeInTicks, &node->children[x], finalModel);
}

core::Model::Model(){ modelLoaded = false; };

void core::Model::render(float dt, glm::mat4 &model, glm::mat4 &viewMat, glm::mat4 &projMat, glm::vec4 &clipPlane){
	if(!modelLoaded)
		return;
	
	glUseProgram(shader);
	for(std::size_t x = 0; x < meshes.size(); x++){
		glBindVertexArray(meshes[x].vao);
		
		glUniform3f(glGetUniformLocation(shader, "viewPos"), getPos().x, getPos().y, getPos().z);
		glUniform4fv(glGetUniformLocation(shader, "plane"), 1, glm::value_ptr(clipPlane));
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"),   1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shader, "viewMat"), 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "projMat"), 1, GL_FALSE, glm::value_ptr(projMat));
		glUniformMatrix4fv(glGetUniformLocation(shader, "boneTransformation"), animations[currentAnim].boneTrans.size(), GL_FALSE, (GLfloat*)&animations[currentAnim].boneTrans[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader, "modelTransformation"), 1, GL_FALSE, (GLfloat*)&modelTrans[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].vbo);
		glEnableVertexAttribArray(meshes[x].posAttrib);
		glVertexAttribPointer(meshes[x].posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if(meshes[x].normals.size() > 0){
			glBindBuffer(GL_ARRAY_BUFFER, meshes[x].nbo);
			glEnableVertexAttribArray(meshes[x].normAttrib);
			glVertexAttribPointer(meshes[x].normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].uvb);
		glEnableVertexAttribArray(meshes[x].texAttrib);
		glVertexAttribPointer(meshes[x].texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindTexture(GL_TEXTURE_2D, meshes[x].tex);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 meshes[x].image.getSize().x,
					 meshes[x].image.getSize().y,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 meshes[x].image.getPixelsPtr()
		);
		glUniform1i(glGetUniformLocation(shader, "tex"), 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glDrawElements(GL_TRIANGLES, meshes[x].indices.size(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
}

void core::Model::init(){
	if(!modelLoaded)
		return;

	shader = loadShaders("skinning.vert", "skinning.frag");

	for(std::size_t x = 0; x < meshes.size(); x++){
		glGenVertexArrays(1, &meshes[x].vao);
		glBindVertexArray(meshes[x].vao);

		glGenBuffers(1, &meshes[x].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshes[x].vertices.size(), &meshes[x].vertices[0], GL_STATIC_DRAW);

		if(meshes[x].normals.size() > 0){
			glGenBuffers(1, &meshes[x].nbo);
			glBindBuffer(GL_ARRAY_BUFFER, meshes[x].nbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshes[x].normals.size(), &meshes[x].normals[0], GL_STATIC_DRAW);
		}

		glGenBuffers(1, &meshes[x].ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[x].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshes[x].indices.size(), &meshes[x].indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].uvb);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].uvb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * meshes[x].uvs.size(), &meshes[x].uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].wbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].wbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * meshes[x].weights.size(), &meshes[x].weights[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].idbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].idbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * meshes[x].boneID.size(), &meshes[x].boneID[0], GL_STATIC_DRAW);

		glGenTextures(1, &meshes[x].tex);
		glBindTexture(GL_TEXTURE_2D, meshes[x].tex);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 meshes[x].image.getSize().x,
					 meshes[x].image.getSize().y,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 meshes[x].image.getPixelsPtr()
		);
		glUniform1i(glGetUniformLocation(shader, "tex"), 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].uvb);
		meshes[x].texAttrib = glGetAttribLocation(shader, "texCoords");
		glEnableVertexAttribArray(meshes[x].texAttrib);
		glVertexAttribPointer(meshes[x].texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].vbo);
		meshes[x].posAttrib = glGetAttribLocation(shader, "pos");
		glEnableVertexAttribArray(meshes[x].posAttrib);
		glVertexAttribPointer(meshes[x].posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		if(meshes[x].normals.size() > 0){
			glBindBuffer(GL_ARRAY_BUFFER, meshes[x].nbo);
			meshes[x].normAttrib = glGetAttribLocation(shader, "normal");
			glEnableVertexAttribArray(meshes[x].normAttrib);
			glVertexAttribPointer(meshes[x].normAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].wbo);
		meshes[x].weightAttrib = glGetAttribLocation(shader, "weight");
		glEnableVertexAttribArray(meshes[x].weightAttrib);
		glVertexAttribPointer(meshes[x].weightAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].idbo);
		meshes[x].boneAttrib = glGetAttribLocation(shader, "boneID");
		glEnableVertexAttribArray(meshes[x].boneAttrib);
		glVertexAttribPointer(meshes[x].boneAttrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
	}
}

void core::Model::setModelTrans(glm::mat4 &in){ modelTrans = in; };

glm::mat4 core::toMat4(aiMatrix4x4 *ai){
	glm::mat4 mat;

	mat[0][0] = ai->a1; mat[1][0] = ai->a2; mat[2][0] = ai->a3; mat[3][0] = ai->a4;
	mat[0][1] = ai->b1; mat[1][1] = ai->b2; mat[2][1] = ai->b3; mat[3][1] = ai->b4;
	mat[0][2] = ai->c1; mat[1][2] = ai->c2; mat[2][2] = ai->c3; mat[3][2] = ai->c4;
	mat[0][3] = ai->d1; mat[1][3] = ai->d2; mat[2][3] = ai->d3; mat[3][3] = ai->d4;

	return mat;
}
#pragma endregion animations