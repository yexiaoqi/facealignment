#pragma once

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>
#include <string> 
#include <vector>
#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>

using namespace dlib;
using namespace std;

static int  frameCount;
class DlibMarkDetector
{
public:
	DlibMarkDetector(std::string dlibModelPath) :
		dlibModelPath(dlibModelPath)
	{
		InitializeModel();
	}

	~DlibMarkDetector() {}

	

	int FaceDectionAndAlignment(cv::Mat srcFrame, std::vector<point> &markers)
	{
		// Turn OpenCV's Mat into something dlib can deal with.  Note that this just
		// wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
		// long as temp is valid.  Also don't do anything to temp that would cause it
		// to reallocate the memory which stores the image as that will make cimg
		// contain dangling pointers.  This basically means you shouldn't modify temp
		// while using cimg.

		cv::Mat frameCopy;
		srcFrame.copyTo(frameCopy);
		cv_image<bgr_pixel> cimg(frameCopy);


		std::vector<full_object_detection> shapes;


		//detect faces
		if (frameCount < 2) {
			faces = detector(cimg);
			frameCount++;
		}

		//find the pose of each face
		full_object_detection shape;
		for (unsigned long i = 0;i < faces.size();++i)
		{
			shape = pose_model(cimg, faces[i]);
			shapes.push_back(shape);

			//return the detected markers
			for (int j = 0;j < (int)shape.num_parts();++j)
				markers.push_back(shape.part(j));
		}
		return 0;
	}
private:
	//cv::Mat srcFrame;
	std::string dlibModelPath;
	frontal_face_detector detector;
	shape_predictor pose_model;

	std::vector<rectangle> faces;

	int InitializeModel()
	{
		//load face-alignment model
		this->detector = get_frontal_face_detector();
		deserialize(this->dlibModelPath) >> (this->pose_model);
		printf("Load model from %s\n", (this->dlibModelPath).c_str());
		return 0;
	}
};