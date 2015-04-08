#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoords;

uniform mat4 VP;
uniform mat4 M;

void main(){
	gl_Position  = VP * M * vec4(pos, 1.0f);
	outNormal    = mat3(transpose(inverse(M))) * normal;
	outFragPos   = vec3(M * vec4(pos, 1.0f));
	//outTexCoords = texCoords;
	outTexCoords = vec2(texCoords.x, 1.0 - texCoords.y);
}