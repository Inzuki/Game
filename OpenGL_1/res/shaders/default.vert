#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 toCamVec;
out vec3 outNormal;
out vec3 toLightVec;
out vec2 outTexCoords;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 model;
uniform vec4 plane;
uniform vec3 lightPos;

void main(){
	vec4 worldPos = model * vec4(pos, 1.0);
	
	gl_ClipDistance[0] = dot(worldPos, plane);
	gl_Position	  = viewMat * projMat * worldPos;
	
	outTexCoords  = vec2(texCoords.x, 1.0 - texCoords.y);
	outNormal     = (model * vec4(normal, 0.0)).xyz;
	toLightVec    = lightPos - worldPos.xyz;
	toCamVec      = (inverse(viewMat) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;
}