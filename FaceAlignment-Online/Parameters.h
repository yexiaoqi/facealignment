#pragma once

#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Eigen\Eigen>

#define NUM_BLENDSHAPE 51
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Camera Parameters */
class CameraParameters
{
public:
	CameraParameters() {}
	CameraParameters(double focal, double far_c, int image_width, int image_height);
	static CameraParameters DefaultParameters(int image_width, int image_height);
	friend std::istream& operator >> (std::istream& is, CameraParameters& params);
	friend std::ostream& operator<<(std::ostream& os, const CameraParameters& params);

	double far_c_;
	double focal_;
	double aspect_;
	glm::dvec2 center_;
	glm::dvec2 imgSize_;
};

inline std::istream& operator >> (std::istream& is, CameraParameters& params) 
{
	is >> params.far_c_ >> params.focal_
		>> params.center_.x >> params.center_.y
		>> params.imgSize_.x >> params.imgSize_.y;
	return is;
}

inline std::ostream& operator<<(std::ostream& os, const CameraParameters& params) 
{
	os << params.far_c_ << std::endl;
	os << params.focal_ << std::endl;
	os << params.center_.x << ' ' << params.center_.y << std::endl;
	os << params.imgSize_.x << ' ' << params.imgSize_.y;
	return os;
}


/* Blendshape Model Parameters */
//��ʼ��ģ��
//����ģ�Ͳ���
//�õ���̬����
//�õ�ģ�Ͳ���
class ModelParametersBs 
{
public:
	ModelParametersBs() { this->Init(NUM_BLENDSHAPE); }
	ModelParametersBs(int nBs) { this->Init(nBs); }

	//���
	//	���¶����С
	//	��ÿһ��landmark����ʼ��Ϊ0
	//	����Rx_, Ry_, Rz_, Tx_, Ty_, Tz_
	void Init(int nBs);//yqy���ԣ���Щ��������ȷ��ģ�͵ĳ�ʼλ��
	void ResetBsCoeff();//  ����bsϵ��
	//std::vector<double> GetPoseParams();//    �õ���̬����(����λ�ƺ���ת)
	//std::vector<double> GetBsParams();//�õ�bs����
	std::vector<double> params_;
	double Rx_, Ry_, Rz_, Tx_, Ty_, Tz_;
};

/* Blendshape Model Parameters */
struct ModelParametersBs_Video 
{
	//��bs_file����mesh�ļ���
	//	resize bsvideo��֡��
	//	resize model_bs
	//	��ÿһ֡�е�ÿһ��bs������model_bs
	//	�ر�
	bool LoadBsCoeff(std::string bs_file, int nFrame, bool isBin);

	//��bs_file
	//resize bsvideo��֡��
	//resize model_bs
	//��ÿһ֡�е�ÿһ��bs����¼���bsvideo�Ĳ���
	//�ر�
	bool SaveBsCoeff(std::string bs_file, bool isBin);
	std::vector<ModelParametersBs> bsVideo_;
	int nFrame_;
};
