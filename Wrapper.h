#pragma once
#include "myMath.h"
#include <vector>

typedef struct face_struct {
	std::vector<int> v_index;
	vec3 normal;
}Face;

class Wrapper{
public:
	Wrapper() {}
	~Wrapper() {}
	static void InitWrapper(Wrapper &,const std::string);
	std::vector<Face> Faces;
	std::vector<vec3> Vertices;
};