#pragma once
#include <glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#pragma comment(lib,"assimp.lib")

#include <SOIL.h>
#ifdef _DEBUG
	#pragma comment (lib, "SOILd.lib")
#else
	#pragma comment (lib, "SOIL.lib")
#endif
#include <vector>
#include <string>
#include "Mesh.h"
#include "myMath.h"
#include "Shader.h"
#include "LightSrc.h"
#include "DepthMap.h"
#include "Camera.h"
#include "Wrapper.h"

typedef struct mesh_data_struct {
	Mesh* meshes;
	unsigned int mesh_num;
}MeshData;

class Model {
public:
	Model(GLenum type, unsigned int level_num = 1):draw_type(type),wrapper(NULL),level_num(level_num){
		this->data = new MeshData[this->level_num];
		for (unsigned int i = 0; i < this->level_num; i++) {
			this->data[i].meshes = NULL;
			this->data[i].mesh_num = 0;
		}
	}
	virtual ~Model(){ 
		for (unsigned int i = 0; i < this->level_num; i++) {
			delete[] this->data[i].meshes;
		}
		delete[] this->data; 
		delete this->wrappers;
	}
	void Model::LoadWrapper(std::string* filelist,unsigned int wrapper_number) {
		this->wrappers = new Wrapper[wrapper_num];
		for (unsigned int i = 0; i < wrapper_num; i++) {
			Wrapper::InitWrapper(this->wrappers[i],filelist[i]);
		}

	}
	void Load(std::string path, unsigned int level_index = 0);

	Shader shader;

	Wrapper* wrappers;
	unsigned int wrapper_num;
protected:
	MeshData* data;
	unsigned int level_num;

	GLenum draw_type;

	void LoadMeshData(Mesh & mesh, aiMesh* raw_mesh, const aiScene* scene, std::string & directory);
	std::vector<Texture> LoadMeshMaterial(aiMaterial* material, aiTextureType type, std::string & directory);
	std::vector<Texture> all_textures;
	static GLuint Model::LoadTexture(const char* filename, std::string & directory);
};

class StaticModel:public Model {
public:
	StaticModel():Model(GL_STATIC_DRAW){}
	~StaticModel(){}
	virtual void Render(vec3 & position, unsigned int level_index, vec3 & pivot, float angle, const LightSrc & light, const DepthMap & depth_buffer) const;
	virtual void RenderFrame(vec3 & position, unsigned int level_index, vec3 & pivot, float angle, const Shader & frame_shader) const;
protected:
	void Translate(vec3 & position, vec3 pivot, float angle) const;
};
