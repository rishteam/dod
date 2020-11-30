#version 450 core
out vec4 FragColor;

in vec2 v_LightPosition;
uniform vec4 v_Color;
uniform float v_Linear;
uniform float v_Quadratic;
uniform vec2 v_ViewPort;
uniform vec2 zoom;

void main()
{
    //
    vec3 ndc = vec3(((gl_FragCoord.x/v_ViewPort.x) * 2 -1), (gl_FragCoord.y/v_ViewPort.y) * 2 -1, gl_FragCoord.z * 2 -1) ;
    ndc = vec3(ndc.x * zoom.x, ndc.y * zoom.y, ndc.z);
    vec2 light = vec2(v_LightPosition.x * zoom.x, v_LightPosition.y * zoom.y);

    float distance = length(light - ndc.xy);

    float attenuation = 1.0 / (1 + v_Linear * distance + v_Quadratic * (distance * distance));

    vec4 color = vec4(attenuation, attenuation, attenuation, pow(attenuation, 3)) * v_Color;

//    vec4 color = {1, 1, 1, 1};

    FragColor = color;
}
