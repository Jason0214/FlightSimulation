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

#define FRUSTUM_NUM 2

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
		glGetFloatv(GL_MODELVIEW_MATRIX, this->model_matrix);
		glPushMatrix();
	}
	Instance(const Instance & instance):position(instance.position) {
		this->model = instance.model;
		memcpy(this->model_matrix, instance.model_matrix, 16 * sizeof(GLfloat));
	}
	~Instance() {}
	GLfloat model_matrix[16];
	vec3 position;
	StaticModel* model;
	GLfloat distance;
};

class Scene{
public:
	Scene();
	~Scene() { 
		this->FreeAll(); 
	}
	void FreeAll();

	void AppendObject(vec3 & position, 
					StaticModel* instance, 
					vec3 & rotate = vec3(0.0f,0.0f,0.0f), 
					float angle = 0.0f);
	void RenderAll(const LightSrc & sun, 
					const vec3 & camera_pos, 
					const vec3 & camera_front);
	void GenerateProjectionMatrix(GLuint width, GLuint height);
	void CheckCollision() const;
	bool OBBdetection(Wrapper & a, Wrapper & b)const;

	BackGround* background;
	PlaneModel* plane;
	DepthBuffer shadow_map;

	GLint window_width;
	GLint window_height;

	Shader detailed_shader;
	Shader detailed_shader_support_alpha;
	Shader basic_shader;
	Shader basic_shader_support_alpha;
	Shader shadow_map_shader;
private:
	void Arrange(const vec3 & camera_front, const vec3 & camera_position);
	void GenerateShadowMap();

	std::vector<Instance*> object_list;
	std::vector<Instance*> object_grid_map[MAP_SIDE_NUM][MAP_SIDE_NUM];
	GLfloat projection_matrix[FRUSTUM_NUM][16];

	const GLfloat frustum_clip[FRUSTUM_NUM + 1] = { 1.0f, 500.0f, 2500.0f};
};