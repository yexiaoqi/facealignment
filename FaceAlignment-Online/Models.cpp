#include "Models.h"

///////////////////////////////////////// Blendshape Model ///////////////////////////////////////////////
//�������д�faceshift�õ���mesh.obj
//����blendshape��bs��n��mesh���
void Blendshape::LoadBs(std::string bs_folder, int nBs) 
{
	nBs_ = nBs;
	Mesh obj_mesh;
	std::string bs_file;
	char filename[512];
	//1. load neutral first
	sprintf_s(filename, "%s/neutral.obj", bs_folder.c_str());
	obj_mesh.load_obj(filename);
	//obj_mesh.center_model();//jisy center model 170109
	//obj_mesh.normalize_model();
	obj_mesh.update_normal();
	blendshapes_.push_back(obj_mesh);
	//2. load blendshapes
	for (int kk = 0; kk < nBs; kk++) 
	{
		sprintf_s(filename, "mesh_%d.obj", kk);
		bs_file = bs_folder + "/" + std::string(filename);
		//std::cout << "Loading blendshape " << bs_file << std::endl;
		obj_mesh.load_obj(bs_file);
		//obj_mesh.center_model();//jisy center model 170109
		obj_mesh.update_normal();
		blendshapes_.push_back(obj_mesh);
	}
	std::cout << "Finished Loading blendshape models " << std::endl;
}

//����bs�����Ƕ����ƻ���bs
void Blendshape::LoadBsBin(std::string bs_file_bin, int nBs) 
{
	nBs_ = nBs;
	int nVert_in = 0;
	int nVert_x3_in = 0;
	int nBs_in = 0;
	int nFaces = 0;
	Mesh obj_mesh;
	std::cout << "Reading blendshape file " << bs_file_bin << std::endl;
	std::fstream fin;
	fin.open(bs_file_bin, std::ios::in | std::ios::binary);
	fin.read(reinterpret_cast<char*>(&(nVert_x3_in)), sizeof(int));
	fin.read(reinterpret_cast<char*>(&(nBs_in)), sizeof(int));
	fin.read(reinterpret_cast<char*>(&(nFaces)), sizeof(int));
	std::cout << "#Vertex  = " << nVert_x3_in / 3 << " #Blendshape = " << nBs_in - 1 << " #Faces = " << nFaces << std::endl;
	// prepare obj meshs
	nVert_in = nVert_x3_in / 3;
	obj_mesh.n_verts_ = nVert_in;
	obj_mesh.n_tri_ = nFaces;
	obj_mesh.position_ = Eigen::MatrixXf(nVert_in, 3);	obj_mesh.position_.setZero();
	obj_mesh.color_ = Eigen::MatrixXf(nVert_in, 3);	obj_mesh.color_.setOnes(); /*this->color_ = this->color_ * 255.f;*/
	obj_mesh.normal_ = Eigen::MatrixXf(nVert_in, 3); obj_mesh.normal_.setZero();
	obj_mesh.tex_coord_ = Eigen::MatrixXf(nVert_in, 2);	obj_mesh.tex_coord_.setZero();
	obj_mesh.tri_list_ = Eigen::MatrixXi(nFaces, 3); obj_mesh.tri_list_.setZero();
	obj_mesh.face_normal_ = Eigen::MatrixXf(nFaces, 3); obj_mesh.face_normal_.setZero();
	// read triangle list
	fin.read(reinterpret_cast<char*>(obj_mesh.tri_list_.data()), sizeof(int)*((int)nFaces) * 3);
	// read vertices
	for (int kk = 0; kk < nBs_in; kk++) 
	{
		fin.read(reinterpret_cast<char*>(obj_mesh.position_.data()), sizeof(float)*((int)nVert_x3_in));
		obj_mesh.update_normal();
		blendshapes_.push_back(obj_mesh);
	}
	fin.close();
	std::cout << "done." << std::endl;
}

//ͨ������һ��ϵ��������blendshape
void Blendshape::UpdateMesh(std::vector<double> coef, Mesh& recons) ////coef��params_
{
	float coef_f;
	recons.position_.setZero();
	recons.position_.array() = this->blendshapes_[0].position_.array() * coef[0];
	for (int kk = 1; kk < coef.size(); kk++) 
	{
		coef_f = coef[kk];
		recons.position_.array() += (this->blendshapes_[kk].position_.array() - this->blendshapes_[0].position_.array()) * coef_f;
	}
	return;
}

//�������landmarker��Ӧ��ģ�Ͷ����index��Ȼ��blendshape��ÿ��ģ�͵Ķ�Ӧlandmarker������ֵ������
//landmarkers_model_�С���ά����
void Blendshape::ProjectLandmark(std::vector<int> index3D) 
{
	this->landmarks_model_.resize(index3D.size());
	std::vector<Eigen::Vector3d> model_i;
	Eigen::Vector3d pos_i;
	model_i.resize(nBs_ + 1);//����51�����黹Ҫ������Ȼ����1��
	for (int ii = 0; ii < index3D.size(); ii++) 
	{
		for (int kk = 0; kk < nBs_ + 1; kk++) 
		{
			//position_��һ��nVert_in��3�е�����
			pos_i(0) = this->blendshapes_[kk].position_(index3D[ii], 0);//��index3D[ii]�е�0�У���Ϊ�������кܶ༸����ÿ��mesh������ֻҪlandmark��Ӧ���Ǽ���
			pos_i(1) = this->blendshapes_[kk].position_(index3D[ii], 1);
			pos_i(2) = this->blendshapes_[kk].position_(index3D[ii], 2);
			model_i[kk] = pos_i;
		}
		this->landmarks_model_[ii] = model_i;
	}
}
///////////////////////////////////// Basel Face Model //////////////////////////////////////////////////////////////