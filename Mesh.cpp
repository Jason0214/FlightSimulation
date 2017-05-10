#include "Mesh.h"
#include <string>
using namespace std;

Mesh::~Mesh() {
	for(unsigned int i = 0; i < this->specular_texture.size(); i++){
		glDeleteTextures(1, &(this->specular_texture[i].ID));		
	}
	for(unsigned int i = 0; i < this->diffuse_texture.size(); i++){
		glDeleteTextures(1, &(this->diffuse_texture[i].ID));		
	}
	for(unsigned int i = 0; i < level_num; i++){
		delete[] this->data[i].vertices;
		delete[] this->data[i].indices;
		glDeleteBuffers(1,&(this->data[i].VBO));
		glDeleteBuffers(1,&(this->data[i].EBO));
		glDeleteBuffers(1,&(this->data[i].VAO));		
	}
	delete[] this->data;
	this->level_num = 0;
}

void Mesh::init(unsigned int level, unsigned int v_num, unsigned int i_num) {
	MeshData & current_mesh_data = this->data[level];
	current_mesh_data.v_num = v_num;
	current_mesh_data.i_num = i_num;
	current_mesh_data.vertices = new Vertex[v_num];
	current_mesh_data.indices = new GLuint[i_num];
}

void Mesh::deploy(GLenum model_type, unsigned int level) {
	MeshData & current_mesh_data = this->data[level];
	glGenVertexArrays(1, &(current_mesh_data.VAO));
	glGenBuffers(1, &(current_mesh_data.VBO));
	glGenBuffers(1, &(current_mesh_data.EBO));
	glBindVertexArray(current_mesh_data.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, current_mesh_data.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*current_mesh_data.v_num, current_mesh_data.vertices, model_type);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_mesh_data.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*current_mesh_data.i_num, current_mesh_data.indices, model_type);
		//positipn
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));
		glEnableVertexAttribArray(0);
	//normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
	//texture
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void Mesh::draw(GLuint program, unsigned int level) const {
	MeshData & current_mesh_data = this->data[level];
	glBindVertexArray(current_mesh_data.VAO);
	//XXX:  to simplify only use the first diffuse_texture and specular_texture
	if (this->diffuse_texture.size() > 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->diffuse_texture[0].ID);
		glUniform1i(glGetUniformLocation(program, "diffuse_texture"), 0);
	}
	if (this->specular_texture.size() > 0) {
		glUniform1f(glGetUniformLocation(program,"specular_strength"), 0.5);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->specular_texture[0].ID);
		glUniform1i(glGetUniformLocation(program, "specular_texture"), 1);
	}
	else {
		glUniform1f(glGetUniformLocation(program, "specular_strength"), 0);
	}
	glDrawElements(GL_TRIANGLES, current_mesh_data.i_num, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}