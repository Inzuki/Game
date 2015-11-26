#version 330 core

struct Material {
	sampler2D diffuse;
};

in vec3 outNormal;
in vec2 outTexCoords;
in vec3 outFragPos;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform vec3 lightPos;

void main(){
	// ambient lighting
		vec3 ambient = vec3(0.2, 0.2, 0.2) * vec3(texture(material.diffuse, outTexCoords));

	// diffuse lighting
		vec3 norm     = normalize(outNormal);
		vec3 lightDir = normalize(lightPos - outFragPos);
		float diff    = max(dot(norm, lightDir), 0.0);
		vec3 diffuse  = vec3(0.5, 0.5, 0.5) * diff * vec3(texture(material.diffuse, outTexCoords));

	// final result
		color = vec4(ambient + diffuse, 1.0f);
}