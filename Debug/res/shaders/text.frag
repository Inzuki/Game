#version 330 core

uniform sampler2D tex;
in vec2 texCoords;
out vec4 fragColor;
uniform vec4 color;

void main(void) {
	fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;
}