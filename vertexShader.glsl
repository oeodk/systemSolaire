#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 texCoord;


uniform vec3 offset;
uniform float radius;
uniform mat4 rotation;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool invert_light;

void main()
{
	vec3 new_pos = offset + vec3(vec4(inPosition * radius,1) * rotation);
    FragPos = vec3(model * vec4(new_pos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
	texCoord = inTexCoord;
	if(invert_light)
	{
		Normal = - mat3(transpose(inverse(model))) * vec3(vec4(inPosition,1) * rotation);	
	}
	else
	{
		Normal = mat3(transpose(inverse(model))) * vec3(vec4(inPosition,1) * rotation);	
	}
}
