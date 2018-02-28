#version 430 core

// directional light
struct DirLight{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	float shininess;
	float v;
};

// in normal & fragment position & fragment color & fragment texture coordinate & fragment triangle index
in vec3 fg_normal;
in vec3 fg_pos;
in vec4 fg_color;
in vec2 fg_tex;
in vec2 fg_tri;

// in 
uniform DirLight dirLight;
uniform sampler2D texture1;
uniform float colorMode;

// out
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_computing;

// rendering
vec3 CalcDirLight(vec3 rgb,DirLight light,vec3 normal,vec3 viewDir);

void main()
{
	vec3 normal= normalize(fg_normal);
	vec3 viewDir=normalize(vec3(0,0,0)-fg_pos);
	vec3 mRGB;
	// default color & vertice color & texture color 
	if(colorMode == 0.f) mRGB = vec3(0.8,0.8,0.8);
	else if(colorMode == 1.f) mRGB = vec3(fg_color);
	else if(colorMode == 2.f) mRGB = vec3(texture(texture1,fg_tex));
    else mRGB = vec3(0.5,0.5,0.5);

	// rendering
	vec3 result=CalcDirLight(mRGB,dirLight,normal,viewDir);
	//out_color = vec4(mRGB,1.0);
	out_color=vec4(result,1.0);
	out_color=vec4(result,1.0);
	// copy triangle index
	out_computing = vec4(fg_tri.x, fg_tri.x, fg_tri.x, 1.0);
}

vec3 CalcDirLight(vec3 rgb,DirLight light,vec3 normal,vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal,lightDir),0.0);
	//specular shading
	vec3 reflectDir = reflect(-lightDir,normal);
	float spec=pow(max(dot(viewDir,reflectDir),0.0),light.v);
	//ambient & diffuse
	vec3 ambient = light.ambient*rgb;
	vec3 diffuse = light.diffuse*diff*rgb;
	vec3 specular = light.diffuse*light.shininess*spec;
	return (ambient+diffuse+specular);
}




