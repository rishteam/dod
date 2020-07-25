#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(a_Position, 1.0);
}