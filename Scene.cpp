#include "Scene.h"
#include "Exception.h"
#include <cstring>
#include <cstdlib>

#define FRUSTUM_NEAR (0.5f)
#define FRUSTUM_RATIO (500)

#include <iostream>
using namespace std;

Scene::Scene(int frustum_num){
	this->frustum_num = frustum_num;
	this->object_num = 0;
	this->background = NULL;
	this->plane = NULL;
	this->sort_buf = NULL;
	this->sort_buf_count = NULL;
	memset(this->object_map, NULL, MAP_SIDE_NUM *MAP_SIDE_NUM * sizeof(ListNode*));
}

void Scene::AppendObject(vec3 & position, StaticModel* instance, vec3 & rotate, float angle){
	ListNode* list_ptr = new ListNode(position, instance, rotate, angle);
	if (this->object_list) {
		ListNode* ptr_temp = this->object_list->next;
		this->object_list->next = list_ptr;
		list_ptr->next = ptr_temp;
	}
	else {
		this->object_list = list_ptr;
	}

	ListNode* map_ptr = new ListNode(position, instance, rotate, angle);
	unsigned int i = (unsigned int)(position.x() / MAP_SIDE_LEN), j = (unsigned int)(position.z() /MAP_SIDE_LEN);
	if(this->object_map[i][j]){
		ListNode* ptr_temp = this->object_map[i][j]->next;
		this->object_map[i][j]->next = map_ptr;
		map_ptr->next = ptr_temp;
	}
	else{
		this->object_map[i][j] = map_ptr;
	}
	this->object_num++;
}

static int comp(const void* a,const void* b) {
	return (int)((*(RenderObj**)a)->distance - (*(RenderObj**)b)->distance);
}

void Scene::Arrange(const vec3 & camera_front,const vec3 & camera_position){
	if (!this->sort_buf) {
		this->sort_buf_count = new int[this->frustum_num];
		this->sort_buf = new RenderObj**[this->frustum_num];
		memset(this->sort_buf_count, 0, this->frustum_num * sizeof(int));
		for (int i = 0; i < this->frustum_num; i++) {
			this->sort_buf[i] = new RenderObj*[this->object_num];
			memset(this->sort_buf[i], 0, this->object_num*sizeof(RenderObj*));
		}
	}
	
	for (ListNode* ptr = this->object_list; ptr != NULL; ptr = ptr->next) {
		float distance = normalize(camera_front)*(ptr->position - camera_position);
		// group all the object into different frustum according there distance to the camera
			for (int i = 0; i < this->frustum_num; i++) {
				if (FRUSTUM_NEAR * Power(FRUSTUM_RATIO, i) <= distance && distance < FRUSTUM_NEAR * Power(FRUSTUM_RATIO, i + 1)) {
					RenderObj* temp = this->sort_buf[i][this->sort_buf_count[i]];
					if (!temp) {
						temp = new RenderObj;
						this->sort_buf[i][this->sort_buf_count[i]] = temp;
					}
					temp->distance = distance;
					temp->node = ptr;
					this->sort_buf_count[i]++;
			}
		}
	}
	for (int i = 0; i < this->frustum_num; i++) {
		qsort(this->sort_buf[i], this->sort_buf_count[i], sizeof(RenderObj*),comp);
	}
}

void Scene::ResetArrange() {
	memset(this->sort_buf_count, 0, this->frustum_num*sizeof(int));
}

void Scene::ReProject(int level_index) const{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)this->window_width / (GLfloat)this->window_height, 
		FRUSTUM_NEAR*Power(FRUSTUM_RATIO, level_index), FRUSTUM_NEAR*Power(FRUSTUM_RATIO, level_index+1));
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Scene::RenderAll(const LightSrc & sun, const DepthMap & depth_buffer)const{
// draw objects from far to near in order to fit with alpha value
	for (int i = this->frustum_num - 1; i >= 0; i--) {
		this->ReProject(i); // change frustum and clear depth
		for (int j = 0; j < this->sort_buf_count[i]; j++) {
			RenderObj* ptr = this->sort_buf[i][j];
			if(ptr->distance < 50.0f)
				ptr->node->instance->Render(0, ptr->node->model_mat, sun, depth_buffer);
			else
				ptr->node->instance->Render(1, ptr->node->model_mat, sun, depth_buffer);
		}
		if (this->background)
			this->background->Render(sun, depth_buffer);
	}
	if (this->plane)
		this->plane->Render(sun, depth_buffer);
}

void Scene::RenderFrame(const Shader & frame_shader)const{
	frame_shader.Use();
	for (int i = this->frustum_num - 1; i >= 0; i--) {
		for (int j = 0; j < this->sort_buf_count[i]; j++) {
			RenderObj* ptr = this->sort_buf[i][j];
			if(ptr->distance < 50.0f)
				ptr->node->instance->RenderFrame(0, ptr->node->model_mat, frame_shader);
			else
				ptr->node->instance->RenderFrame(1, ptr->node->model_mat, frame_shader);
		}
	}
	if (this->plane && !this->plane->is_crash)
		this->plane->RenderFrame(frame_shader);
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
			for (ListNode* ptr = this->object_map[i][j]; ptr != NULL; ptr = ptr->next) {
				int k_num = ptr->instance->wrapper_num;
				int v_num = this->plane->wrapper_num;
				for (int k = 0; k < k_num; k++) {
					for (int v = 0; v < v_num; v++) {
						if (OBBdetection(plane_wrappers[v], ptr->instance->wrappers[k].Translate(mat4(ptr->model_mat)))) {
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
	ListNode* p = this->object_list,*q;
	while (p) {
		q = p->next;
		delete p;
		p = q;
	}

	for (unsigned int i = 0; i < MAP_SIDE_NUM; i++) {
		for (unsigned int j = 0; j < MAP_SIDE_NUM; j++) {
			ListNode* p = this->object_map[i][j],*q = NULL;
			while (p) {
				q = p->next;
				delete p;
				p = q;
			}
			this->object_map[i][j] = NULL;
		}
	}
	this->object_num = 0;

	delete[] this->sort_buf_count;
	for (int i = 0; i < this->frustum_num; i++) {
		for (int j = 0; j < this->object_num; j++) {
			delete this->sort_buf[i][j];
		}
		delete[] this->sort_buf[i];
	}
	delete[] this->sort_buf;
}