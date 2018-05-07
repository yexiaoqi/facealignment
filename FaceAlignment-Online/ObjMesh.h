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
	void update_vertices(const Eigen::VectorXd &vertices);//没用到

	~ObjMesh() {}

	// io functions
	
	//void load_obj(std::string filename);//comment  yqy180424
	void write_obj(std::string filename);//没用到
	void print_summary();//没用到
	void update_normal();
	void normalize_model();//没用到
	void center_model();//没用到

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



// 表示一个用于渲染的最小实体
class Mesh
{
public:
	void Mesh::update_normal(BufModel &model);////add yqy180425
	//void update_normal();//comment yqy180425
	//add yqy180424	
	bool processNode(const aiNode* node, const aiScene* sceneObjPtr);
	bool processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr, BufModel& meshObj);
	bool processMaterial(const aiMaterial* matPtr, const aiScene* sceneObjPtr,
		const aiTextureType textureType, std::vector<Texture>& textures);
	bool load_obj(const std::string& filePath);
	void draw(const Shader& shader) const;
	//add end yqy180424
	BufModel _bufmodel;
	std::vector<BufModel> bufmodels; // 保存Mesh
	//add yqy180504
	/*int n_verts_;
	int n_tri_;*/
	//add end yqy180504
private:
	
	
	std::string modelFileDir; // 保存模型文件的文件夹路径
	typedef std::map<std::string, Texture> LoadedTextMapType; // key = texture file path
	LoadedTextMapType loadedTextureMap; // 保存已经加载的纹理
};
typedef Mesh ObjMesh;
//add end yqy180428