#pragma once
#include "myMath.h"
#include "Model.h"
#include "LightSrc.h"
#include "DepthMap.h"
#include "BackGround.h"
#include "Plane.h"
#include <map>

class PositionListNode{
public:
	PositionListNode(const vec3 & position, StaticModel* instance, const vec3 & pivot, float angle)
			:next(NULL),position(position),instance(instance),pivot(pivot),angle(angle){}
	~PositionListNode() {}
	vec3 position;
	vec3 pivot;
	float angle;
	StaticModel* instance;
	PositionListNode* next;
};

class InstanceListNode {
public:
	InstanceListNode(StaticModel* instance):instance(instance),next(NULL){}
	~InstanceListNode() {};
	StaticModel* instance;
	InstanceListNode* next;
};

class Scene{
public:
	Scene(int frustum_num);
	~Scene() { 
		this->FreeAll(); 
		delete [] this->dist_order_dict;
	}
	void FreeAll();

	void AppendObject(vec3 & position, StaticModel* instance, vec3 & rotate = vec3(0.0f,0.0f,0.0f), float angle = 0.0f);
	void ReProject(int level_index)const;

	void Arrange(const vec3 & camera_front,const vec3 & camera_position);
	void ResetArrange();

	void RenderAll(const LightSrc & sun, const DepthMap & depth_buffer)const;//XXX
	void RenderFrame(const Shader & frame_shader)const;

	void CheckCollision() const;

	BackGround* background;
	PlaneModel* plane;

	GLint window_width;
	GLint window_height;
private:
	int frustum_num;
	int object_num;
	PositionListNode* object_list;
	InstanceListNode* object_map[GRID_NUM][GRID_NUM];
	std::map<GLfloat,PositionListNode*>* dist_order_dict;  //XXX: better to be self-built
};