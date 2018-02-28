#include <string>
#include <iterator>
#include <algorithm>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include"BufModel.h"
#include "GenModels.h"

GenModels::GenModels(){}

GenModels::~GenModels(){

	std::map<std::string, Model>::iterator it;
	for (it = GameModelList.begin(); it != GameModelList.end(); ++it){
		//delete VAO and VBOs (if many)
		unsigned int* p = &it->second.vao;
		glDeleteVertexArrays(1, p);
		glDeleteBuffers(it->second.vbos.size(), &it->second.vbos[0]);
		it->second.vbos.clear();
	}
	GameModelList.clear();
}

void GenModels::CreateDynamicFaceModel_MRT(const std::string& gameModelName, Mesh& ref_model, bool isFrontFace, bool useMeshTex, Eigen::MatrixXf& face_mask){

	unsigned int vao;
	unsigned int vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	std::vector<VertexFormat> vertices;
	nTriFront = 0;
	if (isFrontFace){
		// iterate over the triangles
		for (size_t idx = 0;idx < ref_model.n_tri_;idx++){
			unsigned int id1 = ref_model.tri_list_(idx, 2);
			unsigned int id2 = ref_model.tri_list_(idx, 1);
			unsigned int id3 = ref_model.tri_list_(idx, 0);
			if (face_mask(id1) > 0.5 && face_mask(id2, 0) > 0.5 && face_mask(id3, 0) > 0.5){
				size_t offset1 = 3 * id1;
				size_t offset2 = 3 * id2;
				size_t offset3 = 3 * id3;
				
				if (useMeshTex){

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
						glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
						glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
						glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
						glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
						glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
						glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
						glm::vec2(idx + 1, idx + 1)));
				}
				else{
					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
						glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
						glm::vec3(1,0,0),
						glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
						glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
						glm::vec3(0,1,0),
						glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
						glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
						glm::vec3(0,0,1),
						glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
						glm::vec2(idx + 1, idx + 1)));
				}


				nTriFront++;
			}
		}
	}else{
		for (size_t idx = 0;idx < ref_model.n_tri_;idx++){
			unsigned int id1 = ref_model.tri_list_(idx, 2);
			unsigned int id2 = ref_model.tri_list_(idx, 1);
			unsigned int id3 = ref_model.tri_list_(idx, 0);
			
			size_t offset1 = 3 * id1;
			size_t offset2 = 3 * id2;
			size_t offset3 = 3 * id3;

			if (useMeshTex){

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
					glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
					glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
					glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
					glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
					glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
					glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
					glm::vec2(idx + 1, idx + 1)));
			}
			else{
			
				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
					glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
					glm::vec3(1, 0, 0),
					glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
					glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
					glm::vec3(0, 1, 0),
					glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
					glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
					glm::vec3(0, 0, 1),
					glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
					glm::vec2(idx + 1, idx + 1)));
			}
			nTriFront++;
		}
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat),
		&vertices[0], GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	sizeof(VertexFormat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,	sizeof(VertexFormat), (void*)12);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,	sizeof(VertexFormat), (void*)28);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,	sizeof(VertexFormat), (void*)40);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)52);

	Model myModel;
	myModel.vao = vao;
	myModel.vbos.push_back(vbo);
	GameModelList[gameModelName] = myModel;
}

void GenModels::UpdateVBO_MRT(const std::string& gameModelName, Mesh& ref_model, bool isFrontFace, bool useMeshTex, Eigen::MatrixXf& face_mask)
{
	unsigned int vao;
	unsigned int vbo;
	vao = GameModelList[gameModelName].vao;
	vbo = GameModelList[gameModelName].vbos[0];

	std::vector<VertexFormat> vertices;
	if (isFrontFace)
	{
		for (size_t idx = 0; idx < ref_model.n_tri_; idx++) {
			unsigned int id1 = ref_model.tri_list_(idx, 2);
			unsigned int id2 = ref_model.tri_list_(idx, 1);
			unsigned int id3 = ref_model.tri_list_(idx, 0);
			if (face_mask(id1) > 0.5 && face_mask(id2, 0) > 0.5 && face_mask(id3, 0) > 0.5) {
				size_t offset1 = 3 * id1;
				size_t offset2 = 3 * id2;
				size_t offset3 = 3 * id3;

				if (useMeshTex){
					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
						glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
						glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
						glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
						glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
						glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
						glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
						glm::vec2(idx + 1, idx + 1)));
				}
				else{
					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
						glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
						glm::vec3(1, 0, 0),
						glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
						glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
						glm::vec3(0, 1, 0),
						glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
						glm::vec2(idx + 1, idx + 1)));

					vertices.push_back(VertexFormat(
						glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
						glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
						glm::vec3(0, 0, 1),
						glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
						glm::vec2(idx + 1, idx + 1)));
				}
			}
		}
	}
	else {
		for (size_t idx = 0; idx < ref_model.n_tri_; idx++) {

			unsigned int id1 = ref_model.tri_list_(idx, 2);
			unsigned int id2 = ref_model.tri_list_(idx, 1);
			unsigned int id3 = ref_model.tri_list_(idx, 0);
			size_t offset1 = 3 * id1;
			size_t offset2 = 3 * id2;
			size_t offset3 = 3 * id3;

			if (useMeshTex){

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
					glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
					glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
					glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
					glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
					glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
					glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
					glm::vec2(idx + 1, idx + 1)));
			}
			else{
				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id1, 0), ref_model.position_(id1, 1), ref_model.position_(id1, 2)),
					glm::vec4(ref_model.color_(id1, 0), ref_model.color_(id1, 1), ref_model.color_(id1, 2), 1),
					glm::vec3(1, 0, 0),
					glm::vec3(ref_model.normal_(id1, 0), ref_model.normal_(id1, 1), ref_model.normal_(id1, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id2, 0), ref_model.position_(id2, 1), ref_model.position_(id2, 2)),
					glm::vec4(ref_model.color_(id2, 0), ref_model.color_(id2, 1), ref_model.color_(id2, 2), 1),
					glm::vec3(0, 1, 0),
					glm::vec3(ref_model.normal_(id2, 0), ref_model.normal_(id2, 1), ref_model.normal_(id2, 2)),
					glm::vec2(idx + 1, idx + 1)));

				vertices.push_back(VertexFormat(
					glm::vec3(ref_model.position_(id3, 0), ref_model.position_(id3, 1), ref_model.position_(id3, 2)),
					glm::vec4(ref_model.color_(id3, 0), ref_model.color_(id3, 1), ref_model.color_(id3, 2), 1),
					glm::vec3(0, 0, 1),
					glm::vec3(ref_model.normal_(id3, 0), ref_model.normal_(id3, 1), ref_model.normal_(id3, 2)),
					glm::vec2(idx + 1, idx + 1)));
			}

		}
	}
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLfloat* data;
	data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	memcpy(data, &vertices[0], vertices.size() * sizeof(VertexFormat));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void GenModels::CreateBGplaneModel(const std::string& gameModelName){

	unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	std::vector<VertexFormat> vertices;
	vertices.push_back(VertexFormat(glm::vec3(-1.0, -1.0, 0.99),
		glm::vec3(0.0, 0.0,0.0))); 
	vertices.push_back(VertexFormat(glm::vec3(1.0, -1.0, 0.99),
		glm::vec3(1.0, 0.0,0.0))); 
	vertices.push_back(VertexFormat(glm::vec3(1.0, 1.0, 0.99),
		glm::vec3(1.0, 1.0,0.0))); 
	vertices.push_back(VertexFormat(glm::vec3(-1.0, 1.0, 0.99),
		glm::vec3(0.0, 1.0,0.0))); 

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexFormat) * 4, &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)28);

	Model myModel;
	myModel.vao = vao;
	myModel.vbos.push_back(vbo);
	GameModelList[gameModelName] = myModel;

}

void GenModels::DeleteModel(const std::string& gameModelName)
{
	Model model = GameModelList[gameModelName];
	unsigned int p = model.vao;
	glDeleteVertexArrays(1, &p);
	glDeleteBuffers(model.vbos.size(), &model.vbos[0]);
	model.vbos.clear();
	GameModelList.erase(gameModelName);

}

unsigned int GenModels::GetModel(const std::string& gameModelName)
{
	return GameModelList[gameModelName].vao;
}

