#version 430 core

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_Tiling;

out vec4 color;

uniform sampler2D u_Textures[32];

void main()
{
    int index = int(v_TexIndex);
    vec4 texColor = texture(u_Textures[index], v_TexCoord * v_Tiling);
    color = texColor * v_Color;
}