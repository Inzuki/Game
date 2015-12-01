#version 330 core

in mat4 VP;
in vec3 toCamVec;
in vec3 outNormal;
in vec3 toLightVec;
in vec2 outTexCoords;
in vec3 outFragPos;

out vec4 color;

uniform sampler2D bgTex;
uniform sampler2D rTex;
uniform sampler2D gTex;
uniform sampler2D bTex;
uniform sampler2D blendMap;
uniform vec2 resolution;
uniform vec3 lightColor;
uniform float shineDamper;
uniform float reflectivity;

void main(){
	vec4 blendMapColor = texture(blendMap, outTexCoords);
	
	float backTexAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = outTexCoords;
	vec4 bgTexColor = texture(bgTex, tiledCoords) * backTexAmount;
	vec4 rTexColor  = texture(rTex, tiledCoords) * blendMapColor.r;
	vec4 gTexColor  = texture(gTex, tiledCoords) * blendMapColor.g;
	vec4 bTexColor  = texture(bTex, tiledCoords) * blendMapColor.b;
	
	vec4 totalColor = bgTexColor + rTexColor + gTexColor + bTexColor;

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
	
	color = vec4(diffuse, 1.0) * totalColor + vec4(finalSpecular, 1.0);
}