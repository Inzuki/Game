#version 330 core

struct Material {
	sampler2D diffuse;
};

in vec3 toCamVec;
in vec3 outNormal;
in vec3 toLightVec;
in vec2 outTexCoords;

out vec4 color;

uniform Material material;
uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

void main(){
	vec3 unitNormal = normalize(outNormal);
	vec3 unitLightVec = normalize(toLightVec);
	
	float nDot1 = dot(unitNormal, unitLightVec);
	float brightness = max(nDot1, 0.2);
	vec3 diffuse = brightness * lightColor;
	
	vec3 unitVecToCam = normalize(toCamVec);
	vec3 lightDir = -unitLightVec;
	vec3 reflectedLightDir = reflect(lightDir, unitNormal);
	
	float specularFactor = dot(reflectedLightDir, unitVecToCam);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor, shineDamper);
	vec3 finalSpecular = dampedFactor * reflectivity * lightColor;

	color = vec4(diffuse, 1.0) * texture(material.diffuse, outTexCoords) + vec4(finalSpecular, 1.0);
}