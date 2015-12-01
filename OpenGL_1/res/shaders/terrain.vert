#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 toCamVec;
out vec3 outNormal;
out vec3 toLightVec;
out vec2 outTexCoords;
out vec3 outFragPos;
out mat4 VP;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 model;
uniform vec4 plane;
uniform vec3 lightPos;

void main(){
	vec4 worldPos = model * vec4(pos, 1.0f);
	VP = viewMat * projMat;

	gl_ClipDistance[0] = dot(worldPos, plane);
	gl_Position   = VP * worldPos;
	
	outFragPos    = vec3(worldPos);
	outTexCoords  = texCoords;
	outNormal     = (model * vec4(normal, 0.0)).xyz;
	toLightVec    = lightPos - worldPos.xyz;
	toCamVec      = (inverse(viewMat) * vec4(0.0, 0.0, 0.0, 1.0)).xyz - worldPos.xyz;
}