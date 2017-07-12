#include "DepthBuffer.h"
#include "Exception.h"
#include "Scene.h"
#include <iostream>

using namespace std;

DepthBuffer::~DepthBuffer(){
	for (int i = 0; i < CASCADE_NUM; i++)
		glDeleteTextures(1, &(this->depth_textureID[i]));
	for (int i = 0; i < CASCADE_NUM; i++)
		glDeleteFramebuffers(1, &(this->FBO[i]));
}

void DepthBuffer::BufferWriteConfig(const vec3 & light_dir, const vec3 & camera_position, GLfloat view_matrx[]) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 0.0f, light_dir[0], light_dir[1], light_dir[2],  0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, this->light_space_view);
	glPopMatrix();
	this->FrustumToOtho(view_matrx);
}

void DepthBuffer::BufferReadConfig(const Shader & shader) const{
	glUniform1fv(glGetUniformLocation(shader.ProgramID, "shadow_clip"), CASCADE_NUM+1, this->z_clip);
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, "light_space_view"), 1, GL_FALSE, this->light_space_view);
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, "light_space_projection"), CASCADE_NUM, GL_FALSE, (const GLfloat*)this->light_space_project);
	GLint texture_index[CASCADE_NUM] = { 4 };
	glActiveTexture(GL_TEXTURE4);
	glActiveTexture(GL_TEXTURE5);
	glActiveTexture(GL_TEXTURE6);
	glUniform1iv(glGetUniformLocation(shader.ProgramID, "shadow_map"), CASCADE_NUM, texture_index);
}

void DepthBuffer::FrustumToOtho(GLfloat view_matrx[]) {
	glViewport(0, 0, this->map_width, this->map_height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	for (int i = 0; i < CASCADE_NUM; i++) {
		glLoadIdentity();
	}
	glPopMatrix();
}

void DepthBuffer::init(std::string vs_path, std::string fs_path){
	this->shader.LoadShader(vs_path.c_str(), fs_path.c_str());
	for (int i = 0; i < CASCADE_NUM; i++) {
		// gen texture ID
		glGenTextures(1, &(this->depth_textureID[i]));
		glBindTexture(GL_TEXTURE_2D, this->depth_textureID[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->map_width, this->map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// gen FBO
		glGenFramebuffers(1, &(this->FBO[i]));
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_textureID[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) cout << "not done.";
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

#ifdef DEPTH_BUFFER_TEST
void DepthBuffer::test_init() {
	GLfloat vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &(this->test_VAO));
	glGenBuffers(1, &(this->test_VBO));
	glBindVertexArray(this->test_VAO);
		glBindBuffer(GL_ARRAY_BUFFER,this->test_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*4,vertices,GL_STATIC_DRAW);
		//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(0));
		//texture
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
}


void DepthBuffer::ShowTexture()const {
	glDisable(GL_DEPTH_TEST);
	this->test_shader.Use();
	glBindVertexArray(this->test_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->depth_textureID[0]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
#endif