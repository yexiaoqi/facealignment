#include "GLRender.h"
#include <glm\gtc\type_ptr.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////
GLRender::GLRender() {}
GLRender::~GLRender() {}

void GLRender::Init(Eigen::Vector2i winSize)
{
	this->win_size_ = winSize;

	glGenFramebuffers(1, &framebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	//create color attachment and depth attachment
	GLenum attachment_type;
	attachment_type = GL_RGBA;
	glGenTextures(1, &colorAttach_);
	glBindTexture(GL_TEXTURE_2D, colorAttach_);
	glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, winSize[0], winSize[1], 0, attachment_type, GL_UNSIGNED_BYTE, NULL);//给纹理附件分配内存但是不填充它
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttach_, 0);

	glGenTextures(1, &depthAttach_);
	glBindTexture(GL_TEXTURE_2D, depthAttach_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, winSize[0], winSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttach_, 0);

	GLenum DrawBuffers[2] = { GL_COLOR_ATTACHMENT0 ,GL_DEPTH_ATTACHMENT };
	glDrawBuffers(2, DrawBuffers); // "1" is the size of DrawBuffers

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete! " << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::cout << "init-> " << winSize[0] << " " << winSize[1] << std::endl;
}

void GLRender::GetThumb(cv::Mat & disp)
{
	int width = win_size_[0];
	int height = win_size_[1];
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	unsigned char* colorDisp = new unsigned char[4 * width * height];
	//glReadPixels：读取一些像素。当前可以简单理解为“把已经绘制好的像素（它可能已经被保存到显卡的显存中）读取到内存”
	//七个参数表示一个指针，像素数据被读取后，将被保存到这个指针所表示的地址。注意，需要保证
	//该地址有足够的可以使用的空间，以容纳读取的像素数据。例如一幅大小为256*256的图象，如果读
	//取其RGB数据，且每一数据被保存为GLubyte，总大小就是：256*256*3 = 196608字节，即192千字节
	//。如果是读取RGBA数据，则总大小就是256*256*4 = 262144字节，即256千字节。
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorDisp);
	cv::Mat _tmp = cv::Mat(height, width, CV_8UC4, colorDisp);
	_tmp.copyTo(this->render_color_);
	cv::flip(this->render_color_, this->render_color_, 0);
	delete colorDisp;
	this->render_color_.copyTo(disp);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRender::GetThumbDepth(cv::Mat& disp) {

	int width = win_size_[0];
	int height = win_size_[1];
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	float* depthData = new float[width * height];
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depthData);
	cv::Mat _tmp = cv::Mat(height, width, CV_32FC1, depthData);
	_tmp.copyTo(disp);
	delete depthData;
	// cvt to image
	double min;
	double max;
	cv::minMaxIdx(disp, &min, &max);
	//std::cout << "Min & Max is " << min <<"  "<<max << std::endl;
	cv::Mat adjMap;
	disp.convertTo(adjMap, CV_8UC1, 255 / (max - min), -min);
	cv::Mat falseColorsMap;
	applyColorMap(adjMap, falseColorsMap, cv::COLORMAP_AUTUMN);
	cv::cvtColor(falseColorsMap, disp, CV_RGB2RGBA);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

