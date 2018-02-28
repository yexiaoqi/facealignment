#pragma once

#include <opencv2\opencv.hpp>
#include <vector>

class DrawRectTool {

public:

	static DrawRectTool* instance;
	static DrawRectTool* Instance(void) {
		if (instance == NULL) instance = new DrawRectTool();
		return instance;
	}

	// callback function
	static void onMouseChange(int event, int x, int y, int flags, void* ustc);

	// launch
	void launch(cv::Mat& img, cv::Rect& faceReg);
	// release
	void release();
	void drawImg();

private:
	// opencv landmark modifying
	cv::Mat srcImg;
	cv::Mat dispImg;
	cv::Rect _faceReg;
	cv::Rect _faceRegOrig;
	cv::Point topLeft;
	cv::Point bottomRight;
	bool lbDown;
};