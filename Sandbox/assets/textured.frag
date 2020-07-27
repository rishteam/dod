#version 330 core
// input
in vec3 v_Position;
in vec2 v_TexCord;
// output
out vec4 color;
// uniforms
uniform sampler2D u_Texture;

void main()
{
    color = texture(u_Texture, v_TexCord);
//    color = vec4(v_TexCord, 0.0, 1.0);
}
