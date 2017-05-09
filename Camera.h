#pragma once
#include "myMath.h"


class Camera {
public:
	Camera(float x, float y, float z);
	~Camera() {}
	float yaw; //ƫ����
	float pitch; //������
//	float roll;  //��ת��
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