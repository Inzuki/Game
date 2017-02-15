#version 330 core

out vec4 color;
in vec2 TexCoords;

uniform sampler2D depthMap;

uniform float near_plane;
uniform float far_plane;

float linearizeDepth(float depth){
	float z = depth * 2.f - 1.f;
	return (2.f * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main(){
	float depthValue = texture(depthMap, TexCoords).r;
	//color = vec4(vec3(depthValue), 1.f);
	color = vec4(vec3(linearizeDepth(depthValue) / far_plane), 1.f);
}