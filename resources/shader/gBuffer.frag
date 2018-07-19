#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColorSpec;

in VS_OUT
{
	vec3 Normal;
	vec3 FragCoords;
	vec2 TexCoords;
}fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main(void)
{
	gPosition = fs_in.FragCoords;
	gNormal = normalize(fs_in.Normal);
	gColorSpec.xyz = texture(diffuseTexture, fs_in.TexCoords).rgb;
	gColorSpec.w = texture(specularTexture, fs_in.TexCoords).a;
}