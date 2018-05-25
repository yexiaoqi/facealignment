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
	void update_vertices(const Eigen::VectorXd &vertices);//û�õ�

	~ObjMesh() {}

	// io functions
	
	//void load_obj(std::string filename);//comment  yqy180424
	void write_obj(std::string filename);//û�õ�
	void print_summary();//û�õ�
	void update_normal();
	void normalize_model();//û�õ�
	void center_model();//û�õ�

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
#include <map>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Eigen\Eigen>
#include "shader.h"
#include "BufModel.h"
#include "texture.h"



// ��ʾһ��������Ⱦ����Сʵ��
class Mesh
{
public:



	//add yqy180525
	int mNumMeshes;
	// vertices - position & color & normal & tex coordinate
	Eigen::MatrixXf mesh_position_;
	Eigen::MatrixXf mesh_color_;
	Eigen::MatrixXf mesh_normal_;
	Eigen::MatrixXf mesh_tex_coord_;
	// triangle - face normal & triangle list
	Eigen::MatrixXf mesh_face_normal_;
	Eigen::MatrixXi mesh_tri_list_;
	int mesh_n_verts_;
	int mesh_n_tri_;
	//add end180525


	//void Mesh::update_normal(BufModel &model);////add yqy180425
	void update_normal();//comment yqy180425
	//add yqy180424	
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr);
	
	bool load_obj(const std::string& filePath);
	void draw(const Shader& shader) const;

	void CreateDispModel()
	{
		for (std::vector<BufModel>::iterator it = this->bufmodels.begin(); this->bufmodels.end() != it; ++it)
		{
			it->CreateDispModelbuf();//add yqy180513
								  //it->setupMesh();//comment yqy180513
		}
	}

	//add end yqy180424
	//BufModel _bufmodel;//�ص�Ϊ��recons���� //comment yqy180524
	std::vector<BufModel> bufmodels; // ����Mesh

	

	//add yqy180504
	/*int n_verts_;
	int n_tri_;*/
	//add end yqy180504
private:
	
	
	std::string modelFileDir; // ����ģ���ļ����ļ���·��
	
};
typedef Mesh ObjMesh;
//add end yqy180428