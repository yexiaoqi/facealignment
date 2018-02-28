#pragma once

#include <opencv2\opencv.hpp>
#include <vector>

#define LANDMARK_FACESHAPE_MAX_DIST 500
#define LANDMARK_CV_FACESHAPE_PT_COLOR cv::Scalar(0,255,0,255)
#define LANDMARK_CV_FACESHAPE_LINE_COLOR cv::Scalar(255,0,0,255)
#define LANDMARK_FACESHAPE_PNTNUM 78

class LandmarkModifierTool {

public:

	static LandmarkModifierTool* instance;
	static LandmarkModifierTool* Instance(void) {
		if (instance == NULL) instance = new LandmarkModifierTool();
		return instance;
	}

	// callback function
	static void onMouseChange(int event, int x, int y, int flags, void* ustc);

	// launch
	void launch(cv::Mat& img, std::vector<cv::Point2i>& old_pts, std::vector<cv::Point2i>& new_pts);
	// release
	void release();
	void drawImg();

private:
	// opencv landmark modifying
	cv::Mat cvSrcImg_;
	cv::Mat cvTmpImg_;
	cv::Mat cvDispImg_;
	std::vector<cv::Point2i> cvPts_;
	cv::Point2i cvLastPt_;
	int cvCnt_;
	bool cvLb_;
};