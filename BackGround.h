#pragma once
#include "Model.h"
#include <string>

class BackGround : public StaticModel {
public:
	BackGround();
	~BackGround(){}
	// inheirts from class::StaticModel
	void Render(const LightSrc & light, const DepthMap & depth_buffer, unsigned int level_index = 0) const{
		this->StaticModel::Render(level_index, this->model_mat, light, depth_buffer);
	}
	// inheirts from class::StaticModel
	void RenderFrame(const Shader & frame_shader, unsigned int level_index = 0)const{
		this->StaticModel::RenderFrame(level_index, this->model_mat, frame_shader);
	}


	// load height map from outside file, used for collision detection
	// between terrain and plane model
	void LoadHeightData(const std::string &);

	// given a (x,z) coordinate, return its height in the terrain
	float GetHeight(float x, float z);

	// boolen test for a (x,z) coordinate whether at the area of an airport
	// XXX: multiple airport
	bool IsInAirport(float x, float z);
	unsigned int airport[2][2];

	static const unsigned int GRID_LEN = 2;
	static const unsigned int GRID_NUM = 1000;
private:
	float grid[GRID_NUM][GRID_NUM];
	GLfloat model_mat[16];
};