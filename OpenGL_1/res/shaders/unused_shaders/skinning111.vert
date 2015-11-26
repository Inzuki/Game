#version 330 core

layout (location = 0) in vec3  pos;
layout (location = 1) in vec2  texCoords;
layout (location = 2) in vec3  normal;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4  weights;

in vec3 Position;
in vec2 Texcoord;
in vec4 Weight;
in vec4 BoneID;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoords;

uniform mat4 VP;
uniform mat4 M;
uniform mat4 boneTrans[64];
uniform mat4 mdlTrans;

void main(){
	int b1 = int(BoneID.x);
	int b2 = int(BoneID.y);
	int b3 = int(BoneID.z);
	int b4 = int(BoneID.w);

	mat4 bTrans  = boneTrans[b1] * Weight.x;
	if(b2 != -1)
		 bTrans += boneTrans[b2] * Weight.y;
	if(b3 != -1)
		 bTrans += boneTrans[b3] * Weight.z;
	if(b4 != -1)
		 bTrans += boneTrans[b4] * Weight.w;

	/*
	mat4 bTrans  = boneTrans[boneIDs.x] * weights.x;
		 bTrans += boneTrans[boneIDs.y] * weights.y;
		 bTrans += boneTrans[boneIDs.z] * weights.z;
		 bTrans += boneTrans[boneIDs.w] * weights.w;
	*/
	
	gl_Position = mdlTrans * bTrans * VP * M * vec4(Position, 1.f);

	outNormal    = mat3(transpose(inverse(M))) * normal;
	outFragPos   = vec3(M * vec4(Position, 1.0f));
	outTexCoords = vec2(texCoords.x, 1.0 - texCoords.y);
}