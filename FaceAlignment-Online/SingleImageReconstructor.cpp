#include "SingleImageReconstructor.h"
#include"dlib_mark_detection.h"

#include <Windows.h>

const char* str_facemdlpath = "data/model/haarcascade_frontalface_301911.xml";
const char* str_fshpmdlpath = "data/model/ilc_78pnts_basic.bmd";
const char* str_dlibModelPath = "shape_predictor_68_face_landmarks.dat";

// todo : fix bug of faceshape detector
//CxFaceShapeRegressor fshpdetector(str_fshpmdlpath);

void SingleImageReconstructor::ResetParams() 
{	
	this->recon_model_.ResetBsCoeff();
	return;
}

void SingleImageReconstructor::LandmarkDetect(cv::Mat input_img) 
{
	pts2D_.resize(this->numPts_);
	std::vector<dlib::point> pts;
	this->dlibMarkerDetector->FaceDectionAndAlignment(input_img, pts);
	for (int kk = 0; kk < this->numPts_ ; kk++) 
	{
		if (kk < pts.size())
		{
			this->pts2D_[kk].x = pts[kk](0);
			this->pts2D_[kk].y = pts[kk](1);
		}
		else 
		{
			this->pts2D_[kk].x = 0.0;	// jisy : fake data to enable the costfuntion
			this->pts2D_[kk].y = 0.0;
		}	
	}
}

void SingleImageReconstructor::Fit()
{
	double E0 = ComputeError();
	double E1 = E0;
	double timeElapse = 0;
	//std::cout << "Init Error = " << E << "-------------------------------" << std::endl;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER freq;
	int maxIteration = 1;
	//this->ResetParams();
	for (int iter = 0; iter < maxIteration; iter++) 
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);
		//OptimizeByLandmark_PoseImp();
		OptimizeByLandmark_BsImp();
		//OptimizeByLandmark_BsImpMouth();
		QueryPerformanceCounter(&end);
		timeElapse = (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart;
		//printf("time cost : %.10f seconds \n", timeElapse);
	}
	E1 = ComputeError();
	this->recon_modelLast_ = this->recon_model_;
	std::cout << "time = " << timeElapse << "Error Change = " << E0-E1 << std::endl;
	//std::cout << "Final Error = " << E << "-------------------------------" << std::endl;
	return;
}

void SingleImageReconstructor::FitFirstFrame()
{
	double E = ComputeError();//计算误差
	//std::cout << "Init Error = " << E << "-------------------------------" << std::endl;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER freq;
	int maxIteration = 10;
	this->ResetParams();//重置bs参数
	for (int iter = 0; iter < maxIteration; iter++) //for每次迭代：
	{
		QueryPerformanceFrequency(&freq);//得到频率
		QueryPerformanceCounter(&start);//计时开始
		OptimizeByLandmark_PoseImp();//优化landmark_pose
		OptimizeByLandmark_BsImp();//优化landmark_bs
		OptimizeByLandmark_BsImpMouth();
		QueryPerformanceCounter(&end);//计时结束
		//printf("time cost : %.10f seconds \n", (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart);
	}
	E = ComputeError();//计算误差
	this->recon_modelLast_ = this->recon_model_;

	//std::cout << "Final Error = " << E << "-------------------------------" << std::endl;
	return;
}

void SingleImageReconstructor::OptimizeFocalLength()
{
	std::cout << std::endl << "Start OptimizeFocalLength..." << std::endl;
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);//硬件支持的高精度计算的频率
	QueryPerformanceCounter(&start);//精确计算执行时间start
	// Implement your optimization code
	double delta_F = 0.3;//初始焦距存在Init_F中
	double Init_F = this->recon_camera_.focal_;
	int N_F = 10;
	double minErr_F = 99999999;
	double best_F = Init_F;
	//在Init - 10 * 3到Init + 10 * 3中，以0.3为步长寻找最佳焦距
	for (int kk = 0; kk <= 2 * N_F; kk++)
	{		
		double delta1 = delta_F * (kk - N_F);
		this->recon_camera_.focal_ = Init_F - delta1;
		if (this->recon_camera_.focal_ <= 0) this->recon_camera_.focal_ = 0.001;//if 焦距<0:设置焦距为0.001
		ResetParams();//	重置bs系数
		OptimizeByLandmark_PoseImp();//	优化landmark_pose
		OptimizeByLandmark_BsImp();//	优化landmark_bs
		double err_F = ComputeError();//	计算误差存为err_F
		if (err_F < minErr_F)//	if err_F<minErr_F : minErr_F = err_F, best_F = 焦距
		{
			minErr_F = err_F;
			best_F = recon_camera_.focal_;
		}
	}
	//		在屏幕上输出最佳焦距和误差
	std::cout << "=========================================== Finished =============================================================" << std::endl;
	std::cout << "Focal Search in " << Init_F - N_F * delta_F << " -> " << delta_F << " -> " << Init_F + N_F * delta_F << std::endl;
	std::cout << "Found Best Focal = " << best_F << " , Error = " << minErr_F << std::endl;
	this->recon_camera_.focal_ = best_F;//		现在的焦距 = 最佳焦距
	ResetParams();//		重置bs系数

	QueryPerformanceCounter(&end);//		精确计算执行时间end
	printf("time cost : %.10f seconds \n", (double)(end.QuadPart - start.QuadPart) / (double)freq.QuadPart);//		打印时间
}

void SingleImageReconstructor::OptimizeByLandmark_PoseImp() 
{ 
	double E = 0.0;
	std::vector<double> poseParams;
	poseParams2Vec(poseParams);//将recon_model_的旋转和位移传入poseParams
	double focal = recon_camera_.focal_;
	ceres::Problem problem0;
	for (int kk = 0; kk < this->numPts_; kk++) //	numPts_来源：mImgRecon.numPts_ = m_numLandmark;为68
	{
		if (kk >16)
		{
			{
				float px_2d = this->pts2D_[kk].x;
				float py_2d = this->pts2D_[kk].y;
				cv::Vec2f pt_2d(px_2d, py_2d);
				float px_3d = pts3D_[kk](0);
				float py_3d = pts3D_[kk](1);
				float pz_3d = pts3D_[kk](2);
				cv::Vec4f pt_3d(px_3d, py_3d, pz_3d, 1.0);
				//Remark: AutoDiff is better than analytic in the aspect of convergence speed (less iteration steps) 
#ifdef USE_ANALYTIC_COST_FUNCTIONS
				PoseEstimateResidual_Analytic* pResidualX = new PoseEstimateResidual_Analytic(pt_2d, pt_3d, imgWidth_, imgHeight_,focal);
				problem0.AddResidualBlock(pResidualX, NULL, poseParams.data());
#else
				PoseEstimateResidual_AutoDiff* pResidualX = new PoseEstimateResidual_AutoDiff(pt_2d, pt_3d, imgWidth_, imgHeight_, focal);
				problem0.AddResidualBlock(new ceres::AutoDiffCostFunction<PoseEstimateResidual_AutoDiff, 2, 6>(pResidualX), NULL, poseParams.data());
#endif
			}
		}
	}
	ceres::Solver::Options m_options0;
	ceres::Solver::Summary m_summary0;
	m_options0.max_num_iterations = 20;
	m_options0.linear_solver_type = ceres::DENSE_QR;
	m_options0.minimizer_progress_to_stdout = false;
	ceres::Solve(m_options0, &problem0, &m_summary0);
	poseParamsVec2pose(poseParams);//将poseParams的旋转和位移传入recon_model_
	// Re-estimate the pose with all landmarks, including the contour landmarks:
	ceres::Problem problem1;
	for (int kk = 0; kk < this->numPts_; kk++)
	{
		float px_2d = this->pts2D_[kk].x;
		float py_2d = this->pts2D_[kk].y;
		cv::Vec2f pt_2d(px_2d, py_2d);
		float px_3d = pts3D_[kk](0);
		float py_3d = pts3D_[kk](1);
		float pz_3d = pts3D_[kk](2);
		cv::Vec4f pt_3d(px_3d, py_3d, pz_3d, 1.0);

#ifdef USE_ANALYTIC_COST_FUNCTIONS
		PoseEstimateResidual_Analytic* pResidualX = new PoseEstimateResidual_Analytic(pt_2d, pt_3d, imgWidth_, imgHeight_,focal);
		problem1.AddResidualBlock(pResidualX, NULL, poseParams.data());
#else
		PoseEstimateResidual_AutoDiff* pResidualX = new PoseEstimateResidual_AutoDiff(pt_2d, pt_3d, imgWidth_, imgHeight_, focal);
		problem1.AddResidualBlock(new ceres::AutoDiffCostFunction<PoseEstimateResidual_AutoDiff, 2, 6>(pResidualX), NULL, poseParams.data());
#endif

	}
	ceres::Solver::Options m_options1;
	ceres::Solver::Summary m_summary1;
	m_options1.max_num_iterations = 10;
	m_options1.linear_solver_type = ceres::DENSE_QR;
	m_options1.minimizer_progress_to_stdout = false;
	ceres::Solve(m_options1, &problem1, &m_summary1);
	poseParamsVec2pose(poseParams);
	E = ComputeError();
	//std::cout << "Pose Optimization, Error = " << E << "-------------------------------" << std::endl;
}

void SingleImageReconstructor::OptimizeByLandmark_BsImp() 
{
	double E = 0.0;
	// solver the blendshape coefficient
	std::vector<double> poseParameters_vec;
	poseFocalParams2Vec(poseParameters_vec);//将recon_model_的旋转位移和焦距传入poseParameters_vec
	ceres::Problem problem2;
	for (int kk = 0; kk < this->numPts_; kk++) 
	{
		//if(kk>16)
		/*if (kk <= 52 || kk >= 70)*/ 
		{
			float px_2d = this->pts2D_[kk].x;
			float py_2d = this->pts2D_[kk].y;
			cv::Vec2f pt_2d(px_2d, py_2d);
			ShapeEstimateByLandmarkResidual_AutoDiff * pResidualX = new ShapeEstimateByLandmarkResidual_AutoDiff(pt_2d, 
				this->constraints2D_[kk].mean_pt, this->constraints2D_[kk].basis_shape, poseParameters_vec, imgWidth_, imgHeight_,this->constraints2D_[kk].weight);
			problem2.AddResidualBlock(new ceres::AutoDiffCostFunction<ShapeEstimateByLandmarkResidual_AutoDiff, 2, NUM_BLENDSHAPE>(pResidualX), NULL, recon_model_.params_.data() + 1);
		}
	}
	// add regularization term
	ceres::DynamicNumericDiffCostFunction<ExpressionRegularizationTerm> *reg_cost_function =
		new ceres::DynamicNumericDiffCostFunction<ExpressionRegularizationTerm>(
		new ExpressionRegularizationTerm(10.0, NUM_BLENDSHAPE)
			);
	reg_cost_function->AddParameterBlock(recon_model_.params_.size() - 1);
	reg_cost_function->SetNumResiduals(recon_model_.params_.size() - 1);
	problem2.AddResidualBlock(reg_cost_function, NULL, recon_model_.params_.data() + 1);
	for (int i = 0; i<recon_model_.params_.size() - 1; ++i) {
		problem2.SetParameterLowerBound(recon_model_.params_.data() + 1, i, 0.0);
		problem2.SetParameterUpperBound(recon_model_.params_.data() + 1, i, 1.0);
	}
	ceres::Solver::Options m_options2;
	m_options2.max_num_iterations = 30;//30,
	m_options2.num_threads = 8;
	m_options2.num_linear_solver_threads = 8;
	m_options2.initial_trust_region_radius = 1.0;//1
	m_options2.min_trust_region_radius = 0.5;//0.5
	m_options2.max_trust_region_radius = 2.0;//2.0
	m_options2.min_lm_diagonal = 1.0;//1
	m_options2.max_lm_diagonal = 1.0;//1
	ceres::Solver::Summary m_summary2;
	ceres::Solve(m_options2, &problem2, &m_summary2);
	m_options2.max_num_iterations = 2;//2,//?
	m_options2.minimizer_type = ceres::LINE_SEARCH;
	m_options2.line_search_direction_type = ceres::LBFGS;
	ceres::Solve(m_options2, &problem2, &m_summary2);//?
	// update current landmark model
	this->UpdatePts3D();
	E = ComputeError();
	//std::cout << "Blendshape Optimization, Error = " << E << "-------------------------------" << std::endl;

}

void SingleImageReconstructor::OptimizeByLandmark_BsImpMouth(){

	double E = 0.0;
	// solve mouth blendshape parameters 19-45
	// landmark index = 30 - 52
	//jisy fix:MOUTH MARKER 48-67
	//const int NUM_MOUTH_BS = 20;
	const int NUM_MOUTH_BS = 27;
	std::vector<double> mouthBS(NUM_MOUTH_BS);
	for (int kk = 0; kk < NUM_MOUTH_BS; kk++) mouthBS[kk] = 0.0;

	std::vector<double> poseParameters_vec;
	poseFocalParams2Vec(poseParameters_vec);
	ceres::Problem problem2;
	for (int kk = 0; kk < this->numPts_; kk++) 
	{
		//if (kk <= 52 || kk >= 30)
		if (47<kk<68 ) { // Mouth landmarks //jisy landmarker fix
			float px_2d = this->pts2D_[kk].x;
			float py_2d = this->pts2D_[kk].y;
			cv::Vec2f pt_2d(px_2d, py_2d);
			ShapeEstimateByLandmarkResidual_AutoDiff * pResidualX = new ShapeEstimateByLandmarkResidual_AutoDiff(pt_2d,
				this->constraints2D_[kk].mean_pt, this->constraints2D_[kk].basis_shape, poseParameters_vec, imgWidth_, imgHeight_, this->constraints2D_[kk].weight);
			problem2.AddResidualBlock(new ceres::AutoDiffCostFunction<ShapeEstimateByLandmarkResidual_AutoDiff, 2, NUM_MOUTH_BS>(pResidualX), NULL, mouthBS.data());
		}
	}
	// add regularization term
	ceres::DynamicNumericDiffCostFunction<ExpressionRegularizationTerm> *reg_cost_function =
		new ceres::DynamicNumericDiffCostFunction<ExpressionRegularizationTerm>(
		new ExpressionRegularizationTerm(10.0,NUM_MOUTH_BS)
		);
	reg_cost_function->AddParameterBlock(NUM_MOUTH_BS);
	reg_cost_function->SetNumResiduals(NUM_MOUTH_BS);
	problem2.AddResidualBlock(reg_cost_function, NULL, mouthBS.data());
	for (int i = 0; i<NUM_MOUTH_BS; ++i) {
		problem2.SetParameterLowerBound(mouthBS.data(), i, 0.0);
		problem2.SetParameterUpperBound(mouthBS.data(), i, 1.0);
	}
	ceres::Solver::Options m_options2;
	m_options2.max_num_iterations = 30;
	m_options2.num_threads = 8;
	m_options2.num_linear_solver_threads = 8;
	m_options2.initial_trust_region_radius = 1.0;
	m_options2.min_trust_region_radius = 0.5;
	m_options2.max_trust_region_radius = 2.0;
	m_options2.min_lm_diagonal = 1.0;
	m_options2.max_lm_diagonal = 1.0;
	ceres::Solver::Summary m_summary2;
	ceres::Solve(m_options2, &problem2, &m_summary2);
	m_options2.max_num_iterations = 2;
	m_options2.minimizer_type = ceres::LINE_SEARCH;
	m_options2.line_search_direction_type = ceres::LBFGS;
	ceres::Solve(m_options2, &problem2, &m_summary2);
	// copy back to recon parameters 19-45
	for (int kk = 0; kk < NUM_MOUTH_BS; kk++)
		this->recon_model_.params_[kk + 21] = mouthBS[kk];

	this->UpdatePts3D();
	E = ComputeError();
	//std::cout << "Blendshape Optimization, Error = " << E << "-------------------------------" << std::endl;
}
//////////////////////////////////////////////////////////// Gauss Newton Solver //////////////////////////////////////////////////////
void SingleImageReconstructor::OptimizeFocalLengthGN(){


}

void SingleImageReconstructor::OptimizeByLandmark_PoseImpGN(){


	// These will be the final 2D and 3D points used for the fitting:
	//vector<cv::Vec4f> model_points; // the points in the 3D shape model
	//vector<int> vertex_indices; // their vertex indices
	//vector<cv::Vec2f> image_points; // the corresponding 2D landmark points
	//model_points.clear();
	//vertex_indices.clear();
	//image_points.clear();
	//float* contour = new float[n_strip * 3]; //image_x, image_y, vertex_index;
	////------------------------------
	////-----first roughly pose estimation by using all the inner corresponding landmarks
	//Problem problem_0;
	//for (int kk = 0; kk < n_landmarks_clm; kk++)//add landmarks
	//{
	//	if (kk <= 52 || kk >= 70)//inner point 
	//	{
	//		//Use some of inner points to accelarate
	//		if (kk == 0 || kk == 4 || kk == 5 || kk == 9 || kk == 10 || kk == 14 || kk == 18 || kk == 22 || kk == 29 || kk == 30 || kk == 32 || kk == 33 || kk == 36 || kk == 39 || kk == 42)
	//		{
	//			float fx1 = m_landmarks_clm[2 * kk];
	//			float fy1 = m_landmarks_clm[2 * kk + 1];
	//			cv::Vec2f lmp_2(fx1, fy1);
	//			int idx1 = index_landmarks_clm[kk];
	//			int offset1 = 3 * idx1;
	//			cv::Vec4f lmp_3(gModels->get_cur_face_by_index(offset1), gModels->get_cur_face_by_index(offset1 + 1), gModels->get_cur_face_by_index(offset1 + 2), 1.0);

	//			//Remark: AutoDiff is better than analytic in the aspect of convergence speed (less iteration steps) 
	//			//PoseEstimateResidual_AutoDiff* pResidualX = new PoseEstimateResidual_AutoDiff(lmp_2, lmp_3,winWidth,winHeight,_zFar);
	//			//problem_0.AddResidualBlock(new AutoDiffCostFunction<PoseEstimateResidual_AutoDiff, 2, 7>(pResidualX), NULL, poseParameters);

	//			PoseEstimateResidual_Analytic* pResidualX = new PoseEstimateResidual_Analytic(lmp_2, lmp_3, winWidth, winHeight, _zFar);
	//			problem_0.AddResidualBlock(pResidualX, NULL, poseParameters);
	//		}
	//	}
	//}

	//Solver::Options m_options_0;
	//Solver::Summary m_summary_0;
	//m_options_0.max_num_iterations = 20;
	//m_options_0.linear_solver_type = ceres::DENSE_QR;
	//m_options_0.minimizer_progress_to_stdout = false;

	//Solve(m_options_0, &problem_0, &m_summary_0);

	////	cout << m_summary_0.FullReport() << "\n";
	//R_x = poseParameters[0];
	//R_y = poseParameters[1];
	//R_z = poseParameters[2];
	//T_x = poseParameters[3];
	//T_y = poseParameters[4];
	//T_z = poseParameters[5];
	//Focal = poseParameters[6];
	////cout << "R_x: " << R_x << endl;
	////cout << "R_y: " << R_y << endl;
	////cout << "R_z: " << R_z << endl;
	////cout << "T_x: " << T_x << endl;
	////cout << "T_y: " << T_y << endl;
	////cout << "T_z: " << T_z << endl;
	////cout << "Focal: " << Focal << endl << endl;

	////update projectionmatrix
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glFrustum(-1.0*aspect, 1.0*aspect, -1.0, 1.0, Focal, _zFar);
	//glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	////update modelviewmatrix
	//get_model_matrix_from_prams(poseParameters, modelMatrix_fit);
	////---------------------------------
	////---------------------------------
	//for (int lp = 0; lp < 1; lp++)
	//{
	//	//------------------------------
	//	// Given the estimated pose, find 2D-3D contour correspondences:
	//	int numC = 0;
	//	get_contour_for_each_strip(numC, contour); //cout << numC << endl;
	//	nC = numC;
	//	for (size_t i = 0; i < numC; i++) // only for display
	//	{
	//		contour_display[3 * i] = contour[3 * i];
	//		contour_display[3 * i + 1] = contour[3 * i + 1];
	//		contour_display[3 * i + 2] = contour[3 * i + 2];
	//	}
	//	// for each image contour landmark, search the nearest one from all 3D contour
	//	for (int kk = 0; kk < n_landmarks_clm; kk++)//add landmarks
	//	{
	//		if (kk <= 52 || kk >= 70)//inner point 
	//		{
	//			continue;
	//		}
	//		else // contour point 
	//		{
	//			float fx1 = m_landmarks_clm[2 * kk];
	//			float fy1 = m_landmarks_clm[2 * kk + 1];
	//			float minDist = 1.0e8;
	//			for (int t0 = 0; t0 < nC; t0++)
	//			{
	//				float fx0 = contour[3 * t0];
	//				float fy0 = contour[3 * t0 + 1];
	//				//float dist = abs(fx1 - fx0) + abs(fy1 - fy0);
	//				float dist = sqrt((fx1 - fx0)*(fx1 - fx0) + (fy1 - fy0)*(fy1 - fy0));
	//				if (dist < minDist)
	//				{
	//					minDist = dist;
	//					index_landmarks_clm[kk] = contour[3 * t0 + 2];
	//				}
	//			}
	//		}
	//	}
	//	//------------------------------
	//	// Re-estimate the pose with all landmarks, including the contour landmarks:
	//	Problem problem_1;
	//	for (int kk = 0; kk < n_landmarks_clm; kk++)//add landmarks
	//	{
	//		float fx1 = m_landmarks_clm[2 * kk];
	//		float fy1 = m_landmarks_clm[2 * kk + 1];
	//		cv::Vec2f lmp_2(fx1, fy1);

	//		int idx1 = index_landmarks_clm[kk];
	//		int offset1 = 3 * idx1;
	//		cv::Vec4f lmp_3(gModels->get_cur_face_by_index(offset1), gModels->get_cur_face_by_index(offset1 + 1), gModels->get_cur_face_by_index(offset1 + 2), 1.0);

	//		PoseEstimateResidual_AutoDiff* pResidualX = new PoseEstimateResidual_AutoDiff(lmp_2, lmp_3, winWidth, winHeight, _zFar);
	//		problem_1.AddResidualBlock(new AutoDiffCostFunction<PoseEstimateResidual_AutoDiff, 2, 7>(pResidualX), NULL, poseParameters);
	//	}

	//	Solver::Options m_options_1;
	//	Solver::Summary m_summary_1;
	//	m_options_1.max_num_iterations = 20;
	//	m_options_1.linear_solver_type = ceres::DENSE_QR;
	//	m_options_1.minimizer_progress_to_stdout = false;

	//	Solve(m_options_1, &problem_1, &m_summary_1);

	//	//cout << m_summary_1.FullReport() << "\n";
	//	R_x = poseParameters[0];
	//	R_y = poseParameters[1];
	//	R_z = poseParameters[2];
	//	T_x = poseParameters[3];
	//	T_y = poseParameters[4];
	//	T_z = poseParameters[5];
	//	Focal = poseParameters[6];
	//	//cout << "R_x: " << R_x << endl;
	//	//cout << "R_y: " << R_y << endl;
	//	//cout << "R_z: " << R_z << endl;
	//	//cout << "T_x: " << T_x << endl;
	//	//cout << "T_y: " << T_y << endl;
	//	//cout << "T_z: " << T_z << endl;
	//	//cout << "Focal: " << Focal << endl << endl;

	//	//update projectionmatrix
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadIdentity();
	//	glFrustum(-1.0*aspect, 1.0*aspect, -1.0, 1.0, Focal, _zFar);
	//	glGetFloatv(GL_PROJECTION_MATRIX, projectionMatrix);
	//	//update modelviewmatrix
	//	get_model_matrix_from_prams(poseParameters, modelMatrix_fit);
	//}
	////------------------------------
	//// Given the re-estimated pose, re-find 2D-3D contour correspondences:
	//for (int i = 0; i < n_strip * 3; i++) contour[i] = 0.0;
	//int numC2 = 0;
	//get_contour_for_each_strip(numC2, contour); //cout << numC2 << endl;
	//nC = numC2;
	//for (size_t i = 0; i < numC2; i++) // only for display
	//{
	//	contour_display[3 * i] = contour[3 * i];
	//	contour_display[3 * i + 1] = contour[3 * i + 1];
	//	contour_display[3 * i + 2] = contour[3 * i + 2];
	//}
	//// for each image contour landmark, search the nearest one from all 3D contour
	//for (int kk = 0; kk < n_landmarks_clm; kk++)//add landmarks
	//{
	//	if (kk <= 52 || kk >= 70)//inner point 
	//	{
	//		continue;
	//	}
	//	else // contour point 
	//	{
	//		float fx1 = m_landmarks_clm[2 * kk];
	//		float fy1 = m_landmarks_clm[2 * kk + 1];
	//		float minDist = 1.0e8;
	//		for (int t0 = 0; t0 < nC; t0++)
	//		{
	//			float fx0 = contour[3 * t0];
	//			float fy0 = contour[3 * t0 + 1];
	//			//float dist = abs(fx1 - fx0) + abs(fy1 - fy0);
	//			float dist = sqrt((fx1 - fx0)*(fx1 - fx0) + (fy1 - fy0)*(fy1 - fy0));
	//			if (dist < minDist)
	//			{
	//				minDist = dist;
	//				index_landmarks_clm[kk] = contour[3 * t0 + 2];
	//			}
	//		}
	//	}
	//}
	//delete[] contour;

	//// shape estimation with landmark by Ceres
	//Problem problem_2;
	//std::vector<cv::Vec3f> basis_shape; // the corresponding 2D landmark points
	//std::vector<double> tmpPoseParams(7);
	//for (int kk = 0; kk < 7; kk++) tmpPoseParams[kk] = poseParameters[kk];

	//for (int kk = 0; kk < n_landmarks_clm; kk++)//add landmarks
	//{
	//	float fx1 = m_landmarks_clm[2 * kk];
	//	float fy1 = m_landmarks_clm[2 * kk + 1];
	//	cv::Vec2f lmp_2(fx1, fy1);
	//	image_points.emplace_back(lmp_2);

	//	int idx1 = index_landmarks_clm[kk];
	//	int offset = 3 * idx1;
	//	cv::Vec3f mean_shape(_face_mean(offset, 0), _face_mean(offset + 1, 0), _face_mean(offset + 2, 0));
	//	basis_shape.clear();
	//	for (int a = 0; a < 99; a++)
	//	{
	//		float x_ = gModels->getShape_ev(a)*gModels->getShape_v(offset, a);
	//		float y_ = gModels->getShape_ev(a)*gModels->getShape_v(offset + 1, a);
	//		float z_ = gModels->getShape_ev(a)*gModels->getShape_v(offset + 2, a);
	//		basis_shape.emplace_back(cv::Vec3f(x_, y_, z_));
	//	}
	//	ShapeEstimateByLandmarkResidual_AutoDiff * pResidualX = new ShapeEstimateByLandmarkResidual_AutoDiff(lmp_2, mean_shape, basis_shape, tmpPoseParams, winWidth, winHeight, _zFar);
	//	problem_2.AddResidualBlock(new AutoDiffCostFunction<ShapeEstimateByLandmarkResidual_AutoDiff, 2, 99>(pResidualX), NULL, alphaParameters);
	//}
	//// add regularization term
	//problem_2.AddResidualBlock(new AutoDiffCostFunction<ShapeTextureEstimateResidual_Reg, 99, 99>(new ShapeTextureEstimateResidual_Reg(1)), NULL, alphaParameters);
	//Solver::Options m_options_2;
	//Solver::Summary m_summary_2;
	//m_options_2.max_num_iterations = 20;
	//m_options_2.linear_solver_type = ceres::DENSE_QR;
	//m_options_2.minimizer_progress_to_stdout = false;

	//Solve(m_options_2, &problem_2, &m_summary_2);
	//cout << m_summary_2.FullReport() << "\n";

	////update alpha and update model
	//std::cout << "identity parameters = " << std::endl;
	//for (int i = 0; i < 99; i++)
	//{
	//	alpha(i, 0) = alphaParameters[i];
	//	std::cout << alphaParameters[i] << " ";
	//}
	//std::cout << std::endl;

	//gModels->gen_model(alpha, beta);

//
//	boost::timer::auto_cpu_timer timer_all(
//		"[Pose optimization] Total time = %w seconds.\n");
//
//	ceres::Problem problem;
//	vector<double> params{ params_model.R[0], params_model.R[1], params_model.R[2], params_model.T[0], params_model.T[1], params_model.T[2] };
//
//	{
//		boost::timer::auto_cpu_timer timer_construction(
//			"[Pose optimization] Problem construction time = %w seconds.\n");
//
//		for (size_t i = 0; i < indices.size(); ++i) {
//			auto &model_i = model_projected[i];
//			//model_i.ApplyWeights(params_model.Wid, params_model.Wexp);
//			Constraint2D cons_i = params_recon.cons[i];
//			if (i<15) cons_i.weight = 0.3 * iteration;
//			else if (i>45 && i<64) cons_i.weight = 0.3 * iteration;
//			else cons_i.weight = 1.0;
//
//#if USE_ANALYTIC_COST_FUNCTIONS
//			ceres::CostFunction *cost_function =
//				new PoseCostFunction_analytic(model_i, cons_i,
//				params_cam);
//			problem.AddResidualBlock(cost_function, NULL, params.data(),
//				params.data() + 3);
//#else
//			ceres::CostFunction *cost_function =
//				new ceres::NumericDiffCostFunction<PoseCostFunction, ceres::CENTRAL, 1, 6>(
//				new PoseCostFunction(model_i,
//				cons_i,
//				params_cam));
//			problem.AddResidualBlock(cost_function, NULL, params.data());
//#endif
//		}
//
//#if 1
//		// Add a regularization term
//		ceres::CostFunction *reg_cost_function =
//			new ceres::NumericDiffCostFunction<PoseRegularizationTerm, ceres::CENTRAL, 1, 3>(
//			new PoseRegularizationTerm(1.0)
//			);
//		problem.AddResidualBlock(reg_cost_function, NULL, params.data());
//#endif
//	}
//
//  {
//	  boost::timer::auto_cpu_timer timer_solve(
//		  "[Pose optimization] Problem solve time = %w seconds.\n");
//
//	  ceres::Solver::Options options;
//	  options.max_num_iterations = 15;
//
//	  options.num_threads = 8;
//	  options.num_linear_solver_threads = 8;
//
//	  //options.minimizer_type = ceres::LINE_SEARCH;
//	  //options.line_search_direction_type = ceres::LBFGS;
//
//	  DEBUG_EXPR(options.minimizer_progress_to_stdout = true;)
//		  ceres::Solver::Summary summary;
//
//	  Solve(options, &problem, &summary);
//	  DEBUG_OUTPUT(summary.BriefReport())
//  }
//
//  Vector3d newR(params[0], params[1], params[2]);
//  Vector3d newT(params[3], params[4], params[5]);
//  DEBUG_OUTPUT(
//	  "R: " << params_model.R.transpose() << " -> " << newR.transpose())
//	  DEBUG_OUTPUT(
//	  "T: " << params_model.T.transpose() << " -> " << newT.transpose())
//	  params_model.R = newR;
//  params_model.T = newT;



//	double E = 0.0;
//
//
//
//	std::vector<double> poseParams;
//	poseParams2Vec(poseParams);
//	double focal = recon_camera_.focal_;
//	ceres::Problem problem0;
//	for (int kk = 0; kk < this->numPts_; kk++) {
//
//		if (kk <= 52 || kk >= 70) {
//			if (kk == 0 || kk == 4 || kk == 5 || kk == 9 || kk == 10 || kk == 14 || kk == 18 || kk == 22 ||
//				kk == 29 || kk == 30 || kk == 32 || kk == 33 || kk == 36 || kk == 39 || kk == 42) {
//				float px_2d = this->pts2D_[kk].x;
//				float py_2d = this->pts2D_[kk].y;
//				cv::Vec2f pt_2d(px_2d, py_2d);
//				float px_3d = pts3D_[kk](0);
//				float py_3d = pts3D_[kk](1);
//				float pz_3d = pts3D_[kk](2);
//				cv::Vec4f pt_3d(px_3d, py_3d, pz_3d, 1.0);
//				//Remark: AutoDiff is better than analytic in the aspect of convergence speed (less iteration steps) 
//#ifdef USE_ANALYTIC_COST_FUNCTIONS
//				PoseEstimateResidual_Analytic* pResidualX = new PoseEstimateResidual_Analytic(pt_2d, pt_3d, imgWidth_, imgHeight_);
//				problem0.AddResidualBlock(pResidualX, NULL, poseParams.data());
//#else
//				PoseEstimateResidual_AutoDiff* pResidualX = new PoseEstimateResidual_AutoDiff(pt_2d, pt_3d, imgWidth_, imgHeight_, focal);
//				problem0.AddResidualBlock(new ceres::AutoDiffCostFunction<PoseEstimateResidual_AutoDiff, 2, 6>(pResidualX), NULL, poseParams.data());
//#endif
//
//			}
//		}
//	}
//
//
//	ceres::Solver::Options m_options0;
//	ceres::Solver::Summary m_summary0;
//	m_options0.max_num_iterations = 20;
//	m_options0.linear_solver_type = ceres::DENSE_QR;
//	m_options0.minimizer_progress_to_stdout = false;
//	ceres::Solve(m_options0, &problem0, &m_summary0);
//	poseParamsVec2pose(poseParams);
//	// Re-estimate the pose with all landmarks, including the contour landmarks:
//	ceres::Problem problem1;
//	for (int kk = 0; kk < this->numPts_; kk++){
//
//		float px_2d = this->pts2D_[kk].x;
//		float py_2d = this->pts2D_[kk].y;
//		cv::Vec2f pt_2d(px_2d, py_2d);
//		float px_3d = pts3D_[kk](0);
//		float py_3d = pts3D_[kk](1);
//		float pz_3d = pts3D_[kk](2);
//		cv::Vec4f pt_3d(px_3d, py_3d, pz_3d, 1.0);
//
//#ifdef USE_ANALYTIC_COST_FUNCTIONS
//		PoseEstimateResidual_Analytic* pResidualX = new PoseEstimateResidual_Analytic(pt_2d, pt_3d, imgWidth_, imgHeight_);
//		problem1.AddResidualBlock(pResidualX, NULL, poseParams.data());
//#else
//		PoseEstimateResidual_AutoDiff* pResidualX = new PoseEstimateResidual_AutoDiff(pt_2d, pt_3d, imgWidth_, imgHeight_, focal);
//		problem1.AddResidualBlock(new ceres::AutoDiffCostFunction<PoseEstimateResidual_AutoDiff, 2, 6>(pResidualX), NULL, poseParams.data());
//#endif
//
//	}
//	ceres::Solver::Options m_options1;
//	ceres::Solver::Summary m_summary1;
//	m_options1.max_num_iterations = 10;
//	m_options1.linear_solver_type = ceres::DENSE_QR;
//	m_options1.minimizer_progress_to_stdout = false;
//	ceres::Solve(m_options1, &problem1, &m_summary1);
//	poseParamsVec2pose(poseParams);
//	E = ComputeError();
//	std::cout << "Pose Optimization, Error = " << E << "-------------------------------" << std::endl;

}

void SingleImageReconstructor::OptimizeByLandmark_BsImpGN(){

}

void SingleImageReconstructor::FitGN(){

}

void SingleImageReconstructor::FitFirstFrameGN(){

}

///////////////////////////////////////////////////////////// Utils ////////////////////////////////////////////////////////////////////
void SingleImageReconstructor::Init() 
{
	// init camera parameters && model parameters
	recon_model_.Init(NUM_BLENDSHAPE);
	recon_camera_ = CameraParameters::DefaultParameters(imgWidth_, imgHeight_);
	std::cout << "default focal length is " << recon_camera_.focal_ << std::endl;
	// default model, view, projection matrix
	//recon_model_的Rx_等位移和旋转参数在Parameters.cpp中的Init里已经初始化，即模型的初始位置
	float Rx = (float)recon_model_.Rx_;
	float Ry = (float)recon_model_.Ry_;
	float Rz = (float)recon_model_.Rz_;
	float Tx = (float)recon_model_.Tx_;
	float Ty = (float)recon_model_.Ty_;
	float Tz = (float)recon_model_.Tz_;
	float aspect = (float)recon_camera_.aspect_;
	float focal = (float)recon_camera_.focal_;
	float far_c = (float)recon_camera_.far_c_;
	auto Rmat = glm::eulerAngleXYZ(Rx, Ry, Rz);
	auto Tmat = glm::translate(glm::fmat4(1.0), glm::fvec3(Tx, Ty, Tz));
	model_mat_ = Tmat * Rmat;
	camera_view_ = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//frustum创建一个透视型的视景体。其操作是创建一个透视投影的矩阵，并且用这个矩阵乘以当前矩阵。这个函数的参数只定
	//义近裁剪平面的左下角点和右上角点的三维空间坐标，即（left，bottom， - near）和（right，top， - near）
	//; 最后一个参数far是远裁剪平面的离视点的距离值，其左下角点和右上角点空间坐标由函数根据透视投影原理自动生
	//	成。near和far表示离视点的远近，它们总为正值(near / far 必须>0)。
	//frustumLH(left, right, bottom, top, nearVal, farVal);
	projection_ = glm::frustum(-aspect, aspect, -1.0f, 1.0f, (float)focal, far_c);
	//projection_ = glm::perspective(45.0f, (float)recon_camera_.imgSize_[0]/ (float)recon_camera_.imgSize_[1],focal,far_c);
	// init pts3D
	this->pts3D_.resize(this->index3D_.size());
	for (int ii = 0; ii < this->index3D_.size(); ii++) 
	{
		this->pts3D_[ii](0) = (double)this->bsData_.blendshapes_[0].position_(index3D_[ii], 0);
		this->pts3D_[ii](1) = (double)this->bsData_.blendshapes_[0].position_(index3D_[ii], 1);
		this->pts3D_[ii](2) = (double)this->bsData_.blendshapes_[0].position_(index3D_[ii], 2);
	}
	on_the_fly = false;

	//jisy initialize dlib
	dlibMarkerDetector = new DlibMarkDetector(str_dlibModelPath);
}

void SingleImageReconstructor::PrepareConstraints()
{
	this->constraints2D_.resize(this->numPts_);
	std::vector<cv::Vec3f> basis_shape;
	cv::Vec3f mean_pt_3d;

	for (int kk = 0; kk < this->numPts_; kk++) 
	{
			int idx_3d = this->index3D_[kk];
			float mean_px_3d = this->bsData_.landmarks_model_[kk][0](0);
			float mean_py_3d = this->bsData_.landmarks_model_[kk][0](1);
			float mean_pz_3d = this->bsData_.landmarks_model_[kk][0](2);
			mean_pt_3d[0] = mean_px_3d;
			mean_pt_3d[1] = mean_py_3d;
			mean_pt_3d[2] = mean_pz_3d;
			basis_shape.clear();
			for (int a = 1; a < NUM_BLENDSHAPE + 1; a++) 
			{
				float x_ = this->bsData_.landmarks_model_[kk][a](0);
				float y_ = this->bsData_.landmarks_model_[kk][a](1);
				float z_ = this->bsData_.landmarks_model_[kk][a](2);
				basis_shape.emplace_back(cv::Vec3f(x_, y_, z_));//emplace_back类似于push_back
			}
			this->constraints2D_[kk].basis_shape = basis_shape;
			this->constraints2D_[kk].mean_pt = mean_pt_3d;
			this->constraints2D_[kk].vidx = idx_3d;
			// Setup weight
			//if (kk >= 10 && kk <= 17) this->constraints2D_[kk].weight = 20.0;  // left eye   //jisy fix
			//else if (kk >= 18 && kk <= 25) this->constraints2D_[kk].weight = 20.0; // right eye
			//else if (kk >= 33 && kk <= 44) this->constraints2D_[kk].weight = 10.0; // outer mouth
			//else if (kk >= 45 && kk <= 52) this->constraints2D_[kk].weight = 20.0; // inner mouth
			//else this->constraints2D_[kk].weight = 1.0;

			//if (kk == 33 || kk == 45 || kk == 49 || kk == 39) this->constraints2D_[kk].weight = 20.0; // mouth corner

			if (kk >= 36 && kk <= 41) this->constraints2D_[kk].weight = 20.0;  // left eye   //jisy landmarker fix
			else if (kk >= 42 && kk <= 47) this->constraints2D_[kk].weight = 20.0; // right eye
			else if (kk >= 48 && kk <= 59) this->constraints2D_[kk].weight =10.0; // outer mouth
			else if (kk >= 60 && kk <= 67) this->constraints2D_[kk].weight = 20.0; // inner mouth
			else this->constraints2D_[kk].weight = 1.0;

			if (kk == 48 || kk == 59 || kk == 60 || kk == 67) this->constraints2D_[kk].weight = 20.0; // mouth corner
			//this->constraints2D_[kk].weight = 1.0;
	}
	return;
}

void SingleImageReconstructor::poseParams2Vec(std::vector<double>& params)
{	
	params.resize(6);
	params[0] = recon_model_.Rx_;
	params[1] = recon_model_.Ry_;
	params[2] = recon_model_.Rz_;
	params[3] = recon_model_.Tx_;
	params[4] = recon_model_.Ty_;
	params[5] = recon_model_.Tz_;
}

void SingleImageReconstructor::poseParamsVec2pose(std::vector<double>& params)
{
	recon_model_.Rx_ = params[0];
	recon_model_.Ry_ = params[1];
	recon_model_.Rz_ = params[2];
	recon_model_.Tx_ = params[3];
	recon_model_.Ty_ = params[4];
	recon_model_.Tz_ = params[5];
}

void SingleImageReconstructor::poseFocalParams2Vec(std::vector<double>& params)
{	
	params.resize(7);
	params[0] = recon_model_.Rx_;
	params[1] = recon_model_.Ry_;
	params[2] = recon_model_.Rz_;
	params[3] = recon_model_.Tx_;
	params[4] = recon_model_.Ty_;
	params[5] = recon_model_.Tz_;
	params[6] = recon_camera_.focal_;
}

void SingleImageReconstructor::printPoseFocalParams()
{	
	std::cout << "Rx = " << recon_model_.Rx_ << std::endl;
	std::cout << "Ry = " << recon_model_.Ry_ << std::endl;
	std::cout << "Rz = " << recon_model_.Rz_ << std::endl;
	std::cout << "Tx = " << recon_model_.Tx_ << std::endl;
	std::cout << "Ty = " << recon_model_.Ty_ << std::endl;
	std::cout << "Tz = " << recon_model_.Tz_ << std::endl;
	std::cout << "Focal = " << recon_camera_.focal_ << std::endl;

}

void SingleImageReconstructor::SetInputImage(cv::Mat input_img) 
{
	input_img.copyTo(input_img_);
	imgWidth_ = input_img_.cols;
	imgHeight_ = input_img_.rows;
}

Eigen::Vector2d SingleImageReconstructor::ProjectPoint(Eigen::Vector3d pt3d) 
{
	// 3D point as input
	double ox = pt3d(0);
	double oy = pt3d(1);
	double oz = pt3d(2);
	double ow = 1.0;

	// parameters to be optimized
	double rot_x_pitch = recon_model_.Rx_;
	double rot_y_yaw = recon_model_.Ry_;
	double rot_z_roll = recon_model_.Rz_;
	double tx = recon_model_.Tx_;
	double ty = recon_model_.Ty_;
	double tz = recon_model_.Tz_;
	double focal = recon_camera_.focal_;

	//cv::Mat rot_mtx_x = (cv::Mat_<T>(4, 4) <<
	//	1.0f, 0.0f, 0.0f, 0.0f,
	//	0.0f, cos(rot_x_pitch), -sin(rot_x_pitch), 0.0f,
	//	0.0f, sin(rot_x_pitch), cos(rot_x_pitch), 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);
	double ox2 = ox;
	double oy2 = cos(rot_x_pitch)*oy - sin(rot_x_pitch)*oz;
	double oz2 = sin(rot_x_pitch)*oy + cos(rot_x_pitch)*oz;
	double ow2 = ow;

	//cv::Mat rot_mtx_y = (cv::Mat_<T>(4, 4) <<
	//	cos(rot_y_yaw), 0.0f, sin(rot_y_yaw), 0.0f,
	//	0.0f, 1.0f, 0.0f, 0.0f,
	//	-sin(rot_y_yaw), 0.0f, cos(rot_y_yaw), 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);
	double ox3 = cos(rot_y_yaw)*ox2 + sin(rot_y_yaw)*oz2;
	double oy3 = oy2;
	double oz3 = -sin(rot_y_yaw)*ox2 + cos(rot_y_yaw)*oz2;
	double ow3 = ow2;

	//cv::Mat rot_mtx_z = (cv::Mat_<T>(4, 4) <<
	//	cos(rot_z_roll), -sin(rot_z_roll), 0.0f, 0.0f,
	//	sin(rot_z_roll), cos(rot_z_roll), 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f);
	double ox4 = cos(rot_z_roll)*ox3 - sin(rot_z_roll)*oy3;
	double oy4 = sin(rot_z_roll)*ox3 + cos(rot_z_roll)*oy3;
	double oz4 = oz3;
	double ow4 = ow3;

	//cv::Mat t_mtx = (cv::Mat_<T>(4, 4) <<
	//	1.0f, 0.0f, 0.0f, tx,
	//	0.0f, 1.0f, 0.0f, ty,
	//	0.0f, 0.0f, 1.0f, tz,
	//	0.0f, 0.0f, 0.0f, 1.0f);
	double ox5 = ox4 + tx;
	double oy5 = oy4 + ty;
	double oz5 = oz4 + tz;
	double ow5 = ow4;

	double aspect = 1.0 * imgWidth_ / imgHeight_;
	double l_ = -aspect;
	double r_ = aspect;
	double b_ = -1.0;
	double t_ = 1.0;
	double n_ = focal;
	double f_ = 1000.;

	//cv::Mat p_mtx = (cv::Mat_<T>(4, 4) <<
	//	T(2.0)*n_/(r_-l_), 0.0, 0.0, 0.0,
	//	0.0, T(2.0)*n_/(t_-b_), 0.0, 0.0,
	//	0.0, 0.0, -(n_ + f_) / (f_ - n_), (T(-2.0) * n_ * f_) / (f_ - n_),
	//	0.0, 0.0, -1.0, 0.0);
	double ox6 = double(2.0)*n_ / (r_ - l_)*ox5;
	double oy6 = double(2.0)*n_ / (t_ - b_)*oy5;
	double oz6 = -(n_ + f_) / (f_ - n_)*oz5 + (double(-2.0) * n_ * f_) / (f_ - n_);
	double ow6 = -oz5;

	// transformed to image coordinates
	double fx1 = double(0.5)*(ox6 / ow6 + double(1.0)) * double(imgWidth_);
	double fy1 = double(0.5)*(oy6 / ow6 + double(1.0)) * double(imgHeight_);

	//int x1 = floor(fx1 + 0.5);
	//if (x1 >= width)
	//	x1 = width - 1;
	//else if (x1 < 0)
	//	x1 = 0;
	//int y1 = floor(fy1 + 0.5);
	//if (y1 >= height)
	//	y1 = height - 1;
	//else if (y1 < 0)
	//	y1 = 0;

	double predicted_x = fx1;
	double predicted_y = fy1;


	//std::cout << "Rx = " << rot_x_pitch << std::endl;
	//std::cout << "Ry = " << rot_y_yaw << std::endl;
	//std::cout << "Rz = " << rot_z_roll << std::endl;
	//std::cout << "Tx = " << tx << std::endl;
	//std::cout << "Ty = " << ty << std::endl;
	//std::cout << "Tz = " << tz << std::endl;
	//std::cout << "Focal = " << focal << std::endl;
	//std::cout << fx1 << " , " << fy1 << std::endl;
	return Eigen::Vector2d(fx1, fy1);
}


/* compute error */
double SingleImageReconstructor::ComputeError() 
{
	cv::Mat frame_out;
	double E = 0;
	for (int i = 0; i < this->index3D_.size(); ++i) 
	{
		auto pos_3d = pts3D_[i];
		auto pos_2d = ProjectPoint(pos_3d);
		double dx = pos_2d(0) - this->pts2D_[i].x;
		double dy = pos_2d(1) - this->pts2D_[i].y;
		E += dx * dx + dy * dy;
	}
	return E;
}


/* Load 3D Landmarks' Index */
void SingleImageReconstructor::LoadIndex3D(std::string filename) 
{
	index3D_.resize(numPts_);
	std::ifstream in(filename);
	if (in.is_open()) 
	{
		for (int kk = 0; kk < numPts_; kk++) 
		{
			in >> this->index3D_[kk];
		}
	}
	in.close();
}

/* Update 2D Landmarks [remove last two landmarks]*/
void SingleImageReconstructor::UpdatePts2D(std::string landmark_file) 
{
	this->pts2D_.resize(numPts_);
	std::ifstream in(landmark_file);
	if (in.is_open()) 
	{
		for (int kk = 0; kk < numPts_; kk++) 
		{
			in >> this->pts2D_[kk].x >> this->pts2D_[kk].y;
			/*this->pts2D_[kk].x = this->pts2D_[kk].x;
			this->pts2D_[kk].y = this->pts2D_[kk].y;*/
			this->pts2D_[kk].y = imgHeight_ - this->pts2D_[kk].y;
		}
		in.close();
	}
}

/* Update 2D Landmarks from Outside */
void SingleImageReconstructor::UpdatePts2D(std::vector<cv::Point2d> pts2D) 
{
	this->pts2D_ = pts2D;
}

/* Update 3D Landmarks */
void SingleImageReconstructor::UpdatePts3D() 
{
	for (int ii = 0; ii < this->index3D_.size(); ii++) 
	{
		this->pts3D_[ii].setZero();
		this->pts3D_[ii] = this->pts3D_[ii] + this->bsData_.landmarks_model_[ii][0] * recon_model_.params_[0];
		for (int kk = 1; kk < NUM_BLENDSHAPE + 1; kk++) 
		{
			this->pts3D_[ii] = this->pts3D_[ii] +
				(bsData_.landmarks_model_[ii][kk] - bsData_.landmarks_model_[ii][0]) * recon_model_.params_[kk];
		}
	}
	return;
}

/* Update Landmark Position */
void SingleImageReconstructor::UpdateLandmarkPos() {

	landmark3DPos_ = Eigen::MatrixXf(numPts_, 3);
	for (int kk = 0; kk < numPts_; kk++) {
		landmark3DPos_(kk, 0) = recon_mesh_.position_(index3D_[kk], 0);
		landmark3DPos_(kk, 1) = recon_mesh_.position_(index3D_[kk], 1);
		landmark3DPos_(kk, 2) = recon_mesh_.position_(index3D_[kk], 2);
		//landmark3DPos_(kk, 0) = cur_landmark_[kk](0);
		//landmark3DPos_(kk, 1) = cur_landmark_[kk](1);
		//landmark3DPos_(kk, 2) = cur_landmark_[kk](2);
	}
	return;

}

void SingleImageReconstructor::UpdateModelMat() {

	float Rx = (float)recon_model_.Rx_;
	float Ry = (float)recon_model_.Ry_;
	float Rz = (float)recon_model_.Rz_;
	float Tx = (float)recon_model_.Tx_;
	float Ty = (float)recon_model_.Ty_;
	float Tz = (float)recon_model_.Tz_;
	auto Rmat = glm::eulerAngleXYZ(Rx, Ry, Rz);
	auto Tmat = glm::translate(glm::fmat4(1.0), glm::fvec3(Tx, Ty, Tz));
	model_mat_ = Tmat * Rmat;
}
void SingleImageReconstructor::UpdateProjectionMat() {
	float aspect = (float)recon_camera_.aspect_;
	float focal = (float)recon_camera_.focal_;
	float far_c = (float)recon_camera_.far_c_;
	projection_ = glm::frustum(-aspect, aspect, -1.0f, 1.0f, (float)focal, far_c);
	//projection_ = glm::perspective(45.0f, (float)recon_camera_.imgSize_[0] / (float)recon_camera_.imgSize_[1], focal, far_c);
}

void SingleImageReconstructor::UpdateMesh(std::vector<double> coef) {

	this->bsData_.UpdateMesh(coef, this->recon_mesh_);
	return;
}
