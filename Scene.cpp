#include "Scene.h"

using namespace std;

Scene::Scene(int level_num) :level_num(level_num) {
	this->object_list = NULL;
	this->object_num = 0;
	this->background = NULL;
	this->leveled_set = new map<float, ListNode*>[level_num];
}

void Scene::AppendObject(vec3 & position, Model* type, vec3 & rotate, float angle){
	ListNode* ptr_new = new ListNode(position,type,rotate,angle);
	if(this->object_list){
		ListNode* ptr_temp = this->object_list->next;
		this->object_list->next = ptr_new;
		ptr_new->next = ptr_temp;
	}
	else{
		this->object_list = ptr_new;
	}
	this->object_num++;
}

void Scene::Arrange(const vec3 & camera_front,const vec3 & camera_position){
	for(ListNode* ptr = this->object_list; ptr != NULL; ptr = ptr->next){
		float distance = camera_front*(ptr->position - camera_position);
		if(distance < 50.0f){
			this->leveled_set[0][distance] = ptr;
		}
	}
}

void Scene::ResetArrange() {
	for (int i = this->level_num - 1; i >= 0; i--) {
		this->leveled_set[i].clear();
	}
}

void Scene::RenderAll(const LightSrc & sun, const DepthMap & depth_buffer)const{
// draw objects from far to near in order to fit with alpha value
	for (int i = this->level_num - 1; i >= 0; i--) {
		for (map<float, ListNode*>::const_iterator ptr = this->leveled_set[i].begin(); ptr != this->leveled_set[i].end(); ptr++) {
			ptr->second->type->Render(ptr->second->position,ptr->second->pivot,ptr->second->angle, sun, depth_buffer);
		}
	}
	if(this->background)
		this->background->type->Render(this->background->position, this->background->pivot, this->background->angle, sun, depth_buffer);
}

void Scene::RenderFrame(const Shader & frame_shader)const{
	frame_shader.Use();
	for (int i = this->level_num - 1; i >= 0; i--) {
		for (map<float, ListNode*>::const_iterator ptr = this->leveled_set[i].begin(); ptr != this->leveled_set[i].end(); ptr++) {
			ptr->second->type->RenderFrame(ptr->second->position, ptr->second->pivot, ptr->second->angle, frame_shader);
		}
	}
	//if(this->background)
	//	this->background->type->RenderFrame(this->background->position, this->background->pivot, this->background->angle, frame_shader);
}

void Scene::FreeAll(){
	for(ListNode* ptr = this->object_list; ptr != NULL; ptr = ptr->next){
		delete ptr;
	}
	this->object_list = NULL;
	this->object_num = 0;
	for(int i = 0; i < this->level_num; i++){
		this->leveled_set[i].clear();
	}
}