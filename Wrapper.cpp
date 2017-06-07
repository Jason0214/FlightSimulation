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
			stringstream line(buf);
			Face face;
			while (line >> buf) {
				size_t slash_index_1 = buf.find('/', 0);
				size_t slash_index_2 = buf.find('/', slash_index_1 + 1);
				face.v_index.push_back(stoi(buf.substr(0, slash_index_1)));
				face.normal += vNormal[stoi(buf.substr(slash_index_2))];
			}
			face.normal = normalize(face.normal);
			wrapper.Faces.push_back(face);
		}
		else {
			getline(fin, buf);
		}
	}
}