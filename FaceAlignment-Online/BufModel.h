#pragma once

//#include "ObjMesh.h"
#include "texture.h"
#include <vector>
#include <Eigen\Eigen>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv2\opencv.hpp>
#include "shader.h"

#if 0
//comment yqy180425
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
//comment end yqyq180425
#endif



//// 表示一个顶点属性
//struct Vertex
//{
//	glm::vec3 position;
//	glm::vec2 texCoords;
//	glm::vec3 normal;
//};

// 表示一个Texture
struct Texture
{
	GLuint id;
	aiTextureType type;
	std::string path;
};
#if 1
/* vertex format */
//add yqy180425
struct Vertex
{
	//position,texCoords,normal声明的顺序不能颠倒，是前三个
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
	glm::vec3 face_normal;
	glm::vec4 texColor;
	glm::vec2 tri;
	Vertex()
	{
	}
	Vertex(const glm::vec3 &iPos, const glm::vec4 &iColor)
	{
		position = iPos;
		texColor = iColor;
		normal.x = normal.y = normal.z = 0;
		texCoords.x = texCoords.y = 0;
	}

	Vertex(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec2 &itri)
	{
		position = iPos;
		texColor = iColor;
		tri = itri;
		normal.x = normal.y = normal.z = 0;
		texCoords.x = texCoords.y = 0;
	}

	Vertex(const glm::vec3 &iPos, const glm::vec2 &iTex)
	{
		position = iPos;
		texCoords = iTex;
		texColor.r = texColor.g = texColor.b = 0.0;
		normal.x = normal.y = normal.z = 0;
	}

	Vertex(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec3 &iNormal)
	{
		position = iPos;
		texColor = iColor;
		normal = iNormal;
		texCoords.x = texCoords.y = 0;
	}

	Vertex(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec3 &iNormal, const glm::vec2 &itri)
	{
		position = iPos;
		texColor = iColor;
		normal = iNormal;
		tri = itri;
		texCoords.x = texCoords.y = 0;
	}

	Vertex(const glm::vec3 &iPos, const glm::vec4 &iColor, const glm::vec2 &iTex, const glm::vec3 &iNormal)
	{
		position = iPos;
		texColor = iColor;
		texCoords = iTex;
		normal = iNormal;
	}

};
//add end yqy180425
#endif

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

/* shader 着色器*/
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
	//add yqy180425
	BufModel() :VAOId(0), VBOId(0), EBOId(0) { Reset(); }
	BufModel(const std::vector<Vertex>& vertData,
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices) :VAOId(0), VBOId(0), EBOId(0) // 构造一个Mesh
	{
		Reset();
		setData(vertData, textures, indices);
	}

	// vertices - position & color & normal & tex coordinate
	Eigen::MatrixXf position_;
	Eigen::MatrixXf color_;
	Eigen::MatrixXf normal_;
	Eigen::MatrixXf tex_coord_;
	// triangle - face normal & triangle list
	Eigen::MatrixXf face_normal_;
	Eigen::MatrixXi tri_list_;
	// number of vertices & triangles
	int n_verts_;
	int n_tri_;
	bool request_tex_coord_ = true;
	


	void setData(const std::vector<Vertex>& vertData,
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices)
	{
		this->vertData = vertData;
		this->indices = indices;
		this->textures = textures;
		if (!vertData.empty() && !indices.empty())
		{
			//this->CreateDispModel();
			//this->setupMesh();//comment yqy180426
		}
	}
	void final() const
	{
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
		glDeleteBuffers(1, &this->EBOId);
	}
	~BufModel()
	{

		//delete VAO and VBOs (if many)
		unsigned int p = this->vao_;
		glDeleteVertexArrays(1, &p);
		glDeleteBuffers(this->vbos_.size(), &this->vbos_[0]);
		this->vbos_.clear();
		// 不要再这里释放VBO等空间 因为Mesh对象传递时 临时对象销毁后这里会清理VBO等空间
	}
	//add endyqy180425

	//commnet yqy180425
	//BufModel();
	//~BufModel();
	//commnet end yqy180425

#if 1
	//commnet yqy180424
	void CreateBGplaneModel();
	//void CreateDispModel(bool isStatic);
	//void CreateDispModel(Mesh& model, bool isStatic);//commnet yqy180424
	void BufModel::UpdateDispModel();
	//void UpdateDispModel(Mesh& model);//commnet yqy180425
	//commnet end yqy180424
#endif
	/* shader */
	void LoadShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) 
	{
		shader_program_ = CreateProgram(vertexShaderFilename, fragmentShaderFilename);
	}
	/* texture */
	void LoadTextureFile(const std::string& filename, unsigned int width, unsigned int height) { this->tex_ = LoadTexture(filename, width, height);}//没用到
	void BufCreateTexture(unsigned int width, unsigned int height){ this->tex_ = CreateTexture(width, height); }//创建输入侧的背景
	void BufUpdateTexture(cv::Mat& image){ UpdateTexture(this->tex_, image); }//更新输入侧的背景



//add yqy180425
	void draw(const Shader& shader) const;// 绘制Mesh
	//add end yqy180425



	/* render function */
	void Draw();

	/* pose parameters */
	void Reset() 
	{ 
		VAOId = 0;//add yqy180425
		//vao_ = 0; //comment yqy180425
		shader_program_ = 0; 
		VBOId = 0;//add yqy180425
		//vbos_.clear(); //comment yqy180425
		tex_ = 0; 
		model_ = view_ = proj_ = glm::mat4(1.0f); 
		nTri_ = 0; 
		isBg_ = false; 
		isDraw_ = false;
		colorMode_ = 0;
	}
	/* data in buffer model */

	unsigned int vao_; // vertex array object //comment yqy180425
	unsigned int shader_program_; // shader program
	std::vector<unsigned int> vbos_; // vertex buffer objects//comment yqy180425
	GLuint tex_; // texture
	glm::mat4 model_, view_, proj_; // model & view & projection
	DirLight dirLight_; // dir light
	
	/* controller parameters */
	unsigned int nTri_;
	bool isBg_;
	bool isDraw_;
	int colorMode_;

	//add yqy180425
	std::vector<Vertex> vertData;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	GLuint VAOId, VBOId, EBOId;
	void CreateDispModel();//assimp里的setupmesh
	//add end yqy180425

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
		request_tex_coord_ = false;//add yqyq180425
		//template_sphere_.request_tex_coord_ = false;//comment yqy180424
		//template_sphere_.load_obj("data/sphere.obj"); //comment yqy180424
		this->CreateDispModel();//add yqyq180425
		//this->CreateDispModel(false);//add yqyq180425
		//this->CreateDispModel(template_sphere_, false); //comment yqy180425
		this->nTri_ = n_tri_;//add yqyq180425
		//this->nTri_ = template_sphere_.n_tri_;//comment yqy180424
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

	//Mesh template_sphere_;//comment yqy180424
	Eigen::MatrixXf center_positions_;
	float r_, g_, b_;
	float scale_;
	int nPoint_;

};



