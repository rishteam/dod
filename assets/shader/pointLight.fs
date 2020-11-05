#version 450 core
out vec4 FragColor;

in vec2 v_LightPosition;
in vec4 v_Color;
in float v_Constant;
in float v_Linear;
in float v_Quadratic;

uniform float screenWidth;
uniform float screenHeight;

void main()
{
    //
    vec3 ndc = vec3(((gl_FragCoord.x/screenWidth) * 2 -1), (gl_FragCoord.y/screenHeight) * 2 -1, gl_FragCoord.z * 2 -1) ;

    float distance = length(v_LightPosition - ndc.xy) * 50;

    float attenuation = 1.0 / (v_Constant + v_Linear * distance + v_Quadratic * (distance * distance));

    vec4 color = vec4(attenuation, attenuation, attenuation, pow(attenuation, 3)) * v_Color;

    FragColor = color;

//    FragColor = vec4(v_LightPosition.x, 0, 0, 1);
}
