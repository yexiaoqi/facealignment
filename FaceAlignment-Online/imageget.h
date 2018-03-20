#pragma once
#include "stdafx.h"
#include <opencv2\opencv.hpp> 
#include<iostream>
//windows��ͷ�ļ�������Ҫ����ȻNuiApi.h�ò���
#include <Windows.h>
//Kinect for windows ��ͷ�ļ�
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
	//yqyӦ����dmΪ��λ
	//const int THRESHOLD = 3500;//3.5m
	const int MAX_DISTANCE = 3500;//3.5m
	const int MIN_DISTANCE = 200;//0.2m
	const LONG m_depthWidth = 640;
	const LONG m_depthHeight = 480;
	const LONG m_colorWidth = 640;
	const LONG m_colorHeight = 480;
	//��ɫͼ��
	Mat image_rgb;
	//���ͼ��
	Mat image_depth;
	int colorFrameCount ;
	int depthFrameCount ;
	//һ��KINECTʵ��ָ��
	INuiSensor* m_pNuiSensor ;
	//��¼��ǰ����KINECT��������Ϊ��������׼����
	int iSensorCount;
	//��õ�ǰKINECT������
	HRESULT hr;
};