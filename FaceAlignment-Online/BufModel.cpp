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
	/*seekg（）是对输入文件定位，它有两个参数：第一个参数是偏移量，第二个参数是基地址。
		对于第一个参数，可以是正负数值，正的表示向后偏移，负的表示向前偏移。而第二个参数可以是：
		ios：：beg：表示输入流的开始位置
		ios：：cur：表示输入流的当前位置
		ios：：end：表示输入流的结束位置*/
	file.seekg(0, std::ios::end);
	//tellg（）函数不需要带参数，它返回当前定位指针的位置，也代表着输入流的大小。这里因为指针在end，所以大小为文件大小
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
	cv::flip(img_res, img_res, 0); // flip图像向下翻转
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

//comment yqy180425
#if 0
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
//comment end 180425
#endif




#if 0
//commnent yqy180424
//  创建用于模型显示的GL相关内存
//CreateDispModel：创建和设置显示模型的VAO, VBO, IBO
void BufModel::CreateDispModel(Mesh& model, bool isStatic) 
{
	this->Reset();
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	// gen vertex array
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//std::vector<VertexFormat> vertices;//comment yqy180425
	std::vector<Vertex> vertices;//add yqy180425 
	std::vector<unsigned int> indices;
	int nVerts = model.n_verts_;
	int nTri = model.n_tri_;
	for (size_t kk = 0; kk < nVerts; kk++) 
	{
		vertices.push_back(Vertex(glm::vec3(model.position_(kk, 0), // verts//add yqy180425
		//vertices.push_back(VertexFormat(glm::vec3(model.position_(kk, 0), // verts//comment yqy180425
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo); //顶点属性
	//若不会去改变这个buffer的内容，用GL_STATIC_DRAW标志
	if (isStatic)
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),//add yqy180425
		//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat),//comment yqy180425
			&vertices[0], GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),//add yqy180425
		//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat),
			&vertices[0], GL_DYNAMIC_DRAW);


	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //索引数据
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		//sizeof(VertexFormat), (void*)0);
	   sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		//sizeof(VertexFormat),
		(void*)12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (void*)28);
		//sizeof(VertexFormat), (void*)28);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex),
		//sizeof(VertexFormat),
		(void*)36);

	this->vao_ = vao;
	this->vbos_.push_back(vbo);
	this->vbos_.push_back(ibo);
	this->nTri_ = nTri;
	this->isBg_ = false;
	return;
}
#endif


//  创建用于模型显示的GL相关内存
//CreateDispModel：创建和设置显示模型的VAO, VBO, IBO
void BufModel::CreateDispModel()  // 建立VAO,VBO等缓冲区
{
	glGenVertexArrays(1, &this->VAOId);
	glGenBuffers(1, &this->VBOId);
	glGenBuffers(1, &this->EBOId);

	glBindVertexArray(this->VAOId);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertData.size(),
		&this->vertData[0], GL_STATIC_DRAW);
	// 顶点位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 顶点纹理坐标
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// 顶点法向量属性
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	// 索引数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* this->indices.size(),
		&this->indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



//add endyqy180425

#if 0
//comment yqy180426
//创建用于背景显示的GL相关内存
//CreateBGplaneModel() :创建和设置背景的VAO, VBO, IBO，
//背景坐标 - 1.0, -1.0, 0.99 、1.0, -1.0, 0.99 、 1.0, 1.0, 0.99、 - 1.0, 1.0, 0.99
void BufModel::CreateBGplaneModel() 
{
	this->Reset();
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(-1.0, -1.0, 0.99),
		glm::vec2(0.0, 0.0)));
	vertices.push_back(Vertex(glm::vec3(1.0, -1.0, 0.99),
		glm::vec2(1.0, 0.0)));
	vertices.push_back(Vertex(glm::vec3(1.0, 1.0, 0.99),
		glm::vec2(1.0, 1.0)));
	vertices.push_back(Vertex(glm::vec3(-1.0, 1.0, 0.99),
		glm::vec2(0.0, 1.0)));
	/*std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(glm::vec3(-1.0, -1.0, 0.99),
		glm::vec2(0.0, 0.0)));
	vertices.push_back(VertexFormat(glm::vec3(1.0, -1.0, 0.99),
		glm::vec2(1.0, 0.0)));
	vertices.push_back(VertexFormat(glm::vec3(1.0, 1.0, 0.99),
		glm::vec2(1.0, 1.0)));
	vertices.push_back(VertexFormat(glm::vec3(-1.0, 1.0, 0.99),
		glm::vec2(0.0, 1.0)));*/

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &vertices[0], GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 4, &vertices[0], GL_STATIC_DRAW);

	GLuint _indices[] = 
	{  // Note that we start from 0!
		0, 1, 3,  // First Triangle
		1, 2, 3   // Second Triangle
	};

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)28);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)28);
	glBindVertexArray(0);

	this->vao_ = vao;
	this->vbos_.push_back(vbo);
	this->vbos_.push_back(ibo);
	this->nTri_ = 2;
	this->isBg_ = true;

	return;
}
//comment end yqy180426
#endif



//创建用于背景显示的GL相关内存
//CreateBGplaneModel() :创建和设置背景的VAO, VBO, IBO，
//背景坐标 - 1.0, -1.0, 0.99 、1.0, -1.0, 0.99 、 1.0, 1.0, 0.99、 - 1.0, 1.0, 0.99
void BufModel::CreateBGplaneModel()
{
	this->Reset();
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(glm::vec3(-1.0, -1.0, 0.99),
		glm::vec2(0.0, 0.0)));
	vertices.push_back(Vertex(glm::vec3(1.0, -1.0, 0.99),
		glm::vec2(1.0, 0.0)));
	vertices.push_back(Vertex(glm::vec3(1.0, 1.0, 0.99),
		glm::vec2(1.0, 1.0)));
	vertices.push_back(Vertex(glm::vec3(-1.0, 1.0, 0.99),
		glm::vec2(0.0, 1.0)));
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &vertices[0], GL_STATIC_DRAW);

	GLuint _indices[] =
	{  // Note that we start from 0!
		0, 1, 3,  // First Triangle
		1, 2, 3   // Second Triangle
	};

	glGenBuffers(1, &EBOId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)28);
	glBindVertexArray(0);

	this->nTri_ = 2;
	this->isBg_ = true;

	return;
}


//add yqy180426
//更新模型数据的内存
//UpdateDispModel：更新模型的VBO
void BufModel::UpdateDispModel()
{
	//unsigned int vao;
	//unsigned int vbo, ibo;
	////vao = this->vao_;
	//vbo = this->vbos_[0];//vbos_包括vbo和ibo，vbos_[0]是vbo
	std::vector<Vertex> vertices;
	//std::vector<VertexFormat> vertices;
	vertices.clear();
	//std::vector<unsigned int> indices2;
	int nVerts = this->n_verts_;
	int nTri = this->n_tri_;
	for (size_t kk = 0; kk < nVerts; ++kk)
	{
		vertices.push_back(Vertex(this->vertData[kk].position, this->vertData[kk].texCoords, this->vertData[kk].normal));
	}
	//for (int kk = 0; kk < nTri; ++kk)
	//{
	//	indices2.push_back(this->indices[kk]);
	//	//indices.push_back(this->vertData[kk].tri[0]);
	//}
	//for (size_t kk = 0; kk < nVerts; kk++)
	//{
	//	vertices.push_back(Vertex(glm::vec3(this->position_(kk, 0), // verts
	//																//vertices.push_back(VertexFormat(glm::vec3(model.position_(kk, 0), // verts
	//		this->position_(kk, 1),
	//		this->position_(kk, 2)),
	//		glm::vec4(this->color_(kk, 0) / 255.f,	// colors
	//			this->color_(kk, 1) / 255.f,
	//			this->color_(kk, 2) / 255.f, 1.f),
	//		glm::vec2(this->tex_coord_(kk, 0), // tex coord
	//			this->tex_coord_(kk, 1)),
	//		glm::vec3(this->normal_(kk, 0),  // normal
	//			this->normal_(kk, 1),
	//			this->normal_(kk, 2))));
	//}
	//std::cout << "Updating vertex buffer array <<<<<<<<<<<<<<<< ----> " << vertices.size() << " vertices" << std::endl;
	glBindVertexArray(VAOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	//将一个缓冲区对象中的数据映射为客户端中的地址空间
	GLfloat* data;
	// 获取缓冲区的映射指针data
	data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	// 拷贝我们的数据到指针所指向的位置
	memcpy(data, &vertices[0], vertices.size() * sizeof(Vertex));
	//memcpy(data, &vertices[0], vertices.size() * sizeof(VertexFormat));
	// 使用完之后释放映射的指针
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindVertexArray(0);
	return;
}
//add end yqy



#if 0
//comment  yqy180424
//更新模型数据的内存
//UpdateDispModel：更新模型的VBO
void BufModel::UpdateDispModel(Mesh& model) 
{
	unsigned int vao;
	unsigned int vbo,ibo;
	vao = this->vao_;
	vbo = this->vbos_[0];
	std::vector<Vertex> vertices;
	//std::vector<VertexFormat> vertices;
	vertices.clear();
	std::vector<unsigned int> indices;
	int nVerts = model.n_verts_;
	int nTri = model.n_tri_;
	
	for (size_t kk = 0; kk < nVerts; kk++) 
	{
		vertices.push_back(Vertex(glm::vec3(model.position_(kk, 0), // verts
		//vertices.push_back(VertexFormat(glm::vec3(model.position_(kk, 0), // verts
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
	//将一个缓冲区对象中的数据映射为客户端中的地址空间
	GLfloat* data;
	// 获取缓冲区的映射指针data
	data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	// 拷贝我们的数据到指针所指向的位置
	memcpy(data, &vertices[0], vertices.size() * sizeof(Vertex));
	//memcpy(data, &vertices[0], vertices.size() * sizeof(VertexFormat));
	// 使用完之后释放映射的指针
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindVertexArray(0);
	return;
}
//comment end yqy180424
#endif




//  Draw():渲染模型，带模型矩阵变换的GLSL，此次渲染是渲染到framebuffer的texture
void BufModel::Draw(const Shader& shader)const
{
	if (VAOId == 0
		|| VBOId == 0
		|| EBOId == 0)
	{
		return;
	}
	if (isDraw_)
	{
		// rendering
		if (this->isBg_)
		{
			glMatrixMode(GL_MODELVIEW);//将当前矩阵指定为投影矩阵
									   //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLoadIdentity(); //然后把矩阵设为单位矩阵
			glBindVertexArray(this->VAOId);
			glBindVertexArray(this->VAOId);
			shader.use();
			glEnable(GL_TEXTURE_2D);
			int diffuseCnt = 0, specularCnt = 0, texUnitCnt = 0;
			for (std::vector<Texture>::const_iterator it = this->textures.begin();
				this->textures.end() != it; ++it)
			{
				switch (it->type)
				{
				case aiTextureType_DIFFUSE://人脸模型进入这个case
				{
					//要使用纹理必须在使用之前激活对应的纹理单元，默认状态下0号纹理
					//单元是激活的，因此即使没有显式地激活也能工作。激活并使用纹理
					//的代码如下：
					glActiveTexture(GL_TEXTURE0 + texUnitCnt);
					glBindTexture(GL_TEXTURE_2D, it->id);
					std::stringstream samplerNameStr;
					samplerNameStr << "texture_diffuse" << diffuseCnt++;
					glUniform1i(glGetUniformLocation(shader.programId,
						samplerNameStr.str().c_str()), texUnitCnt++);
					//上述glUniform1i将纹理单元作为整数传递给片元着色器，片元着色器
					//中使用uniform变量对应这个纹理采样器，使用变量类型为: samplerNameStr.str().c_str()
				}
				break;
				default:
					std::cerr << "Warning::Mesh::draw, texture type" << it->type
						<< " current not supported." << std::endl;
					break;
				}
			}
			//这里的索引绘制我们使用的函数是glDrawElements而不是glDrawArrays。第
			//一个参数是要渲染的图元的类型(和glDrawArrays一样)；第二个参数是索引
			//缓冲中用于产生图元的索引个数；后面的参数是每一个索引值的数据类型。
			//必须要告诉GPU单个索引值的大小，否则GPU无法知道如何解析索引缓冲区。
			//索引值可用的类型主要有：GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, 
			//GL_UNSIGNED_INT。如果索引的范围很小应该选择小的数据类型来节省空间，
			//如果索引范围很大自然就要根据需要选择更大的数据类型；最后一个参数
			//告诉GPU从索引缓冲区起始位置到到第一个需要扫描的索引值得偏移byte数
			//，这个在使用同一个索引缓冲来保存多个物体的索引时很有用，通过定义
			//偏移量和索引个数可以告诉GPU去渲染哪一个物体，在我们的例子中我们从
			//一开始扫描所以定义偏移量为0。注意最后一个参数的类型是GLvoid*，所
			//以如果不是0的话必须要转换成那个参数类型。
			glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
			glDisable(GL_TEXTURE_2D);
			glBindVertexArray(0);
			glUseProgram(0);
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
			glBindVertexArray(this->VAOId);
			shader.use();

			//glUseProgram(shader_program);
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "modelViewMatrix"), 1, GL_FALSE, glm::value_ptr(modelview_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader_program, "projectionMatrix"), 1, GL_FALSE, glm::value_ptr(proj_mat));
			// Set rendering parameters -- dirlight only 
			SetDirLight(shader_program, dir_light);
			if (colorMode_ == 0)
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 0.0);
			else if (colorMode_ == 1)
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 1.0);
			else if (colorMode_ == 2)
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 2.0);
			else
				glUniform1f(glGetUniformLocation(shader_program, "colorMode"), 0.0);

			glEnable(GL_TEXTURE_2D);

			int diffuseCnt = 0, specularCnt = 0, texUnitCnt = 0;
			for (std::vector<Texture>::const_iterator it = this->textures.begin();
				this->textures.end() != it; ++it)
			{
				switch (it->type)
				{
				case aiTextureType_DIFFUSE://人脸模型进入这个case
				{
					glActiveTexture(GL_TEXTURE0 + texUnitCnt);
					glBindTexture(GL_TEXTURE_2D, it->id);
					std::stringstream samplerNameStr;
					samplerNameStr << "texture_diffuse" << diffuseCnt++;
					glUniform1i(glGetUniformLocation(shader.programId,
						samplerNameStr.str().c_str()), texUnitCnt++);
				}
				break;
				default:
					std::cerr << "Warning::Mesh::draw, texture type" << it->type
						<< " current not supported." << std::endl;
					break;
				}
			}
			glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
			glDisable(GL_TEXTURE_2D);
			glBindVertexArray(0);
			glUseProgram(0);
		}
	}
	return;
}


#if 0
//comment yqy180503
//  Draw():渲染模型，带模型矩阵变换的GLSL，此次渲染是渲染到framebuffer的texture
void BufModel::Draw() 
{
	if (isDraw_) 
	{
		// rendering
		if (this->isBg_) 
		{
			glMatrixMode(GL_MODELVIEW);//将当前矩阵指定为投影矩阵
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glLoadIdentity(); //然后把矩阵设为单位矩阵
			glBindVertexArray(this->VAOId);
			//glBindVertexArray(this->vao_);
			glUseProgram(this->shader_program_);
			glEnable(GL_TEXTURE_2D);
			//要使用纹理必须在使用之前激活对应的纹理单元，默认状态下0号纹理
			//单元是激活的，因此即使没有显式地激活也能工作。激活并使用纹理
			//的代码如下：
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->tex_);
			unsigned int textureLocation = glGetUniformLocation(this->shader_program_, "texture1");
			glUniform1i(textureLocation, 0);
			//上述glUniform1i将纹理单元作为整数传递给片元着色器，片元着色器
			//中使用uniform变量对应这个纹理采样器，使用变量类型为: texture1
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
			glBindVertexArray(this->VAOId);
			//glBindVertexArray(this->vao_);
			glDrawElements(GL_TRIANGLES, 3 * this->nTri_, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	return;
}
//comment end 180503
#endif


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
			//glBindVertexArray(this->vao_);
			glBindVertexArray(this->VAOId);
			glDrawElements(GL_TRIANGLES, 3 * this->nTri_, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glUseProgram(0);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		}
	}
	return;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 0
//add yqy180425
void BufModel::draw(const Shader& shader) const// 绘制Mesh
{
	if (VAOId == 0
		|| VBOId == 0
		|| EBOId == 0)
	{
		return;
	}
	shader.use();
	glBindVertexArray(this->VAOId);
	int diffuseCnt = 0, specularCnt = 0, texUnitCnt = 0;
	for (std::vector<Texture>::const_iterator it = this->textures.begin();
		this->textures.end() != it; ++it)
	{
		switch (it->type)
		{
		case aiTextureType_DIFFUSE:
		{
			glActiveTexture(GL_TEXTURE0 + texUnitCnt);
			glBindTexture(GL_TEXTURE_2D, it->id);
			std::stringstream samplerNameStr;
			samplerNameStr << "texture_diffuse" << diffuseCnt++;
			glUniform1i(glGetUniformLocation(shader.programId,
				samplerNameStr.str().c_str()), texUnitCnt++);
		}
		break;
		case aiTextureType_SPECULAR:
		{
			glActiveTexture(GL_TEXTURE0 + texUnitCnt);
			glBindTexture(GL_TEXTURE_2D, it->id);
			std::stringstream samplerNameStr;
			samplerNameStr << "texture_specular" << specularCnt++;
			glUniform1i(glGetUniformLocation(shader.programId,
				samplerNameStr.str().c_str()), texUnitCnt++);
		}
		break;
		default:
			std::cerr << "Warning::Mesh::draw, texture type" << it->type
				<< " current not supported." << std::endl;
			break;
		}
	}

	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
#endif
