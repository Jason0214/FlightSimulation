#include "myMath.h"
const static float RAD_PER_DEGREE = 3.1415926f/180;
float aCos(float theta) {
	return cos((theta)*RAD_PER_DEGREE);
}
float aSin(float theta) {
	return sin((theta)*RAD_PER_DEGREE);
}
float aTan(float theta) {
	return tan((theta)*RAD_PER_DEGREE);
}

static inline void vec_subtract(float* v_data,const float* v1_data,const float* v2_data, int len) {
	for (int i = 0; i < len; i++) {
		v_data[i] = v1_data[i] - v2_data[i];
	}
}

static inline void vec_add(float* v_data,const float* v1_data,const float* v2_data, int len) {
	for (int i = 0; i < len; i++) {
		v_data[i] = v1_data[i] + v2_data[i];
	}
}

static inline void vec_multi(float* v_data,const float* v1_data, float k, int len) {
	for (int i = 0; i < len; i++) {
		v_data[i] = v1_data[i] * k;
	}
}

static inline void vec_assign(float* v_data,const float* v1_data, int len) {
	for (int i = 0; i < len; i++) {
		v_data[i] = v1_data[i];
	}
}

static inline float vec_dot(const float* v1_data,const float* v2_data, int len) {
	float ret = 0;
	for (int i = 0; i < len; i++) {
		ret += v1_data[i] * v2_data[i];
	}
	return ret;
}

static inline void vec_normalize(float* v_data,int len){
	float module = 0;
	for (int i = 0; i < len; i++) {
		module += (v_data[i] * v_data[i]);
	}
	for (int i = 0; i < len; i++) {
		v_data[i] /= module;
	}
}

vec2 vec2::operator* (float k) const {
	vec2 ret;
	vec_multi(ret.data(), this->data(), k, 2);
	return ret;
}
vec2 vec2::operator+ (vec2 v) const {
	vec2 ret;
	vec_add(ret.data(), this->data(), v.data(), 2);
	return ret;
}
vec2 vec2::operator- (vec2 v) const {
	vec2 ret;
	vec_subtract(ret.data(), this->data(), v.data(), 2);
	return ret;
}
float vec2::operator* (vec2 v) const {
	return vec_dot(this->data(), v.data(), 2);
}
vec2 & vec2::operator= (vec2 & v) {
	if (this != &v) {
		vec_assign(this->data(), v.data(), 2);
	}
	return *this;
}

vec3 vec3::operator* (float k) const {
	vec3 ret;
	vec_multi(ret.data(), this->data(), k, 3);
	return ret;
}
vec3 vec3::operator+ (vec3 v) const {
	vec3 ret;
	vec_add(ret.data(), this->data(), v.data(), 3);
	return ret;
}
vec3 vec3::operator- (vec3 v) const {
	vec3 ret;
	vec_subtract(ret.data(), this->data(), v.data(), 3);
	return ret;
}
float vec3::operator* (vec3 v) const {
	return vec_dot(this->data(), v.data(), 3);
}
vec3 & vec3::operator= (vec3 & v) {
	if (this != &v) {
		vec_assign(this->data(), v.data(), 3);
	}
	return *this;
}

vec4 vec4::operator* (float k) const {
	vec4 ret;
	vec_multi(ret.data(), this->data(), k, 4);
	return ret;
}
vec4 vec4::operator+ (vec4 v) const {
	vec4 ret;
	vec_add(ret.data(), this->data(), v.data(), 4);
	return ret;
}
vec4 vec4::operator- (vec4 v) const {
	vec4 ret;
	vec_subtract(ret.data(), this->data(), v.data(), 4);
	return ret;
}
float vec4::operator* (vec4 v) const {
	return vec_dot(this->data(), v.data(), 4);
}
vec4 & vec4::operator= (vec4 & v) {
	if (this != &v) {
		vec_assign(this->data(), v.data(), 4);
	}
	return *this;
}


void normalize(vec2 v){
	vec_normalize(v.data(),2);
}

void normalize(vec3 v){
	vec_normalize(v.data(),3);
}

void normalize(vec4 v){
	vec_normalize(v.data(),4);
}


vec3 cross(const vec3 v1, const vec3 v2){
	vec3 ret;
	ret[0] = v1[1] * v2[2] - v1[2] * v1[1];
	ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
	ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return ret;
}