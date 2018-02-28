#include "LandmarkModifierTool.h"

LandmarkModifierTool* LandmarkModifierTool::instance;

void LandmarkModifierTool::onMouseChange(int event, int x, int y, int flags, void* ustc) {

	LandmarkModifierTool *ui = LandmarkModifierTool::Instance();

	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN && !ui->cvLb_) {
		double dist = 999999999.0;
		for (int index = 0; index<ui->cvPts_.size(); ++index) {
			double deltax = x - ui->cvPts_[index].x;
			double deltay = y - ui->cvPts_[index].y;
			double d = deltax * deltax + deltay * deltay;
			if (dist>d /*&& d <= FACESHAPE_MAX_DIST*/) {
				dist = d;
				ui->cvCnt_ = index;

			}
		}
		std::cout << "(x,y) = " << x << " , " << y << "--" << ui->cvPts_[ui->cvCnt_].x << " , " << ui->cvPts_[ui->cvCnt_].y << std::endl;
		std::cout << "picked pts " << ui->cvCnt_ << std::endl;
		ui->cvLb_ = true;
	}
	if (event == CV_EVENT_LBUTTONUP) {
		ui->cvLb_ = false;
	}
	if (event == CV_EVENT_MOUSEMOVE) {
		if (ui->cvLb_) {

			ui->cvPts_[ui->cvCnt_] = ui->cvPts_[ui->cvCnt_] + cv::Point2i(x, y) - ui->cvLastPt_;

		}
	}
	ui->drawImg();
	cv::imshow("Image", ui->cvDispImg_);
	ui->cvLastPt_ = cv::Point2i(x, y);
}

void LandmarkModifierTool::launch(cv::Mat& img, std::vector<cv::Point2i>& old_pts, std::vector<cv::Point2i>& new_pts) {

	img.copyTo(cvSrcImg_);
	this->cvLb_ = false;
	//cv::flip(_cv_src_img, _cv_src_img, 1); // flip frame
	cvTmpImg_ = cvSrcImg_.clone();
	cvPts_ = old_pts;

	cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
	cv::moveWindow("Image", 0 ,0);
	this->drawImg();
	cv::imshow("Image", cvDispImg_);
	cv::setMouseCallback("Image", onMouseChange, 0);
	while (1) {
		char ch = cv::waitKey(0);
		if (ch == 13) {
			break;
		}
	}
	new_pts = cvPts_;
	return;
}

void LandmarkModifierTool::release() {
	
	cv::destroyWindow("Image");
	return;
}

void LandmarkModifierTool::drawImg() {

	cvTmpImg_ = cvSrcImg_.clone();
	// add pts
	for (int kk = 0; kk < LANDMARK_FACESHAPE_PNTNUM;kk++)
		cv::circle(cvTmpImg_, this->cvPts_[kk], 2, LANDMARK_CV_FACESHAPE_PT_COLOR);
	// add lines
	for (int kk = 53; kk < 69 ; kk++) 
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);
	
	for (int kk = 0; kk < 4 ; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 70; kk < 72; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	cv::line(cvTmpImg_, this->cvPts_[0], this->cvPts_[70], LANDMARK_CV_FACESHAPE_LINE_COLOR);
	cv::line(cvTmpImg_, this->cvPts_[4], this->cvPts_[72], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 5; kk < 9 ; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 73; kk < 75; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	cv::line(cvTmpImg_, this->cvPts_[9], this->cvPts_[75], LANDMARK_CV_FACESHAPE_LINE_COLOR);
	cv::line(cvTmpImg_, this->cvPts_[5], this->cvPts_[73], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 10; kk < 17; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 18; kk < 25; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 26; kk < 29; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 30; kk < 32; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 33; kk < 44; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	cv::line(cvTmpImg_, this->cvPts_[33], this->cvPts_[44], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	for (int kk = 45; kk < 52; kk++)
		cv::line(cvTmpImg_, this->cvPts_[kk], this->cvPts_[kk + 1], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	cv::line(cvTmpImg_, this->cvPts_[45], this->cvPts_[52], LANDMARK_CV_FACESHAPE_LINE_COLOR);

	//float factor = 0.5f;
	//cv::resize(cvTmpImg_, _cv_disp_img, cv::Size((int)cvTmpImg_.cols * factor,(int)cvTmpImg_.rows * factor));
	cvTmpImg_.copyTo(cvDispImg_);
	return;
}