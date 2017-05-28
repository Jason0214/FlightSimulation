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

class DepthMap;

class Model {
public:
	Model(GLenum type):model_type(type),mesh_num(0),meshes(NULL){}
	virtual ~Model(){ delete[] meshes;}
	virtual void Load(std::string path);
	virtual void Render(vec3 & position, vec3 & pivot, float angle, const LightSrc & light, const DepthMap & depth_buffer) const;
	virtual void RenderFrame(vec3 & position, vec3 & pivot, float angle, const Shader & frame_shader) const;
	Shader shader;
protected:
	void LoadMeshData(Mesh & mesh, aiMesh* raw_mesh, const aiScene* scene, std::string & directory);
	std::vector<Texture> LoadMeshMaterial(aiMaterial* material, aiTextureType type, std::string & directory);
	Mesh* meshes;
	unsigned int mesh_num;
	std::vector<Texture> all_textures;
	GLenum model_type;
};
