#include "Shader.h"
#include "Exception.h"
#include <string>
#include <fstream>

using namespace std;

static char* ReadShaderCode(const char* shader_path);
static void CompileShader(GLuint shader_ID, const char* v_shader_code, GLuint program_ID);

Shader::Shader(){};

Shader::~Shader(){};

void Shader::LoadShader(const char* vertex_shader_path, const char* fragment_shader_path){
	this->ProgramID = glCreateProgram();
	GLuint v_shader_ID;
	char* v_shader_code = NULL;
	GLuint f_shader_ID;
	char* f_shader_code = NULL;
	v_shader_code = ReadShaderCode(vertex_shader_path);
	f_shader_code = ReadShaderCode(fragment_shader_path);
	// generate openGL ID for both shaders.
	v_shader_ID = glCreateShader(GL_VERTEX_SHADER);
	f_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
	// compile the shader codes.
	try {
		CompileShader(v_shader_ID, v_shader_code, this->ProgramID);
	}
	catch (ShaderCompileError & e) {
		e.error_info = string(vertex_shader_path) + ": " + e.error_info;
		throw e;
	}
	try {
		CompileShader(f_shader_ID, f_shader_code, this->ProgramID);
	}
	catch (ShaderCompileError & e) {
		e.error_info = string(fragment_shader_path) + ": " + e.error_info;
		throw e;
	}
	delete [] v_shader_code;
	delete [] f_shader_code;
}

void Shader::Use() const{
	glUseProgram(this->ProgramID);
}

static char* ReadShaderCode(const char* shader_path) {
	using namespace std;
	ifstream fin;
	fin.open(shader_path, ios::in | ios::binary);
	if (!fin) throw LoadFileError(shader_path);;
	fin.seekg(0,ios::end);
	unsigned int length = fin.tellg();
	fin.seekg(0,ios::beg);
	char* shader_code = new char[length+1]; 
	fin.read(shader_code, length);
	shader_code[length] = '\0';
	fin.close();
	return shader_code;
}

static void CompileShader(GLuint shader_ID, const char* shader_code, GLuint program_ID) {
	GLint success;
	char infoLog[512];
	glShaderSource(shader_ID, 1, &shader_code, NULL);
	glCompileShader(shader_ID);
	// check whether complie success
	glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader_ID, 512, NULL, infoLog);
		throw ShaderCompileError(infoLog);
	};
	glAttachShader(program_ID, shader_ID);
	glLinkProgram(program_ID);
	glGetProgramiv(program_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program_ID, 512, NULL, infoLog);
		throw ShaderCompileError(infoLog);
	}
	glDeleteShader(shader_ID);
}
