#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

uniform vec3 offset;
uniform float radius;
uniform mat4 rotation;

void main()
{
	vec3 new_pos = offset + vec3(vec4(aPos * radius,1) * rotation);

    gl_Position = model * vec4(new_pos, 1.0);
}