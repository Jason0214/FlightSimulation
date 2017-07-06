#pragma once
#include "myMath.h"
#include "Model.h"
#include "LightSrc.h"
#include "DepthBuffer.h"
#include "BackGround.h"
#include "Plane.h"
#include <vector>

#define MAP_SIDE_LEN  2
#define MAP_SIDE_NUM 1000
#define SCENE_LEN (2000.0f)

class Instance{
public:
	Instance(const vec3 & position, StaticModel* model, const vec3 & pivot, float angle)
		:position(position), model(model){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(position[0], position[1], position[2]);
		if (angle != 0.0f)
			glRotatef(angle, pivot[0], pivot[1], pivot[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, this->model_mat);
		glPushMatrix();
	}
	Instance(const Instance & instance):position(instance.position) {
		this->model = instance.model;
		memcpy(this->model_mat, instance.model_mat, 16 * sizeof(GLfloat));
	}
	~Instance() {}
	GLfloat model_mat[16];
	vec3 position;
	StaticModel* model;
};

struct InstancePtrWithDist {
	Instance* instance_ptr;
	GLfloat distance;
};

class Scene{
public:
	Scene();
	~Scene() { 
		this->FreeAll(); 
	}
	void FreeAll();

	void AppendObject(vec3 & position, StaticModel* instance, vec3 & rotate = vec3(0.0f,0.0f,0.0f), float angle = 0.0f);
	void Arrange(const vec3 & camera_front,const vec3 & camera_position);

	void RenderAll(const LightSrc & sun)const;
	void GenerateShadowMap();
	void GenerateProjectionMatrix();
	void CheckCollision() const;
	bool OBBdetection(Wrapper & a, Wrapper & b)const;

	BackGround* background;
	PlaneModel* plane;
	DepthBuffer shadow_map;

	GLint window_width;
	GLint window_height;
private:
	std::vector<Instance*> object_list;
	std::vector<Instance*> object_grid_map[MAP_SIDE_NUM][MAP_SIDE_NUM];
	GLfloat projection_mat[CASCADE_NUM][16];

	InstancePtrWithDist** buf_for_sort;  //XXX: better to be self-built

	const GLfloat z_clip[CASCADE_NUM + 1] = { 1.0f, 40.0f, 200.0f, 10000.0f };
};