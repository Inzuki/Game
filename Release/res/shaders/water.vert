#version 400 core

layout (location = 0) in vec2 pos;

out vec4 clipSpace;
out vec2 texCoords;
out vec3 toCamVec;
out vec3 fromLightVec;

uniform mat4 VP;
uniform mat4 M;
uniform vec3 camPos;

uniform vec3 lightPos;

const float tiling = 4.0;

void main(){
	vec4 worldPos = M * vec4(pos.x, 0.0, pos.y, 1.0);
	clipSpace = VP * worldPos;
	gl_Position = clipSpace;
	texCoords = vec2(pos.x / 2.0 + 0.5, pos.y / 2.0 + 0.5) * tiling;
	toCamVec = camPos - worldPos.xyz;
	fromLightVec = worldPos.xyz - lightPos;
}