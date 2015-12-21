#version 330 core

#define MAX_LIGHTS 3

in mat4 VP;
in vec3 toCamVec;
in vec3 outNormal;
in vec3 toLightVec[MAX_LIGHTS];
in vec2 outTexCoords;
in vec3 outFragPos;
in float visibility;

out vec4 color;

uniform sampler2D bgTex;
uniform sampler2D rTex;
uniform sampler2D gTex;
uniform sampler2D bTex;
uniform sampler2D blendMap;

uniform vec2 resolution;
uniform vec3 lightColor[MAX_LIGHTS];
uniform vec3 attenuation[MAX_LIGHTS];
uniform vec3 skyColor;
uniform float shineDamper;
uniform float reflectivity;

void main(){
	vec4 blendMapColor = texture(blendMap, outTexCoords);
	
	float backTexAmount = 1.0 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = outTexCoords * 40.0;
	vec4 bgTexColor = texture(bgTex, tiledCoords) * backTexAmount;
	vec4 rTexColor  = texture(rTex,  tiledCoords) * blendMapColor.r;
	vec4 gTexColor  = texture(gTex,  tiledCoords) * blendMapColor.g;
	vec4 bTexColor  = texture(bTex,  tiledCoords) * blendMapColor.b;
	
	vec4 totalColor = bgTexColor + rTexColor + gTexColor + bTexColor;

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
	
	color = vec4(totalDiffuse, 1.0) * totalColor + vec4(totalSpecular, 1.0);
	//color = mix(vec4(skyColor, 1.0), color, visibility);
}