#version 450 core
out vec4 FragColor;

in vec2 v_LightPosition;
in vec4 v_Color;
in float v_Constant;
in float v_Linear;
in float v_Quadratic;

void main()
{
    float distance = length(v_LightPosition - gl_FragCoord.xy);

    float attenuation = 1.0 / (v_Constant + v_Linear * distance + v_Quadratic * (distance * distance));

    vec4 color = vec4(attenuation, attenuation, attenuation, pow(attenuation, 3)) * v_Color;

    FragColor = color;
}
