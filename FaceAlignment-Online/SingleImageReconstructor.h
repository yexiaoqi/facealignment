#pragma once

#include <fstream>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <Eigen\Eigen>
#include <opencv2/opencv.hpp>

#include "CostFunctions.h"
//#include "Utils.h"

#include "DrawRectTool.h"
#include "LandmarkModifierTool.h"
//#include "landmark\FaceShapeRegressor.hpp"
#include "dlib_mark_detection.h"

//#define USE_ANALYTIC_COST_FUNCTIONS 1
#define FACESHAPE_PNTNUM 68

// Optimization Constraints
template <typename T>
struct Constraint 
{
	Constraint() : vidx(-1), weight(1.0), data(T()) {}

	std::vector<cv::Vec3f> basis_shape;
	cv::Vec3f mean_pt;
	int vidx;         // vertex index
	double weight;    // weight for this constraint
	T data;
};
using Constraint2D = Constraint<cv::Vec2f>;
using Constraint3D = Constraint<cv::Vec3f>;
using Constraint2D_Depth = Constraint<cv::Vec3f>;


class SingleImageReconstructor 
{
public:
	SingleImageReconstructor() {}
	~SingleImageReconstructor() {}

	void Init();
	void PrepareConstraints();
	void SetInputImage(cv::Mat input_img);
	Eigen::Vector2d ProjectPoint(Eigen::Vector3d pt3d);
	double ComputeError();
	void LoadIndex3D(std::string filename);
	void UpdatePts2D(std::string landmark_file);
	void UpdatePts2D(std::vector<cv::Point2d> pts2D);
	void UpdatePts3D();
	void UpdateLandmarkPos();
	void UpdateModelMat();
	void UpdateProjectionMat();
	void UpdateMesh(std::vector<double> coef);
	void poseParams2Vec(std::vector<double>& params);
	void poseParamsVec2pose(std::vector<double>& params);
	void poseFocalParams2Vec(std::vector<double>& params);
	void printPoseFocalParams();
	/////////////////////////////////////////////////////////////  Optimization Part ///////////////////////////////////////////////////////////////////
	void ResetParams();
	// Landmark
	void LandmarkDetect(cv::Mat input_img);
	// Ceres Solver
	void OptimizeFocalLength();
	void OptimizeByLandmark_PoseImp();
	void OptimizeByLandmark_BsImp();
	void OptimizeByLandmark_BsImpMouth();
	void Fit();
	void FitFirstFrame();
	// My GN Solver
	void OptimizeFocalLengthGN();
	void OptimizeByLandmark_PoseImpGN();
	void OptimizeByLandmark_BsImpGN();
	void FitGN();
	void FitFirstFrameGN();


public:

	glm::mat4 model_mat_, camera_view_, projection_;
	Eigen::MatrixXf landmark3DPos_;
	Mesh recon_mesh_;
	// Parameters
	ModelParametersBs recon_model_;
	ModelParametersBs recon_modelLast_;
	CameraParameters recon_camera_;
	Eigen::VectorXf paramsVector;
	// Data
	Blendshape bsData_;
	cv::Mat input_img_;
	int imgWidth_, imgHeight_;
	// Landmark data
	int numPts_;
	std::vector<Eigen::Vector3d> pts3D_;
	std::vector<cv::Point2d> pts2D_;
	std::vector<int> index3D_;
	std::vector<Constraint2D> constraints2D_;
	// Landmark
	float m_landmarks_clm[2 * FACESHAPE_PNTNUM];
	float px_pre[FACESHAPE_PNTNUM], py_pre[FACESHAPE_PNTNUM], px_cur[FACESHAPE_PNTNUM], py_cur[FACESHAPE_PNTNUM];
	int upleft_x, upleft_y, downright_x, downright_y;
	bool on_the_fly;
	std::vector<cv::Rect> faces;
	//CascadeClassifier facedetector;
	DlibMarkDetector *dlibMarkerDetector;

};



