#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoords;

uniform mat4 VP;
uniform mat4 model;
uniform vec4 plane;

void main(){
	gl_ClipDistance[0] = dot(model * vec4(pos, 1.0), plane);
	gl_Position  = VP * model * vec4(pos, 1.0f);
	outFragPos   = vec3(model * vec4(pos, 1.0f));
	outNormal    = mat3(transpose(inverse(model))) * normal;
	outTexCoords = texCoords;
}