#version 330 core

in vec2 textureCoords;
in vec3 fragColor;

out vec4 out_Color;

uniform sampler2D guiTexture;

void main(void){

	//out_Color = vec4(fragColor, 1.0) * texture(guiTexture, textureCoords);
	out_Color = texture(guiTexture, textureCoords);
	//out_Color = vec4(fragColor, 1.0);
	//out_Color = vec4(textureCoords.x, textureCoords.y, 0.5, 0.5);
	
	//vec3 tex = vec3(texture(guiTexture, textureCoords));
	//out_Color = vec4(tex, 1.0);
	
}