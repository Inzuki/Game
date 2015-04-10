#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D two;
	sampler2D thr;
};

struct Lamp {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 outNormal;
in vec2 outTexCoords;
in vec3 outFragPos;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Lamp light;

void main(){
	const float fRange1 = 0.15f; 
    const float fRange2 = 0.3f; 
    const float fRange3 = 0.65f; 
    const float fRange4 = 0.85f; 

	float scale = outFragPos.y / 1.5f;

	vec3 texColor = vec3(0.0f);

	if(scale >= 0.0 && scale < fRange1)
		texColor = vec3(texture(material.thr, outTexCoords));
	else if(scale <= fRange2){
		scale -= fRange1;
		scale /= (fRange2 - fRange1);

		float scale2 = scale;
		scale = 1.0f - scale;

		texColor += vec3(texture(material.thr, outTexCoords)) * scale;
		texColor += vec3(texture(material.two, outTexCoords)) * scale2;
	}else if(scale <= fRange3)
		texColor = vec3(texture(material.two, outTexCoords));
	else if(scale <= fRange4){
		scale -= fRange3;
		scale /= (fRange4 - fRange3);

		float scale2 = scale;
		scale = 1.0f - scale;

		texColor += vec3(texture(material.two, outTexCoords)) * scale;
		texColor += vec3(texture(material.diffuse, outTexCoords)) * scale2;
	}else
		texColor = vec3(texture(material.diffuse, outTexCoords));

	// ambient lighting
		// vec3 ambient = light.ambient * vec3(texture(material.two, outTexCoords));
		vec3 ambient = light.ambient * texColor;

	// diffuse lighting
		vec3 norm     = normalize(outNormal);
		vec3 lightDir = normalize(light.position - outFragPos);
		float diff    = max(dot(norm, lightDir), 0.0);
		// vec3 diffuse  = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoords));
		vec3 diffuse  = light.diffuse * diff * texColor;

	// final result
		color = vec4(diffuse + ambient, 1.0f);
}