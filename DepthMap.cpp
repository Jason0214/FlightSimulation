#include "DepthMap.h"
#include "Exception.h"
#include <iostream>

using namespace std;

DepthMap::~DepthMap(){
	glDeleteTextures(1, &(this->depth_textureID));
	glDeleteFramebuffers(1, &(this->FBO));
}

void DepthMap::begRenderDirLight(vec3 & light_dir,GLfloat ortho_radius){
	// height_offset stands for the highest object in the scene
	vec3 light_position = light_dir*ortho_radius;
	glViewport(0, 0, this->map_width, this->map_height);
	// change matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ortho_radius, ortho_radius,-module(cross(light_dir,normalize(vec3(light_dir.x(), 0.0f, light_dir.z()))))*ortho_radius, 
		ortho_radius, 0.0f, light_dir*normalize(vec3(light_dir.x(), 0.0f, light_dir.z()))*ortho_radius+ortho_radius);
//	cout << light_dir*normalize(vec3(light_dir.x(), 0.0f, light_dir.z()))*ortho_radius;
	//glOrtho(-ortho_radius, ortho_radius, -ortho_radius, ortho_radius, 1.0f, 2 * ortho_radius);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(light_position.x(),light_position.y(),light_position.z(),0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	glGetFloatv(GL_PROJECTION_MATRIX, this->light_space_project);
	glGetFloatv(GL_MODELVIEW_MATRIX, this->light_space_view);
	// begin render
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
}

void DepthMap::endRender() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DepthMap::init(){
// gen texture ID
    glGenTextures(1, &(this->depth_textureID));
    glBindTexture(GL_TEXTURE_2D, this->depth_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->map_width, this->map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// gen FBO
	glGenFramebuffers(1, &(this->FBO));
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_textureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) cout << "not done.";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
// gen test VAO
	//this->test_init();
}

//void DepthMap::test_init() {
//	GLfloat vertices[] = {
//		-1.0f,  1.0f,  0.0f, 1.0f,
//		-1.0f, -1.0f,  0.0f, 0.0f,
//		1.0f, -1.0f,  1.0f, 0.0f,
//		-1.0f,  1.0f,  0.0f, 1.0f,
//		1.0f, -1.0f,  1.0f, 0.0f,
//		1.0f,  1.0f,  1.0f, 1.0f
//	};
//	glGenVertexArrays(1, &(this->test_VAO));
//	glGenBuffers(1, &(this->test_VBO));
//	glBindVertexArray(this->test_VAO);
//		glBindBuffer(GL_ARRAY_BUFFER,this->test_VBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*4,vertices,GL_STATIC_DRAW);
//		//position
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(0));
//		//texture
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
//	glBindVertexArray(0);
//}
//
//
//void DepthMap::ShowTexture()const {
//	glDisable(GL_DEPTH_TEST);
//	this->test_shader.Use();
//	glBindVertexArray(this->test_VAO);
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, this->depth_textureID);
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//	glBindVertexArray(0);
//	glEnable(GL_DEPTH_TEST);
//}