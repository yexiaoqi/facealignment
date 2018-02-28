#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Eigen\Eigen>
#include <ceres\ceres.h>

#include "Models.h"
#include "Parameters.h"
///////////////////////////////////////////////////////// Cost Function for Reconstructor ////////////////////////////////////////////
struct PoseEstimateResidual_AutoDiff
{
	PoseEstimateResidual_AutoDiff(cv::Vec2f image_point, cv::Vec4f model_point, int width,
		int height, double focal) :
		image_point_(image_point), model_point_(model_point), width_(width), height_(height),
		focal_(focal) {}

	template<typename T>
	bool operator()(const T* const X, T* residual)const
	{
		// 3D point as input
		T ox = T(model_point_[0]);
		T oy = T(model_point_[1]);
		T oz = T(model_point_[2]);
		T ow = T(1.0);
		// parameters to be optimized
		T rot_x_pitch = X[0];
		T rot_y_yaw = X[1];
		T rot_z_roll = X[2];
		T tx = X[3];
		T ty = X[4];
		T tz = X[5];
		T focal = T(focal_);

		//绕z轴旋转
		T ox2 = ox;
		T oy2 = cos(rot_x_pitch)*oy - sin(rot_x_pitch)*oz;
		T oz2 = sin(rot_x_pitch)*oy + cos(rot_x_pitch)*oz;
		T ow2 = ow;

		//绕y轴旋转
		T ox3 = cos(rot_y_yaw)*ox2 + sin(rot_y_yaw)*oz2;
		T oy3 = oy2;
		T oz3 = -sin(rot_y_yaw)*ox2 + cos(rot_y_yaw)*oz2;
		T ow3 = ow2;

		//绕x轴旋转
		T ox4 = cos(rot_z_roll)*ox3 - sin(rot_z_roll)*oy3;
		T oy4 = sin(rot_z_roll)*ox3 + cos(rot_z_roll)*oy3;
		T oz4 = oz3;
		T ow4 = ow3;

		//在x,y,z方向的位移
		T ox5 = ox4 + tx;
		T oy5 = oy4 + ty;
		T oz5 = oz4 + tz;
		T ow5 = ow4;

		T aspect = T(width_) / T(height_);
		T l_ = T(-1.0*aspect);
		T r_ = T(1.0*aspect);
		T b_ = T(-1.0);
		T t_ = T(1.0);
		T n_ = focal;
		T f_ = T(1000.);
		
		T ox6 = T(2.0)*n_ / (r_ - l_)*ox5;
		T oy6 = T(2.0)*n_ / (t_ - b_)*oy5;
		T oz6 = -(n_ + f_) / (f_ - n_)*oz5 + (T(-2.0) * n_ * f_) / (f_ - n_);
		T ow6 = -oz5;

		// transformed to image coordinates
		T fx1 = T(0.5)*(ox6 / ow6 + T(1.0)) * T(width_);
		T fy1 = T(0.5)*(oy6 / ow6 + T(1.0)) * T(height_);

		T predicted_x = fx1;
		T predicted_y = fy1;
		// The error is the difference between the predicted and observed position.
		residual[0] = predicted_x - T(image_point_[0]);
		residual[1] = predicted_y - T(image_point_[1]);
		//std::cout << "residual is " << residual[0] << " , " << residual[1] << std::endl;

		return true;
	}

private:
	cv::Vec2f image_point_;
	cv::Vec4f model_point_;
	int width_;
	int height_;
	double focal_;
};

//PoseEstimateResidual_Analytic可以不看，用的上面的PoseEstimateResidual_AutoDiff
// lower effecicy than PoseEstimateResidual_AutoDiff
class PoseEstimateResidual_Analytic : public ceres::SizedCostFunction<2 /* number of residuals */, 6 /* size of first parameter */>
{
public:
	PoseEstimateResidual_Analytic(cv::Vec2f image_point, cv::Vec4f model_point, int width, int height,double focal) :
		image_point_(image_point), model_point_(model_point), width_(width), height_(height),focal_(focal) {}

	virtual bool Evaluate(double const* const* parameters,
		double* residuals,
		double** jacobians) const
	{
		// parameters to be optimized
		double rot_x_pitch = parameters[0][0];
		double rot_y_yaw = parameters[0][1];
		double rot_z_roll = parameters[0][2];
		double tx = parameters[0][3];
		double ty = parameters[0][4];
		double tz = parameters[0][5];
		//double focal = parameters[0][6];
		double focal = focal_;

		// 3D point as input
		double ox = model_point_[0];
		double oy = model_point_[1];
		double oz = model_point_[2];
		double ow = 1.0;

		cv::Mat rot_mtx_x = (cv::Mat_<double>(4, 4) <<
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, cos(rot_x_pitch), -sin(rot_x_pitch), 0.0f,
			0.0f, sin(rot_x_pitch), cos(rot_x_pitch), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		double ox2 = ox;
		double oy2 = cos(rot_x_pitch)*oy - sin(rot_x_pitch)*oz;
		double oz2 = sin(rot_x_pitch)*oy + cos(rot_x_pitch)*oz;
		double ow2 = ow;

		cv::Mat rot_mtx_y = (cv::Mat_<double>(4, 4) <<
			cos(rot_y_yaw), 0.0f, sin(rot_y_yaw), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sin(rot_y_yaw), 0.0f, cos(rot_y_yaw), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		double ox3 = cos(rot_y_yaw)*ox2 + sin(rot_y_yaw)*oz2;
		double oy3 = oy2;
		double oz3 = -sin(rot_y_yaw)*ox2 + cos(rot_y_yaw)*oz2;
		double ow3 = ow2;

		cv::Mat rot_mtx_z = (cv::Mat_<double>(4, 4) <<
			cos(rot_z_roll), -sin(rot_z_roll), 0.0f, 0.0f,
			sin(rot_z_roll), cos(rot_z_roll), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		double ox4 = cos(rot_z_roll)*ox3 - sin(rot_z_roll)*oy3;
		double oy4 = sin(rot_z_roll)*ox3 + cos(rot_z_roll)*oy3;
		double oz4 = oz3;
		double ow4 = ow3;

		cv::Mat t_mtx = (cv::Mat_<double>(4, 4) <<
			1.0f, 0.0f, 0.0f, tx,
			0.0f, 1.0f, 0.0f, ty,
			0.0f, 0.0f, 1.0f, tz,
			0.0f, 0.0f, 0.0f, 1.0f);
		double ox5 = ox4 + tx;
		double oy5 = oy4 + ty;
		double oz5 = oz4 + tz;
		double ow5 = ow4;

		cv::Mat mv_mtx = t_mtx*rot_mtx_z*rot_mtx_y*rot_mtx_x;

		double aspect = double(width_) / double(height_);
		double l_ = (-1.0*aspect);
		double r_ = (1.0*aspect);
		double b_ = (-1.0);
		double t_ = (1.0);
		double n_ = focal;
		double f_ = 1000.0;
	
		double ox6 = (2.0)*n_ / (r_ - l_)*ox5;
		double oy6 = (2.0)*n_ / (t_ - b_)*oy5;
		double oz6 = -(n_ + f_) / (f_ - n_)*oz5 + ((-2.0) * n_ * f_) / (f_ - n_);
		double ow6 = -oz5;

		// transformed to image coordinates
		double fx1 = (0.5)*(ox6 / ow6 + (1.0)) * (width_);
		double fy1 = (0.5)*(oy6 / ow6 + (1.0)) * (height_);

		double predicted_x = fx1;
		double predicted_y = fy1;

		// The error is the difference between the predicted and observed position.
		residuals[0] = predicted_x - image_point_[0];
		residuals[1] = predicted_y - image_point_[1];

		// derivative 
		double a11 = 2.0 * n_ / (r_ - l_); //projection matrix
		double a22 = 2.0  * n_ / (t_ - b_);
		double a33 = -(n_ + f_) / (f_ - n_);
		double a34 = (-2.0 * n_ * f_) / (f_ - n_);
		double a43 = -1.0; //-1
		double w11 = mv_mtx.at<double>(0, 0);
		double w12 = mv_mtx.at<double>(0, 1);
		double w13 = mv_mtx.at<double>(0, 2);
		double w14 = mv_mtx.at<double>(0, 3);
		double w21 = mv_mtx.at<double>(1, 0);
		double w22 = mv_mtx.at<double>(1, 1);
		double w23 = mv_mtx.at<double>(1, 2);
		double w24 = mv_mtx.at<double>(1, 3);
		double w31 = mv_mtx.at<double>(2, 0);
		double w32 = mv_mtx.at<double>(2, 1);
		double w33 = mv_mtx.at<double>(2, 2);
		double w34 = mv_mtx.at<double>(2, 3);
		float denom = ow6;//a43*(w31*p_center.x + w32*p_center.y + w33*p_center.z + w34);
		//float Du_Dfocal = 2.0 / (r_ - l_) * (w11*ox + w12*oy + w13*oz + w14) / denom;
		//float Dv_Dfocal = 2.0 / (t_ - b_) * (w21*ox + w22*oy + w23*oz + w24) / denom;
		float Du_DTx = a11 / denom;
		float Dv_DTx = 0;
		float Du_DTy = 0;
		float Dv_DTy = a22 / denom;
		float Du_DTz = (-a43) * ox6 / (ow6*ow6);
		float Dv_DTz = (-a43) * oy6 / (ow6*ow6);
		float Wx_Rx = (cos(rot_x_pitch)*sin(rot_y_yaw)*cos(rot_z_roll) + sin(rot_x_pitch)*sin(rot_z_roll))*oy + (-sin(rot_x_pitch)*sin(rot_y_yaw)*cos(rot_z_roll) + cos(rot_x_pitch)*sin(rot_z_roll))*oz;
		float Wx_Ry = (-sin(rot_y_yaw)*cos(rot_z_roll))*ox + (sin(rot_x_pitch)*cos(rot_y_yaw)*cos(rot_z_roll))*oy + (cos(rot_x_pitch)*cos(rot_y_yaw)*cos(rot_z_roll))*oz;
		float Wx_Rz = (-sin(rot_z_roll)*cos(rot_y_yaw))*ox + (-sin(rot_x_pitch)*sin(rot_y_yaw)*sin(rot_z_roll) - cos(rot_x_pitch)*cos(rot_z_roll))*oy + (-cos(rot_x_pitch)*sin(rot_y_yaw)*sin(rot_z_roll) + sin(rot_x_pitch)*cos(rot_z_roll))*oz;
		float Wy_Rx = (cos(rot_x_pitch)*sin(rot_y_yaw)*sin(rot_z_roll) - sin(rot_x_pitch)*cos(rot_z_roll))*oy + (-sin(rot_x_pitch)*sin(rot_y_yaw)*sin(rot_z_roll) - cos(rot_x_pitch)*cos(rot_z_roll))*oz;
		float Wy_Ry = (-sin(rot_y_yaw)*sin(rot_z_roll))*ox + (sin(rot_x_pitch)*cos(rot_y_yaw)*sin(rot_z_roll))*oy + (cos(rot_x_pitch)*cos(rot_y_yaw)*sin(rot_z_roll))*oz;
		float Wy_Rz = (cos(rot_y_yaw)*cos(rot_z_roll))*ox + (sin(rot_x_pitch)*sin(rot_y_yaw)*cos(rot_z_roll) - cos(rot_x_pitch)*sin(rot_z_roll))*oy + (cos(rot_x_pitch)*sin(rot_y_yaw)*cos(rot_z_roll) + sin(rot_x_pitch)*sin(rot_z_roll))*oz;
		float Wz_Rx = cos(rot_x_pitch)*cos(rot_y_yaw)*oy - sin(rot_x_pitch)*cos(rot_y_yaw)*oz;
		float Wz_Ry = -cos(rot_y_yaw)*ox - sin(rot_x_pitch)*sin(rot_y_yaw)*oy - cos(rot_x_pitch)*sin(rot_y_yaw)*oz;
		float Wz_Rz = 0;
		float Du_DRx = (a11*Wx_Rx*ow6 - ox6*a43*Wz_Rx) / (ow6*ow6);
		float Du_DRy = (a11*Wx_Ry*ow6 - ox6*a43*Wz_Ry) / (ow6*ow6);
		float Du_DRz = (a11*Wx_Rz*ow6 - ox6*a43*Wz_Rz) / (ow6*ow6);
		float Dv_DRx = (a22*Wy_Rx*ow6 - oy6*a43*Wz_Rx) / (ow6*ow6);
		float Dv_DRy = (a22*Wy_Ry*ow6 - oy6*a43*Wz_Ry) / (ow6*ow6);
		float Dv_DRz = (a22*Wy_Rz*ow6 - oy6*a43*Wz_Rz) / (ow6*ow6);
		//jacobians[i] is an array that contains CostFunction::num_residuals_ x CostFunction::parameter_block_sizes_[i] elements.
		//Each Jacobian matrix is stored in row - major order, i.e., jacobians[i][r * parameter_block_size_[i] + c] = ?residual[r] / ?parameters[i][c]	
		if (jacobians != NULL && jacobians[0] != NULL)
		{
			jacobians[0][0 * 6 + 0] = Du_DRx;
			jacobians[0][1 * 6 + 0] = Dv_DRx;
			jacobians[0][0 * 6 + 1] = Du_DRy;
			jacobians[0][1 * 6 + 1] = Dv_DRy;
			jacobians[0][0 * 6 + 2] = Du_DRz;
			jacobians[0][1 * 6 + 2] = Dv_DRz;
			jacobians[0][0 * 6 + 3] = Du_DTx;
			jacobians[0][1 * 6 + 3] = Dv_DTx;
			jacobians[0][0 * 6 + 4] = Du_DTy;
			jacobians[0][1 * 6 + 4] = Dv_DTy;
			jacobians[0][0 * 6 + 5] = Du_DTz;
			jacobians[0][1 * 6 + 5] = Dv_DTz;
		}
		return true;
	}

private:
	double focal_;
	cv::Vec2f image_point_;
	cv::Vec4f model_point_;
	int width_;
	int height_;
};

struct ShapeEstimateByLandmarkResidual_AutoDiff
{
	ShapeEstimateByLandmarkResidual_AutoDiff(cv::Vec2f image_point, cv::Vec3f mean_shape, std::vector<cv::Vec3f> basis_shape, std::vector<double> poseParameters,
		int width, int height,double weight) :
		image_point_(image_point), mean_shape_(mean_shape), basis_shape_(basis_shape), poseParameters_(poseParameters), width_(width), height_(height), weight_(weight){}

	template<typename T>
	bool operator()(const T* const X, T* residual)const
	{
		// 3D point as input 自然表情作为输入
		T ox = T(mean_shape_[0]);
		T oy = T(mean_shape_[1]);
		T oz = T(mean_shape_[2]);
		T ow = T(1.0);
		for (int i = 0; i < NUM_BLENDSHAPE; i++)
		{
			//一个新的表情F（x）=b0+△Bx,△B=[b1-b0,...,bn-b0],x=[x1,...xn]^T是blendshape的权重
			ox += X[i] * T(basis_shape_[i][0] - mean_shape_[0]);
			oy += X[i] * T(basis_shape_[i][1] - mean_shape_[1]);
			oz += X[i] * T(basis_shape_[i][2] - mean_shape_[2]);
		}
		T rot_x_pitch = T(poseParameters_[0]);
		T rot_y_yaw = T(poseParameters_[1]);
		T rot_z_roll = T(poseParameters_[2]);
		T tx = T(poseParameters_[3]);
		T ty = T(poseParameters_[4]);
		T tz = T(poseParameters_[5]);
		T focal = T(poseParameters_[6]);

		T ox2 = ox;
		T oy2 = cos(rot_x_pitch)*oy - sin(rot_x_pitch)*oz;
		T oz2 = sin(rot_x_pitch)*oy + cos(rot_x_pitch)*oz;
		T ow2 = ow;

		T ox3 = cos(rot_y_yaw)*ox2 + sin(rot_y_yaw)*oz2;
		T oy3 = oy2;
		T oz3 = -sin(rot_y_yaw)*ox2 + cos(rot_y_yaw)*oz2;
		T ow3 = ow2;

		T ox4 = cos(rot_z_roll)*ox3 - sin(rot_z_roll)*oy3;
		T oy4 = sin(rot_z_roll)*ox3 + cos(rot_z_roll)*oy3;
		T oz4 = oz3;
		T ow4 = ow3;

		T ox5 = ox4 + tx;
		T oy5 = oy4 + ty;
		T oz5 = oz4 + tz;
		T ow5 = ow4;

		/*屏幕宽高比是一个必要的参数，因为我们要在一个宽高相等的单位化的盒子内展示所有的坐标系，而通常屏幕
		的宽度是大于屏幕的高度的，所以需要在水平方向上的轴线上布置更加密集的坐标点，竖直方向上相对稀疏。
		这样经过变换，我们就可以在保证看到更宽阔屏幕图像的需求下，根据X轴在单位盒子空间内的比例，在X方向
		上添加更多的X坐标。*/
		T aspect = T(width_) / T(height_);
		//以下6个参数中l_,b_,n_构成近裁剪平面左下角点，另外三个构成远裁剪平面右上角点
		T l_ = T(-1.0*aspect);//left
		T r_ = T(1.0*aspect);//right
		T b_ = T(-1.0);
		T t_ = T(1.0);
		T n_ = focal;//near.Z轴近平面的位置：近平面用于将离相机太近的物体裁剪掉；
		T f_ = T(1000.0);//far Z轴远平面的位置：远平面用于将离相机太远的物体裁剪掉

		//最终的透视变换矩阵如下
		//cv::Mat p_mtx = (cv::Mat_<T>(4, 4) <<
		 //	T(2.0)*n_/(r_-l_), 0.0, 0.0, 0.0,
		//	0.0, T(2.0)*n_/(t_-b_), 0.0, 0.0,
		//	0.0, 0.0, -(n_ + f_) / (f_ - n_), (T(-2.0) * n_ * f_) / (f_ - n_),
		 //	0.0, 0.0, -1.0, 0.0);
		T ox6 = T(2.0)*n_ / (r_ - l_)*ox5;
		T oy6 = T(2.0)*n_ / (t_ - b_)*oy5;
		//以下请看【一步步学OpenGL 12】 -《透视投影》 - Mr_厚厚的博客 - CSDN博客 得到的A,B就是下面这条公式
		T oz6 = -(n_ + f_) / (f_ - n_)*oz5 + (T(-2.0) * n_ * f_) / (f_ - n_);
		T ow6 = -oz5;

		// transformed to image coordinates
		T fx1 = T(0.5)*(ox6 / ow6 + T(1.0)) * T(width_);
		T fy1 = T(0.5)*(oy6 / ow6 + T(1.0)) * T(height_);

		T predicted_x = fx1;
		T predicted_y = fy1;

		// The error is the difference between the predicted and observed position.
		residual[0] = T(weight_) * (predicted_x - T(image_point_[0]));
		residual[1] = T(weight_) * (predicted_y - T(image_point_[1]));

		return true;
	}

private:
	cv::Vec2f image_point_;
	cv::Vec3f mean_shape_;
	std::vector<cv::Vec3f> basis_shape_;
	std::vector<double> poseParameters_;
	int width_, height_;
	double weight_;
};

struct ShapeTextureEstimateResidual_Reg
{
	ShapeTextureEstimateResidual_Reg(const double& eigenvalue, const double& weight)
		: eigenvalue_(eigenvalue),weight_(weight) {}

	template<typename T>
	bool operator()(const T* const X, T* residual)const
	{
		for (int i = 0; i < NUM_BLENDSHAPE; i++)
		{
			residual[i] = T(weight_)*X[i] / T(eigenvalue_);
		}
		return true;
	}

private:
	const double eigenvalue_;
	const double weight_;
};


// expression regularization term,  min ||w||
struct ExpressionRegularizationTerm 
{
	ExpressionRegularizationTerm(double weight, int nBs) : weight_(weight), nBs_(nBs){}

	bool operator()(const double *const *w, double *residual) const 
	{
		for (int i = 0; i<nBs_; ++i)
		{
			residual[i] = sqrt(fabs(w[0][i])) * weight_;//fabs求浮点数x的绝对值
		}
		return true;
	}
	double weight_;
	int nBs_;
};

