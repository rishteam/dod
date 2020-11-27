#version 450 core
out vec4 FragColor;

in vec2 v_LightPosition;
in vec4 v_Color;
in float v_Constant;
in float v_Linear;
in float v_Quadratic;
in mat4 v_ViewProjection;

uniform float screenWidth;
uniform float screenHeight;
uniform vec2 zoom;

void main()
{
    //
    vec3 ndc = vec3(((gl_FragCoord.x/screenWidth) * 2 -1), (gl_FragCoord.y/screenHeight) * 2 -1, gl_FragCoord.z * 2 -1) ;
    ndc = vec3(ndc.x * zoom.x, ndc.y * zoom.y, ndc.z);
    vec2 light = vec2(v_LightPosition.x * zoom.x, v_LightPosition.y * zoom.y);

    float distance = length(light - ndc.xy) * 25;

    float attenuation = 1.0 / (v_Constant + v_Linear * distance + v_Quadratic * (distance * distance));

    vec4 color = vec4(attenuation, attenuation, attenuation, pow(attenuation, 1)) * v_Color;

//    vec4 color = {1, 1, 1, 1};

    FragColor = color;
}
