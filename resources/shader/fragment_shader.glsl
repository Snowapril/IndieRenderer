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
vec4 lightSpaceCoords;
}fs_in;

out vec4 fragColor;

uniform Material material;
uniform DirLight dirLight;
uniform sampler2D depthMap;
uniform samplerCube depthCube;

uniform float far_plane;
uniform vec3 viewPos;
uniform float gamma;
uniform int shadowSamples;

vec3 gridSamplingDisk[20] = vec3[]
(
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

float calculateShadow(vec4 lightSpaceCoords, vec3 lightDirection, vec3 normal);
float calculatePointShadow(vec3 fragPos, vec3 lightPos);
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

float calculateShadow(vec4 lightSpaceCoords, vec3 lightDirection, vec3 normal)
{
	vec3 NDCcoords = lightSpaceCoords.xyz / lightSpaceCoords.w;
	NDCcoords = NDCcoords * 0.5 + 0.5;

	if (NDCcoords.z > 1.0) return 0.0;

	float currentDepth = NDCcoords.z;
	//float closestDepth = texture(depthMap, NDCcoords.xy).r;
	//
	const float bias = max(0.05 * (1.0 - dot(lightDirection, normal)), 0.05);
	//
	//float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for (int x = -3; x <= 3; ++x)
	{
		for (int y = -3; y <= 3; ++y)
		{
			float pcfDepth = texture(depthMap, NDCcoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 49;

	return shadow;
}

float calculatePointShadow(vec3 fragPos, vec3 lightPos)
{
	// get vector between fragment position and light position
	vec3 fragToLight = fragPos - lightPos;
	// use the fragment to light vector to sample from the depth map    
	// float closestDepth = texture(depthMap, fragToLight).r;
	// it is currently in linear range between [0,1], let's re-transform it back to original depth value
	// closestDepth *= far_plane;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// test for shadows
	// float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
	// float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
	// PCF
	// float shadow = 0.0;
	// float bias = 0.05; 
	// float samples = 4.0;
	// float offset = 0.1;
	// for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	// {
	// for(float y = -offset; y < offset; y += offset / (samples * 0.5))
	// {
	// for(float z = -offset; z < offset; z += offset / (samples * 0.5))
	// {
	// float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
	// closestDepth *= far_plane;   // Undo mapping [0;1]
	// if(currentDepth - bias > closestDepth)
	// shadow += 1.0;
	// }
	// }
	// }
	// shadow /= (samples * samples * samples);
	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(depthCube, fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= far_plane;   // undo mapping [0;1]
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

	// display closestDepth as debug (to visualize depth cubemap)
	// FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    

	return shadow;
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

	float shadow = calculateShadow(fs_in.lightSpaceCoords, direction, normal);
	if (shadow == 0.0) calculatePointShadow(fs_in.FragCoords, dirLight.position);

	return ambient + (1.0 - shadow) * (diffuse + specular);
}