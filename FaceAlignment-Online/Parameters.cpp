#include "Parameters.h"


/////////////////////////////////////////////////////////// Camera Parameters ///////////////////////////////////////////////////////
//确定中心、图片大小和图片宽高比（aspect_）
CameraParameters::CameraParameters(double focal, double far_c, int image_width, 
	int image_height) : focal_(focal), far_c_(far_c) 
{
	center_ = glm::vec2(image_width * 0.5, image_height * 0.5);
	imgSize_ = glm::vec2(image_width, image_height);
	aspect_ = 1.0 * image_width / image_height;
}

//默认的far_c和focal，返回带far_c和focal，width，height的CameraParameters
CameraParameters CameraParameters::DefaultParameters(int image_width, int image_height) 
{
	const double far_c = 10000.0;//
	const double focal = 10;
	return CameraParameters(focal, far_c, image_width, image_height);
}

////////////////////////////////////////////////////////////// Model Parameters of Blendshape ////////////////////////////////////////////
/* Blendshape Model Parameters */
//yqy测试，这些参数就是确定模型的初始位置
//清除
//	重新定义大小
//	对每一个landmark，初始化为0
//	设置Rx_, Ry_, Rz_, Tx_, Ty_, Tz_
void ModelParametersBs::Init(int nBs) 
{
	this->params_.clear();
	this->params_.resize(nBs + 1);
	for (int kk = 0; kk < nBs + 1; kk++) this->params_[kk] = 0.0;
	this->params_[0] = 1.0;
	Rx_ = Ry_ = Rz_ = 0.0;
	Tx_ = 0.2;
	Ty_ = 0.0;
	Tz_ = -3000.0;
	//Tz_ = -1000.0;yqy测试，这些参数就是确定模型的初始位置
	//Tz_ = -20.0;//use luming.model
	
}

//  重置bs系数
void ModelParametersBs::ResetBsCoeff() 
{
	for (int kk = 0; kk < params_.size(); kk++) this->params_[kk] = 0.0;
	this->params_[0] = 1.0;
}

////   得到姿态参数(包括位移和旋转)
//std::vector<double> ModelParametersBs::GetPoseParams() 
//{
//	std::vector<double> pose_params(6);
//	pose_params[0] = Rx_; pose_params[1] = Ry_; pose_params[2] = Rz_;
//	pose_params[3] = Tx_; pose_params[4] = Ty_; pose_params[5] = Tz_;
//	return pose_params;
//}
//
//std::vector<double> ModelParametersBs::GetBsParams() 
//{
//	return params_;
//}

/* Blendshape Model Parameters */
//打开bs_file（即mesh文件）
//resize bsvideo的帧数
//resize model_bs
//对每一帧中的每一个bs，读入model_bs
//关闭
bool ModelParametersBs_Video::LoadBsCoeff(std::string bs_file, int nFrame, bool isBin) 
{
	this->nFrame_ = nFrame;
	std::ifstream bs_in;
	std::vector<double> model_bs;
	if (isBin) { bs_in.open(bs_file, std::ios::binary); }
	else { bs_in.open(bs_file); }
	if (bs_in.is_open()) 
	{
		this->bsVideo_.resize(nFrame);
		model_bs.resize(NUM_BLENDSHAPE + 1);
		for (int idx_frame = 0; idx_frame < nFrame_; idx_frame++) 
		{
			model_bs[0] = 1.0;
			for (int idx_bs = 1; idx_bs < NUM_BLENDSHAPE + 1; idx_bs++) 
			{
				if (isBin)
					bs_in.read(reinterpret_cast<char*>(&(model_bs[idx_bs])), sizeof(double));
				else
					bs_in >> model_bs[idx_bs];
			}
			this->bsVideo_[idx_frame].params_ = model_bs;
		}
		bs_in.close();
		return true;
	}
	else
	{
		return false;
	}
}

//打开bs_file
//resize bsvideo的帧数
//resize model_bs
//对每一帧中的每一个bs，记录输出bsvideo的参数
//关闭
bool ModelParametersBs_Video::SaveBsCoeff(std::string bs_file, bool isBin) 
{
	std::ofstream bs_out;
	if (isBin) { bs_out.open(bs_file, std::ios::binary); }
	else { bs_out.open(bs_file); }
	if (bs_out.is_open()) 
	{
		for (int kk = 0; kk < this->bsVideo_.size(); kk++)
		{
			for (int ii = 0; ii < NUM_BLENDSHAPE; ii++) 
			{
				if (isBin)
					bs_out.write(reinterpret_cast<const char*>(&(this->bsVideo_[kk].params_[ii + 1])), sizeof(double));
				else
					bs_out << this->bsVideo_[kk].params_[ii + 1] << " ";
			}
			if (!isBin) { bs_out << std::endl; }
		}
		bs_out.close();
		return true;
	}
	else 
	{
		return false;
	}
}




