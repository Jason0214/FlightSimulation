#pragma once
#define GLEW_STATIC 
#include <glew.h>
#pragma comment (lib, "glew32s.lib")
#include "myMath.h"
#include "Shader.h"

//#define DEPTH_TEST

#define CASCADE_NUM 3

class DepthBuffer{
public:
	DepthBuffer(GLuint map_width, GLuint map_height) :map_width(map_width), map_height(map_height) { };
	~DepthBuffer();
	void DirLightRender(vec3 & light_dir, vec3 & center, void(*render)(void));
	void DotLightRender();
	void init();
	const GLfloat* GetLightViewMatrx(int index) const{
		return this->light_space_view[index];
	}
	const GLfloat* GetLightProjectMatrix(int index) const{
		return this->light_space_project[index];
	}
	GLuint GetFBO(int index) const{
		return this->FBO[index];
	}
	GLuint GetDepthTextureID(int index) const{
		return this->depth_textureID[index];
	}
	vec3 current_center;
	Shader shader;
	GLuint map_width;
	GLuint map_height;
	const float BASE_DIST = 10.0f;
	const int RATIO = 50;
#ifdef DEPTH_TEST
	void ShowTexture()const;
	Shader test_shader;
	void test_init();
	GLuint test_VAO;
	GLuint test_VBO;
#endif
private:
	GLuint FBO[CASCADE_NUM];
	GLuint depth_textureID[CASCADE_NUM];
	GLfloat light_space_view[CASCADE_NUM][16];
	GLfloat light_space_project[CASCADE_NUM][16];
};