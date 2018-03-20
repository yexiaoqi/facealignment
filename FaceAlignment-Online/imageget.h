#pragma once
#include "stdafx.h"
#include <opencv2\opencv.hpp> 
#include<iostream>
//windows的头文件，必须要，不然NuiApi.h用不了
#include <Windows.h>
//Kinect for windows 的头文件
#include <d3d11.h>
#include "NuiApi.h"

using namespace std;
using namespace cv;

class imageget
{
public:
	imageget():colorFrameCount(1),depthFrameCount(1), m_pNuiSensor(NULL){}
	int image();
	int KinectInitialize();
	~imageget() {}
private:
	//yqy应该是dm为单位
	//const int THRESHOLD = 3500;//3.5m
	const int MAX_DISTANCE = 3500;//3.5m
	const int MIN_DISTANCE = 200;//0.2m
	const LONG m_depthWidth = 640;
	const LONG m_depthHeight = 480;
	const LONG m_colorWidth = 640;
	const LONG m_colorHeight = 480;
	//彩色图像
	Mat image_rgb;
	//深度图像
	Mat image_depth;
	int colorFrameCount ;
	int depthFrameCount ;
	//一个KINECT实例指针
	INuiSensor* m_pNuiSensor ;
	//记录当前连接KINECT的数量（为多连接做准备）
	int iSensorCount;
	//获得当前KINECT的数量
	HRESULT hr;
};