#pragma once

#include <vector>
#include <map>

#include <Eigen\Eigen>
#include <GL\glew.h>
#include <GL\freeglut.h>

#include "ObjMesh.h"
//#include "VertexFormat.h"
#include "glm/glm.hpp"
#include"BufModel.h"
struct Model{
	unsigned int vao;
	std::vector<unsigned int> vbos;
	Model(){};
};

class GenModels{

public:
	GenModels();
	~GenModels();

	void CreateDynamicFaceModel_MRT(const std::string& gameModelName, Mesh& ref_model, bool isFrontFace, bool useMeshTex, Eigen::MatrixXf& face_mask);
	void UpdateVBO_MRT(const std::string& gameModelName, Mesh& ref_model, bool isFrontFace, bool useMeshTex, Eigen::MatrixXf& face_mask);
	void CreateBGplaneModel(const std::string& gameModelName);
	void DeleteModel(const std::string& gameModelName);
	unsigned int GetModel(const std::string& gameModelName);
	int nTriFront;

private:
	std::map<std::string, Model> GameModelList;

};

