#version 330 core

#define MAX_LIGHTS 2

struct Material {
	sampler2D diffuse;
};

in vec3 toCamVec;
in vec3 outNormal;
in vec3 toLightVec[MAX_LIGHTS];
in vec2 outTexCoords;
in vec4 FragPosLightSpace;
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

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir){
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5f + 0.5f;
	
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	float bias = max(0.05f * (1.f - dot(normalize(outNormal), lightDir)), 0.001f);
	float shadow = 0.f;
	
	vec2 texelSize = 1.f / textureSize(shadowMap, 0);
	
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.f;
	
	if(projCoords.z > 1.f)
		shadow = 0.f;
	
	return shadow;
}

void main(){
	/*
	vec3 unitVecToCam = normalize(toCamVec);
	vec3 totalDiffuse = vec3(0.0);
	vec3 totalSpecular = vec3(0.0);
	
	for(int i = 0; i < MAX_LIGHTS; i++){
		float distance = length(toLightVec[i]);
		float attFactor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		vec3 unitLightVec = normalize(toLightVec[i]);
		float nDot1 = dot(normal, unitLightVec);
		float brightness = max(nDot1, 0.0);
		vec3 lightDir = -unitLightVec;
		vec3 reflectedLightDir = reflect(lightDir, normal);
		float specularFactor = dot(reflectedLightDir, unitVecToCam);
		specularFactor = max(specularFactor, 0.0);
		float dampedFactor = pow(specularFactor, shineDamper);
		totalDiffuse = totalDiffuse + (brightness * lightColor[i]) / attFactor;
		totalSpecular = totalSpecular + (dampedFactor * reflectivity * lightColor[i]) / attFactor;
	}
	totalDiffuse = max(totalDiffuse, 0.3);
	*/
	
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
	
	float shadow = ShadowCalculation(FragPosLightSpace, lightDir);
	vec3 lighting = (ambient + (1.f - shadow) * (diffuse + specular)) * color;
	
	outColor = vec4(lighting, 1.f);
}