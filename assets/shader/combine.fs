#version 430 core

in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Textures;
uniform sampler2D u_Textures2;

void main()
{
    vec4 worldColor = texture(u_Textures, v_TexCoord);
    vec4 lightColor = texture(u_Textures2, v_TexCoord);
    color = worldColor * lightColor;
}