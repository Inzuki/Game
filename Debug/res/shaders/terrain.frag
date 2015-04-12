#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D two;
	sampler2D thr;
	sampler2D fou;
	sampler2D path;
};

struct Lamp {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
};

in vec3 outNormal;
in vec2 outTexCoords;
in vec3 outFragPos;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Lamp light;
uniform vec2 resolution;
uniform mat4 VP;

void main(){
	// multi-texturing
		const float fRange1 = 0.15f; 
		const float fRange2 = 0.3f; 
		const float fRange3 = 0.65f; 
		const float fRange4 = 0.85f; 

		float scale = outFragPos.y / 1.0;

		vec3 texColor = vec3(0.0f);
		
	// path (multi-texturing cont.)
		vec4 ndcPos;
		ndcPos.xy = (2.0 * gl_FragCoord.xy) / (resolution.xy) - 1;
		ndcPos.z  = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
		ndcPos.w  = 1.0f;

		vec4 clipPos = ndcPos / gl_FragCoord.w;

		vec4 temp = inverse(VP) * clipPos;

		vec2 POS = vec2(temp.x / 400, temp.z / 400);
		vec4 POScolor = texture2D(material.path, POS);

		float avg = (POScolor.x + POScolor.y + POScolor.z) / 3;

		if(avg < 0.9f){
			float test = 1.0 - avg;

			vec3 t1 = vec3(1.0);
			vec3 t2 = vec3(1.0);

			if(scale >= 0.0 && scale < fRange1)
				texColor += vec3(texture(material.thr, outTexCoords)) * avg;
			else if(scale <= fRange2){
				t1 = vec3(texture(material.thr, outTexCoords));
				t2 = vec3(texture(material.two, outTexCoords));

				texColor.x = ((t1.x + t2.x) / 2) * avg;
				texColor.y = ((t1.y + t2.y) / 2) * avg;
				texColor.z = ((t1.z + t2.z) / 2) * avg;
			}else if(scale <= fRange3)
				texColor += vec3(texture(material.two, outTexCoords)) * avg;
			else if(scale <= fRange4){
				t1 = vec3(texture(material.two, outTexCoords));
				t2 = vec3(texture(material.diffuse, outTexCoords));

				texColor.x = ((t1.x + t2.x)) / 2 * avg;
				texColor.y = ((t1.y + t2.y)) / 2 * avg;
				texColor.z = ((t1.z + t2.z)) / 2 * avg;
			}else
				texColor += vec3(texture(material.diffuse, outTexCoords)) * avg;
			
			texColor += vec3(texture(material.fou, outTexCoords)) * test;
		}else {
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
		}

	// ambient lighting
		vec3 ambient = light.ambient * texColor;

	// diffuse lighting
		vec3 norm     = normalize(outNormal);
		vec3 lightDir = normalize(light.position - outFragPos);
		float diff    = max(dot(norm, lightDir), 0.0);
		vec3 diffuse  = light.diffuse * diff * texColor;

	// final result
		color = vec4(diffuse + ambient, 1.0f);
}