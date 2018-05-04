#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include<GL\glew.h>
//#include <GLEW/glew.h>
#include <iostream>
#include <fstream>
#include <SOIL.h>
class TextureHelper
{
public:
	/*
	/* 成功加载2D纹理则返回纹理对象Id 否则返回0
	*/
	static  GLuint load2DTexture(const char* filename, GLint internalFormat = GL_RGBA,
		GLenum picFormat = GL_RGBA, int loadChannels = SOIL_LOAD_RGBA)
	{
		// Step1 创建并绑定纹理对象
		GLuint textureId = 0;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);//这里我们绑定到GL_TEXTURE_2D目标，表示二维纹理。
												// Step2 设定wrap参数
												//GL_REPEAT:坐标的整数部分被忽略，重复纹理，这是OpenGL纹理默认的处理方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Step3 设定filter参数
		/*	一个纹素最终对应屏幕上的多个像素 这称之为放大(magnification)
		一个纹素对应屏幕上的一个像素 这种情况不需要滤波方法
		一个纹素对应少于一个像素，或者说多个纹素对应屏幕上的一个像素 这个称之为缩小
		在OpenGL中通过使用下面的函数，为纹理的放大和缩小滤波设置相关的控制选项*/
		//当物体在场景中离观察者很远，最终只用一个屏幕像素来显
		//示时，这个像素该如何通过纹素确定呢？如果使用最近邻滤
		//波来获取这个纹素，那么显示效果并不理想。需要使用纹
		//的均值来反映物体在场景中离我们很远这个效果，对于一个
		//256×256的纹理，计算平均值是一个耗时工作，不能实时计
		//算，因此可以通过提前计算一组这样的纹理用来满足这种需
		//求。这组提前计算的按比例缩小的纹理就是Mipmaps。Mipmaps
		//纹理大小每级是前一等级的一半
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR对应线性滤波
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR); // 为MipMap设定filter方法
									  // Step4 使用SOIL加载纹理
		GLubyte *imageData = NULL;
		int picWidth, picHeight;
		int channels = 0;
		imageData = SOIL_load_image(filename, &picWidth, &picHeight, &channels, loadChannels);// 读取图片数据
		if (imageData == NULL)
		{
			std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
			return 0;
		}
		//glTexImage2D函数定义纹理图像的格式，宽度和高度等信息，具体参数如下：

		//	API void glTexImage2D(GLenum target,
		//		GLint level,
		//		GLint internalFormat,
		//		GLsizei width,
		//		GLsizei height,
		//		GLint border,
		//		GLenum format,
		//		GLenum type,
		//		const GLvoid * data);
		//1.target参数指定设置的纹理目标，必须是GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D等参数。
		//	2.level指定纹理等级，0代表原始纹理，其余等级对应Mipmap纹理等级。
		//	3.internalFormat指定OpenGL存储纹理的格式，我们读取的图片格式包含RGB颜色，因此这里也是用RGB颜色。
		//	4.width和height参数指定存储的纹理大小，我们之前利用SOIL读取图片时已经获取了图片大小，这里直接使用即可。
		//	5. border 参数为历史遗留参数，只能设置为0.
		//	6. 最后三个参数指定原始图片数据的格式(format)和数据类型(type, 为GL_UNSIGNED_BYTE, GL_BYTE等值)，以及数据的内存地址(data指针)。
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, picWidth, picHeight,
			0, picFormat, GL_UNSIGNED_BYTE, imageData);// 定义纹理图像
													   //使用MipMap时设置GL_TEXTURE_MIN_FILTER选项才能起作用，设置GL_TEXTURE_MAG_FILTER的Mipmap选项将会导致无效操作
		glGenerateMipmap(GL_TEXTURE_2D);//OpenGL中通过函数glGenerateMipmap(GL_TEXTURE_2D);来生成Mipmap，前提是已经指定了原始纹理。原始纹理必须自己通过读取纹理图片来加载
										// Step5 释放纹理图片资源
		SOIL_free_image_data(imageData);
		glBindTexture(GL_TEXTURE_2D, 0);
		return textureId;
	}
};

#endif