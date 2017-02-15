#version 150

// layout (location = 2) in vec3 normal;

in vec3 pos;
in vec2 texCoords;
in vec4 weight;
in vec4 boneID;

uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 model;
uniform mat4 boneTransformation[32];
uniform mat4 modelTransformation;
uniform vec4 plane;

out vec2 outTexCoords;

void main(){
	gl_ClipDistance[0] = dot(model * vec4(pos, 1.0), plane);

	int b1 = int(boneID.x);
	int b2 = int(boneID.y);
	int b3 = int(boneID.z);
	int b4 = int(boneID.w);

	mat4 bTrans  = boneTransformation[b1] * weight.x;
		 bTrans += boneTransformation[b2] * weight.y;
		 bTrans += boneTransformation[b3] * weight.z;
		 bTrans += boneTransformation[b4] * weight.w;

	outTexCoords = texCoords;
	gl_Position  = viewMat * projMat * model * modelTransformation * bTrans * vec4(pos, 1.0f);
}