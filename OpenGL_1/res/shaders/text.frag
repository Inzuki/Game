#version 330 core

varying vec2 texCoord;

uniform sampler2D tex;
uniform vec4 color;

void main(void){
	gl_FragColor = vec4(1.f, 1.f, 1.f, texture2D(tex, texCoord).a) * color;
}