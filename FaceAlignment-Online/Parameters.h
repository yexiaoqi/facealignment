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
//初始化模型
//重置模型参数
//得到姿态参数
//得到模型参数
class ModelParametersBs 
{
public:
	ModelParametersBs() { this->Init(NUM_BLENDSHAPE); }
	ModelParametersBs(int nBs) { this->Init(nBs); }

	//清除
	//	重新定义大小
	//	对每一个landmark，初始化为0
	//	设置Rx_, Ry_, Rz_, Tx_, Ty_, Tz_
	void Init(int nBs);//yqy测试，这些参数就是确定模型的初始位置
	void ResetBsCoeff();//  重置bs系数
	//std::vector<double> GetPoseParams();//    得到姿态参数(包括位移和旋转)
	//std::vector<double> GetBsParams();//得到bs参数
	std::vector<double> params_;
	double Rx_, Ry_, Rz_, Tx_, Ty_, Tz_;
};

/* Blendshape Model Parameters */
struct ModelParametersBs_Video 
{
	//打开bs_file（即mesh文件）
	//	resize bsvideo的帧数
	//	resize model_bs
	//	对每一帧中的每一个bs，读入model_bs
	//	关闭
	bool LoadBsCoeff(std::string bs_file, int nFrame, bool isBin);

	//打开bs_file
	//resize bsvideo的帧数
	//resize model_bs
	//对每一帧中的每一个bs，记录输出bsvideo的参数
	//关闭
	bool SaveBsCoeff(std::string bs_file, bool isBin);
	std::vector<ModelParametersBs> bsVideo_;
	int nFrame_;
};
