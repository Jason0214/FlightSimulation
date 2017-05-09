#pragma once

#define GLEW_STATIC
#include "glew.h"
#pragma comment (lib, "glew32s.lib")


class Shader {
public:
	Shader();
	~Shader();
	void LoadShader(const char* vertex_shader_path, const char* fragment_shader_path);
	void Use() const;
	GLuint ProgramID;
};