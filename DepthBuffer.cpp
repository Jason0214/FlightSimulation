#include "DepthBuffer.h"
#include "Exception.h"
#include "Scene.h"
#include <sstream>

using namespace std;

DepthBuffer::~DepthBuffer(){
	for (int i = 0; i < CASCADE_NUM; i++)
		glDeleteTextures(1, &(this->depth_textureID[i]));
	for (int i = 0; i < CASCADE_NUM; i++)
		glDeleteFramebuffers(1, &(this->FBO[i]));
}

void DepthBuffer::BufferWriteConfig(const vec3 & light_dir, GLfloat aspect_ratio) {
	// get the inverse of current view matrix
	GLfloat buf[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, buf);
	mat4 view_to_world_matrix = inverse(mat4(buf));

	glPushMatrix();
		glLoadIdentity();
		gluLookAt(0.0f, 0.0f, 0.0f, -light_dir[0], -light_dir[1], -light_dir[2], 0.0f, 1.0f, 0.0f);
		glGetFloatv(GL_MODELVIEW_MATRIX, this->light_space_view);
	glPopMatrix();

	mat4 light_view_convert_matrix(this->light_space_view);

	vec4 frustum_world_position[8];
	for (int i = 0; i < CASCADE_NUM; i++) {
		// initially load frustum position in view space
		// field of view default to be 45'
		GLfloat half_near_height = TanAngle(45.0f / 2) * this->z_clip[i];
		GLfloat half_near_width = aspect_ratio * half_near_height;
		GLfloat half_far_height = TanAngle(45.0f / 2) * this->z_clip[i + 1];
		GLfloat half_far_width = aspect_ratio * half_far_height;
		// init with frustum_view_position
		frustum_world_position[0] = vec4(-half_near_width, half_near_height, -this->z_clip[i], 1);
		frustum_world_position[1] = vec4(half_near_width, half_near_height, -this->z_clip[i], 1);
		frustum_world_position[2] = vec4(half_near_width, -half_near_height, -this->z_clip[i], 1);
		frustum_world_position[3] = vec4(-half_near_width, -half_near_height, -this->z_clip[i], 1);
		frustum_world_position[4] = vec4(-half_far_width, half_far_height, -this->z_clip[i + 1], 1);
		frustum_world_position[5] = vec4(half_far_width, half_far_height, -this->z_clip[i + 1], 1);
		frustum_world_position[6] = vec4(half_far_width, -half_far_height, -this->z_clip[i + 1], 1);
		frustum_world_position[7] = vec4(-half_far_width, -half_far_height, -this->z_clip[i + 1], 1);

		// convert frustum postion to light perspective 
		for (int j = 0; j < 8; j++) {
			frustum_world_position[j] = light_view_convert_matrix * view_to_world_matrix * frustum_world_position[j];
			frustum_world_position[j] /= frustum_world_position[j].w();
		}

		GLfloat ortho_params[6] = {
			frustum_world_position[0].x(),
			frustum_world_position[0].x(),
			frustum_world_position[0].y(),
			frustum_world_position[0].y(),
			frustum_world_position[0].z(),
			frustum_world_position[0].z(),
		};
		for (int j = 1; j < 8; j++) {
			if (frustum_world_position[j].x() < ortho_params[0]) ortho_params[0] = frustum_world_position[j].x();
			if (frustum_world_position[j].x() > ortho_params[1]) ortho_params[1] = frustum_world_position[j].x();
			if (frustum_world_position[j].y() < ortho_params[2]) ortho_params[2] = frustum_world_position[j].y();
			if (frustum_world_position[j].y() > ortho_params[3]) ortho_params[3] = frustum_world_position[j].y();
			if (frustum_world_position[j].z() < ortho_params[4]) ortho_params[4] = frustum_world_position[j].z();
			if (frustum_world_position[j].z() > ortho_params[5]) ortho_params[5] = frustum_world_position[j].z();
		}
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(ortho_params[0], ortho_params[1], ortho_params[2], ortho_params[3], -ortho_params[5], -ortho_params[4]);
		glGetFloatv(GL_PROJECTION_MATRIX, this->light_space_projection[i]);
		glPopMatrix();
	}
}

void DepthBuffer::BufferReadConfig(const Shader & shader) const {
	glUniform1fv(glGetUniformLocation(shader.ProgramID, "shadow_clip"), CASCADE_NUM + 1, this->z_clip);
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, "light_space_view"), 1, GL_FALSE, this->light_space_view);
	glUniformMatrix4fv(glGetUniformLocation(shader.ProgramID, "light_space_projection"), CASCADE_NUM, GL_FALSE, (const GLfloat*)this->light_space_projection);
	for (int i = 0; i < CASCADE_NUM; i++) {
		glActiveTexture(GL_TEXTURE4 + i);
		glBindTexture(GL_TEXTURE_2D, this->depth_textureID[i]);
		glUniform1i(glGetUniformLocation(shader.ProgramID, ("shadow_map[" + stringstream(i).str() + "]").c_str()), 4 + i);
	}
//	GLint texture_index[MAX_CASCADE] = { 4, 5, 6 };
//	glUniform1iv(glGetUniformLocation(shader.ProgramID, "shadow_map"), CASCADE_NUM, (GLint*)texture_index);
}

void DepthBuffer::init(std::string vs_path, std::string fs_path){
	this->shader.LoadShader(vs_path.c_str(), fs_path.c_str());
	for (int i = 0; i < CASCADE_NUM; i++) {
		// gen texture ID
		glGenTextures(1, &(this->depth_textureID[i]));
		glBindTexture(GL_TEXTURE_2D, this->depth_textureID[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->map_width, this->map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// gen FBO
		glGenFramebuffers(1, &(this->FBO[i]));
		glBindFramebuffer(GL_FRAMEBUFFER, this->FBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_textureID[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

#ifdef DEPTH_BUFFER_TEST
void DepthBuffer::test_init() {
	this->test_shader.LoadShader("./Shaders/frame_texture_view_shader.vs", "./Shaders/frame_texture_view_shader.fs");
	GLfloat vertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &(this->test_VAO));
	glGenBuffers(1, &(this->test_VBO));
	glBindVertexArray(this->test_VAO);
		glBindBuffer(GL_ARRAY_BUFFER,this->test_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*4,vertices,GL_STATIC_DRAW);
		//position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(0));
		//texture
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
}


void DepthBuffer::ShowTexture(int texture_index) {
	if (!is_inited) {
		this->test_init();
	}
	glDisable(GL_DEPTH_TEST);
	this->test_shader.Use();
	glBindVertexArray(this->test_VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->depth_textureID[texture_index]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}
#endif