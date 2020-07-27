#version 330 core
// input
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCord;
// output
out vec3 v_Position;
out vec2 v_TexCord;
// uniforms
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    // Pass to Frag
    v_Position = a_Position;
    v_TexCord = a_TexCord;
}