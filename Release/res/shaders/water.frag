#version 400 core

in vec4 clipSpace;
in vec2 texCoords;
in vec3 toCamVec;
in vec3 fromLightVec;

out vec4 color;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform vec3 sunColor;

uniform float moveFactor;

const float waveStrength = 0.03;
const float shineDamper  = 20.0;
const float reflectivity = 0.50;

void main(){
	vec2 ndc = ((clipSpace.xy / clipSpace.w) / 2.0) + 0.5;
	vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);
	vec2 refractTexCoords = vec2(ndc.x,  ndc.y);
	
	// to find near and far, check projection in the Camera file
	float near = 0.1;
	float far  = 1000.0;
	float depth = texture(depthMap, refractTexCoords).r;
	float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	
	depth = gl_FragCoord.z;
	float waterDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float waterDepth = floorDistance - waterDistance;
	
	// distort the water
	vec2 distortedTexCoords = texture(dudvMap, vec2(texCoords.x + moveFactor, texCoords.y)).rg * 0.1;
	distortedTexCoords = texCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 20.0, 0.0, 1.0);
	
	reflectTexCoords += totalDistortion;
	reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
	reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, -0.001);
	refractTexCoords += totalDistortion;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
	
	vec4 reflectColor = texture(reflectionTex, reflectTexCoords);
	vec4 refractColor = texture(refractionTex, refractTexCoords);
	
	// add normals to the water
	vec4 normalColor = texture(normalMap, distortedTexCoords);
	vec3 normal = vec3(normalColor.r * 2.0 - 1.0, normalColor.b * 2.0, normalColor.g * 2.0 - 1.0);
	normal = normalize(normal);
	
	// Fresnel effect
	vec3 viewVec = normalize(toCamVec);
	float refractFactor = dot(viewVec, normal);
	// change intensity of Fresnel effect
	refractFactor = pow(refractFactor, 0.5);
	refractFactor = clamp(refractFactor, 0.0, 1.0);
	
	vec3 reflectedLight = reflect(normalize(fromLightVec), normal);
	float specular = max(dot(reflectedLight, viewVec), 0.0);
	specular = pow(specular, shineDamper);
	// change sunColor to vec3(1.0, 1.0, 1.0) for white highlights always
	vec3 specularHighlights = sunColor * specular * reflectivity * clamp(waterDepth / 5.0, 0.0, 1.0);

	// apply texture to quad
	color = mix(reflectColor, refractColor, refractFactor);
	// add a blue tint to make it feel more lively
	color = mix(color, vec4(0.3, 0.4, 0.9, 1.0), 0.1) + vec4(specularHighlights, 0.0);
	color.a = clamp(waterDepth / 5.0, 0.0, 1.0);
}