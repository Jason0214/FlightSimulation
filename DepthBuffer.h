#pragma once
#define GLEW_STATIC 
#include <glew.h>
#pragma comment (lib, "glew32s.lib")
#include "myMath.h"
#include "Shader.h"
#include <string>

//#define DEPTH_BUFFER_TEST

#define MAX_CASCADE 3 // must keep consistent with cascade number in shader

class Scene;

class DepthBuffer{
public:
	DepthBuffer(){};
	~DepthBuffer();
	void BufferWriteConfig(const vec3 & light_dir,
						   GLfloat aspect_ratio);
	void BufferReadConfig(const Shader & shader)const;
	void init(std::string vs_path, std::string fs_path);
	const GLfloat* GetViewMatrix() {
		return this->light_space_view;
	}
	const GLfloat* GetProjectionMatrix(int index) {
		return this->light_space_projection[index];
	}
	GLuint GetFrameBuffer(int index) {
		return this->FBO[index];
	}
	GLfloat GetClip(int index) {
		return this->z_clip[index];
	}

	Shader shader;
	static const GLuint map_width = 1024;
	static const GLuint map_height = 1024;
	static const int CASCADE_NUM = 2;

#ifdef DEPTH_BUFFER_TEST
	void ShowTexture(int texture_index);
	Shader test_shader;
	void test_init();
	bool is_inited = false;
	GLuint test_VAO;
	GLuint test_VBO;
#endif
private:
	GLfloat light_space_view[16];
	GLfloat light_space_projection[CASCADE_NUM][16];

	GLuint FBO[CASCADE_NUM];
	GLuint depth_textureID[CASCADE_NUM];

	GLfloat z_clip[MAX_CASCADE + 1] = { 1.0f, 20.0f, 500.0f, 500.0f};
};