#version 330 core

#define MAX_LIGHTS 3

struct Material {
	sampler2D diffuse;
};

in vec3 toCamVec;
in vec3 outNormal;
in vec3 toLightVec[MAX_LIGHTS];
in vec2 outTexCoords;
in float visibility;

out vec4 color;

uniform Material material;
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 attenuation[MAX_LIGHTS];
uniform float shineDamper;
uniform float reflectivity;
uniform vec3 skyColor;

void main(){
	vec3 unitNormal = normalize(outNormal);
	vec3 unitVecToCam = normalize(toCamVec);
	
	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	
	for(int i = 0; i < MAX_LIGHTS; i++){
		float distance = length(toLightVec[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		vec3 unitLightVec = normalize(toLightVec[i]);
		float nDot1 = dot(unitNormal, unitLightVec);
		float brightness = max(nDot1, 0.0);
		vec3 lightDir = -unitLightVec;
		vec3 reflectedLightDir = reflect(lightDir, unitNormal);
		float specularFactor = dot(reflectedLightDir, unitVecToCam);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, shineDamper);
		totalDiffuse = totalDiffuse + (brightness * lightColor[i]) / attFactor;
		totalSpecular = totalSpecular + (dampedFactor * reflectivity * lightColor[i]) / attFactor;
	}
	totalDiffuse = max(totalDiffuse, 0.3);

	color = vec4(totalDiffuse, 1.0) * texture(material.diffuse, outTexCoords) + vec4(totalSpecular, 1.0);
	//color = mix(vec4(skyColor, 1.0), color, visibility);
}