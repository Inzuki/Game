#version 150

// layout (location = 2) in vec3 normal;

in vec3 pos;
in vec2 texCoords;
in vec4 weight;
in vec4 boneID;

uniform mat4 VP;
uniform mat4 M;
uniform mat4 boneTransformation[32];
uniform mat4 modelTransformation;

out vec2 outTexCoords;

void main(){
	int b1 = int(boneID.x);
	int b2 = int(boneID.y);
	int b3 = int(boneID.z);
	int b4 = int(boneID.w);

	mat4 bTrans  = boneTransformation[b1] * weight.x;
		 bTrans += boneTransformation[b2] * weight.y;
		 bTrans += boneTransformation[b3] * weight.z;
		 bTrans += boneTransformation[b4] * weight.w;

	outTexCoords = texCoords;
	gl_Position  = VP * M * modelTransformation * bTrans * vec4(pos, 1.0f);
}