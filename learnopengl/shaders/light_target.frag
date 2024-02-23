#version 330 core
in vec3 normal;
in vec3 fragPos;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
	float ambient = 0.1;

	float diffuseStrength = 1.0;
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffuse = max(dot(norm, lightDir), 0.0) * diffuseStrength;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specular = pow(max(dot(viewDir, reflectDir), 0.0), 64) * specularStrength;

	FragColor = vec4((ambient + diffuse + specular) * lightColor * objectColor, 1.0);
}