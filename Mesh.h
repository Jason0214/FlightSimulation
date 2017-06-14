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

	// generate openGL objects with raw data
	void deploy(GLenum model_type);
	
	// render the mesh
	// pay attention that projection&model_view matrix
	// won't be set in this function, it is the job of
	// class::Model
	void render(GLuint program) const;

	// only render the frame of mesh with no textures,
	// used for generate shadow map
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