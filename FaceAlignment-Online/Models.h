#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Eigen\Eigen>

#include "ObjMesh.h"
#include"BufModel.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Blendshape class */
class Blendshape 
{
public:
	void LoadBs(std::string bs_folder, int nBs);
	//void LoadBsBin(std::string bs_file_bin, int nBs);
	//void UpdateMesh(std::vector<double> coef, BufModel& recons);//add yqy180511
	void UpdateMesh(std::vector<double> coef, Mesh& recons);//comment yqy180511
	void ProjectLandmark(std::vector<int> index3D);

	int nBs_;
	std::vector<std::vector<Eigen::Vector3d>> landmarks_model_;
	std::vector<Mesh> blendshapes_;
	//std::vector<BufModel> bufmodels_;//add yqy180424
};
