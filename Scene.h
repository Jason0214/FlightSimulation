#pragma once
#include "myMath.h"
#include "Model.h"
#include "LightSrc.h"
#include "DepthMap.h"
#include <map>

class ListNode{
public:
	ListNode(const vec3 & position, Model* type, const vec3 & pivot, float angle):next(NULL),position(position),type(type),pivot(pivot),angle(angle){}
	~ListNode() {}
	vec3 position;
	vec3 pivot;
	float angle;
	Model* type;
	ListNode* next;
};

class Scene{
public:
	Scene(int level_num);
	~Scene() { this->FreeAll(); delete [] this->leveled_set; }
	void AppendObject(vec3 & position, Model* type, vec3 & rotate = vec3(0.0f,0.0f,0.0f), float angle = 0.0f);
	void AppendBackGround(vec3 & position, Model* instance, vec3 & rotate = vec3(0.0f, 0.0f, 0.0f), float angle = 0.0f) {
		ListNode* ptr_new = new ListNode(position, instance, rotate, angle);
		this->background = ptr_new;
	}
	void Arrange(const vec3 & camera_front,const vec3 & camera_position);
	void ResetArrange();
	void ReProject(int level_index)const;
	void RenderAll(const LightSrc & sun, const DepthMap & depth_buffer)const;//XXX
	void RenderFrame(const Shader & frame_shader)const;
	void FreeAll();
	GLfloat radius;
	GLint window_width;
	GLint window_height;
private:
	int level_num;
	int object_num;
	ListNode* object_list;
	std::map<float,ListNode*>* leveled_set;  //XXX: better to be self-built
	ListNode* background;
};