#pragma once

#include "ObjMesh.h"

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <opencv2\opencv.hpp>

/* vertex format */
struct VertexFormat
{
	glm::vec3 position;
	glm::vec4 texColor;
	glm::vec2 texCoordinate;
	glm::vec3 normal;
	glm::vec2 tri;

	VertexFormat(const glm::vec3 &iPos, const glm::vec4 &iColor)
	{
		position = iPos;
		texColor = iColor;
		normal.x = normal.y = normal.z = 0;
		texCoordinate.x = texCoordinate.y = 0;
	}

	VertexFormat(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec2 &itri)
	{
		position = iPos;
		texColor = iColor;
		tri = itri;
		normal.x = normal.y = normal.z = 0;
		texCoordinate.x = texCoordinate.y = 0;
	}

	VertexFormat(const glm::vec3 &iPos, const glm::vec2 &iTex)
	{
		position = iPos;
		texCoordinate = iTex;
		texColor.r = texColor.g = texColor.b = 0.0;
		normal.x = normal.y = normal.z = 0;
	}

	VertexFormat(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec3 &iNormal)
	{
		position = iPos;
		texColor = iColor;
		normal = iNormal;
		texCoordinate.x = texCoordinate.y = 0;
	}

	VertexFormat(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec3 &iNormal, const glm::vec2 &itri)
	{
		position = iPos;
		texColor = iColor;
		normal = iNormal;
		tri = itri;
		texCoordinate.x = texCoordinate.y = 0;
	}

	VertexFormat(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec2 &iTex, const glm::vec3 &iNormal)
	{
		position = iPos;
		texColor = iColor;
		texCoordinate = iTex;
		normal = iNormal;
	}

};

/* lighting */
struct DirLight 
{
	DirLight() 
	{
		direction[0] = 0.f;
		direction[1] = 0.5f;
		direction[2] = 0.866025f;
		for (int kk = 0; kk < 3; kk++) ambient[kk] = 0.4f;
		for (int kk = 0; kk < 3; kk++) diffuse[kk] = 0.5f;
		shininess = 0.06f;
		v = 40.f;
		max_shininess = 1.0f;
		max_v = 5.0;
	}
	float direction[3];
	float ambient[3];
	float diffuse[3];
	float shininess;
	float v;
	float max_shininess;
	float max_v;
};

/* set directional light */
void SetDirLight(GLuint shader_program, DirLight& dl_tmp);

/* shader ×ÅÉ«Æ÷*/
std::string ReadShader(const char *filename);
GLuint CreateShader(GLenum shaderType, std::string source, char* shaderName);
GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);

/* texture */
unsigned int LoadTexture(const std::string& filename, unsigned int width, unsigned int height);
unsigned int CreateTexture(unsigned int width, unsigned int height);
void UpdateTexture(unsigned int texId, cv::Mat& image);

/* buffer model */
class BufModel 
{
public:

	BufModel();
	~BufModel();

	void CreateBGplaneModel();
	void CreateDispModel(Mesh& model, bool isStatic);
	void UpdateDispModel(Mesh& model);

	/* shader */
	void LoadShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) 
	{
		shader_program_ = CreateProgram(vertexShaderFilename, fragmentShaderFilename);
	}
	/* texture */
	void LoadTextureFile(const std::string& filename, unsigned int width, unsigned int height) { this->tex_ = LoadTexture(filename, width, height);}
	void BufCreateTexture(unsigned int width, unsigned int height){ this->tex_ = CreateTexture(width, height); }
	void BufUpdateTexture(cv::Mat& image){ UpdateTexture(this->tex_, image); }


	/* render function */
	void Draw();

	/* pose parameters */
	void Reset() 
	{ 
		vao_ = 0; 
		shader_program_ = 0; 
		vbos_.clear(); 
		tex_ = 0; 
		model_ = view_ = proj_ = glm::mat4(1.0f); 
		nTri_ = 0; 
		isBg_ = false; 
		isDraw_ = false;
		colorMode_ = 0;
	}
	/* data in buffer model */

	unsigned int vao_; // vertex array object
	unsigned int shader_program_; // shader program
	std::vector<unsigned int> vbos_; // vertex buffer objects
	GLuint tex_; // texture
	glm::mat4 model_, view_, proj_; // model & view & projection
	DirLight dirLight_; // dir light
	
	/* controller parameters */
	unsigned int nTri_;
	bool isBg_;
	bool isDraw_;
	int colorMode_;
};

/* solid sphere - buffer model */
class SolidSphere :public BufModel 
{

public:
	// load the template sphere model
	SolidSphere() { exception_idx_ = -1; }
	~SolidSphere() {}
	void Init() 
	{
		//jisy edit 
		template_sphere_.request_tex_coord_ = false;
		template_sphere_.load_obj("data/sphere.obj"); this->CreateDispModel(template_sphere_, false); this->nTri_ = template_sphere_.n_tri_;
		this->nPoint_ = 0;
		this->drawSphere_ = false;
		this->scale_ = 1.0;
	}
	void SetData(Eigen::MatrixXf pos);
	void SetColor(float r, float g, float b);
	void SetScale(float scale);
	void Draw();
	bool drawSphere_;
	int exception_idx_;

private:
	// mesh util for sphere model
	Mesh template_sphere_;
	Eigen::MatrixXf center_positions_;
	float r_, g_, b_;
	float scale_;
	int nPoint_;

};



