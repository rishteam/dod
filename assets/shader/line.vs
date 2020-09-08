#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ViewProjection;
uniform vec2 u_ViewPort;      // Width and Height of the viewport

out vec4 v_Color;
out vec2 v_LineCenter;

void main(void)
{
    vec4 pp = u_ViewProjection * vec4(a_Position, 1.0);
    gl_Position = pp;
    //
    vec2 vp = u_ViewPort;
    v_LineCenter = 0.5*(pp.xy + vec2(1, 1))*vp;
    v_Color = a_Color;
};
