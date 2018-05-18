#include "ObjMesh.h"

#include <omp.h>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>



void Mesh::update_normal()
{
	for (std::vector<BufModel>::iterator it = this->bufmodels.begin(); this->bufmodels.end() != it; ++it)
	{
		it->normal_.resize(it->n_verts_, 3);
		it->face_normal_.resize(it->n_tri_, 3);
		it->normal_.setZero();
		it->face_normal_.setZero();
		std::vector<float> area_sum(it->n_verts_, 0.f);
		omp_lock_t writelock;
		omp_init_lock(&writelock);

#pragma omp parallel for
		//ԭ���뿴��һ����ѧOpenGL 18�� -������⡷ - Mr_���Ĳ��� - CSDN������CalcNormals��������
		for (int i = 0; i < (it->n_tri_); ++i) {
			auto vidx0 = it->tri_list_(i, 0);
			auto vidx1 = it->tri_list_(i, 1);
			auto vidx2 = it->tri_list_(i, 2);

			auto v0 = Eigen::Vector3f(it->position_.row(vidx0));
			auto v1 = Eigen::Vector3f(it->position_.row(vidx1));
			auto v2 = Eigen::Vector3f(it->position_.row(vidx2));

			auto v0v1 = v1 - v0;
			auto v0v2 = v2 - v0;
			auto n = v0v2.cross(v0v1);
			double area = n.norm();
			//����ÿ�������η���������ͨ������ӵ�һ�����������������������������������Ĳ���õ��ġ��������ۼ�֮ǰҪ���Ƚ��䵥λ������Ϊ���������
			//�����һ���ǵ�λ������
			omp_set_lock(&writelock);
			//�ۼӼ���������ÿ������ķ�����
			it->normal_.row(vidx0) += n;
			it->normal_.row(vidx1) += n;
			it->normal_.row(vidx2) += n;
			area_sum[vidx0] += area;
			area_sum[vidx1] += area;
			area_sum[vidx2] += area;
			omp_unset_lock(&writelock);
			n.normalize();
			it->face_normal_.row(i) = n;//�淨��
		}
		omp_destroy_lock(&writelock);

#pragma omp parallel for
		for (int i = 0; i < (it->n_verts_); ++i)
		{
			it->normal_.row(i) /= area_sum[i];
		}

	}
}


#if 0
//modify yqy180504
//�����淨�ߺͶ��㷨�� 
void Mesh::update_normal()
{
	for (std::vector<BufModel>::iterator it = this->bufmodels.begin(); this->bufmodels.end() != it; ++it)
	{
		it->vertData.resize(it->n_verts_);
		it->face_normal.resize(it->n_tri_);
		for (int ii = 0; ii < (it->n_verts_); ++ii)
		{
			it->vertData[ii].normal.x = 0;
			it->vertData[ii].normal.y = 0;
			it->vertData[ii].normal.z = 0;
		}		
		for (int jj = 0; jj < (it->n_tri_); ++jj)
		{
			it->face_normal[jj] = Eigen::Vector3f(0, 0, 0);
			//�����������ô��ֵ�����=��Ϊ�������������,���������������Ͳ�һ��
			/*it->face_normal[jj].x = 0;
			it->face_normal[jj].y = 0;
			it->face_normal[jj].z = 0;*/
		}
		std::vector<float> area_sum(it->n_verts_, 0.f);
		omp_lock_t writelock;
		omp_init_lock(&writelock);

#pragma omp parallel for
		for (int i = 0; i < (it->n_tri_); ++i)
		{
			auto vidx0 = it->tri_list[i].mIndices[0];//��i�������εĵ�һ������vidx0
			auto vidx1 = it->tri_list[i].mIndices[1]; //��i�������εĵ�2������
			auto vidx2 = it->tri_list[i].mIndices[2]; // ��i�������εĵ�3������
			auto v0 = Eigen::Vector3f(it->vertData[vidx0].position.x);//����vidx0��x����
			auto v1 = Eigen::Vector3f(it->vertData[vidx1].position.y);
			auto v2 = Eigen::Vector3f(it->vertData[vidx2].position.z);
			auto v0v1 = v1 - v0;
			auto v0v2 = v2 - v0;
			auto n = v0v2.cross(v0v1);
			double area = n.norm();
			omp_set_lock(&writelock);
			//ע������n��eigen��vector3f�ͣ�����ֱ��it->vertData[vidx0].normal +=n

			it->vertData[vidx0].normal += glm::vec3(n(vidx0,0), n(vidx0, 1), n(vidx0, 2));
			it->vertData[vidx1].normal += glm::vec3(n(vidx1, 0), n(vidx1, 1), n(vidx1, 2));
			it->vertData[vidx2].normal += glm::vec3(n(vidx2, 0), n(vidx2, 1), n(vidx2, 2));

			area_sum[vidx0] += area;
			area_sum[vidx1] += area;
			area_sum[vidx2] += area;
			omp_unset_lock(&writelock);
			n.normalize();
			it->face_normal[i] = n;
		}
		omp_destroy_lock(&writelock);
#pragma omp parallel for
		for (int i = 0; i< (it->n_verts_); ++i)
		{
			it->vertData[i].normal /= area_sum[i];
		}
		//}

		//comment yqy180504
		//model.normal_.resize(model.n_verts_, 3);//����������*3����
		//model.face_normal_.resize(model.n_tri_, 3);//��������*3����
		//model.normal_.setZero();//��ʼ��Ϊ0
		//model.face_normal_.setZero();//��ʼ��Ϊ0
		//std::vector<float> area_sum(model.n_verts_, 0.f);//����һ��vector��Ԫ�ظ���Ϊ��������ֵ��ʼ��Ϊ0
		//comment end yqy180504
	//	omp_lock_t writelock;
	//	omp_init_lock(&writelock);
	//
	//#pragma omp parallel for
		//ԭ���뿴��һ����ѧOpenGL 18�� -������⡷ - Mr_���Ĳ��� - CSDN������CalcNormals��������
	//	for (int i = 0; i<model.n_tri_; ++i) {
	//		auto vidx0 = model.tri_list_(i, 0);//��������
	//		auto vidx1 = model.tri_list_(i, 1);
	//		auto vidx2 = model.tri_list_(i, 2);
	//
	//		auto v0 = Eigen::Vector3f(model.position_.row(vidx0));//������������ȡ��ÿ�������ε���������
	//		auto v1 = Eigen::Vector3f(model.position_.row(vidx1));
	//		auto v2 = Eigen::Vector3f(model.position_.row(vidx2));
	//
	//		auto v0v1 = v1 - v0;
	//		auto v0v2 = v2 - v0;
	//		auto n = v0v2.cross(v0v1);
	////����ÿ�������η���������ͨ������ӵ�һ�����������������������������������Ĳ���õ��ġ�
	//		double area = n.norm();
	//		//�������ۼ�֮ǰҪ���Ƚ��䵥λ������Ϊ��������Ľ����һ���ǵ�λ������
	//		omp_set_lock(&writelock);
	//		//�ۼӼ���������ÿ������ķ�����,��Ϊһ��������Ա���������ι��ã�����Ҫ����������������������εķ�����֮��
	//		model.normal_.row(vidx0) += n;
	//		model.normal_.row(vidx1) += n;
	//		model.normal_.row(vidx2) += n;
	//		area_sum[vidx0] += area;
	//		area_sum[vidx1] += area;
	//		area_sum[vidx2] += area;
	//		omp_unset_lock(&writelock);
	//		n.normalize();
	//		model.face_normal_.row(i) = n;//�淨��
	//	}
		/*omp_destroy_lock(&writelock);*/

//#pragma omp parallel for
//		for (int i = 0; i < model.n_verts_; ++i)
//		{
//			model.normal_.row(i) /= area_sum[i];//�����������鲢��λ��ÿ������ķ�����������������ͬ�ڽ��ۼӵ���������ƽ����������һ��Ϊ��λ���ȵĶ��㷨�ߡ�
//		}
	}
}
#endif



#if 0
//comment yqy180425
void update_normal()
{

	this->normal_.resize(n_verts_, 3);
	this->face_normal_.resize(n_tri_, 3);
	this->normal_.setZero();
	this->face_normal_.setZero();
	std::vector<float> area_sum(this->n_verts_, 0.f);
	omp_lock_t writelock;
	omp_init_lock(&writelock);

#pragma omp parallel for
	//ԭ���뿴��һ����ѧOpenGL 18�� -������⡷ - Mr_���Ĳ��� - CSDN������CalcNormals��������
	for (int i = 0; i<n_tri_; ++i) {
		auto vidx0 = tri_list_(i, 0);
		auto vidx1 = tri_list_(i, 1);
		auto vidx2 = tri_list_(i, 2);

		auto v0 = Eigen::Vector3f(position_.row(vidx0));
		auto v1 = Eigen::Vector3f(position_.row(vidx1));
		auto v2 = Eigen::Vector3f(position_.row(vidx2));

		auto v0v1 = v1 - v0;
		auto v0v2 = v2 - v0;
		auto n = v0v2.cross(v0v1);
		double area = n.norm();
		//����ÿ�������η���������ͨ������ӵ�һ�����������������������������������Ĳ���õ��ġ��������ۼ�֮ǰҪ���Ƚ��䵥λ������Ϊ���������
		//�����һ���ǵ�λ������
		omp_set_lock(&writelock);
		//�ۼӼ���������ÿ������ķ�����
		this->normal_.row(vidx0) += n;
		this->normal_.row(vidx1) += n;
		this->normal_.row(vidx2) += n;
		area_sum[vidx0] += area;
		area_sum[vidx1] += area;
		area_sum[vidx2] += area;
		omp_unset_lock(&writelock);
		n.normalize();
		this->face_normal_.row(i) = n;//�淨��
	}
	omp_destroy_lock(&writelock);

#pragma omp parallel for
	for (int i = 0; i<n_verts_; ++i)
	{
		this->normal_.row(i) /= area_sum[i];
	}

}
}
//comment end yqy180425
#endif

//add yqy180424
void Mesh::draw(const Shader& shader) const
{
	for (std::vector<BufModel>::const_iterator it = this->bufmodels.begin(); this->bufmodels.end() != it; ++it)
	{
		it->Draw(shader);//add yqy180503
		//it->draw(shader);//comment yqy180503
	}
}
bool Mesh::load_obj(const std::string& filePath)
{
	Assimp::Importer importer;
	if (filePath.empty())
	{
		std::cerr << "Error:Model::loadModel, empty model file path." << std::endl;
		return false;
	}
	const aiScene* sceneObjPtr = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_FlipUVs);
//ReadFile�����еڶ����������Ǻ���ѡ�����һ��ö������aiPostProcessSteps������ʹ��λ������������ѡ�����
//ѡ��aiProcess_MakeLeftHanded��ʾ��Ĭ�ϵ�����ϵ��������ת��Ϊ����ϵ�������ݣ�aiProcess_Triangulateѡ�������
//�ݶ���3���Ķ���λ���Ϊ��������Σ���������ʹ�������ν��л��ơ�
	if (!sceneObjPtr
		|| sceneObjPtr->mFlags == AI_SCENE_FLAGS_INCOMPLETE
		|| !sceneObjPtr->mRootNode)
	{
		std::cerr << "Error:Model::loadModel, description: "
			<< importer.GetErrorString() << std::endl;
		return false;
	}
	this->modelFileDir = filePath.substr(0, filePath.find_last_of('/'));
	if (!this->processNode(sceneObjPtr->mRootNode, sceneObjPtr))//����processNode
	{
		std::cerr << "Error:Model::loadModel, process node failed." << std::endl;
		return false;
	}
	return true;
}
/*
* �ݹ鴦��ģ�͵Ľ��
*/
bool Mesh::processNode(const aiNode* node, const aiScene* sceneObjPtr)
{

	if (!node || !sceneObjPtr)
	{
		return false;
	}
	// �ȴ���������
	std::cout << "node->mNumMeshes=" << node->mNumMeshes << std::endl;
	for (size_t i = 0; i < node->mNumMeshes; ++i)
	{
		// ע��node�е�mesh�Ƕ�sceneObject��mesh������
		const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]];
		if (meshPtr)
		{
			BufModel meshObj;
			if (meshObj.processMesh(meshPtr, sceneObjPtr))
			{
				//_bufmodel = meshObj;//add yqy180425
				this->bufmodels.push_back(meshObj);//comment yqy180425

			}
		}
	}
	// �����ӽ��
	for (size_t i = 0; i < node->mNumChildren; ++i)
	{
		this->processNode(node->mChildren[i], sceneObjPtr);
	}
	return true;
}


#if 0
//comment yqy180424
void ObjMesh::load_obj(std::string filename)
{
	
	// prepare all the prefixs
	std::vector<double> coords;
	std::vector<int> tris;
	std::vector<double> vColor;
	std::vector<double> vNormal;
	std::vector<double> tex_coords;

	printf("Loading OBJ file %s...\n", filename.c_str());
	double x, y, z, r, g, b, nx, ny, nz;
	double tx, ty, tz;
	std::vector<unsigned int> vertexIndices, uvIndices,normalIndices;
	// open the file, return if open fails
	FILE * file = fopen(filename.c_str(), "r");//��filename�е����ݶ�����file��
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? \n");
		getchar();
			return ;
	}
	// prepare prefix
	int pref_cnt = 0;
	if (request_position_) pref_cnt++;
	if (request_normal_) pref_cnt++;
	if (request_tex_coord_) pref_cnt++;

	while (1) 
	{

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);//fscanf�����ո�ͻ���ʱ������ע��ո�ʱҲ����
		if (res == EOF)
			break;
		// parse the file
		if (strcmp(lineHeader, "v") == 0) //�����嶥��
		{
			if (request_position_ && request_color_) 
			{
				fscanf(file, "%lf %lf %lf %lf %lf %lf\n", &x, &y, &z, &r, &g, &b);

				coords.push_back(x); coords.push_back(y); coords.push_back(z);
				vColor.push_back(r); vColor.push_back(g); vColor.push_back(b);
			}
			else 
			{
				fscanf(file, "%lf %lf %lf %lf %lf %lf\n", &x, &y, &z);

				coords.push_back(x); coords.push_back(y); coords.push_back(z);
			}
		}
		else if (strcmp(lineHeader, "vn") == 0) //vn���㷨��
		{
			fscanf(file, "%lf %lf %lf\n", &nx, &ny, &nz);
			vNormal.push_back(nx); vNormal.push_back(ny); vNormal.push_back(nz);
		}
		else if (strcmp(lineHeader, "vt") == 0) //��ͼ�����
		{
			fscanf(file, "%lf %lf\n", &tx, &ty);
			tex_coords.push_back(tx);
			tex_coords.push_back(ty);

			//fscanf(file, "%lf %lf %lf\n", &tx, &ty, &tz);
			//tex_coords.push_back(tx);
			//tex_coords.push_back(ty);
			// tex_coord.push_back(tz);
		}
		else if (strcmp(lineHeader, "f") == 0) //��
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];//��Ŷ���������������������������
			if (pref_cnt == 1)//pref_cnt == 1,˵��ֻ��request_position_Ϊtrue��ֻҪ������Ϣ
				fscanf(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
			else if (pref_cnt == 2) //��Ҫ���߻���ͼ������Ϣ
			{
				if (request_normal_)
					fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				else if (request_tex_coord_)
					fscanf(file, "%d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2]);
			}
			else if (pref_cnt == 3)//������Ϣ��Ҫ
				fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0],
					&vertexIndex[1], &uvIndex[1], &normalIndex[1],
					&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			//��Ϊ������������1��ʼ
			tris.push_back(vertexIndex[0]-1);
			tris.push_back(vertexIndex[1]-1);
			tris.push_back(vertexIndex[2]-1);
		}
		else 
		{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}
	fclose(file);
	// post process
	this->n_tri_ = tris.size() / 3;//�ж��ٸ���������
	this->n_verts_ = coords.size() / 3;//�ж��ٸ�����
	this->position_ = Eigen::MatrixXf(n_verts_, 3);	this->position_.setZero();
	this->color_ = Eigen::MatrixXf(n_verts_, 3);	this->color_.setOnes(); /*this->color_ = this->color_ * 255.f;*/
	this->normal_ = Eigen::MatrixXf(n_verts_, 3); this->normal_.setZero();
	this->tex_coord_ = Eigen::MatrixXf(n_verts_, 2);	this->tex_coord_.setZero();

	this->tri_list_ = Eigen::MatrixXi(n_tri_, 3); this->tri_list_.setZero();
	this->face_normal_ = Eigen::MatrixXf(n_tri_, 3); this->face_normal_.setZero();
	// copy data
	int off3, off2;
	for (int kk = 0; kk < n_verts_; kk++) 
	{
		off3 = 3 * kk; off2 = 2 * kk;
		if (request_position_)
			this->position_(kk, 0) = coords[off3]; this->position_(kk, 1) = coords[off3+1]; this->position_(kk, 2) = coords[off3+2];
		if (request_color_) 
		{
			this->color_(kk, 0) = vColor[off3]; this->color_(kk, 1) = vColor[off3 + 1]; this->color_(kk, 2) = vColor[off3 + 2];
		}
		if (request_normal_)
		{
			this->normal_(kk, 0) = vNormal[off3]; this->normal_(kk, 1) = vNormal[off3 + 1]; this->normal_(kk, 2) = vNormal[off3 + 2];
		}
		if (request_tex_coord_) 
		{
			this->tex_coord_(kk, 0) = tex_coords[off2]; this->tex_coord_(kk, 1) = tex_coords[off2 + 1];
		}
	}
	for (int kk = 0; kk < this->n_tri_; kk++) 
	{
		off3 = 3 * kk;
		if (request_tri_list_)
			this->tri_list_(kk, 0) = tris[off3]; this->tri_list_(kk, 1) = tris[off3+1]; this->tri_list_(kk, 2) = tris[off3+2];
	}

	return;
	
}




void ObjMesh::write_obj(std::string filename) 
{	
	std::ofstream obj_file(filename);
	int pref_cnt = 1;
	if (save_normal_) pref_cnt++;
	if (save_tex_coord_) pref_cnt++;
	// write vertices
	if (save_color_) 
	{
		for (std::size_t i = 0; i < position_.rows(); ++i) 
		{
			obj_file << "v " << position_(i, 0) << " " << position_(i, 1) << " " << position_(i, 2)//yqy�޸ģ��о�������������
			//obj_file << "v " << position_(i, 0) << " " << position_(i, 0) << " " << position_(i, 0)
				<< " " << color_(i, 0) << " " << color_(i, 1) << " " << color_(i, 2) << " " << std::endl;
		}
	}
	else 
	{
		for (std::size_t i = 0; i < position_.rows(); ++i) 
		{
			obj_file << "v " << position_(i, 0) << " " << position_(i, 1) << " " << position_(i, 2) << " " << std::endl;
		}
	}
	// write normal
	if (save_normal_) 
	{
		for (std::size_t i = 0; i < normal_.rows(); ++i) 
		{
			obj_file << "vn " << normal_(i, 0) << " " << normal_(i, 1) << " " << normal_(i, 2) << " " << std::endl;
		}
	}
	// write tex coord
	if (save_tex_coord_) 
	{
		for (std::size_t i = 0; i < tex_coord_.rows(); ++i) 
		{
			obj_file << "vt " << tex_coord_(i, 0) << " " << tex_coord_(i, 1) << " " << std::endl;
		}
	}
	// write triangles
	for (int kk = 0; kk < tri_list_.rows(); kk++) 
	{
		// Add one because obj starts counting triangle indices at 1
		if (pref_cnt == 1) 
		{
			obj_file << "f " << tri_list_(kk, 0) + 1 << " " << tri_list_(kk, 1) + 1 << " " << tri_list_(kk, 2) + 1 << std::endl;
		}
		else if (pref_cnt == 2) 
		{
			obj_file << "f " << tri_list_(kk, 0) + 1 << "/" << tri_list_(kk, 0) + 1 << " "
							 << tri_list_(kk, 1) + 1 << "/" << tri_list_(kk, 1) + 1 << " "
							 << tri_list_(kk, 2) + 1 << "/" << tri_list_(kk, 2) + 1 << std::endl;
		}
		else if (pref_cnt == 3) 
		{
			obj_file << "f " << tri_list_(kk, 0) + 1 << "/" << tri_list_(kk, 0) + 1 << "/" << tri_list_(kk, 0) + 1 << " "
							<< tri_list_(kk, 1) + 1 << "/" << tri_list_(kk, 1) + 1 << "/" << tri_list_(kk, 1) + 1 << " "
							<< tri_list_(kk, 2) + 1 << "/" << tri_list_(kk, 2) + 1 << "/" << tri_list_(kk, 2) + 1 << std::endl;
		}
	}
	return;
}

void ObjMesh::print_summary() 
{
	std::cout << std::endl;
	std::cout << "Summary of Mesh Model: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	std::cout << "n vertices is " << n_verts_ << std::endl;
	std::cout << "n triangles is " << n_tri_ << std::endl;
	std::cout << "position of vertice-0 is ( " << position_(0, 0) << " , "
		<< position_(0, 1) << " , "
		<< position_(0, 2) << " ) " << std::endl;

	std::cout << "color of vertice-0 is ( " << color_(0, 0) << " , "
		<< color_(0, 1) << " , "
		<< color_(0, 2) << " ) " << std::endl;

	std::cout << "normal of vertice-0 is ( " << normal_(0, 0) << " , "
		<< normal_(0, 1) << " , "
		<< normal_(0, 2) << " ) " << std::endl;

	std::cout << "index of triangle-0 is ( " << tri_list_(0, 0) << " , "
		<< tri_list_(0, 1) << " , "
		<< tri_list_(0, 2) << " ) " << std::endl;

	std::cout << "index of triangle-1 is ( " << tri_list_(1, 0) << " , "
		<< tri_list_(1, 1) << " , "
		<< tri_list_(1, 2) << " ) " << std::endl;

	std::cout << "End of Summary: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
	return;
}

void ObjMesh::update_normal() 
{
	this->normal_.resize(n_verts_, 3);
	this->face_normal_.resize(n_tri_, 3);
	this->normal_.setZero();
	this->face_normal_.setZero();
	std::vector<float> area_sum(this->n_verts_, 0.f);
	omp_lock_t writelock;
	omp_init_lock(&writelock);

#pragma omp parallel for
	//ԭ���뿴��һ����ѧOpenGL 18�� -������⡷ - Mr_���Ĳ��� - CSDN������CalcNormals��������
	for (int i = 0; i<n_tri_; ++i) {
		auto vidx0 = tri_list_(i, 0);
		auto vidx1 = tri_list_(i, 1);
		auto vidx2 = tri_list_(i, 2);

		auto v0 = Eigen::Vector3f(position_.row(vidx0));
		auto v1 = Eigen::Vector3f(position_.row(vidx1));
		auto v2 = Eigen::Vector3f(position_.row(vidx2));

		auto v0v1 = v1 - v0;
		auto v0v2 = v2 - v0;
		auto n = v0v2.cross(v0v1);
		double area = n.norm();
		//����ÿ�������η���������ͨ������ӵ�һ�����������������������������������Ĳ���õ��ġ��������ۼ�֮ǰҪ���Ƚ��䵥λ������Ϊ���������
		//�����һ���ǵ�λ������
		omp_set_lock(&writelock);
		//�ۼӼ���������ÿ������ķ�����
		this->normal_.row(vidx0) += n;
		this->normal_.row(vidx1) += n;
		this->normal_.row(vidx2) += n;
		area_sum[vidx0] += area;
		area_sum[vidx1] += area;
		area_sum[vidx2] += area;
		omp_unset_lock(&writelock);
		n.normalize();
		this->face_normal_.row(i) = n;//�淨��
	}
	omp_destroy_lock(&writelock);

#pragma omp parallel for
	for (int i = 0; i<n_verts_; ++i) 
	{
		this->normal_.row(i) /= area_sum[i];
	}
	
}

void ObjMesh::normalize_model() //��ģ���������ų߶�

{

	float mean_x = this->position_.col(0).sum() / this->n_verts_;
	float mean_y = this->position_.col(1).sum() / this->n_verts_;
	float mean_z = this->position_.col(2).sum() / this->n_verts_;

	this->position_.col(0) = this->position_.col(0).array() - mean_x;
	this->position_.col(1) = this->position_.col(1).array() - mean_y;
	this->position_.col(2) = this->position_.col(2).array() - mean_z;


	float max_x = this->position_.col(0).maxCoeff();
	float max_y = this->position_.col(1).maxCoeff();
	float max_z = this->position_.col(2).maxCoeff();

	float min_x = this->position_.col(0).minCoeff();
	float min_y = this->position_.col(1).minCoeff();
	float min_z = this->position_.col(2).minCoeff();
	// +0.00001��ֹ����0
	float scale_x = 2 / (max_x - min_x + 0.00001);
	float scale_y = 2 / (max_y - min_y + 0.00001);
	float scale_z = 2 / (max_z - min_z + 0.00001);

	float scale = scale_x < scale_y ? scale_x : scale_y;
	scale = scale_z < scale ? scale_z : scale;
	this->position_.col(0) = (this->position_.col(0).array() - min_x) * scale;
	this->position_.col(1) = (this->position_.col(1).array() - min_y) * scale;
	this->position_.col(2) = (this->position_.col(2).array() - min_z) * scale;

	std::cout << "mean position = " << mean_x << " , " << mean_y << " , " << mean_z << std::endl;
	std::cout << "min position = " << min_x << " , " << min_y << " , " << min_z << std::endl;
	std::cout << "scale = " << scale << std::endl;

	this->position_ = this->position_.array() - 1.0f;
	return;
}

void ObjMesh::center_model() //��ǰλ�ü�ȥƽ��λ��
{

	float mean_x = this->position_.col(0).sum() / this->n_verts_;
	float mean_y = this->position_.col(1).sum() / this->n_verts_;
	float mean_z = this->position_.col(2).sum() / this->n_verts_;

	this->position_.col(0) = this->position_.col(0).array() - mean_x;
	this->position_.col(1) = this->position_.col(1).array() - mean_y;
	this->position_.col(2) = this->position_.col(2).array() - mean_z;

	return;
}


void ObjMesh::update_vertices(const Eigen::VectorXd &vertices) {

	const int num_vertices = this->n_verts_;

#pragma omp parallel for
	for (int i = 0; i<num_vertices; ++i) {
		const int offset = i * 3;
		this->position_(i, 0) = vertices(offset + 0);
		this->position_(i, 1) = vertices(offset + 1);
		this->position_(i, 2) = vertices(offset + 2);
	}
}


//comment end yqy180424
#endif