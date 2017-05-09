#pragma once
#define GLEW_STATIC 
#include <glew.h>
#pragma comment (lib, "glew32s.lib")
#include <iostream>

class DepthMap{
public:
	DepthMap(GLuint map_width, GLuint map_height) :map_width(map_width), map_height(map_height) {  };
	~DepthMap();
	GLuint FBO;
	GLuint depth_textureID;
	GLuint map_width;
	GLuint map_height;
private:
	void init();
};