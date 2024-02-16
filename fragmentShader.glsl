#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

in vec2 texCoord;  // Coordonnées de texture du fragment


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cam_pos;

uniform sampler2D textureSampler;


uniform vec3 color;

uniform samplerCube shadowCubeMap;


uniform Material material;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(cam_pos- FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularStrength * spec * material.specular;

	float shadow = 0.0f;
	vec3 fragToLight = FragPos - lightPosition;
	float currentDepth = length(fragToLight);
	float bias = max(0.5f * (1.0f - dot(norm, lightDir)), 0.05f); 
	
	// Not really a radius, more like half the width of a square
	int sampleRadius = 2;
	float offset = 0.02f;
	float farPlane = 100.f;
	for(int z = -sampleRadius; z <= sampleRadius; z++)
	{
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowCubeMap, fragToLight + vec3(x, y, z) * offset).r;
				// Remember that we divided by the farPlane?
				// Also notice how the currentDepth is not in the range [0, 1]
				closestDepth *= farPlane;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
	}
	// Average shadow
	shadow /= pow((sampleRadius * 2 + 1), 3.25);
	
	vec4 textureColor = texture(textureSampler, texCoord);

	
    vec3 result = (ambient + diffuse * (1.f - shadow) + specular * (1.f - shadow)) * lightColor * color;
    //vec3 result = (ambient + diffuse + specular) * lightColor * color;
    FragColor = vec4(result, 1.0) * textureColor;
}
