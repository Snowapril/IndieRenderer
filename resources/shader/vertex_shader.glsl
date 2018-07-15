#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(std140) uniform VP
{
	mat4 view;
mat4 projection;
};

out VS_OUT
{
	vec3 Normal;
vec3 FragCoords;
vec2 TexCoords;
vec4 lightSpaceCoords;

}vs_out;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main(void)
{
	vs_out.TexCoords = aTexCoords;
	vs_out.Normal = transpose(inverse(mat4(model))) * aNormal;
	vs_out.FragCoords = vec3(model * vec4(aPos, 1.0));
	vs_out.lightSpaceCoords = lightSpaceMatrix * vec4(vs_out.FragCoords, 1.0);

	gl_Position = projection * view * vec4(vs_out.FragCoords, 1.0);
}