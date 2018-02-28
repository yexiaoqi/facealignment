#include "BufModel.h"
#include <glm\gtc\type_ptr.hpp>
#include <string>
#include <iterator>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* set directional light */
void SetDirLight(GLuint shader_program, DirLight& dl_tmp) 
{
	glUniform3f(glGetUniformLocation(shader_program, "dirLight.direction"), dl_tmp.direction[0], dl_tmp.direction[1], dl_tmp.direction[2]);

	glUniform3f(glGetUniformLocation(shader_program, "dirLight.ambient"), dl_tmp.ambient[0], dl_tmp.ambient[1], dl_tmp.ambient[2]);

	glUniform3f(glGetUniformLocation(shader_program, "dirLight.diffuse"), dl_tmp.diffuse[0], dl_tmp.diffuse[1], dl_tmp.diffuse[2]);

	glUniform1f(glGetUniformLocation(shader_program, "dirLight.shininess"), dl_tmp.shininess);

	glUniform1f(glGetUniformLocation(shader_program, "dirLight.v"), dl_tmp.v);

	return;
}

/* shader */
std::string ReadShader(const char *filename)
{
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	if (!file.good())
	{
		std::cout << "Can't read file " << filename << std::endl;
		std::terminate();
	}
	/*seekg�����Ƕ������ļ���λ������������������һ��������ƫ�������ڶ��������ǻ���ַ��
		���ڵ�һ��������������������ֵ�����ı�ʾ���ƫ�ƣ����ı�ʾ��ǰƫ�ơ����ڶ������������ǣ�
		ios����beg����ʾ�������Ŀ�ʼλ��
		ios����cur����ʾ�������ĵ�ǰλ��
		ios����end����ʾ�������Ľ���λ��*/
	file.seekg(0, std::ios::end);
	//tellg������������Ҫ�������������ص�ǰ��λָ���λ�ã�Ҳ�������������Ĵ�С��������Ϊָ����end�����Դ�СΪ�ļ���С
	shaderCode.resize((unsigned int)file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

GLuint CreateShader(GLenum shaderType, std::string source, char* shaderName)
{
	int compile_result = 0;

	GLuint shader = glCreateShader(shaderType);
	const char *shader_code_ptr = source.c_str();
	const int shader_code_size = source.size();

	glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

	//check for errors
	if (compile_result == GL_FALSE)
	{
		int info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> shader_log(info_log_length);
		glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);
		std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shader_log[0] << std::endl;
		return 0;
	}
	return shader;
}

GLuint CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) 
{
	//read the shader files and save the code
	std::string vertex_shader_code = ReadShader(vertexShaderFilename);
	std::string fragment_shader_code = ReadShader(fragmentShaderFilename);

	GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, "vertex shader");
	GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, "fragment shader");

	int link_result = 0;
	//create the program handle, attatch the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	//check for link errors
	if (link_result == GL_FALSE)
	{
		int info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector<char> program_log(info_log_length);
		glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
		return -1;
	}
	return program;
}

/* texture */
unsigned int LoadTexture(const std::string& filename, unsigned int width, unsigned int height) 
{
	cv::Mat img, img_rgb, img_res;
	unsigned char* data;
	/* preprocessing */
	img = cv::imread(filename, cv::IMREAD_COLOR);	// bgr image
	cv::cvtColor(img, img_rgb, cv::COLOR_BGR2RGBA);	// bgr to rgba
	cv::resize(img_rgb, img_res, cv::Size(width, height)); // resize 
	cv::flip(img_res, img_res, 0); // flipͼ�����·�ת
	data = img_res.data; // retrieve data
						 /* create opengl texture */
	unsigned int gl_texture_object;
	glGenTextures(1, &gl_texture_object);
	glBindTexture(GL_TEXTURE_2D, gl_texture_object);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	float maxAnisotropy;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	return gl_texture_object;
}

unsigned int CreateTexture(unsigned int width, unsigned int height) 
{
	unsigned int gl_texture_object;
	glGenTextures(1, &gl_texture_object);
	glBindTexture(GL_TEXTURE_2D, gl_texture_object);
	//filtering
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	float maxAnisotropy;
	//Generates texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	return gl_texture_object;
}

void UpdateTexture(unsigned int texId, cv::Mat& image) 
{	
	cv::Mat tmp,img_rgba;
	cv::cvtColor(image, img_rgba, CV_BGR2RGBA);
	img_rgba.copyTo(tmp);
	cv::flip(tmp, tmp, 0);
	int width = image.cols;
	int height = image.rows;
	// default is RGBA
	unsigned char* data;
	data = tmp.data;
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/* constructor */
BufModel::BufModel() 
{
	Reset();
}

BufModel::~BufModel() {

	//delete VAO and VBOs (if many)
	unsigned int p = this->vao_;
	glDeleteVertexArrays(1, &p);
	glDeleteBuffers(this->vbos_.size(), &this->vbos_[0]);
	this->vbos_.clear();
}

//  ��������ģ����ʾ��GL����ڴ�
//CreateDispModel��������������ʾģ�͵�VAO, VBO, IBO
void BufModel::CreateDispModel(Mesh& model, bool isStatic) 
{
	this->Reset();
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	// gen vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;
	int nVerts = model.n_verts_;
	int nTri = model.n_tri_;
	for (size_t kk = 0; kk < nVerts; kk++) 
	{
		vertices.push_back(VertexFormat(glm::vec3(model.position_(kk, 0), // verts
			model.position_(kk, 1),
			model.position_(kk, 2)),
			glm::vec4(model.color_(kk, 0) / 255.f,	// colors
				model.color_(kk, 1) / 255.f,
				model.color_(kk, 2) / 255.f, 1.f),
			glm::vec2(model.tex_coord_(kk, 0), // tex coord
				model.tex_coord_(kk, 1)),
			glm::vec3(model.normal_(kk, 0),  // normal
				model.normal_(kk, 1),
				model.normal_(kk, 2))));
	}
	for (int kk = 0; kk < nTri; kk++) 
	{
		indices.push_back(model.tri_list_(kk, 0));
		indices.push_back(model.tri_list_(kk, 1));
		indices.push_back(model.tri_list_(kk, 2));
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //��������
	//������ȥ�ı����buffer�����ݣ���GL_STATIC_DRAW��־
	if (isStatic)
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat),
			&vertices[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat),
			&vertices[0], GL_DYNAMIC_DRAW);


	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //��������
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexFormat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(VertexFormat),
		(void*)12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		sizeof(VertexFormat), (void*)28);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
		sizeof(VertexFormat),
		(void*)36);

	this->vao_ = vao;
	this->vbos_.push_back(vbo);
	this->vbos_.push_back(ibo);
	this->nTri_ = nTri;
	this->isBg_ = false;
	return;
}

//�������ڱ�����ʾ��GL����ڴ�
//CreateBGplaneModel() :���������ñ�����VAO, VBO, IBO��
//�������� - 1.0, -1.0, 0.99 ��1.0, -1.0, 0.99 �� 1.0, 1.0, 0.99�� - 1.0, 1.0, 0.99
void BufModel::CreateBGplaneModel() 
{
	this->Reset();
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(glm::vec3(-1.0, -1.0, 0.99),
		glm::vec2(0.0, 0.0)));
	vertices.push_back(VertexFormat(glm::vec3(1.0, -1.0, 0.99),
		glm::vec2(1.0, 0.0)));
	vertices.push_back(VertexFormat(glm::vec3(1.0, 1.0, 0.99),
		glm::vec2(1.0, 1.0)));
	vertices.push_back(VertexFormat(glm::vec3(-1.0, 1.0, 0.99),
		glm::vec2(0.0, 1.0)));

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 4, &vertices[0], GL_STATIC_DRAW);

	GLuint _indices[] = 
	{  // Note that we start from 0!
		0, 1, 3,  // First Triangle
		1, 2, 3   // Second Triangle
	};

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)28);
	glBindVertexArray(0);

	this->vao_ = vao;
	this->vbos_.push_back(vbo);
	this->vbos_.push_back(ibo);
	this->nTri_ = 2;
	this->isBg_ = true;

	return;
}


//����ģ�����ݵ��ڴ�
//UpdateDispModel������ģ�͵�VBO
void BufModel::UpdateDispModel(Mesh& model) 
{
	unsigned int vao;
	unsigned int vbo,ibo;
	vao = this->vao_;
	vbo = this->vbos_[0];

	std::vector<VertexFormat> vertices;
	vertices.clear();
	std::vector<unsigned int> indices;
	int nVerts = model.n_verts_;
	int nTri = model.n_tri_;
	
	for (size_t kk = 0; kk < nVerts; kk++) 
	{
		vertices.push_back(VertexFormat(glm::vec3(model.position_(kk, 0), // verts
			model.position_(kk, 1),
			model.position_(kk, 2)),
			glm::vec4(model.color_(kk, 0) / 255.f,	// colors
				model.color_(kk, 1) / 255.f,
				model.color_(kk, 2) / 255.f, 1.f),
			glm::vec2(model.tex_coord_(kk, 0), // tex coord
				model.tex_coord_(kk, 1)),
			glm::vec3(model.normal_(kk, 0),  // normal
				model.normal_(kk, 1),
				model.normal_(kk, 2))));
	}
	//std::cout << "Updating vertex buffer array <<<<<<<<<<<<<<<< ----> " << vertices.size() << " vertices" << std::endl;
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//��һ�������������е�����ӳ��Ϊ�ͻ����еĵ�ַ�ռ�
	GLfloat* data;
	// ��ȡ��������ӳ��ָ��data
	data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	// �������ǵ����ݵ�ָ����ָ���λ��
	memcpy(data, &vertices[0], vertices.size() * sizeof(VertexFormat));
	// ʹ����֮���ͷ�ӳ���ָ��
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindVertexArray(0);
	return;
}


//  Draw():��Ⱦģ�ͣ���ģ�;���任��GLSL���˴���Ⱦ����Ⱦ��framebuffer��texture
void BufModel::Draw() 
{
	if (isDraw_) 
	{
		// rendering
		if (this->isBg_) 
		{
			glMatrixMode(GL_MODELVIEW);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLoadIdentity();
			glBindVertexArray(this->vao_);
			glUseProgram(this->shader_program_);
			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->tex_);
			unsigned int textureLocation = glGetUniformLocation(this->shader_program_, "texture1");
			glUniform1i(textureLocation, 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glDisable(GL_TEXTURE_2D);
			glUseProgram(0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			GLuint shader_program = this->shader_program_;
			glm::mat4 modelview_mat = this->view_ * this->model_;
			glm::mat4 proj_mat = this->proj_;
			DirLight dir_light = this->dirLight_;
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glUseProgram(shader_program);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelview_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(proj_mat));
			// Set rendering parameters -- dirlight only 
			SetDirLight(shader_program, dir_light);
			if(colorMode_ == 0)
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 0.0);
			else if(colorMode_ == 1)
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 1.0);
			else if(colorMode_ == 2)
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 2.0);
			else
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 0.0);

			glEnable(GL_TEXTURE_2D);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->tex_);
			unsigned int textureLocation = glGetUniformLocation(shader_program, "texture1");
			glUniform1i(textureLocation, 0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindVertexArray(this->vao_);
			glDrawElements(GL_TRIANGLES, 3 * this->nTri_, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void SolidSphere::SetData(Eigen::MatrixXf pos) 
{
	this->center_positions_ = pos;
	this->nPoint_ = pos.rows();
}

void SolidSphere::SetColor(float r, float g, float b) 
{
	this->r_ = r; this->g_ = g; this->b_ = b;
}

void SolidSphere::SetScale(float scale) 
{
	this->scale_ = scale;
}

void SolidSphere::Draw()
{
	if (this->drawSphere_) 
	{
		for (int kk = 0;kk < this->nPoint_;kk++) 
		{
			GLuint shader_program = this->shader_program_;
			glm::mat4 model_pos = glm::mat4(1.0);
			model_pos = glm::translate(model_pos, glm::vec3(center_positions_(kk, 0), center_positions_(kk, 1), center_positions_(kk, 2)));
			model_pos = glm::scale(model_pos, glm::vec3(scale_, scale_, scale_));

			glm::mat4 modelview_mat = this->view_ * this->model_ * model_pos;
			glm::mat4 proj_mat = this->proj_;

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glUseProgram(shader_program);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelview_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(proj_mat));

			if(kk == exception_idx_)
				glUniform3f(glGetUniformLocation(shader_program, "ptColor"), 0.f, 1.f, 0.f);
			else
				glUniform3f(glGetUniformLocation(shader_program, "ptColor"), this->r_, this->g_, this->b_);

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindVertexArray(this->vao_);
			glDrawElements(GL_TRIANGLES, 3 * this->nTri_, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		}
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
