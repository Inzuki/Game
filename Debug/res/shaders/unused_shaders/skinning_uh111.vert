#version 330 core

layout (location = 0) in vec3  pos;
layout (location = 1) in vec2  texCoords;
layout (location = 2) in vec3  normal;
layout (location = 3) in ivec4 boneIDs;
layout (location = 4) in vec4  weights;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoords;

uniform mat4 VP;
uniform mat4 M;
uniform mat4 gBones[64];

void main(){
	mat4 bTrans  = gBones[boneIDs[0]] * weights[0];
		 bTrans += gBones[boneIDs[1]] * weights[1];
		 bTrans += gBones[boneIDs[2]] * weights[2];
		 bTrans += gBones[boneIDs[3]] * weights[3];

	vec4 posL    = bTrans * vec4(pos, 1.0f);
	gl_Position  = VP * M * posL;

	vec4 normL   = bTrans * vec4(normal, 0.0f);
	outNormal    = mat3(transpose(inverse(M))) * normL.xyz;

	outFragPos   = vec3(M * vec4(pos, 1.0f));
	outTexCoords = texCoords;
	//outTexCoords = vec2(texCoords.x, 1.0 - texCoords.y);
}