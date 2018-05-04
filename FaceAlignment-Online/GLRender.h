#pragma once

#include "BufModel.h"

#include <Eigen\Eigen>
#include <opencv2\opencv.hpp>
#include <vector>

class GLRender {

public:
	GLRender();
	~GLRender();

	void Init(Eigen::Vector2i winSize);
	void GetThumb(cv::Mat& disp);
	void GetThumbDepth(cv::Mat& disp);//yqy没用到
	int GetWidth() { return win_size_[0]; }
	int GetHeight() { return win_size_[1]; }

	GLuint framebuffer_;
	GLuint colorAttach_;
	GLuint depthAttach_;

	Eigen::Vector2i win_size_;
	cv::Mat render_color_;
	cv::Mat1f render_depth_;

public:
	// add models in this part
	std::map<int, BufModel> models;
	std::map<int, SolidSphere> spheres;

	void addModel(BufModel& model, int modelIdx) { models[modelIdx] = model; }
	void addSphere(SolidSphere& model, int modelIdx) { spheres[modelIdx] = model; }
	void deleteModel(int modelIdx) { models.erase(modelIdx); }
	void deleteSphere(int spIdx) { this->spheres.erase(spIdx); }
	void Render() 
	{
		Shader shader("model.vertex", "model.frag");//add yqy180503
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		int w = win_size_[0];
		int h = win_size_[1];
		glViewport(0, 0, w, h);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);//指定“目标像素与当前像素在z方向上值大小比较”的函数，符合该函数关系的目标像素才进行绘制，否则对目标像素不予绘制。
		//GL_LESS,如果输入的深度值小于参考值，则通过
		glClearDepth(1);          //
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.3, 0.3, 1.0);
		// glShadeModel( GL_SMOOTH ); 

		// render bg and model
		for (int kk = 0; kk < models.size(); kk++) 
		{
			//this->models[kk].Draw();//comment yqy180503
			this->models[kk].Draw(shader);//add yqy180503
		}
		// render sphere
		for (int kk = 0; kk < spheres.size(); kk++) 
		{
			this->spheres[kk].Draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderFace() 
	{
		Shader shader("model.vertex", "model.frag");//add yqy180503
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
		int w = win_size_[0];
		int h = win_size_[1];
		glViewport(0, 0, w, h);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearDepth(1);          //
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.3, 0.3, 1.0);
		// glShadeModel( GL_SMOOTH ); 
		//this->models[1].Draw();//comment yqy180503
		this->models[1].Draw(shader);//add yqy180503
		this->spheres[0].Draw();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

};