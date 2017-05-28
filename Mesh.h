#pragma once
#include <glew.h>
#include <string>
#include <vector>
#include "Shader.h"

typedef struct vertex_struct {
	GLfloat position_x;
	GLfloat position_y;
	GLfloat position_z;
	GLfloat normal_x;
	GLfloat normal_y;
	GLfloat normal_z;
	GLfloat texture_x;
	GLfloat texture_y;
}Vertex;

typedef struct texture_struct {
	GLuint ID;
	std::string filename;
}Texture;

typedef struct mesh_data_struct{
	Vertex* vertices;
	GLuint* indices;
	unsigned int v_num;
	unsigned int i_num;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
}MeshData;

class Mesh {
public:
	Mesh(unsigned int level_num=1):level_num(level_num){
		this->data = new MeshData[this->level_num];
	};
	~Mesh();
	void init(unsigned int level, unsigned int v_num, unsigned int i_num);
	void deploy(GLenum model_type,unsigned int level=0);
	void render(GLuint program,unsigned int level=0) const;
	void render_frame(GLuint program)const;
	Vertex* GetVertexPtr(unsigned int level) {
		return this->data[level].vertices;
	}
	GLuint* GetIndexPtr(unsigned int level) {
		return this->data[level].indices;
	}
	std::vector<Texture> specular_texture;
	std::vector<Texture> diffuse_texture;	
private:
	MeshData* data;
	unsigned int level_num;
};