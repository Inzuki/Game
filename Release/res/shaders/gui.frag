#version 330 core

in vec2 textureCoords;

out vec4 color;

uniform sampler2D guiTexture;

void main(void){
	color = texture(guiTexture, textureCoords);
}