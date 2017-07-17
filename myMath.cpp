#include "myMath.h"
const static float RAD_PER_DEGREE = 3.1415926f/180;
float CosAngle(float theta) {
	return cos((theta)*RAD_PER_DEGREE);
}
float SinAngle(float theta) {
	return sin((theta)*RAD_PER_DEGREE);
}
float TanAngle(float theta) {
	return tan((theta)*RAD_PER_DEGREE);
}

float Power(float x, int exp){
	float res = 1.0f;
	for (int i = 0; i < exp; i++) {
		res *= x;
	}
	return res;
}

float RandFloat() {
	// reference: https://www.zhihu.com/question/25037345
	union {
		float f;
		unsigned int u;
	}ret;
	int temp = rand();
	ret.u = (temp%2) << 31;
	//	1 8 23
	ret.u += (126 << 23);
	ret.u += ((temp % (1 << 24)) >> 1);
	return ret.f;
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

static inline void vec_div(float* des, const float* src, float k, int len) {
	for (int i = 0; i < len; i++) {
		des[i] = src[i] / k;
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

vec2 & vec2::operator+=(const vec2 & v) {
	_add(this->data(), this->data(), v.data(), 2);
	return *this;
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

vec3 & vec3::operator+=(const vec3 & v){
	_add(this->data(), this->data(), v.data(), 3);
	return *this;
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
vec4 & vec4::operator+=(const vec4 & v) {
	_add(this->data(), this->data(), v.data(), 4);
	return *this;
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
vec4 vec4::operator/ (float k) const{
	vec4 ret;
	vec_div(ret.data(), this->data(), k, 4);
	return ret;
}

vec3 vec3::operator/ (float k) const {
	vec3 ret;
	vec_div(ret.data(), this->data(), k, 3);
	return ret;
}

vec2 vec2::operator/ (float k) const {
	vec2 ret;
	vec_div(ret.data(), this->data(), k, 2);
	return ret;
}

void vec4::operator/= (float k){
	vec_div(this->data(), this->data(), k, 4);
}

void vec3::operator/= (float k) {
	vec_div(this->data(), this->data(), k, 3);
}

void vec2::operator/= (float k) {
	vec_div(this->data(), this->data(), k, 2);
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
mat3 mat3::operator-(const mat3 & m){
	mat3 ret;
	_add(ret.data(),this->value,m.data(),9);
	return ret;
}
mat3 mat3::operator+(const mat3 & m){
	mat3 ret;
	_subtract(ret.data(),this->value,m.data(),9);
	return ret;
}

vec4 mat4::operator*(const vec4 & v) {
	vec4 ret;
	for (int i = 0; i < 4; i++) {
		ret[i] = 0;
		for (int j = 0; j < 4; j++) {
			ret[i] += this->value[j * 4 + i] * v[j];
		}
	}
	return ret;
}

mat4 inverse(const mat4 & m) {
	// gauss method
	// reference: http://blog.csdn.net/zhurui_idea/article/details/24864155
	mat4 t = m;
	mat4 ret;
	int k;
	float temp;
	for (int i = 0; i < 4; i++)
	{ 
		float max = t[i][i];
		k = i;
		for (int j = i + 1; j < 4; j++)
		{
			if (fabs(t[i][j]) > fabs(max))
			{
				max = t[i][j];
				k = j;
			}
		}
		//如果主元所在行不是第i行，进行行交换  
		if (k != i)
		{
			for (int j = 0; j < 4; j++)
			{
				temp = t[j][i];
				t[j][i] = t[j][i];
				t[j][k] = temp;

				temp = ret[j][i];
				ret[j][i] = ret[j][k];
				ret[j][k] = temp;
			}
		}
		//判断主元是否为0, 若是, 则矩阵A不是满秩矩阵,不存在逆矩阵  
		if (t[i][i] == 0)
		{
			throw NoInverseMatrix();
		}
		//消去A的第i列除去i行以外的各行元素  
		temp = t[i][i];
		for (int j = 0; j < 4; j++)
		{
			t[j][i] = t[j][i] / temp;        //主对角线上的元素变为1  
			ret[j][i] = ret[j][i] / temp;        //伴随计算  
		}
		for (int j = 0; j < 4; j++)        //第0行->第n行  
		{
			if (j != i)                //不是第i行  
			{
				temp = t[j][i];
				for (k = 0; k < 4; k++)        //第j行元素 - i行元素*j列i行元素  
				{
					t[k][j] = t[k][j] - t[k][i] * temp;
					ret[k][j] = ret[k][j] - ret[k][i] * temp;
				}
			}
		}
	}
}