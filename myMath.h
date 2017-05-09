#pragma once

#include <cmath>

float aCos(float );
float aSin(float );
float aTan(float );


class vec2{
public:
	vec2(float x = 0.0f, float y = 0.0f) { this->value[0] = x; this->value[1] = y; }
	vec2 (const vec2 & v) {
		for (int i = 0; i < 2; i++) {
			this->value[i] = v[i];
		}
	}
	~vec2() {};
	float & x() { return this->value[0]; }
	float & y() { return this->value[1]; }
	float* data() { return &(this->value[0]); }
	const float* data() const { return &(this->value[0]); }
	float & operator[] (int index) { return this->value[index]; }
	float operator[] (int index) const { return this->value[index]; }
	vec2 operator* (float k) const;
	vec2 operator+ (vec2 v) const;
	vec2 operator- (vec2 v) const;
	float operator* (vec2 v) const;
	vec2 & operator= (vec2 & v);
private:
	float value[2];
};

class vec3{
public:
	vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) { 
		this->value[0] = x; 
		this->value[1] = y; 
		this->value[2] = z;
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
	vec3 operator+ (vec3 v) const;
	vec3 operator- (vec3 v) const;
	float operator* (vec3 v) const;
	vec3 & operator= (vec3 & v);
private:
	float value[3];
};

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
	vec4 operator+ (vec4 v) const;
	vec4 operator- (vec4 v) const;
	float operator* (vec4 v) const;
	vec4 & operator= (vec4 & v);
private:
	float value[4];
};


//float dot(vec2 v1, vec2 v2){return v1*v2;}
//float dot(vec3 v1, vec3 v2){return v1*v2;}
//float dot(vec4 v1, vec4 v2){return v1*v2;}

vec3 cross(const vec3 v1, const vec3 v2);

void normalize(vec2 v);
void normalize(vec3 v);
void normalize(vec4 v);