#include "Scene.h"
#include "Exception.h"
#include <cstring>
#include <cstdlib>

#include <iostream>
using namespace std;

static int comp(const void* a, const void* b) {
	return (int)((*(Instance**)a)->distance - (*(Instance**)b)->distance);
}

Scene::Scene():shadow_map(){
	this->background = NULL;
	this->plane = NULL;
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
	for (unsigned int i = 0; i < this->object_list.size(); i++) {
		float distance = normalize(camera_front)*(this->object_list[i]->position - camera_position);
		this->object_list[i]->distance = distance;
	}
	qsort(&(this->object_list[0]), this->object_list.size(), sizeof(Instance*), comp);
}

void Scene::GenerateProjectionMatrix(GLuint width, GLuint height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	for (int i = 0; i < FRUSTUM_NUM; i++) {
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, this->frustum_clip[i], this->frustum_clip[i+1]);
		glGetFloatv(GL_PROJECTION_MATRIX, this->projection_matrix[i]);
	}
	glPopMatrix();
}

void Scene::RenderAll(const LightSrc & sun, const vec3 & camera_pos, const vec3 & camera_front){
// render shadow map
	this->Arrange(camera_front, camera_pos);
	this->GenerateShadowMap(sun.direction, this->window_width/this->window_height);
	//this->shadow_map.ShowTexture(0);
	int instance_index = (int)this->object_list.size()-1;
	for (int i = FRUSTUM_NUM -1; i >=0; i--) {
		glClear(GL_DEPTH_BUFFER_BIT);
		// draw alpha objects from far to near in order to fit with alpha value
		for (; instance_index >= 0; instance_index--) {
			Instance* ptr = this->object_list[instance_index];
			if (ptr->distance < this->frustum_clip[i]) break;
			ptr->model->Render(ptr->distance < 15.0f ? 0 : 1, ptr->model_matrix,
									this->projection_matrix[i], sun, this->shadow_map);
		}
		this->background->Render(this->projection_matrix[i], sun, this->shadow_map);
	}
	this->plane->Render(sun, this->shadow_map, this->projection_matrix[0]);
}

void Scene::GenerateShadowMap(const vec3 & light_direction, GLfloat aspect_ratio){
	glViewport(0, 0, this->shadow_map.map_width, this->shadow_map.map_height);
	this->shadow_map.BufferWriteConfig(aspect_ratio);
	for (int i = 0; i < DepthBuffer::CASCADE_NUM; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, this->shadow_map.GetFrameBuffer(i));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (unsigned int j = 0; j < this->object_list.size(); j++) {
			Instance* ptr = this->object_list[j];
			if (ptr->distance > this->shadow_map.GetClip(i + 1)) break;
			if (ptr->distance > this->shadow_map.GetClip(i)) {
				ptr->model->RenderFrame(ptr->distance < 15.0f ? 0 : 1, 
										ptr->model_matrix,
										this->shadow_map.GetViewMatrix(),
										this->shadow_map.GetProjectionMatrix(i), 
										this->shadow_map.shader);
			}
		}
		//this->background->RenderFrame(this->shadow_map.GetViewMatrix(),
		//	this->shadow_map.GetProjectionMatrix(i), this->shadow_map.shader);
		this->plane->RenderFrame(this->shadow_map.GetViewMatrix(),
			this->shadow_map.GetProjectionMatrix(i), this->shadow_map.shader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, this->window_width, this->window_height);
}

void Scene::CheckCollision() const{
	GLfloat buf[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(plane->position[0], plane->position[1], plane->position[2]);
	glMultMatrixf(plane->posture_mat);
	glGetFloatv(GL_MODELVIEW_MATRIX, buf);
	glPopMatrix();
	mat4 translate_mat(buf);
	Wrapper* plane_wrappers = new Wrapper[this->plane->wrapper_num];
	for (unsigned int i = 0; i < this->plane->wrapper_num; i++) {
		plane_wrappers[i] = this->plane->wrappers[i].Translate(translate_mat);
	}
	// check plane above the terrain
	int yaw_abs = abs((int)this->plane->yaw);
	int roll_abs = abs((int)this->plane->roll);
	int pitch_abs = abs((int)this->plane->pitch);
	
	for (unsigned int k = 0; k < this->plane->wrapper_num; k++) {
		for (vector<vec3>::iterator ptr = plane_wrappers[k].Vertices.begin(); ptr != plane_wrappers[k].Vertices.end(); ptr++) {
			if (ptr->y() <= this->background->GetHeight(ptr->x(), ptr->z())) {
				if (this->background->IsInAirport(ptr->x(), ptr->z()) && yaw_abs < 10 && roll_abs < 10 && pitch_abs < 10) {
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
	int x = (int)(this->plane->position.x()/MAP_SIDE_LEN), z = (int)(this->plane->position.z()/MAP_SIDE_LEN);
	for (int i = x - 2; i < x + 2; i++) {
		for (int j = z - 2; j < z + 2; j++) {
			const vector<Instance*> & model_list = this->object_grid_map[i][j];
			for (unsigned int ii = 0; ii < model_list.size(); ii++) {
				int k_num = model_list[ii]->model->wrapper_num;
				int v_num = this->plane->wrapper_num;
				for (int k = 0; k < k_num; k++) {
					for (int v = 0; v < v_num; v++) {
						if (OBBdetection(plane_wrappers[v], model_list[ii]->model->wrappers[k].Translate(mat4(model_list[ii]->model_matrix)))) {
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
		delete this->object_list[i];
	}
}