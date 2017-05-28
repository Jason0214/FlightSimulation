#include "Skybox.h"
#include "Exception.h"

SkyBox::SkyBox():VAO(0),VBO(0){}

SkyBox::~SkyBox() {
	glDeleteTextures(1, &(this->TextureID));
	glDeleteBuffers(1,&(this->VBO));
	glDeleteBuffers(1,&(this->VAO));
}

void SkyBox::LoadTexture(const char *pic_path[], unsigned int pic_num) {
	glGenTextures(1, &this->TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureID);
	int width, height;
	unsigned char* image = NULL;
	for (unsigned int i = 0; i < pic_num; i++) {
		image = SOIL_load_image(pic_path[i], &width, &height, 0, SOIL_LOAD_RGBA);
		if (!image) throw LoadFileError(pic_path[i]);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//	glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		image = NULL;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SkyBox::DeployTexture() {
	// XXX VEO can be used
	GLfloat skybox_vertices[] = { //triangle
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,

		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,

		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,

		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,

		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	glGenVertexArrays(1, &(this->VAO));
	glGenBuffers(1, &(this->VBO));
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	// if these vertexs hardly change�� use static draw, otherwise dynamic
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	/* ���ö�ȡ�������ݵķ�ʽ
	param1 :  begin index
	param2 :  number of components per point
	param3 :  type
	param4 :  stride
	param5 :  specifies a offset of the first component of the first generic vertex
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void SkyBox::Draw(GLfloat camera_x, GLfloat camera_y, GLfloat camera_z) {
	glPushMatrix();
	/* �������ƶ����������λ�� */
	glTranslatef(camera_x, camera_y, camera_z);
	/* buffer ���ڴ������ */
	GLfloat matrix_buf[16];
	/* ʹ����պе���ɫ�� */
	this->shader.Use();
	/* ���ͶӰ���� ����uniform������ɫ�� */
	glGetFloatv(GL_PROJECTION_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "projection"), 1, GL_FALSE, matrix_buf);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "view"), 1, GL_FALSE, matrix_buf);
	/* ����պеĶ������� */
	glBindVertexArray(this->VAO);
	glActiveTexture(GL_TEXTURE0);
	/* ��0����ƬԪ��ɫ���е� skybox���� */
	glUniform1i(glGetUniformLocation(this->shader.ProgramID, "skybox"), 0);
	/* ʹ����պ����� */
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->TextureID);
	/* �������εķ�ʽ��Ⱦ���� */
	glDrawArrays(GL_TRIANGLES, 0, 36);
	/* �������� */
	glBindVertexArray(0);
	glPopMatrix();
}