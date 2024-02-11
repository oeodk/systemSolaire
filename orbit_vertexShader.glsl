#version 330 core

layout(location = 0) in vec3 inPosition;

out vec3 FragPos;
out vec3 new_center[9];

uniform vec2 center[9];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(inPosition, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
	for(int i = 0;i<9;i++)
	{
		new_center[i] = vec3(model * vec4(center[i],0.f,0.f));
	}
}
