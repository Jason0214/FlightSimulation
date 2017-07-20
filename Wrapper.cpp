#include "Wrapper.h"
#include <fstream>
#include <sstream>
#include <string>
#include "Exception.h"

using namespace std;

void Wrapper::InitWrapper(Wrapper & wrapper,const std::string file_name){
	vector<vec3> vNormal;
	ifstream fin(file_name.c_str());
	if (!fin) throw LoadFileError(file_name);
	string buf;
	vec3 temp_vec;
	while (fin >> buf) {
		if (buf == "v") {
			fin >> temp_vec.x();
			fin >> temp_vec.y();
			fin >> temp_vec.z();
			wrapper.Vertices.push_back(temp_vec);
		}
		else if (buf == "vn") {
			fin >> temp_vec.x();
			fin >> temp_vec.y();
			fin >> temp_vec.z();
			vNormal.push_back(temp_vec);
		}
		else if (buf == "f") {
			getline(fin, buf);
			vec3 normal;
			stringstream line(buf);
			while (line >> buf) {
				size_t slash_index_1 = buf.find('/', 0);
				size_t slash_index_2 = buf.find('/', slash_index_1 + 1);
				normal += normalize(vNormal[stoi(buf.substr(slash_index_2+1))]);
			}
			wrapper.FaceNormal.push_back(normalize(normal));
		}
		else {
			getline(fin, buf);
		}
	}
}

Wrapper Wrapper::Translate(mat4 matrix) {
	Wrapper ret;
	for (vector<vec3>::const_iterator ptr = this->Vertices.begin(); ptr != this->Vertices.end(); ptr++) {
		ret.Vertices.push_back(vec3(matrix * vec4((*ptr)[0], (*ptr)[1], (*ptr)[2], 1.0f)));
	}
	for (vector<vec3>::const_iterator ptr = this->FaceNormal.begin(); ptr != this->FaceNormal.end(); ptr++) {
		ret.FaceNormal.push_back(vec3(matrix * vec4((*ptr)[0], (*ptr)[1], (*ptr)[2], 0.0f)));
	}
	return ret;
}