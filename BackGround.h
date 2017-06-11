#pragma once
#include "Model.h"
#include <string>

class BackGround : public StaticModel {
public:
	BackGround();
	~BackGround(){}
	void Render(const LightSrc & light, const DepthMap & depth_buffer, unsigned int level_index = 0) const{
		this->StaticModel::Render(level_index, this->model_mat, light, depth_buffer);
	}
	void RenderFrame(const Shader & frame_shader, unsigned int level_index = 0)const{
		this->StaticModel::RenderFrame(level_index, this->model_mat, frame_shader);
	}
	void LoadHeightData(const std::string &);
	float GetHeight(float x, float z);
	bool IsInAirport(float x, float z);
	unsigned int airport[2][2];

	static const unsigned int GRID_LEN = 2;
	static const unsigned int GRID_NUM = 1000;
private:
	float grid[GRID_NUM][GRID_NUM];
	GLfloat model_mat[16];
};