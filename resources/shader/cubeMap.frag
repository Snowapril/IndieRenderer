#version 430 core

out vec4 fragColor;
in vec3 fragPos;

uniform samplerCube envMap;
uniform float exposure;
uniform float gamma;
uniform bool useReinhard;

void main(void)
{
    vec3 envColor = texture(envMap, fragPos).rgb;

    //Reinhard tone mapping
	if (useReinhard)
		envColor = envColor / (envColor + vec3(1.0));
	else
		envColor = vec3(1.0) - exp(-envColor * exposure);

	//gamma correction
	envColor = pow(envColor, vec3(1.0/gamma));
 
	fragColor = vec4(envColor , 1.0);
}