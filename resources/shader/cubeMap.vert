#version 430 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform VP 
{
	mat4 view;
	mat4 projection;
};

out vec3 fragPos;

void main(void)
{
    fragPos = aPos;

    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = projection * rotView * vec4(fragPos, 1.0);

    gl_Position = clipPos.xyww;
}