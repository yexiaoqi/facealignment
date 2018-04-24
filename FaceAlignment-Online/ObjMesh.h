#pragma once
#if 0
//comment yqy 180428
#include <vector>
#include <string>
#include <Eigen\Eigen>


struct ObjMesh 
{
	ObjMesh() 
	{
		request_position_ = true;
		request_color_ = false;
		request_normal_ = false;
		//request_tex_coord_ = true;
		request_tex_coord_ = true;
		request_face_normal_ = false;
		request_tri_list_ = true;
		save_color_ = false;
		save_normal_ = false;
		save_tex_coord_ = false;
	}
	ObjMesh(const std::string& filename) { this->load_obj(filename); }
	void set_vertex(int i, const Eigen::Vector3f& v) { this->position_.row(i) = v; }
	Eigen::Vector3f vertex(int i) const { return this->position_.row(i); }
	Eigen::Vector3i face(int i) const { return tri_list_.row(i); }
	Eigen::Vector3f vertex_normal(int i) const { return this->normal_.row(i); }
	Eigen::Vector3f face_normal(int i) const { return this->face_normal_.row(i); }
	void update_vertices(const Eigen::VectorXd &vertices);

	~ObjMesh() {}

	// io functions
	
	//void load_obj(std::string filename);//comment  yqy180424
	void write_obj(std::string filename);
	void print_summary();
	void update_normal();
	void normalize_model();
	void center_model();

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
	// io prefix
	bool request_position_;
	bool request_color_;
	bool request_normal_;
	bool request_tex_coord_;
	bool request_face_normal_;
	bool request_tri_list_;
	bool save_color_;
	bool save_normal_;
	bool save_tex_coord_;

	

};

typedef ObjMesh Mesh;
//commnet end 180428
#endif



//add yqy190428
#include<GL\glew.h>
//#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>       
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Eigen\Eigen>
#include "shader.h"

/* vertex format */
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

// 表示一个用于渲染的最小实体
class Mesh
{
public:
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
	void update_normal();

	void draw(const Shader& shader) const// 绘制Mesh
	{
		if (VAOId == 0
			|| VBOId == 0
			|| EBOId == 0)
		{
			return;
		}
		shader.use();
		glBindVertexArray(this->VAOId);
		int diffuseCnt = 0, specularCnt = 0, texUnitCnt = 0;
		for (std::vector<Texture>::const_iterator it = this->textures.begin();
			this->textures.end() != it; ++it)
		{
			switch (it->type)
			{
			case aiTextureType_DIFFUSE:
			{
				glActiveTexture(GL_TEXTURE0 + texUnitCnt);
				glBindTexture(GL_TEXTURE_2D, it->id);
				std::stringstream samplerNameStr;
				samplerNameStr << "texture_diffuse" << diffuseCnt++;
				glUniform1i(glGetUniformLocation(shader.programId,
					samplerNameStr.str().c_str()), texUnitCnt++);
			}
			break;
			case aiTextureType_SPECULAR:
			{
				glActiveTexture(GL_TEXTURE0 + texUnitCnt);
				glBindTexture(GL_TEXTURE_2D, it->id);
				std::stringstream samplerNameStr;
				samplerNameStr << "texture_specular" << specularCnt++;
				glUniform1i(glGetUniformLocation(shader.programId,
					samplerNameStr.str().c_str()), texUnitCnt++);
			}
			break;
			default:
				std::cerr << "Warning::Mesh::draw, texture type" << it->type
					<< " current not supported." << std::endl;
				break;
			}
		}

		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	Mesh() :VAOId(0), VBOId(0), EBOId(0) {}
	Mesh(const std::vector<Vertex>& vertData,
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices) :VAOId(0), VBOId(0), EBOId(0) // 构造一个Mesh
	{
		setData(vertData, textures, indices);
	}
	void setData(const std::vector<Vertex>& vertData,
		const std::vector<Texture> & textures,
		const std::vector<GLuint>& indices)
	{
		this->vertData = vertData;
		this->indices = indices;
		this->textures = textures;
		if (!vertData.empty() && !indices.empty())
		{
			this->setupMesh();
		}
	}
	void final() const
	{
		glDeleteVertexArrays(1, &this->VAOId);
		glDeleteBuffers(1, &this->VBOId);
		glDeleteBuffers(1, &this->EBOId);
	}
	~Mesh()
	{
		// 不要再这里释放VBO等空间 因为Mesh对象传递时 临时对象销毁后这里会清理VBO等空间
	}
private:
	std::vector<Vertex> vertData;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
	GLuint VAOId, VBOId, EBOId;

	void setupMesh()  // 建立VAO,VBO等缓冲区
	{
		glGenVertexArrays(1, &this->VAOId);
		glGenBuffers(1, &this->VBOId);
		glGenBuffers(1, &this->EBOId);

		glBindVertexArray(this->VAOId);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertData.size(),
			&this->vertData[0], GL_STATIC_DRAW);
		// 顶点位置属性
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		// 顶点纹理坐标
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		// 顶点法向量属性
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		// 索引数据
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* this->indices.size(),
			&this->indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};
typedef Mesh ObjMesh;
//add end yqy180428