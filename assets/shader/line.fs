#version 430 core

in vec4 v_Color;

layout(location = 0) out vec4 f_Color;

void main(void)
{    
    f_Color = v_Color;
}