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
	void begRenderDirLight(vec3 & v, GLfloat ortho_radius);
	void endRender();
//	void RenderDotLight(vec3 & v);
	void init();
	void ShowTexture()const;
	GLuint FBO;
	Shader shader;
	GLuint depth_textureID;
	//Shader test_shader;
	GLfloat light_space_view[16];
	GLfloat light_space_project[16];
private:
	GLuint map_width;
	GLuint map_height;
	//void test_init();
	//GLuint test_VAO;
	//GLuint test_VBO;
};