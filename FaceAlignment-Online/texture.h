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
	/* �ɹ�����2D�����򷵻��������Id ���򷵻�0
	*/
	static  GLuint load2DTexture(const char* filename, GLint internalFormat = GL_RGBA,
		GLenum picFormat = GL_RGBA, int loadChannels = SOIL_LOAD_RGBA)
	{
		// Step1 ���������������
		GLuint textureId = 0;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);//�������ǰ󶨵�GL_TEXTURE_2DĿ�꣬��ʾ��ά����
												// Step2 �趨wrap����
												//GL_REPEAT:������������ֱ����ԣ��ظ���������OpenGL����Ĭ�ϵĴ���ʽ
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Step3 �趨filter����
		/*	һ���������ն�Ӧ��Ļ�ϵĶ������ ���֮Ϊ�Ŵ�(magnification)
		һ�����ض�Ӧ��Ļ�ϵ�һ������ �����������Ҫ�˲�����
		һ�����ض�Ӧ����һ�����أ�����˵������ض�Ӧ��Ļ�ϵ�һ������ �����֮Ϊ��С
		��OpenGL��ͨ��ʹ������ĺ�����Ϊ����ķŴ����С�˲�������صĿ���ѡ��*/
		//�������ڳ�������۲��ߺ�Զ������ֻ��һ����Ļ��������
		//ʾʱ��������ظ����ͨ������ȷ���أ����ʹ���������
		//������ȡ������أ���ô��ʾЧ���������롣��Ҫʹ����
		//�ľ�ֵ����ӳ�����ڳ����������Ǻ�Զ���Ч��������һ��
		//256��256����������ƽ��ֵ��һ����ʱ����������ʵʱ��
		//�㣬��˿���ͨ����ǰ����һ��������������������������
		//��������ǰ����İ�������С���������Mipmaps��Mipmaps
		//�����Сÿ����ǰһ�ȼ���һ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR��Ӧ�����˲�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR); // ΪMipMap�趨filter����
									  // Step4 ʹ��SOIL��������
		GLubyte *imageData = NULL;
		int picWidth, picHeight;
		int channels = 0;
		imageData = SOIL_load_image(filename, &picWidth, &picHeight, &channels, loadChannels);// ��ȡͼƬ����
		if (imageData == NULL)
		{
			std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
			return 0;
		}
		//glTexImage2D������������ͼ��ĸ�ʽ����Ⱥ͸߶ȵ���Ϣ������������£�

		//	API void glTexImage2D(GLenum target,
		//		GLint level,
		//		GLint internalFormat,
		//		GLsizei width,
		//		GLsizei height,
		//		GLint border,
		//		GLenum format,
		//		GLenum type,
		//		const GLvoid * data);
		//1.target����ָ�����õ�����Ŀ�꣬������GL_TEXTURE_2D, GL_PROXY_TEXTURE_2D�Ȳ�����
		//	2.levelָ������ȼ���0����ԭʼ��������ȼ���ӦMipmap����ȼ���
		//	3.internalFormatָ��OpenGL�洢����ĸ�ʽ�����Ƕ�ȡ��ͼƬ��ʽ����RGB��ɫ���������Ҳ����RGB��ɫ��
		//	4.width��height����ָ���洢�������С������֮ǰ����SOIL��ȡͼƬʱ�Ѿ���ȡ��ͼƬ��С������ֱ��ʹ�ü��ɡ�
		//	5. border ����Ϊ��ʷ����������ֻ������Ϊ0.
		//	6. �����������ָ��ԭʼͼƬ���ݵĸ�ʽ(format)����������(type, ΪGL_UNSIGNED_BYTE, GL_BYTE��ֵ)���Լ����ݵ��ڴ��ַ(dataָ��)��
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, picWidth, picHeight,
			0, picFormat, GL_UNSIGNED_BYTE, imageData);// ��������ͼ��
													   //ʹ��MipMapʱ����GL_TEXTURE_MIN_FILTERѡ����������ã�����GL_TEXTURE_MAG_FILTER��Mipmapѡ��ᵼ����Ч����
		glGenerateMipmap(GL_TEXTURE_2D);//OpenGL��ͨ������glGenerateMipmap(GL_TEXTURE_2D);������Mipmap��ǰ�����Ѿ�ָ����ԭʼ����ԭʼ��������Լ�ͨ����ȡ����ͼƬ������
										// Step5 �ͷ�����ͼƬ��Դ
		SOIL_free_image_data(imageData);
		glBindTexture(GL_TEXTURE_2D, 0);
		return textureId;
	}
};

#endif