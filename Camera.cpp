#include "Camera.h"

Camera::Camera(float x, float y, float z):position(x, y, z),front(1.0f,0.0f,0.0f),up(0.0f,1.0f,0.0f){
	this->yaw = -90.0;
	this->pitch = 0.0;
	this->move_speed = 1.5f;
}

void Camera::moveleft() {
	vec3 v_left = normalize(cross(this->up, this->front));
	this->position = this->position + v_left * this->move_speed;
}

void Camera::moveright() {
	vec3 v_left = normalize(cross(this->up, this->front));
	this->position = this->position - v_left * this->move_speed;
}

void Camera::moveback() {
	this->position = this->position - this->front * this->move_speed;
}

void Camera::movefront() {
	this->position = this->position + this->front * this->move_speed;
}

void Camera::reposition() {
	if (this->pitch > 89.0f) this->pitch = 89.0f;
	if (this->pitch < -89.0f) this->pitch = -89.0f;
//	if (this->yaw > 180.0f) this->yaw -= 360.0f;
//	if (this->yaw < -180.0f) this->yaw += 360.0f;
	this->front.x() = aCos(this->yaw)*aCos(this->pitch);
	this->front.y() = aSin(this->pitch);
	this->front.z() = aSin(this->yaw)*aCos(this->pitch);
}