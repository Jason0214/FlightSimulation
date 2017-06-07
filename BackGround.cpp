#include "BackGround.h"
#include "Exception.h"
#include <cstring>
#include <fstream>

using namespace std;

BackGround::BackGround() :StaticModel() {
	memset(this->grid, 0, GRID_NUM * GRID_NUM);
}

void BackGround::LoadHeightData(const string & file_name) {
	ifstream fin = ifstream("file_name");
	if (!fin) throw LoadFileError(file_name);
	for (unsigned int i = 0; i < GRID_NUM; i++) {
		for (unsigned int j = 0; j < GRID_NUM; j++) {
			fin >> this->grid[i][j];
		}
	}
}
