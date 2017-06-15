#pragma once
#include "myMath.h"
#include "Model.h"
#include "LightSrc.h"
#include "DepthMap.h"
#include "BackGround.h"
#include "Plane.h"

#define MAP_SIDE_LEN  2
#define MAP_SIDE_NUM 1000
#define SCENE_LEN (2000.0f)

class ListNode{
public:
	ListNode(const vec3 & position, StaticModel* instance, const vec3 & pivot, float angle)
		:next(NULL),position(position),instance(instance){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(position[0], position[1], position[2]);
		if (angle != 0.0f)
			glRotatef(angle, pivot[0], pivot[1], pivot[2]);
		glGetFloatv(GL_MODELVIEW_MATRIX, this->model_mat);
		glPushMatrix();
	}
	ListNode(const ListNode & node):next(NULL),position(node.position) {
		this->instance = node.instance;
		memcpy(this->model_mat, node.model_mat, 16 * sizeof(GLfloat));
	}
	~ListNode() {}
	GLfloat model_mat[16];
	vec3 position;
	StaticModel* instance;
	ListNode* next;
};

typedef struct render_obj_struct {
	GLfloat distance;
	ListNode* node;
}RenderObj;


class Scene{
public:
	Scene(int frustum_num);
	~Scene() { 
		this->FreeAll(); 
	}
	void FreeAll();

	void AppendObject(vec3 & position, StaticModel* instance, vec3 & rotate = vec3(0.0f,0.0f,0.0f), float angle = 0.0f);
	void ReProject(int level_index)const;

	void Arrange(const vec3 & camera_front,const vec3 & camera_position);
	void ResetArrange();

	void RenderAll(const LightSrc & sun, const DepthMap & depth_buffer)const;//XXX
	void RenderFrame(const Shader & frame_shader)const;

	void CheckCollision() const;
	bool OBBdetection(Wrapper & a, Wrapper & b)const;

	BackGround* background;
	PlaneModel* plane;

	GLint window_width;
	GLint window_height;
private:
	int frustum_num;
	int object_num;
	ListNode* object_list;
	ListNode* object_map[MAP_SIDE_NUM][MAP_SIDE_NUM];

	RenderObj*** sort_buf;  //XXX: better to be self-built
	int* sort_buf_count;
};