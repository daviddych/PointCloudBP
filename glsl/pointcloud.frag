#version 410

in vec3 Color;
uniform float circle_flag;
layout( location = 0 ) out vec4 FragColor;

void main() {

    vec2 coord = gl_PointCoord - vec2(0.5);  //from [0,1] to [-0.5,0.5]
    if(circle_flag > 0.5f && length(coord) > 0.5)   //outside of circle radius?
       discard;

    FragColor = vec4(Color, 1.0);
}