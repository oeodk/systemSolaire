#version 330 core

out vec4 FragColor;

in vec3 FragPos;

void main()
{
    vec3 color = vec3(cos(FragPos.x),1,cos(FragPos.z));
	FragColor = vec4(color,1.f);
}
