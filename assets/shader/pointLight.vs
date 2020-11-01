#version 450 core
layout (location = 0) in vec3 a_QuadPosition;
layout (location = 1) in vec3 a_LightPosition;
layout (location = 2) in vec4 a_Color;
layout (location = 3) in float a_Constant;
layout (location = 4) in float a_Linear;
layout (location = 5) in float a_Quadratic;

out vec2 v_LightPosition;
out vec4 v_Color;
out float v_Constant;
out float v_Linear;
out float v_Quadratic;

uniform mat4 u_ViewProjection;

void main()
{
    gl_Position = u_ViewProjection * vec4(a_QuadPosition, 1.0);
    v_LightPosition = a_LightPosition.xy;
    v_Color = a_Color;
    v_Constant = a_Constant;
    v_Linear = a_Linear;
    v_Quadratic = a_Quadratic;
}