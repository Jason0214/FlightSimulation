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


class Mesh {
public:
	Mesh(){};
	~Mesh();
	void init(unsigned int v_num, unsigned int i_num);
	void deploy(GLenum model_type);
	void render(GLuint program) const;
	void render_frame(GLuint program)const;
	Vertex* GetVertexPtr() {
		return this->vertices;
	}
	GLuint* GetIndexPtr() {
		return this->indices;
	}
	std::vector<Texture> specular_texture;
	std::vector<Texture> diffuse_texture;	

	Vertex* vertices;
	GLuint* indices;
	unsigned int v_num;
	unsigned int i_num;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};