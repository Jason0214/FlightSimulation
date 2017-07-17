#pragma once

#include <cmath>
#include <cstring>

float CosAngle(float );
float SinAngle(float );
float TanAngle(float );

float Power(float x, int exp);

// return float (-1,1)
float RandFloat();

class vec4 {
public:
	vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) {
		this->value[0] = x;
		this->value[1] = y;
		this->value[2] = z;
		this->value[3] = w;
	}
	vec4(const vec4 & v) {
		for (int i = 0; i < 4; i++) {
			this->value[i] = v[i];
		}
	}
	~vec4() {};
	float & x() { return this->value[0]; }
	float & y() { return this->value[1]; }
	float & z() { return this->value[2]; }
	float & w() { return this->value[3]; }
	float & r() { return this->value[0]; }
	float & g() { return this->value[1]; }
	float & b() { return this->value[2]; }
	float & a() { return this->value[3]; }
	float* data() { return &(this->value[0]); }
	const float* data() const { return &(this->value[0]); }
	float & operator[] (int index) { return this->value[index]; }
	float operator[] (int index) const { return this->value[index]; }
	vec4 operator* (float k) const;
	vec4 operator/ (float k) const;
	void operator/= (float k);
	vec4 operator+ (const vec4 & v) const;
	vec4 & operator+=(const vec4 & v);
	vec4 operator- (const vec4 & v) const;
	vec4 operator-() const{return vec4(0.0f,0.0f,0.0f,0.0f) - *this;};	
	float operator* (const vec4 & v) const;
	vec4 & operator= (const vec4 & v);
private:
	float value[4];
};

class vec3{
public:
	vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) { 
		this->value[0] = x; 
		this->value[1] = y; 
		this->value[2] = z;
	}
	vec3(vec4 & v4){
		this->value[0] = v4[0];
		this->value[1] = v4[1];
		this->value[2] = v4[2];
	}
	vec3(const float* array){
		this->value[0] = array[0];
		this->value[1] = array[1];
		this->value[2] = array[2];
	}
	vec3(const vec3 & v) {
		for (int i = 0; i < 3; i++) {
			this->value[i] = v[i];
		}
	}
	~vec3() {};
	float & x() { return this->value[0]; }
	float & y() { return this->value[1]; }
	float & z() { return this->value[2]; }
	float & r() { return this->value[0]; }
	float & g() { return this->value[1]; }
	float & b() { return this->value[2]; }
	float* data() { return &(this->value[0]); }
	const float* data() const{ return &(this->value[0]); }
	float & operator[] (int index) { return this->value[index]; }
	float operator[] (int index) const { return this->value[index]; }
	vec3 operator* (float k) const;
	vec3 operator/ (float k) const;
	void operator/= (float k);
	vec3 operator+ (const vec3 & v) const;
	vec3 & operator+=(const vec3 & v);
	vec3 operator- (const vec3 & v) const;
	vec3 operator-() const{return vec3(0.0f,0.0f,0.0f) - *this;}
	float operator* (const vec3 & v) const;
	vec3 & operator= (const vec3 & v);
private:
	float value[3];
};


class vec2{
public:
	vec2(float x = 0.0f, float y = 0.0f) { this->value[0] = x; this->value[1] = y; }
	vec2 (const vec2 & v) {
		for (int i = 0; i < 2; i++) {
			this->value[i] = v[i];
		}
	}
	vec2(const float* array){
		this->value[0] = array[0];
		this->value[1] = array[1];
	}
	vec2(const vec3 v){
		this->value[0] = v[0];
		this->value[1] = v[1];
	}
	~vec2() {};
	float & x() { return this->value[0]; }
	float & y() { return this->value[1]; }
	float* data() { return &(this->value[0]); }
	const float* data() const { return &(this->value[0]); }
	float & operator[] (int index) { return this->value[index]; }
	float operator[] (int index) const { return this->value[index]; }
	vec2 operator* (float k) const;
	vec2 operator/ (float k) const;
	void operator/= (float k);
	vec2 operator+ (const vec2 & v) const;
	vec2 & operator+=(const vec2 & v);
	vec2 operator- (const vec2 & v) const;
	vec2 operator-() const{return vec2(0.0f,0.0f) - *this;};
	float operator* (const vec2 & v) const;
	vec2 & operator= (vec2 & v);
private:
	float value[2];
};

vec2 dot(const vec2 & v1,const vec2 & v2);
vec3 dot(const vec3 & v1,const vec3 & v2);
vec4 dot(const vec4 & v1,const vec4 & v2);

vec3 cross(const vec3 & v1, const vec3 & v2);

vec2 normalize(const vec2 & v);
vec3 normalize(const vec3 & v);
vec4 normalize(const vec4 & v);

float module(const vec2 & v1);
float module(const vec3 & v1);
float module(const vec4 & v1);

class mat3{
/*
	0 3 6
	1 4 7
	2 5 8
*/
public:
	mat3(){
		memset(this->value,0,sizeof(float)*9);
	}
	mat3(const mat3 & m);
	mat3(const float* array);
	float* operator[](int index){
		return (float*)(&(this->value[index*3]));
	}
	mat3 & operator=(const mat3 & m);
	mat3 operator-(const mat3 & m);
	mat3 operator+(const mat3 & m);
	float* data() { return &(this->value[0]); }
	const float* data() const { return &(this->value[0]); }
private:
	float value[16];	
};

class mat4{
/*
	0 4 8.. 
	1 5 9..
	2 6 ...
	3 7 ...
*/
public:
	mat4(){
		memset(this->value,0,sizeof(float)*16);
	}
	mat4(const float* array){
		memcpy(this->value, array, sizeof(float) * 16);
	}
	float* operator[](int index){
		return (float*)(&(this->value[index*4]));
	}
	const float* operator[](int index) const{
		return (float*)(&(this->value[index * 4]));
	}
	vec4 operator*(const vec4 & v);
private:
	float value[16];
};

mat4 inverse(const mat4 & m);