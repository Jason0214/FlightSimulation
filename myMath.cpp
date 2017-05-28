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

static inline void _subtract(float* des,const float* src1,const float* src2, int len) {
	for (int i = 0; i < len; i++) {
		des[i] = src1[i] - src2[i];
	}
}

static inline void _add(float* des,const float* src1,const float* src2, int len) {
	for (int i = 0; i < len; i++) {
		des[i] = src1[i] + src2[i];
	}
}

static inline void _assign(float* des,const float* src, int len) {
	memcpy(des,src,sizeof(float)*len);
}

static inline void vec_multi(float* des,const float* src, float k, int len) {
	for (int i = 0; i < len; i++) {
		des[i] = src[i] * k;
	}
}

static inline float vec_dot(const float* src1,const float* src2, int len) {
	float ret = 0;
	for (int i = 0; i < len; i++) {
		ret += src1[i] * src2[i];
	}
	return ret;
}

static inline float vec_module(const float* src, int len){
	float ret = 0;
	for (int i = 0; i < len; i++) {
		ret += (src[i] * src[i]);
	}
	return sqrt(ret);
}

static inline void vec_normalize(float* des,const float* src,int len){
	float module = vec_module(src, len);
	for (int i = 0; i < len; i++) {
		des[i] = src[i] / module;
	}
}

vec2 vec2::operator* (float k) const {
	vec2 ret;
	vec_multi(ret.data(), this->data(), k, 2);
	return ret;
}
vec2 vec2::operator+ (const vec2 & v) const {
	vec2 ret;
	_add(ret.data(), this->data(), v.data(), 2);
	return ret;
}
vec2 vec2::operator- (const vec2 & v) const {
	vec2 ret;
	_subtract(ret.data(), this->data(), v.data(), 2);
	return ret;
}
float vec2::operator* (const vec2 & v) const {
	return vec_dot(this->data(), v.data(), 2);
}
vec2 & vec2::operator= (vec2 & v) {
	if (this != &v) {
		_assign(this->data(), v.data(), 2);
	}
	return *this;
}

vec3 vec3::operator* (float k) const {
	vec3 ret;
	vec_multi(ret.data(), this->data(), k, 3);
	return ret;
}
vec3 vec3::operator+ (const vec3 & v) const {
	vec3 ret;
	_add(ret.data(), this->data(), v.data(), 3);
	return ret;
}
vec3 vec3::operator- (const vec3 & v) const {
	vec3 ret;
	_subtract(ret.data(), this->data(), v.data(), 3);
	return ret;
}
float vec3::operator* (const vec3 & v) const {
	return vec_dot(this->data(), v.data(), 3);
}
vec3 & vec3::operator= (const vec3 & v) {
	if (this != &v) {
		_assign(this->data(), v.data(), 3);
	}
	return *this;
}

vec4 vec4::operator* (float k) const {
	vec4 ret;
	vec_multi(ret.data(), this->data(), k, 4);
	return ret;
}
vec4 vec4::operator+ (const vec4 & v) const {
	vec4 ret;
	_add(ret.data(), this->data(), v.data(), 4);
	return ret;
}
vec4 vec4::operator- (const vec4 & v) const {
	vec4 ret;
	_subtract(ret.data(), this->data(), v.data(), 4);
	return ret;
}
float vec4::operator* (const vec4 & v) const {
	return vec_dot(this->data(), v.data(), 4);
}
vec4 & vec4::operator= (const vec4 & v) {
	if (this != &v) {
		_assign(this->data(), v.data(), 4);
	}
	return *this;
}


vec2 normalize(const vec2 & v){
	vec2 ret;
	vec_normalize(ret.data(),v.data(),2);
	return ret;
}

vec3 normalize(const vec3 & v){
	vec3 ret;
	vec_normalize(ret.data(),v.data(),3);
	return ret;
}

vec4 normalize(const vec4 & v){
	vec4 ret;
	vec_normalize(ret.data(),v.data(),4);
	return ret;
}


vec3 cross(const vec3 & v1, const vec3 & v2){
	vec3 ret;
	ret[0] = v1[1] * v2[2] - v1[2] * v1[1];
	ret[1] = v1[2] * v2[0] - v1[0] * v2[2];
	ret[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return ret;
}

float module(const vec2 & v){
	return vec_module(v.data(),2);
}
float module(const vec3 & v){
	return vec_module(v.data(),3);
}
float module(const vec4 & v){
	return vec_module(v.data(),4);
}


mat3::mat3(const mat3 & m){
	_assign(this->data(), m.data(), 9);
}
mat3::mat3(const float* array){
	_assign(this->data(),array, 9);
}
mat3 & mat3::operator=(const mat3 & m){
	if(this != &m){
		_assign(this->data(),m.data(),9);
	}
	return *this;
}
mat3 & mat3::operator-(const mat3 & m){
	mat3 ret;
	_add(ret.data(),this->value,m.data(),9);
	return ret;
}
mat3 & mat3::operator+(const mat3 & m){
	mat3 ret;
	_subtract(ret.data(),this->value,m.data(),9);
	return ret;
}

