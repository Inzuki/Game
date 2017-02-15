#version 330 core

#define MAX_LIGHTS 2

struct Material {
	sampler2D diffuse;
};

in vec3 toCamVec;
in vec3 outNormal;
in vec3 toLightVec[MAX_LIGHTS];
in vec2 outTexCoords;
in vec3 FragPos;

out vec4 outColor;

uniform sampler2D shadowMap;
uniform Material material;
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 attenuation[MAX_LIGHTS];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColor;

// shadow stuff
uniform vec3 lightPos_shade;
uniform vec3 viewPos;

void main(){
	vec3 color = texture(material.diffuse, outTexCoords).rgb;
	vec3 normal = normalize(outNormal);
	vec3 lightColor = vec3(0.9f);
	
	vec3 ambient = 0.3f * color;
	
	vec3 lightDir = normalize(lightPos_shade - FragPos);
	float diff = max(dot(lightDir, normal), 0.f);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - FragPos);
	float spec = 0.f;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.f), 64.f);
	vec3 specular = spec * lightColor;
	
	vec3 lighting = (ambient + (1.f) * (diffuse + specular)) * color;
	
	outColor = vec4(lighting, 1.f);
}