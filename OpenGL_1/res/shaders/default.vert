#version 330 core

#define MAX_LIGHTS 3

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 toCamVec;
out vec3 outNormal;
out vec3 toLightVec[MAX_LIGHTS];
out vec2 outTexCoords;
out float visibility;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 model;
uniform vec4 plane;
uniform vec3 lightPos[MAX_LIGHTS];

const float density  = 0.007;
const float gradient = 1.5;

void main(){
	vec4 worldPos = model * vec4(pos, 1.0);
	vec4 posRelativeToCam = worldPos * viewMat;
	
	gl_ClipDistance[0] = dot(worldPos, plane);
	gl_Position	  = viewMat * projMat * worldPos;
	
	outTexCoords  = vec2(texCoords.x, 1.0 - texCoords.y);
	outNormal     = (model * vec4(normal, 0.0)).xyz;
	toCamVec      = (inverse(viewMat) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;
	
	for(int i = 0; i < MAX_LIGHTS; i++){
		toLightVec[i] = lightPos[i] - worldPos.xyz;
	}
	
	float distance = length(posRelativeToCam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}