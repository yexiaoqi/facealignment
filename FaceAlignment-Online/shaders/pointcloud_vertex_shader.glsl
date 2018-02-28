#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_tex;
layout (location = 3) in vec3 in_normal; 

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec4 color;
out vec2 Tex;

void main()
{
	gl_Position = projectionMatrix*modelViewMatrix*vec4(position,1.0f);
	FragPos = position;
	Normal=in_normal;
	color=in_color;
	Tex = in_tex;
}

