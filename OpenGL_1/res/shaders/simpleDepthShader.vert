#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 lightSpaceMat;
uniform mat4 model;

void main(){
    gl_Position = lightSpaceMat * model * vec4(pos, 1.f);
} 