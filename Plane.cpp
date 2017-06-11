#include "Plane.h"
#include "myMath.h"

#define YAW_PIVOT 1
#define ROLL_PIVOT 2
#define PITCH_PIVOT 3

#include <iostream>

using namespace std;

PlaneModel::PlaneModel(vec3 origin_position) :Model(GL_DYNAMIC_DRAW), origin(origin_position) {
	this->FULL_ACC = 0.1;
	this->MAX_SPEED = 0.5;
	this->AIR_YAW_SPEED = 0.3;
	this->LAND_YAW_SPEED = 0.8;
	this->ROLL_SPEED = 0.8;
	this->PITCH_SPEED = 0.5;

	this->init();
}

void PlaneModel::init() {
	this->front = vec3(1.0f, 0.0f, 0.0f);
	this->position = this->origin;
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->roll = 0.0f;

	this->speed = 0.0f;
	this->fan_spin = 0.0f;
	
	this->is_land = true;

	memset(this->posture_mat, 0, sizeof(GLfloat) * 16);
	this->posture_mat[0] = 1.0f;
	this->posture_mat[5] = 1.0f;
	this->posture_mat[10] = 1.0f;
	this->posture_mat[15] = 1.0f;
}

void PlaneModel::Rotate(GLfloat angle ,int pivot) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(this->posture_mat);;
	switch (pivot) {
	case YAW_PIVOT: glRotatef(angle, 0.0f, 1.0f, 0.0f); break;
	case ROLL_PIVOT: glRotatef(angle, 1.0f, 0.0f, 0.0f); break;
	case PITCH_PIVOT: glRotatef(angle, 0.0f, 0.0f, 1.0f); break;
	}
	glGetFloatv(GL_MODELVIEW_MATRIX, this->posture_mat);
	glPopMatrix();
	this->front[0] = this->posture_mat[0];
	this->front[1] = this->posture_mat[1];
	this->front[2] = this->posture_mat[2];
	this->up[0] = this->posture_mat[4];
	this->up[1] = this->posture_mat[5];
	this->up[2] = this->posture_mat[6];
}

void PlaneModel::Forward() {
	//cout << "speed:" << this->speed << endl;
	//cout << "yaw:" << this->yaw << endl;
	//cout << "roll:" << this->roll << endl;
	//cout << "pitch:" << this->pitch << endl;

	this->fan_spin += this->throttle  * 80;
	if (this->fan_spin > 360.0f) fan_spin -= 360.0f;

	this->speed += this->throttle * this->FULL_ACC - 0.86f * this->speed * this->speed - aSin(this->pitch)*0.05;
	if (this->speed < 0) this->speed = 0.0f;
	GLfloat up_speed;
	if (this->is_land) {
		if (this->speed > 0.30) {
			this->is_land = false;
		}
		up_speed = 0.0f;
	}
	else {
		up_speed = 0.4 * this->speed * this->speed  - 0.02;
	}
	this->position = this->position + this->front * this->speed;

	//if (!this->is_land) {
	//	cout << RandFloat();
	//	this->position += vec3(RandFloat(), RandFloat(), RandFloat());
	//}
}

void PlaneModel::SpeedUp(){
	this->throttle += 0.02f;
	if (this->throttle > 1.0f) this->throttle = 1.0f;
}

void PlaneModel::SpeedDown() {
	this->throttle -= 0.02f;
	if (this->throttle < 0.0f) this->throttle = 0.0f;
}

void PlaneModel::RollLeft() {
	if (!this->is_land) {
		this->roll -= this->ROLL_SPEED;
		if (this->roll < -180.0f) this->roll += 360.0f;
		this->Rotate(-ROLL_SPEED, ROLL_PIVOT);
	}
}

void PlaneModel::RollRight() {
	if (!this->is_land) {
		this->roll += this->ROLL_SPEED;
		if (this->roll > 180.0f) this->roll -= 360.0f;
		this->Rotate(ROLL_SPEED, ROLL_PIVOT);
	}
}

void PlaneModel::RollBack() {
	this->Rotate(0 - this->roll, ROLL_PIVOT);
	this->roll = 0.0f;
	//if (this->roll > 0) {
	//	this->roll -= ROLL_SPEED;
	//	if (this->roll < 0.0f) {
	//		this->Rotate(-this->roll - ROLL_SPEED, ROLL_PIVOT);
	//		this->yaw = 0.0f;
	//	}
	//	else {
	//		this->Rotate(-ROLL_SPEED, ROLL_PIVOT);
	//	}
	//}
	//if (this->roll < 0) {
	//	this->roll += ROLL_SPEED;
	//	if (this->roll > 0.0f) {
	//		this->Rotate(-this->roll + ROLL_PIVOT, ROLL_PIVOT);
	//		this->roll = 0.0f;
	//	}
	//	else {
	//		this->Rotate(ROLL_SPEED, ROLL_PIVOT);
	//	}
	//}
} 

void PlaneModel::YawRight() {
	if (this->yaw > -40.0f) {
		if (this->is_land) {
			this->yaw -= this->LAND_YAW_SPEED;
			this->Rotate(-LAND_YAW_SPEED, YAW_PIVOT);
		}
		else {
			this->yaw -= this->AIR_YAW_SPEED;
			this->Rotate(-AIR_YAW_SPEED, YAW_PIVOT);
		}
	}
}

void PlaneModel::YawLeft() {
	if (this->yaw < 40.0f) {
		if (this->is_land) {
			this->yaw += this->LAND_YAW_SPEED;
			this->Rotate(LAND_YAW_SPEED, YAW_PIVOT);
		}
		else {
			this->yaw += this->AIR_YAW_SPEED;
			this->Rotate(AIR_YAW_SPEED, YAW_PIVOT);
		}
	}
}

void PlaneModel::YawBack() {
	this->Rotate(0 - this->yaw, YAW_PIVOT);
	this->yaw = 0.0f;
}

void PlaneModel::PitchUp() {
	if (!is_land) {
		this->pitch += this->PITCH_SPEED;
		this->Rotate(PITCH_SPEED, PITCH_PIVOT);
	}
}

void PlaneModel::PitchDown() {
	if (!is_land) {
		this->pitch -= this->PITCH_SPEED;
		this->Rotate(-PITCH_SPEED, PITCH_PIVOT);
	}
}

void PlaneModel::PitchBack() {
	this->Rotate(0 - this->pitch, PITCH_PIVOT);
	this->pitch = 0.0f;
}

void PlaneModel::Translate()const{
	glTranslatef(this->position[0], this->position[1], this->position[2]);
	glMultMatrixf(this->posture_mat);
}

void PlaneModel::Render(const LightSrc & light, const DepthMap & depth_buffer) const {
	MeshData & current_mesh_set = this->data[0];
	GLfloat matrix_buf[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	this->Translate();
	this->shader.Use();
	glGetFloatv(GL_PROJECTION_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "projection"), 1, GL_FALSE, matrix_buf);
	glPushMatrix();
		glLoadMatrixf(depth_buffer.light_space_view);
		this->Translate();
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
		glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "light_space_view"), 1, GL_FALSE, matrix_buf);
	glPopMatrix();
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "light_space_projection"), 1, GL_FALSE, depth_buffer.light_space_project);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_buffer.depth_textureID);
	glUniform1i(glGetUniformLocation(this->shader.ProgramID, "depth_map"), 0);
	glUniform3f(glGetUniformLocation(this->shader.ProgramID, "light_direction"), light.direction[0], light.direction[1], light.direction[2]);
	glUniform3f(glGetUniformLocation(this->shader.ProgramID, "light_color"), light.color[0], light.color[1], light.color[2]);

		glPushMatrix();
			glTranslatef(0.393f, 0.928f, 0.0f);
			glRotatef(this->fan_spin, aCos(12), aSin(12), 0.0f);
			glTranslatef(-0.393f, -0.928f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
			glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "view"), 1, GL_FALSE, matrix_buf);
			current_mesh_set.meshes[FAN].render(this->shader.ProgramID);
		glPopMatrix();
		//if (this->yaw > 0.0f) {
		//	glPushMatrix();
		//	glTranslatef(-2.524f, 0.425f,0.028f);
		//	glRotatef(45, -0.208f, -0.978f, 0.0f);
		//	glTranslatef(2.524f, -0.425f, -0.028f);
		//	this->Translate();
		//	current_mesh_set.meshes[YAW_FIN].render(this->shader.ProgramID);
		//	glPopMatrix();
		//}
		//else if (this->yaw < 0.0f) {
		//	glPushMatrix();
		//	glTranslatef(-2.524f, 0.425f, 0.028f);
		//	glRotatef(-45, -0.208f, -0.978f, 0.0f);
		//	glTranslatef(2.524f, -0.425f, -0.028f);
		//	this->Translate();
		//	current_mesh_set.meshes[YAW_FIN].render(this->shader.ProgramID);
		//	glPopMatrix();
		//}
		//else {
		//	glPushMatrix();
		//	this->Translate();
		//	current_mesh_set.meshes[YAW_FIN].render(this->shader.ProgramID);
		//	glPopMatrix();
		//}
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
		glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "view"), 1, GL_FALSE, matrix_buf);
		current_mesh_set.meshes[YAW_FIN].render(this->shader.ProgramID);
		current_mesh_set.meshes[PITCH_FIN].render(this->shader.ProgramID);
		current_mesh_set.meshes[BODY].render(this->shader.ProgramID);
		current_mesh_set.meshes[LEFT_WHEEL].render(this->shader.ProgramID);
		current_mesh_set.meshes[RIGHT_WHEEL].render(this->shader.ProgramID);
		current_mesh_set.meshes[LEFT_DOWN_FLAP].render(this->shader.ProgramID);
		current_mesh_set.meshes[LEFT_UP_FLAP].render(this->shader.ProgramID);
		current_mesh_set.meshes[RITGHT_DOWN_FLAP].render(this->shader.ProgramID);
		current_mesh_set.meshes[RITGHT_UP_FLAP].render(this->shader.ProgramID);
		current_mesh_set.meshes[GUN].render(this->shader.ProgramID);

	glPopMatrix();
}

void PlaneModel::RenderFrame(const Shader & frame_shader) const {
	MeshData & current_mesh_set = this->data[0];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	GLfloat matrix_buf[16];
	this->Translate();
	glGetFloatv(GL_PROJECTION_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(frame_shader.ProgramID, "projection"), 1, GL_FALSE, matrix_buf);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(frame_shader.ProgramID, "view"), 1, GL_FALSE, matrix_buf);
	// draw every mesh
	for (unsigned int i = 0; i < current_mesh_set.mesh_num; i++) {
		current_mesh_set.meshes[i].render_frame(frame_shader.ProgramID);
	}
	glPopMatrix();
}