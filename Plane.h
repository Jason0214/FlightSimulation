#pragma once
#include "Model.h"


class PlaneModel: public Model {
public:
	PlaneModel(vec3 origin_position);
	~PlaneModel(){}
	void Render(const LightSrc & light, const DepthBuffer & depth_buffer, const GLfloat []) const;
	void RenderFrame(const Shader & frame_shader) const;

	bool is_land;
	bool is_crash;

	vec3 position;
	vec3 origin;
	vec3 front;
	vec3 up;
	GLfloat posture_mat[16];
	GLfloat fan_spin;

	//	counter_clock_wise
	GLfloat yaw;	//Æ«º½½Ç
	GLfloat pitch;  //¸©Ñö½Ç
	GLfloat roll;   //¹ö×ª½Ç
	GLfloat throttle;
	GLfloat speed;

	void init();

	void RollLeft();
	void RollRight();
	void RollBack();
	void YawLeft();
	void YawRight();
	void YawBack();
	void PitchUp();
	void PitchDown();
	void PitchBack();

	void SpeedUp();
	void SpeedDown();

	void Forward();
	void Translate()const;

	vec3 View() {
		vec3 temp = this->position - normalize(vec3(this->front.x(),0.0f,this->front.z()))*12.0f;
		return vec3(temp.x(), this->position.y()+5.0f, temp.z());
	}
private:
	void Rotate(GLfloat angle, int pivot);

	GLfloat FULL_ACC;
	GLfloat MAX_SPEED;
	GLfloat PITCH_SPEED;
	GLfloat ROLL_SPEED;
	GLfloat LAND_YAW_SPEED;
	GLfloat AIR_YAW_SPEED;

	static const short BODY = 0;
	static const short YAW_FIN = 1;
	static const short PITCH_FIN = 2;
	static const short RITGHT_UP_FLAP = 3;
	static const short LEFT_UP_FLAP = 4;
	static const short RITGHT_DOWN_FLAP = 5;
	static const short LEFT_DOWN_FLAP = 6;
	static const short FAN = 7;
	static const short GUN = 8;
	static const short RIGHT_WHEEL = 9;
	static const short LEFT_WHEEL = 10;
};