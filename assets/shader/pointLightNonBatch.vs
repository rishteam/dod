#version 450 core
layout (location = 0) in vec3 a_QuadPosition;

out vec2 v_LightPosition;
uniform mat4 u_ViewProjection;
uniform vec3 a_LightPosition;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_QuadPosition, 1.0);
    v_LightPosition = (u_ViewProjection * vec4(a_LightPosition, 1.0)).xy;
}