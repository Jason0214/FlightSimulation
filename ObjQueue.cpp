#include "ObjQueue.h"

using namespace std;

ObjectQueue::ObjectQueue(int level_num) :level_num(level_num) {
	this->leveled_set = new map<float, ListNode*>[level_num];
}

void ObjectQueue::AppendObject(vec3 & position, Model* type, vec3 & rotate, float angle){
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

void ObjectQueue::ReOrganize(const vec3 & camera_front,const vec3 & camera_position){
	for(ListNode* ptr = this->object_list; ptr != NULL; ptr = ptr->next){
		float distance = camera_front*(ptr->position - camera_position);
		if(distance < 50.0f){
			this->leveled_set[0][distance] = ptr;
		}
	}
}

void ObjectQueue::DrawAll(const LightSrc & sun, const DepthMap & depth_buffer){
// draw objects from far to near in order to fit with alpha value
	for (int i = this->level_num - 1; i >= 0; i--) {
		for (map<float, ListNode*>::const_iterator ptr = this->leveled_set[i].begin(); ptr != this->leveled_set[i].end(); ptr++) {
			ptr->second->type->Draw(ptr->second->position,ptr->second->pivot,ptr->second->angle, sun, depth_buffer);
		}
		this->leveled_set[i].clear();
	}
}

void ObjectQueue::FreeAll(){
	for(ListNode* ptr = this->object_list; ptr != NULL; ptr = ptr->next){
		delete ptr;
	}
	this->object_list = NULL;
	this->object_num = 0;
	for(int i = 0; i < this->level_num; i++){
		this->leveled_set[i].clear();
	}
}