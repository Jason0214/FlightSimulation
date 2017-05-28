#pragma once
#include "myMath.h"

class LightSrc {
public:
	LightSrc(vec3 & direction, vec3 & color = vec3(1.0f, 1.0f, 1.0f)) :color(color){
		this->direction = normalize(direction);
	};
	~LightSrc() {};
	vec3 direction;
	vec3 color;
	void Draw() {};
};