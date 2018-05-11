#include "Models.h"

///////////////////////////////////////// Blendshape Model ///////////////////////////////////////////////
//加载所有从faceshift得到的mesh.obj
//加载blendshape，bs由n个mesh组成
void Blendshape::LoadBs(std::string bs_folder, int nBs) 
{
	nBs_ = nBs;
	Mesh obj_mesh;
	//BufModel obj_bufmodel;
	std::string bs_file;
	char filename[512];
	//1. load neutral first
	sprintf_s(filename, "%s/neutral.obj", bs_folder.c_str());
	//obj_bufmodel.load_obj(filename);//add yqy180424
	obj_mesh.load_obj(filename);//comment yqy180424
	//obj_mesh.center_model();//jisy center model 170109
	//obj_mesh.normalize_model();
	obj_mesh.update_normal();//xiugai yqy 180511
	blendshapes_.push_back(obj_mesh);
	//2. load blendshapes
	for (int kk = 0; kk < nBs; kk++) 
	{
		sprintf_s(filename, "mesh_%d.obj", kk);
		bs_file = bs_folder + "/" + std::string(filename);
		//std::cout << "Loading blendshape " << bs_file << std::endl;
		//obj_bufmodel.load_obj(bs_file);//add yqy180424
		obj_mesh.load_obj(bs_file);//comment yqy180424
		//obj_mesh.center_model();//jisy center model 170109
		obj_mesh.update_normal();//xiugai yqy 180425
		blendshapes_.push_back(obj_mesh);
	}
	std::cout << "Finished Loading blendshape models " << std::endl;
}

#if 0
//comment end yqy180425
//加载bs不过是二进制化的bs
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

//通过传入一组系数来更新blendshape
void Blendshape::UpdateMesh(std::vector<double> coef, Mesh& recons) ////coef即params_
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

//传入的是landmarker对应的模型顶点的index，然后将blendshape中每个模型的对应landmarker点坐标值保存在
//landmarkers_model_中。三维坐标
void Blendshape::ProjectLandmark(std::vector<int> index3D) 
{
	this->landmarks_model_.resize(index3D.size());
	std::vector<Eigen::Vector3d> model_i;
	Eigen::Vector3d pos_i;
	model_i.resize(nBs_ + 1);//除了51个表情还要加上自然表情1个
	for (int ii = 0; ii < index3D.size(); ii++) 
	{
		for (int kk = 0; kk < nBs_ + 1; kk++) 
		{
			//position_是一个nVert_in行3列的数据
			pos_i(0) = this->blendshapes_[kk].position_(index3D[ii], 0);//第index3D[ii]行第0列，因为总数据有很多几万行每个mesh，我们只要landmark对应的那几行
			pos_i(1) = this->blendshapes_[kk].position_(index3D[ii], 1);
			pos_i(2) = this->blendshapes_[kk].position_(index3D[ii], 2);
			model_i[kk] = pos_i;
		}
		this->landmarks_model_[ii] = model_i;
	}
}
//comment end yqy180425
#endif




//加载bs不过是二进制化的bs
////yqy用不到
//void Blendshape::LoadBsBin(std::string bs_file_bin, int nBs)
//{
//	nBs_ = nBs;
//	int nVert_in = 0;
//	int nVert_x3_in = 0;
//	int nBs_in = 0;
//	int nFaces = 0;
//	Mesh obj_mesh;
//	std::cout << "Reading blendshape file " << bs_file_bin << std::endl;
//	std::fstream fin;
//	fin.open(bs_file_bin, std::ios::in | std::ios::binary);
//	fin.read(reinterpret_cast<char*>(&(nVert_x3_in)), sizeof(int));
//	fin.read(reinterpret_cast<char*>(&(nBs_in)), sizeof(int));
//	fin.read(reinterpret_cast<char*>(&(nFaces)), sizeof(int));
//	std::cout << "#Vertex  = " << nVert_x3_in / 3 << " #Blendshape = " << nBs_in - 1 << " #Faces = " << nFaces << std::endl;
//	// prepare obj meshs
//	nVert_in = nVert_x3_in / 3;
//
//	for (std::vector<BufModel>::iterator it = obj_mesh.bufmodels.begin(); obj_mesh.bufmodels.end() != it; ++it)
//	{
//		it->n_verts_ = nVert_in;
//		it->n_tri_ = nFaces;
//		it->position_ = Eigen::MatrixXf(nVert_in, 3);	it->position_.setZero();
//		it->color_ = Eigen::MatrixXf(nVert_in, 3);	it->color_.setOnes(); /*this->color_ = this->color_ * 255.f;*/
//		it->normal_ = Eigen::MatrixXf(nVert_in, 3); it->normal_.setZero();
//		it->tex_coord_ = Eigen::MatrixXf(nVert_in, 2);	it->tex_coord_.setZero();
//		it->tri_list_ = Eigen::MatrixXi(nFaces, 3); it->tri_list_.setZero();
//		it->face_normal_ = Eigen::MatrixXf(nFaces, 3); it->face_normal_.setZero();
//		// read triangle list
//		fin.read(reinterpret_cast<char*>(it->tri_list_.data()), sizeof(int)*((int)nFaces) * 3);
//		// read vertices
//		for (int kk = 0; kk < nBs_in; kk++)
//		{
//			fin.read(reinterpret_cast<char*>(it->position_.data()), sizeof(float)*((int)nVert_x3_in));
//			obj_mesh.update_normal();
//			blendshapes_.push_back(obj_mesh);
//		}
//	}
//	fin.close();
//	std::cout << "done." << std::endl;
//}

//modify yqy180511
//通过传入一组系数来更新blendshape
void Blendshape::UpdateMesh(std::vector<double> coef, Mesh& recons) ////coef即params_
{
	float coef_f;
	//for (std::vector<BufModel>::iterator it2 = recons.bufmodels.begin(); recons.bufmodels.end() != it2; ++it2)
	{
		for (std::vector<BufModel>::iterator it = blendshapes_[0].bufmodels.begin(); blendshapes_[0].bufmodels.end() != it; ++it)
		{
		
			recons._bufmodel.position_.setZero();
			recons._bufmodel.position_.array() = it->position_.array() * coef[0];
		}
		for (int kk = 1; kk < coef.size(); kk++)
		{
			for (std::vector<BufModel>::iterator it = blendshapes_[kk].bufmodels.begin(); blendshapes_[kk].bufmodels.end() != it; ++it)
			{
				coef_f = coef[kk];
				recons._bufmodel.position_.array() += (it->position_.array() - it->position_.array()) * coef_f;
			}
		}
	}
	return;
}

//传入的是landmarker对应的模型顶点的index，然后将blendshape中每个模型的对应landmarker点坐标值保存在
//landmarkers_model_中。三维坐标
void Blendshape::ProjectLandmark(std::vector<int> index3D)
{
	this->landmarks_model_.resize(index3D.size());
	std::vector<Eigen::Vector3d> model_i;
	Eigen::Vector3d pos_i;
	model_i.resize(nBs_ + 1);//除了51个表情还要加上自然表情1个
	for (int ii = 0; ii < index3D.size(); ii++)
	{
		for (int kk = 0; kk < nBs_ + 1; kk++)
		{
			for (std::vector<BufModel>::iterator it = blendshapes_[kk].bufmodels.begin(); blendshapes_[kk].bufmodels.end() != it; ++it)
			{
				//position_是一个nVert_in行3列的数据
				pos_i(0) = it->position_(index3D[ii], 0);//第index3D[ii]行第0列，因为总数据有很多几万行每个mesh，我们只要landmark对应的那几行
				pos_i(1) = it->position_(index3D[ii], 1);
				pos_i(2) = it->position_(index3D[ii], 2);
				model_i[kk] = pos_i;
			}
		}
		this->landmarks_model_[ii] = model_i;
	}
}
//modify end
///////////////////////////////////// Basel Face Model //////////////////////////////////////////////////////////////