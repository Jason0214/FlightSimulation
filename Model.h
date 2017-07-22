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
#include "DepthBuffer.h"
#include "Camera.h"
#include "Wrapper.h"

typedef struct mesh_data_struct {
	Mesh* meshes;
	unsigned int mesh_num;
}MeshData;

class Model {
public:
	Model(GLenum type, unsigned int level_num = 1):draw_type(type),wrappers(NULL),level_num(level_num){
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

	// load the wrapper of the model, one model may have several wrappers.
	void Model::LoadWrapper(std::string* filelist,unsigned int wrapper_number) {
		this->wrapper_num = wrapper_number;
		this->wrappers = new Wrapper[this->wrapper_num];
		for (unsigned int i = 0; i < wrapper_num; i++) {
			Wrapper::InitWrapper(this->wrappers[i],filelist[i]);
		}
	}
	
	// load the raw data (vertexs, textures) as well as generate openGL objects.
	void Load(std::string path, unsigned int level_index = 0);

	Shader shader;

	Wrapper* wrappers;
	unsigned int wrapper_num;
protected:
	MeshData* data;
	unsigned int level_num;

	GLenum draw_type;

	// load data of mesh contained in the model.
	void LoadMeshData(Mesh & mesh, aiMesh* raw_mesh, const aiScene* scene, std::string & directory);
	std::vector<Texture> LoadMeshMaterial(aiMaterial* material, aiTextureType type, std::string & directory);
	std::vector<Texture> all_textures;
	static GLuint Model::LoadTexture(const char* filename, std::string & directory);
};

class StaticModel:public Model {
public:
	StaticModel():Model(GL_STATIC_DRAW, 2){}
	~StaticModel(){}
	virtual void Render(unsigned int level_index, 
						const GLfloat model_mat[], 
						const GLfloat projection_mat[],
						const LightSrc & sun, 
						const DepthBuffer & depth_buffer) const;

	virtual void RenderFrame(unsigned int level_index, 
							const GLfloat model_mat[], 
							const GLfloat view_mat[],
							const GLfloat projection_mat[], 
							const Shader & frame_shader) const;
protected:

};
