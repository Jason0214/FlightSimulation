#pragma once
#include "myMath.h"
#include <vector>


class Wrapper{
public:
	Wrapper() {}
	~Wrapper() {}
	static void InitWrapper(Wrapper &,const std::string);
	Wrapper Translate(mat4 );
	std::vector<vec3> FaceNormal;
	std::vector<vec3> Vertices;
};
