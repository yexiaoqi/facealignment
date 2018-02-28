#version 430 core

// out color
layout(location = 0) out vec4 out_color;
// texture & texture coordinate
uniform sampler2D texture1;
in vec2 texcoord;

void main(){
	vec4 color = texture(texture1, texcoord);
	out_color = color;
	//out_color = vec4(1.0, 0.0, 0.0, 1.0);
}