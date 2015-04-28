#version 330 core

attribute vec4 coord;

varying vec2 texCoord;

void main(void){
	gl_Position = vec4(coord.xy, 0.f, 1.f);
	texCoord    = coord.zw;
}