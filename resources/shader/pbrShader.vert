#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform VP 
{
	mat4 view;
	mat4 projection;
};

out VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} vs_out;

uniform mat4 model;

void main(void)
{
	vs_out.TexCoords = aTexCoords;
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.Normal = mat3(model) * aNormal;

	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}