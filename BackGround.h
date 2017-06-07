#pragma once
#include "Model.h"
#include <string>

#define GRID_LEN 2
#define GRID_NUM 1000


class BackGround : public StaticModel {
public:
	BackGround();
	~BackGround(){}
	void Render(const LightSrc & light, const DepthMap & depth_buffer, unsigned int level_index = 0) const{
		this->StaticModel::Render(vec3(0.0f, 0.0f, 0.0f), level_index, vec3(1.0f, 0.0f, 0.0f), 0.0f, light, depth_buffer);
	}
	void RenderFrame(const Shader & frame_shader, unsigned int level_index = 0)const{
		this->StaticModel::RenderFrame(vec3(0.0f, 0.0f, 0.0f), level_index, vec3(1.0f, 0.0f, 0.0f), 0.0f, frame_shader);
	}
	void LoadHeightData(const std::string &);
	float grid[GRID_NUM][GRID_NUM];
};