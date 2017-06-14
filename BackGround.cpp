#include "BackGround.h"
#include "Exception.h"
#include <cstring>
#include <fstream>

using namespace std;

BackGround::BackGround() :StaticModel() {
	memset(this->grid, 0, GRID_NUM * GRID_NUM);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, this->model_mat);
	glPopMatrix();
}

void BackGround::LoadHeightData(const string & file_name) {
	ifstream fin(file_name.c_str());
	for (unsigned int i = 0; i < GRID_NUM; i++) {
		for (unsigned int j = 0; j < GRID_NUM; j++) {
			if (!fin) {
				throw LoadFileError(file_name);
			}
			fin >> this->grid[i][j];
		}
	}
}

float BackGround::GetHeight(float x, float z) {
	unsigned int i = (int)floor(x / GRID_LEN), j = (int)floor(z / GRID_LEN);
	return this->grid[i][j];
}

bool BackGround::IsInAirport(float x, float z) {
	unsigned int i = (int)floor(x / GRID_LEN), j = (int)floor(z / GRID_LEN);
	if (i >= this->airport[0][0] && i <= this->airport[1][0] && j >= this->airport[0][1] && j <= this->airport[1][1]) {
		return true;
	}
	else return false;
}