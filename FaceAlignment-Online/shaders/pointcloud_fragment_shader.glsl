#version 330 core

uniform vec3 ptColor;

in vec3 Normal;
in vec3 FragPos;
in vec4 color;
in vec2 Tex;

layout(location = 0) out vec4 out_color;

void main()
{
    float diff = 1.0;
	//float diff = max(dot(Normal,vec3(1.0,1.0,-1.0)),0.0);
	out_color=vec4(ptColor*diff,1.0);
}

