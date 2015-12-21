#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 model;

void main(){
	gl_Position = viewMat * projMat * model * vec4(pos, 1.0);
}