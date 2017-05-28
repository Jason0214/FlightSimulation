#pragma once
#include <string>

class Exception:public std::exception {
public:
	Exception() {};
	virtual ~Exception() {}
	virtual std::string Info() const { return "Something unexpected happened"; };
};

class LoadFileError :public Exception{
public:
	LoadFileError(char* file_name) : err_file(file_name) {}
	LoadFileError(std::string & file_name) : err_file(file_name) {}
	LoadFileError(const char* file_name = "foo") : err_file(file_name) {}
	~LoadFileError() {};
	std::string Info() const{ return "load from "+err_file+" failed.\n";}
	std::string err_file;
};

class LoadModelError :public Exception {
public:
	LoadModelError(std::string & info) : info(info) {}
	~LoadModelError() {};
	std::string Info() const { return info; }
	std::string info;
};

class ShaderCompileError:public Exception {
public:
	ShaderCompileError() {};
	~ShaderCompileError() {}
	ShaderCompileError(char* log = "..."):error_info(log){};
	std::string Info() const { return "shader compile error: "+error_info; }
	std::string error_info;
};