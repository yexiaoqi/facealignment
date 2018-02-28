#include "DrawRectTool.h"

DrawRectTool* DrawRectTool::instance;

void DrawRectTool::onMouseChange(int event, int x, int y, int flags, void* ustc) {

	DrawRectTool *ui = DrawRectTool::Instance();

	char temp[16];
	if (event == CV_EVENT_LBUTTONDOWN) {

		ui->srcImg.copyTo(ui->dispImg);
		ui->lbDown = true;
		ui->topLeft = cv::Point(x, y);
	}
	if (event == CV_EVENT_LBUTTONUP) {
		ui->lbDown = false;
		ui->_faceReg = cv::Rect(ui->topLeft, ui->bottomRight);
	}
	if (event == CV_EVENT_MOUSEMOVE) {
		if (ui->lbDown) {
			ui->bottomRight = cv::Point(x, y);
			ui->drawImg();
			cv::imshow("Image", ui->dispImg);
		}
	}

}

void DrawRectTool::launch(cv::Mat& img, cv::Rect& faceReg) {

	img.copyTo(srcImg);
	img.copyTo(dispImg);
	this->_faceReg = faceReg;
	this->_faceRegOrig = faceReg;

	cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);
	cv::moveWindow("Image", 20, 30);

	this->drawImg();
	cv::imshow("Image", dispImg);
	cv::setMouseCallback("Image", onMouseChange, 0);
	while (1) {
		char ch = cv::waitKey(0);
		if (ch == 13) {
			break;
		}
	}
	faceReg = _faceReg;
	return;
}

void DrawRectTool::release() {

	cv::destroyWindow("Image");
	return;
}

void DrawRectTool::drawImg() {

	this->srcImg.copyTo(this->dispImg);
	cv::rectangle(this->dispImg, this->_faceRegOrig, cv::Scalar(0,0,255), 2);
	cv::rectangle(this->dispImg, cv::Rect(topLeft,bottomRight), cv::Scalar(0,255,0), 2);
	return;
}