#version 330 core

in vec2 position;

out vec2 textureCoords;
out vec3 fragColor;

uniform mat4 trans;
uniform vec3 spriteColor;

void main(void){
	gl_Position = trans * vec4(position, 0.0, 1.0);
	
	//textureCoords = vec2((position.x+1.0)/2.0, 1 - (position.y+1.0)/2.0);
	textureCoords = vec2(gl_Position.x, gl_Position.y);
	
	fragColor = spriteColor;
}