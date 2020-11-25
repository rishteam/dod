#version 430 core

in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Textures;

void main()
{
    vec4 texColor = texture(u_Textures, v_TexCoord);
    color = texColor;
}