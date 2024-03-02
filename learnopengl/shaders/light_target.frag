#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct Light {
	vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 fragPos;
in vec2 TexCoords;
  
uniform Material material;
uniform Light light;

out vec4 FragColor;

void main()
{
	vec3 diffuseTex = texture(material.diffuse, TexCoords).rgb;
	vec3 specularTex = texture(material.specular, TexCoords).rgb;

	vec3 ambient = light.ambient * diffuseTex;

	vec3 lightDir = normalize(light.position - fragPos);

	vec3 norm = normalize(normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseTex;

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * specularTex;

	vec3 emission = texture(material.emission, TexCoords).rgb;

	FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}