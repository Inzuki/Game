#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 outNormal;
out vec3 outFragPos;
out vec2 outTexCoords;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 model;
uniform vec4 plane;

void main(){
	gl_ClipDistance[0] = dot(model * vec4(pos, 1.0), plane);
	gl_Position		   = viewMat * projMat * model * vec4(pos, 1.0);
	outNormal    	   = mat3(transpose(inverse(model))) * normal;
	outFragPos   	   = vec3(model * vec4(pos, 1.0));
	outTexCoords 	   = vec2(texCoords.x, 1.0 - texCoords.y);
}