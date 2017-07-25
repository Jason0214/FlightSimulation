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
	void BufferWriteConfig(GLfloat aspect_ratio);
	void BufferReadConfig(const Shader & shader)const;
	void init(std::string vs_path,
			std::string fs_path, 
			const vec3 & light_dir);
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
	static const int CASCADE_NUM = 3;

#ifdef DEPTH_BUFFER_TEST
	void ShowTexture(int texture_index);
	Shader test_shader;
	void test_init();
	bool is_inited = false;
	GLuint test_VAO;
	GLuint test_VBO;
#endif
private:
	void ShaderInit(const std::string & vs_path, const std::string & fs_path) {
		this->shader.LoadShader(vs_path.c_str(), fs_path.c_str());
	}
	void LightDirInit(const vec3 & light_dir) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		gluLookAt(0.0f, 0.0f, 0.0f, -light_dir[0], -light_dir[1], -light_dir[2], 0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, this->light_space_view);
		glPopMatrix();
	}

	GLfloat light_space_view[16];
	GLfloat light_space_projection[CASCADE_NUM][16];

	GLuint FBO[CASCADE_NUM];
	GLuint depth_textureID[CASCADE_NUM];

	GLfloat z_clip[MAX_CASCADE + 1] = { 0.2f, 15.0f, 100.0f, 500.0f};
};