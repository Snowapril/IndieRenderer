#version 420 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
};

struct DirLight
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in VS_OUT
{
	vec3 Normal;
	vec3 FragCoords;
	vec2 TexCoords;
}fs_in;

out vec4 fragColor;

uniform Material material;
uniform DirLight dirLight;

uniform vec3 viewPos;
uniform float gamma;

vec3 calculateDirLight(DirLight dirLight, vec3 normal, vec3 viewDir);

void main(void)
{
	vec3 viewDir = normalize(viewPos - fs_in.FragCoords);
	vec3 normal = normalize(fs_in.Normal);

	float max_distance = 1.5;
	float distance = length(fs_in.FragCoords - viewPos);
	float attenuation = 1.0 / ((distance > max_distance) ? max_distance * max_distance : distance * distance);

	vec3 result = calculateDirLight(dirLight, normal, viewDir) * vec3(attenuation);

	fragColor = vec4(pow(result, vec3(1.0 / gamma)), 1.0);
}

vec3 calculateDirLight(DirLight dirLight, vec3 normal, vec3 viewDir)
{
	vec3 direction = normalize(dirLight.position - fs_in.FragCoords);
	vec3 halfwayDir = normalize(viewDir + direction);

	float diff = max(dot(direction, normal), 0.0);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);

	vec3 ambient = dirLight.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 diffuse = diff * dirLight.diffuse * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 specular = spec * dirLight.specular * vec3(texture(material.specular, fs_in.TexCoords));


	return ambient + diffuse + specular;
}