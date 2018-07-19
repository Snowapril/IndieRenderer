#version 430 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColorSpec;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};
const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];

uniform vec3 viewPos;
//uniform float exposure;

void main(void)
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
	vec3 Diffuse = texture(gColorSpec, TexCoords).rgb;
	//Diffuse = vec3(1.0) - exp(-Diffuse * exposure);

	float Specular = texture(gColorSpec, TexCoords).a;

	vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
        // specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;        
    }
	
    fragColor = vec4(lighting, 1.0);
}