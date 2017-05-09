#pragma once
#include "myMath.h"


class Camera {
public:
	Camera(float x, float y, float z);
	~Camera() {}
	float yaw; //Æ«º½½Ç
	float pitch; //¸©Ñö½Ç
//	float roll;  //¹ö×ª½Ç
	vec3 position;
	vec3 front;
	vec3 up;
	void moveleft();
	void moveright();
	void moveback();
	void movefront();
	void reposition();
private:
	float move_speed;
};