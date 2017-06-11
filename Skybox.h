#pragma once

#define GLEW_STATIC 
#include <glew.h>
#pragma comment (lib, "glew32s.lib")
#include <SOIL.h>

#ifdef _DEBUG
#pragma comment (lib, "SOILd.lib")
#else
#pragma comment (lib, "SOIL.lib")
#endif

#include "Shader.h"
#include <string>

class SkyBox {
public:
	SkyBox();
	~SkyBox();
	void LoadTexture(std::string pic_path[], unsigned int pic_num = 6);
	void DeployTexture();
	void Draw(GLfloat camera_x = 0.0f, GLfloat camera_y = 0.0f, GLfloat camera_z = 0.0f);
	GLuint VAO;
	GLuint VBO;
	GLuint TextureID;
	Shader shader;
};