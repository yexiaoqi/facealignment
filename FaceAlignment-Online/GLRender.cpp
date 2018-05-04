#include "GLRender.h"
#include <glm\gtc\type_ptr.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////
GLRender::GLRender() {}
GLRender::~GLRender() {}

//绑定rgb和depth到FBO缓冲区
void GLRender::Init(Eigen::Vector2i winSize)
{
	this->win_size_ = winSize;

	glGenFramebuffers(1, &framebuffer_);//创建FBO
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);//绑定FBO到目标对象，GL_FRAMEBUFFER缓冲区将会用来进行读和写操作
	//create color attachment and depth attachment
	GLenum attachment_type;
	attachment_type = GL_RGBA;
	glGenTextures(1, &colorAttach_);
	glBindTexture(GL_TEXTURE_2D, colorAttach_);
	glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, winSize[0], winSize[1], 0, attachment_type, GL_UNSIGNED_BYTE, NULL);//给纹理附件分配内存但是不填充它
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	/*创建FBO的附加纹理如同平常使用纹理一样，不同的是，这里只是为纹理预分配空间，而不需要真正的加载纹理，因为当使用FBO渲染时渲染结果将会写入到我们创建的这个纹理上去。附加纹理使用函数glFramebufferTexture2D。

		API void glFramebufferTexture2D(GLenum target,
			GLenum attachment,
			GLenum textarget, GLuint texture, GLint level);
	1.target表示绑定目标，参数可选为GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER, or GL_FRAMEBUFFER。
		2.attechment表示附加点，可选值为GL_COLOR_ATTACHMENTi, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT or GL_DEPTH_STENCIL_ATTACHMMENT。
		3. textTarget表示纹理的绑定目标，我们使用二维纹理填写GL_TEXTURE_2D即可。
		4. texture表示实际的纹理对象。
		5. level表示 mipmap级别，我们填写0即可。*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttach_, 0);
	//depthAttach_是我们实际创建的纹理对象,在创建纹理对象时使用代码:
	glGenTextures(1, &depthAttach_);
	glBindTexture(GL_TEXTURE_2D, depthAttach_);
	/*glTexImage2D函数，末尾的NULL表示我们只预分配空间，而不实际加载纹理。

		API void glTexImage2D(GLenum target,
			GLint level,
			GLint internalFormat,
			GLsizei width,
			GLsizei height,
			GLint border,
			GLenum format,
			GLenum type,
			const GLvoid * data);
注意这个纹理需要和我们渲染的屏幕大小保持一致，如果需要绘制与屏幕不一致的纹理，使用glViewport函数进行调节。*/
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


//读取RGB像素值到disp中
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

//yqy没用到
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
	cv::minMaxIdx(disp, &min, &max);//对其中的数据求取最大值和最小值
	//std::cout << "Min & Max is " << min <<"  "<<max << std::endl;
	cv::Mat adjMap;
	disp.convertTo(adjMap, CV_8UC1, 255 / (max - min), -min);
	cv::Mat falseColorsMap;
	applyColorMap(adjMap, falseColorsMap, cv::COLORMAP_AUTUMN);
	cv::cvtColor(falseColorsMap, disp, CV_RGB2RGBA);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

