#version 150

in vec2 outTexCoords;

out vec4 color;

uniform sampler2D tex;

void main(){
	color = texture(tex, outTexCoords);
}