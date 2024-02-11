#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 cam_pos;

uniform vec3 color;


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

    vec3 result = (ambient + diffuse + specular) * lightColor * color;
    FragColor = vec4(result, 1.0);
}
