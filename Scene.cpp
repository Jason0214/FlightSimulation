#include "Scene.h"

#define FRUSTUM_NEAR (0.5f)
#define FRUSTUM_RATIO (500)

#include <iostream>
using namespace std;

Scene::Scene(int frustum_num){
	this->frustum_num = frustum_num;
	this->object_num = 0;
	this->background = NULL;
	this->plane = NULL;
	this->dist_order_dict = new map<GLfloat, PositionListNode*>[this->frustum_num];
	memset(this->object_map, NULL, GRID_NUM * GRID_NUM * sizeof(InstanceListNode*));
}

void Scene::AppendObject(vec3 & position, StaticModel* instance, vec3 & rotate, float angle){
	PositionListNode* new_ptr_1 = new PositionListNode(position, instance, rotate, angle);

	if (this->object_list) {
		PositionListNode* ptr_temp = this->object_list->next;
		this->object_list->next = new_ptr_1;
		new_ptr_1->next = ptr_temp;
	}
	else {
		this->object_list = new_ptr_1;
	}

	InstanceListNode* new_ptr_2 = new InstanceListNode(instance);
	unsigned int i = position.x() / GRID_LEN, j = position.z() / GRID_LEN;
	if(this->object_map[i][j]){
		InstanceListNode* ptr_temp = this->object_map[i][j]->next;
		this->object_map[i][j]->next = new_ptr_2;
		new_ptr_2->next = ptr_temp;
	}
	else{
		this->object_map[i][j] = new_ptr_2;
	}
	this->object_num++;
}

void Scene::Arrange(const vec3 & camera_front,const vec3 & camera_position){
	for (PositionListNode* ptr = this->object_list; ptr != NULL; ptr = ptr->next) {
		float distance = camera_front*(ptr->position - camera_position);
		// group all the object into different frustum according there distance to the camera
			for (int i = 0; i < this->frustum_num; i++) {
				if (FRUSTUM_NEAR * Power(FRUSTUM_RATIO, i) <= distance && distance < FRUSTUM_NEAR * Power(FRUSTUM_RATIO, i + 1)) {
					this->dist_order_dict[i][distance] = ptr;
			}
		}
	}
}

void Scene::ResetArrange() {
	for (int i = 0; i < this->frustum_num; i++) {
		this->dist_order_dict[i].clear();
	}
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
	this->CheckCollision();
	for (int i = this->frustum_num - 1; i >= 0; i--) {
		this->ReProject(i); // change frustum and clear depth
		for (map<float, PositionListNode*>::const_iterator ptr = this->dist_order_dict[i].begin(); ptr != this->dist_order_dict[i].end(); ptr++) {
			ptr->second->instance->Render(ptr->second->position, 0, ptr->second->pivot,ptr->second->angle, sun, depth_buffer);
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
		for (map<float, PositionListNode*>::const_iterator ptr = this->dist_order_dict[i].begin(); ptr != this->dist_order_dict[i].end(); ptr++) {
			ptr->second->instance->RenderFrame(ptr->second->position, 0, ptr->second->pivot, ptr->second->angle, frame_shader);
		}
	}
	if (this->plane)
		this->plane->RenderFrame(frame_shader);
}

void Scene::CheckCollision() const{
	int flag = 0;
	if (!plane->is_land) {
	// check plane above the terrain
		for (unsigned i = 0; i < this->plane->wrapper_num; i++) {
			for (vector<vec3>::iterator ptr = this->plane->wrappers[i].Vertices.begin(); ptr != this->plane->wrappers[i].Vertices.end(); ptr++) {
				int i = floor(ptr->x()), j = floor(ptr->z());
				if (ptr->y() <= this->background->grid[i][j]) {
					if (i >= this->background->airport_x_min && i <= this->background->airport_x_max
						&& j >= this->background->airport_z_min && j <= this->background->airprt_z_min
						&& abs((int)this->plane->yaw) < 5 && abs((int)this->plane->roll) < 5 
						&& abs((int)this->plane->pitch) < 5){
						this->plane->is_land = true;
						this->plane->yaw = 0.0f;
						this->plane->roll = 0.0f;
						this->plane->pitch = 0.0f;
						return;
					}
					else {
						throw Collision();
					}
				}
				else if (i < 50 || i > 950 || j < 50 || j > 950) {
					flag = 1;
				}
				else if (i < 10 || i > 990 || j < 10 || j > 990) {
					flag = 2;
				}
			}
		}
	}
	int x = this->plane->position.x()/GRID_LEN, z = this->plane->position.z()/GRID_LEN;
	for (int i = x - 2; i < x + 2; i++) {
		for (int j = z - 2; j < z + 2; j++) {
			for (InstanceListNode* ptr = this->object_map[i][j]; ptr != NULL; ptr = ptr->next) {
				for (int k = 0; k < ptr->instance->wrapper_num; k++) {
					for (int v = 0; v < this->plane->wrapper_num; v++) {
						if (OBBdetection(ptr->instance->wrappers[k], this->plane->wrappers[v])) {
							throw Collision();
						}
					}
				}
			}
		}
	}
	if (flag == 1) throw WarningBoard();
	if (flag == 2) throw ReachBoard();
}

void Scene::FreeAll(){
	PositionListNode* p = this->object_list,*q;
	while (p) {
		q = p->next;
		delete p;
		p = q;
	}

	for (unsigned int i = 0; i < GRID_NUM; i++) {
		for (unsigned int j = 0; j < GRID_NUM; j++) {
			InstanceListNode* p = this->object_map[i][j],*q = NULL;
			while (p) {
				q = p->next;
				delete p;
				p = q;
			}
			this->object_map[i][j] = NULL;
		}
	}

	this->object_num = 0;
	delete[] this->dist_order_dict;
}