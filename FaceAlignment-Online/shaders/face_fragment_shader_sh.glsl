#version 430 core

// in normal & fragment position & fragment color & fragment texture coordinate & fragment triangle index
in vec3 fg_normal;
in vec3 fg_pos;
in vec4 fg_color;
in vec2 fg_tex;
in vec2 fg_tri;

// in SH parameters
uniform double SHparas[27];
uniform double SH[9];
uniform sampler2D texture1;
uniform float useTexmapping;
uniform float drawMesh;

// out 
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_computing;

// rendering
vec3 CalcSHLight(vec3 rgb, vec3 normal);

void main()
{
	vec3 normal=normalize(Normal);
	vec3 viewDir=normalize(vec3(0,0,0)-FragPos);
	vec3 mRGB;
    // default color & vertice color & texture color 
	if(colorMode == 0.f) mRGB = vec3(0.8,0.8,0.8);
	else if(colorMode == 1.f) mRGB = vec3(color);
	else if(colorMode == 2.f) mRGB = vec3(texture(texture1,Tex));
    else mRGB = vec3(0.5,0.5,0.5);
	vec3 result = CalcSHLight(mRGB, normal); 
	out_color=vec4(result,1.0);
	out_computing = vec4(fg_tri.x, fg_tri.x, fg_tri.x, 1.0);
}

vec3 CalcSHLight(vec3 i_rgb, vec3 normal)
{
	double pi_ = 3.1415926;
	vec3 o_rgb = vec3(SH[0],SH[0],SH[0])*vec3(SHparas[0], SHparas[1], SHparas[2]);
	o_rgb = o_rgb + vec3(SH[1]*normal.z, SH[1]*normal.z, SH[1]*normal.z)*vec3(SHparas[3], SHparas[4], SHparas[5]);
	o_rgb = o_rgb + vec3(SH[2]*normal.y, SH[2]*normal.y, SH[2]*normal.y)*vec3(SHparas[6], SHparas[7], SHparas[8]);
	o_rgb = o_rgb + vec3(SH[3]*normal.x, SH[3]*normal.x, SH[3]*normal.x)*vec3(SHparas[9], SHparas[10], SHparas[11]);
	o_rgb = o_rgb + vec3(SH[4]*(3*normal.z*normal.z-1), SH[4]*(3*normal.z*normal.z-1), SH[4]*(3*normal.z*normal.z-1))*vec3(SHparas[12], SHparas[13], SHparas[14]);
	o_rgb = o_rgb + vec3(SH[5]*normal.y*normal.z, SH[5]*normal.y*normal.z, SH[5]*normal.y*normal.z)*vec3(SHparas[15], SHparas[16], SHparas[17]);
	o_rgb = o_rgb + vec3(SH[6]*normal.x*normal.z, SH[6]*normal.x*normal.z, SH[6]*normal.x*normal.z)*vec3(SHparas[18], SHparas[19], SHparas[20]);
	o_rgb = o_rgb + vec3(SH[7]*normal.x*normal.y, SH[7]*normal.x*normal.y, SH[7]*normal.x*normal.y)*vec3(SHparas[21], SHparas[22], SHparas[23]);
	o_rgb = o_rgb + vec3(SH[8]*(normal.x*normal.x-normal.y*normal.y), SH[8]*(normal.x*normal.x-normal.y*normal.y), SH[8]*(normal.x*normal.x-normal.y*normal.y))*vec3(SHparas[24], SHparas[25], SHparas[26]);
	o_rgb = o_rgb*i_rgb;
	return o_rgb;
}