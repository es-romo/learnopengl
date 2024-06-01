#version 330 core
struct Material {
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct Light {
	// Poistion if w = 1; direction if w = 0
	vec4 vector;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

in vec3 normal;
in vec3 fragPos;
in vec2 TexCoords;
  
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
	vec3 diffuseTex = texture(material.diffuse, TexCoords).rgb;
	vec3 specularTex = texture(material.specular, TexCoords).rgb;

	if (light.vector.w == 0.0f) { 
		vec3 lightDir = normalize(-light.vector.xyz);
		
		//ambient
		vec3 ambient = light.ambient * diffuseTex;

		// diffuse
		vec3 norm = normalize(normal);
		float diff = max(dot(norm, lightDir), 0.0f);
		vec3 diffuse = light.diffuse * diff * diffuseTex;

		// specular
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		vec3 specular = light.specular * spec * specularTex;

		// emission
		//vec3 emission = texture(material.emission, TexCoords).rgb;

		FragColor = vec4(ambient + diffuse + specular, 1.0f);
	} else if(light.vector.w == 1.0f){
		vec3 lightDir = normalize(light.vector.xyz - fragPos);

		// attenuation
		float lightDistance = length(light.vector.xyz - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * lightDistance + light.quadratic * lightDistance * lightDistance);

		// ambient
		vec3 ambient = light.ambient * diffuseTex;
		
		// diffuse
		vec3 norm = normalize(normal);
		float diff = max(dot(norm, lightDir), 0.0f);
		vec3 diffuse = light.diffuse * diff * diffuseTex;


		// specular
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		vec3 specular = light.specular * spec * specularTex;

		//vec3 emission = texture(material.emission, TexCoords).rgb;
		
		FragColor = vec4((ambient + diffuse + specular) * attenuation, 1.0f);
	}
}