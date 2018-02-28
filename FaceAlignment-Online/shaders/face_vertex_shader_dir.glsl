#version 430 core

// in
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_tex;
layout (location = 3) in vec3 in_normal; 
layout (location = 4) in vec2 in_tri;

// in
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

// out
out vec3 fg_normal;
out vec3 fg_pos;
out vec4 fg_color;
out vec2 fg_tex;
out vec2 fg_tri;

void main(){
	gl_Position = projectionMatrix*modelViewMatrix*vec4(in_position,1.0f);
	fg_pos = in_position;
	fg_normal=in_normal;
	fg_color=in_color;
	fg_tex = in_tex;
	fg_tri = in_tri;
}



