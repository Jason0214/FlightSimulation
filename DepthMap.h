#pragma once
#define GLEW_STATIC 
#include <glew.h>
#pragma comment (lib, "glew32s.lib")
#include "myMath.h"
#include "Shader.h"

class DepthMap{
public:
	DepthMap(GLuint map_width, GLuint map_height) :map_width(map_width), map_height(map_height) { };
	~DepthMap();
	void begRenderDirLight(vec3 & v, GLfloat ortho_radius, vec3 & center);
	void endRender();
//	void RenderDotLight(vec3 & v);
	void init();
	void ShowTexture()const;
	GLuint FBO;
	Shader shader;
	GLuint depth_textureID;
	GLfloat light_space_view[16];
	GLfloat light_space_project[16];
	GLuint map_width;
	GLuint map_height;
//for test
	Shader test_shader;
	void test_init();
private:
	GLuint test_VAO;
	GLuint test_VBO;
};