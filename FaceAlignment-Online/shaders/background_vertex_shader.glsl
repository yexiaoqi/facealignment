#version 430 core

// in position & tex coordinate
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texture;

// out 
out vec2 texcoord;

void main(){
	texcoord = in_texture;
	gl_Position = vec4(in_position, 1.0);
}