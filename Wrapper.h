#pragma once
#include "myMath.h"
#include <vector>

// as the name suggested, wrapper is something 
// wrapped around a object, used to perform collision
// detection between models.
class Wrapper{
public:
	Wrapper() {}
	~Wrapper() {}
	static void InitWrapper(Wrapper &,const std::string);
	// get the model matrix and tranlate the wrapper 
	Wrapper Translate(mat4 );
	std::vector<vec3> FaceNormal;
	std::vector<vec3> Vertices;
};
