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

void DepthBuffer::DirLightRender(vec3 & light_dir, vec3 & center, Scene & scene){
	this->current_center = center;
	//for (int i = 0; i < CASCADE_NUM; i++) {
	//	GLfloat ortho_radius = Power(this->RATIO, i) * this->BASE_DIST;
	//	// get the position of the camera in depth buffer rendering
	//	vec3 light_position = light_dir * ortho_radius + center;
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadIdentity();
	//	//glOrtho(-ortho_radius, ortho_radius,-module(cross(light_dir,normalize(vec3(light_dir.x(), 0.0f, light_dir.z()))))*ortho_radius, 
	//	//	ortho_radius, 0.0f, light_dir*normalize(vec3(light_dir.x(), 0.0f, light_dir.z()))*ortho_radius+ortho_radius);
	//	//cout << light_dir*normalize(vec3(light_dir.x(), 0.0f, light_dir.z()))*ortho_radius;
	//	glOrtho(-ortho_radius, ortho_radius, -ortho_radius, ortho_radius, 0.0f, 2 * ortho_radius);
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();
	//	gluLookAt(light_position.x(), light_position.y(), light_position.z(), center.x(), center.y(), center.z(), 0.0f, 1.0f, 0.0f);
	//	glGetFloatv(GL_PROJECTION_MATRIX, this->light_space_project[i]);
	//	glGetFloatv(GL_MODELVIEW_MATRIX, this->light_space_view[i]);
	//	// begin render
	//	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);
	//	glClear(GL_DEPTH_BUFFER_BIT);
	//	scene.RenderFrame(this->shader);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}
}

void DepthBuffer::DotLightRender() {
	//TODO
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
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) cout << "not done.";
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

#ifdef DEPTH_TEST
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