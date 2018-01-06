#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexColor;


out vec3 Color;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	Color = VertexColor;

    gl_Position = MVP * vec4(VertexPosition,1.0);
}
