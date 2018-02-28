#pragma once

#include <vector>
#include <string>
#include <Eigen\Eigen>


struct ObjMesh 
{
	ObjMesh() 
	{
		request_position_ = true;
		request_color_ = false;
		request_normal_ = false;
		//request_tex_coord_ = true;
		request_tex_coord_ = true;
		request_face_normal_ = false;
		request_tri_list_ = true;
		save_color_ = false;
		save_normal_ = false;
		save_tex_coord_ = false;
	}
	ObjMesh(const std::string& filename) { this->load_obj(filename); }
	void set_vertex(int i, const Eigen::Vector3f& v) { this->position_.row(i) = v; }
	Eigen::Vector3f vertex(int i) const { return this->position_.row(i); }
	Eigen::Vector3i face(int i) const { return tri_list_.row(i); }
	Eigen::Vector3f vertex_normal(int i) const { return this->normal_.row(i); }
	Eigen::Vector3f face_normal(int i) const { return this->face_normal_.row(i); }
	void update_vertices(const Eigen::VectorXd &vertices);

	~ObjMesh() {}

	// io functions
	void load_obj(std::string filename);
	void write_obj(std::string filename);
	void print_summary();
	void update_normal();
	void normalize_model();
	void center_model();

	// vertices - position & color & normal & tex coordinate
	Eigen::MatrixXf position_;
	Eigen::MatrixXf color_;
	Eigen::MatrixXf normal_;
	Eigen::MatrixXf tex_coord_;
	// triangle - face normal & triangle list
	Eigen::MatrixXf face_normal_;
	Eigen::MatrixXi tri_list_;
	// number of vertices & triangles
	int n_verts_;
	int n_tri_;
	// io prefix
	bool request_position_;
	bool request_color_;
	bool request_normal_;
	bool request_tex_coord_;
	bool request_face_normal_;
	bool request_tri_list_;
	bool save_color_;
	bool save_normal_;
	bool save_tex_coord_;

};

typedef ObjMesh Mesh;

