#version 430 core

in VS_OUT
{
	vec3 Normal;
	vec3 FragCoords;
	vec2 TexCoords;
}fs_in;

out vec4 fragColor;
uniform vec4 color;

void main(void)
{
	fragColor = color;
}