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
						   const vec3 & camera_position,
						   GLfloat aspect_ratio);
	void BufferReadConfig(const Shader & shader)const;
	void init(std::string vs_path, std::string fs_path);
	GLuint GetFrameBuffer(int index) {
		return this->FBO[index];
	}

	Shader shader;
	const GLuint map_width = 1024;
	const GLuint map_height = 1024;
	const float BASE_DIST = 10.0f;
	const int RATIO = 50;
#ifdef DEPTH_BUFFER_TEST
	void ShowTexture()const;
	Shader test_shader;
	void test_init();
	GLuint test_VAO;
	GLuint test_VBO;
#endif
private:
	void GenerateOrtho(const vec3 & light_dir, 
						const vec3 & camera_position, 
						GLfloat aspect_ratio);
	static const int CASCADE_NUM = 2;

	GLfloat light_space_view[16];
	GLfloat light_space_projection[CASCADE_NUM][16];

	GLuint FBO[CASCADE_NUM];
	GLuint depth_textureID[CASCADE_NUM];
	GLint texture_index[MAX_CASCADE] = { 4, 5, 6 };

	GLfloat z_clip[MAX_CASCADE + 1] = { 1.0f, 20.0f, 500.0f};
};