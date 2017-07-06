#include "Scene.h"
#include "Exception.h"
#include <cstring>
#include <cstdlib>

#define FRUSTUM_NEAR (0.5f)
#define FRUSTUM_RATIO (500)

#include <iostream>
using namespace std;

static int comp(const void* a, const void* b) {
	return (int)((*(InstancePtrWithDist**)a)->distance - (*(InstancePtrWithDist**)b)->distance);
}

Scene::Scene():shadow_map(1024, 1024){
	this->background = NULL;
	this->plane = NULL;
	this->buf_for_sort = NULL;
	memset(this->object_grid_map, NULL, MAP_SIDE_NUM *MAP_SIDE_NUM * sizeof(Instance*));
}

// add a static model to the scene, specify the position and rotation info
void Scene::AppendObject(vec3 & position, StaticModel* instance, vec3 & rotate, float angle){
	Instance* new_instance = new Instance(position, instance, rotate, angle);
	this->object_list.push_back(new_instance);
	// add to grip by x,z coordinates
	unsigned int i = (unsigned int)(position.x() / MAP_SIDE_LEN), j = (unsigned int)(position.z() /MAP_SIDE_LEN);
	this->object_grid_map[i][j].push_back(new_instance);
}

void Scene::Arrange(const vec3 & camera_front,const vec3 & camera_position){
	if (!this->buf_for_sort) {
		//init buf when using it at the first time
		this->buf_for_sort = new InstancePtrWithDist*[this->object_list.size()];
		for (unsigned int i = 0; i < this->object_list.size(); i++) {
			this->buf_for_sort[i] = new InstancePtrWithDist;
		}
	}
	for (unsigned int i = 0; i < this->object_list.size(); i++) {
		float distance = normalize(camera_front)*(this->object_list[i]->position - camera_position);
		this->buf_for_sort[i]->distance = distance;
		this->buf_for_sort[i]->instance_ptr = this->object_list[i];
	}

	qsort(this->buf_for_sort, this->object_list.size(), sizeof(InstancePtrWithDist*), comp);
}

void Scene::GenerateProjectionMatrix() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	for (int i = 0; i < CASCADE_NUM; i++) {
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)this->window_width / (GLfloat)this->window_height, this->z_clip[i], this->z_clip[i+1]);
		glGetFloatv(GL_PROJECTION_MATRIX, this->projection_mat[i]);
	}
	glPopMatrix();
}

void Scene::RenderAll(const LightSrc & sun)const{
// draw objects from far to near in order to fit with alpha value
	for (unsigned int j = 0; j < this->object_list.size(); j++) {
		if (this->buf_for_sort[j]->distance < 40.0f) {
			this->buf_for_sort[j]->instance_ptr->model->Render(0, this->buf_for_sort[j]->instance_ptr->model_mat,
				sun, this->shadow_map, this->z_clip, this->projection_mat);
		}
		else {
			this->buf_for_sort[j]->instance_ptr->model->Render(1, this->buf_for_sort[j]->instance_ptr->model_mat,
				sun, this->shadow_map, this->z_clip, this->projection_mat);
		}
	}
	this->background->Render(sun, this->shadow_map, this->z_clip, this->projection_mat);
	this->plane->Render(sun, this->shadow_map, this->projection_mat[0]);
}

void Scene::GenerateShadowMap(){
	
}

void Scene::CheckCollision() const{
	GLfloat buf[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	this->plane->Translate();
	glGetFloatv(GL_MODELVIEW_MATRIX, buf);
	glPopMatrix();
	mat4 translate_mat(buf);
	Wrapper* plane_wrappers = new Wrapper[this->plane->wrapper_num];
	for (unsigned int i = 0; i < this->plane->wrapper_num; i++) {
		plane_wrappers[i] = this->plane->wrappers[i].Translate(translate_mat);
	}
	if (!plane->is_land) {
	// check plane above the terrain
		int yaw_abs = abs((int)this->plane->yaw);
		int roll_abs = abs((int)this->plane->roll);
		int pitch_abs = abs((int)this->plane->pitch);
		
		for (unsigned int k = 0; k < this->plane->wrapper_num; k++) {
			for (vector<vec3>::iterator ptr = plane_wrappers[k].Vertices.begin(); ptr != plane_wrappers[k].Vertices.end(); ptr++) {
				if (ptr->y() <= this->background->GetHeight(ptr->x(), ptr->z())) {
					if (this->background->IsInAirport(ptr->x(),ptr->z()) && yaw_abs < 10 && roll_abs < 10 && pitch_abs < 10){
						this->plane->is_land = true;
						this->plane->YawBack();
						this->plane->RollBack();
						this->plane->PitchBack();
						this->plane->position.y() = this->background->GetHeight(this->plane->position.x(), this->plane->position.z());
						return;
					}
					else {
						throw Collision();
					}
				}
			}
		}
	}
	int x = (int)(this->plane->position.x()/MAP_SIDE_LEN), z = (int)(this->plane->position.z()/MAP_SIDE_LEN);
	for (int i = x - 2; i < x + 2; i++) {
		for (int j = z - 2; j < z + 2; j++) {
			const vector<Instance*> & model_list = this->object_grid_map[i][j];
			for (unsigned int k = 0; k < model_list.size(); k++) {
				int k_num = model_list[k]->model->wrapper_num;
				int v_num = this->plane->wrapper_num;
				for (int k = 0; k < k_num; k++) {
					for (int v = 0; v < v_num; v++) {
						if (OBBdetection(plane_wrappers[v], model_list[k]->model->wrappers[k].Translate(mat4(model_list[k]->model_mat)))) {
							throw Collision();
						}
					}
				}
			}
		}
	}
	if (this->plane->position.x() <= 100.0f || this->plane->position.x() >=1900.0f ||
		this->plane->position.z() <= 100.0f || this->plane->position.z() >=1900.0f) throw WarningBoard();
	if (this->plane->position.x() <= 20.0f || this->plane->position.x() >= 1980.0f ||
		this->plane->position.z() <= 20.0f || this->plane->position.z() >= 1980.0f) throw ReachBoard();
}

bool Scene::OBBdetection(Wrapper & a, Wrapper & b) const{
	vector<vec3>::const_iterator normal_ptr;
	for (normal_ptr = b.FaceNormal.begin(); normal_ptr != b.FaceNormal.end(); normal_ptr++) {
		float a_min = INFINITY, a_max = -INFINITY, project;
		float b_min = INFINITY, b_max = -INFINITY;
		vector<vec3>::const_iterator vertex_ptr;
		for (vertex_ptr = a.Vertices.begin(); vertex_ptr != a.Vertices.end(); vertex_ptr++) {
			project = (*normal_ptr) * (*vertex_ptr);
			if (project > a_max) a_max = project;
			if (project < a_min) a_min = project;
		}
		for (vertex_ptr = b.Vertices.begin(); vertex_ptr != b.Vertices.end(); vertex_ptr++) {
			project = (*normal_ptr) * (*vertex_ptr);
			if (project > b_max) b_max = project;
			if (project < b_min) b_min = project;
		}
		if (b_max < a_min || b_min > a_max) return false;
	}
	for (normal_ptr = a.FaceNormal.begin(); normal_ptr != a.FaceNormal.end(); normal_ptr++) {
		float a_min = INFINITY, a_max = -INFINITY, project;
		float b_min = INFINITY, b_max = -INFINITY;
		vector<vec3>::const_iterator vertex_ptr;
		for (vertex_ptr = a.Vertices.begin(); vertex_ptr != a.Vertices.end(); vertex_ptr++) {
			project = (*normal_ptr) * (*vertex_ptr);
			if (project > a_max) a_max = project;
			if (project < a_min) a_min = project;
		}
		for (vertex_ptr = b.Vertices.begin(); vertex_ptr != b.Vertices.end(); vertex_ptr++) {
			project = (*normal_ptr) * (*vertex_ptr);
			if (project > b_max) b_max = project;
			if (project < b_min) b_min = project;
		}
		if (b_max < a_min || b_min > a_max) return false;
	}
	return true;
}

void Scene::FreeAll(){
	for (unsigned int i = 0; i < this->object_list.size(); i++) {
		delete this->buf_for_sort[i];
	}
	delete[] this->buf_for_sort;
	for (unsigned int i = 0; i < this->object_list.size(); i++) {
		delete this->object_list[i];
	}
}