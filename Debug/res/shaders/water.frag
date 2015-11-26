#version 330 core

struct Lamp {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
};

out vec4 color;

#define NUM_LIGHTS 1

uniform vec3 viewPos;
uniform Lamp light[NUM_LIGHTS];

void main(){
	// ambient lighting
	//	vec3 ambient = light[0].ambient * vec3(texture(material.diffuse, outTexCoords));

	// diffuse lighting
	//	vec3 norm     = normalize(outNormal);
	//	vec3 lightDir = normalize(light[0].position - outFragPos);
	//	float diff    = max(dot(norm, lightDir), 0.0);
	//	vec3 diffuse  = light[0].diffuse * diff * vec3(texture(material.diffuse, outTexCoords));

	// final result
		color = vec4(0.0, 0.0, 1.0, 1.0f);
}