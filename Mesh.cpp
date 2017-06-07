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
	delete[] this->vertices;
	delete[] this->indices;
	glDeleteBuffers(1,&(this->VBO));
	glDeleteBuffers(1,&(this->EBO));
	glDeleteBuffers(1,&(this->VAO));		
}

void Mesh::init(unsigned int v_num, unsigned int i_num) {
	this->v_num = v_num;
	this->i_num = i_num;
	this->vertices = new Vertex[v_num];
	this->indices = new GLuint[i_num];
}

void Mesh::deploy(GLenum model_type) {
	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &(this->VBO));
	glGenBuffers(1, &(this->EBO));
	glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*this->v_num, this->vertices, model_type);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*this->i_num, this->indices, model_type);
	//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(0));

	//normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//texture
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void Mesh::render(GLuint program) const {
	glBindVertexArray(this->VAO);
	//XXX:  to simplify only use the first diffuse_texture and specular_texture
	if (this->diffuse_texture.size() > 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->diffuse_texture[0].ID);
		glUniform1i(glGetUniformLocation(program, "diffuse_texture"), 1);
	}
	if (this->specular_texture.size() > 0) {
		glUniform1f(glGetUniformLocation(program,"specular_strength"), 0.5);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, this->specular_texture[0].ID);
		glUniform1i(glGetUniformLocation(program, "specular_texture"), 2);
	}
	else {
		glUniform1f(glGetUniformLocation(program, "specular_strength"), 0);
	}
	glDrawElements(GL_TRIANGLES, this->i_num, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}

void Mesh::render_frame(GLuint program) const {
	glBindVertexArray(this->VAO);
	if (this->diffuse_texture.size() > 0) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->diffuse_texture[0].ID);
		glUniform1i(glGetUniformLocation(program, "diffuse_texture"), 1);
	}
	glDrawElements(GL_TRIANGLES, this->i_num, GL_UNSIGNED_INT, (GLvoid*)0);
	glBindVertexArray(0);
}