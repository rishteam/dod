#version 430 core

in vec4 v_Color;
in vec2 v_LineCenter;

uniform float u_LineWidth;
uniform float u_BlendFactor; //1.5..2.5

layout(location = 0) out vec4 f_Color;

void main(void)
{
    vec4 col = v_Color;        
    double distance = length(v_LineCenter-gl_FragCoord.xy);
    double width = u_LineWidth;
    if (distance>width)
        col.w = 0;
    else
        col.w *= pow(float((width-distance)/width), u_BlendFactor);
    f_Color = col;
}