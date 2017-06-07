#include "Model.h"
#include "Exception.h"
#include <string>

using namespace std;

GLuint Model::LoadTexture(const char* filename,string & directory) {
	string path = directory + "/"+ string(filename);
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	if (!image) throw LoadFileError(path);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

vector<Texture> Model::LoadMeshMaterial(aiMaterial* material, aiTextureType type, string & directory) {
	vector<Texture> textures;
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (unsigned int j = 0; j < this->all_textures.size(); j++)
		{
			if (std::strcmp(this->all_textures[j].filename.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(this->all_textures[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. 
				break;
			}
		}
		if (!skip)
		{   
		// If texture hasn't been loaded already, load it
			Texture texture;
			texture.ID = LoadTexture(str.C_Str(), directory);
			texture.filename = string(str.C_Str());
			textures.push_back(texture);
			this->all_textures.push_back(texture); 
		}
	}
	return textures;
}

void Model::LoadMeshData(Mesh & mesh, aiMesh* raw_mesh, const aiScene* scene, string & directory) {
	mesh.init(raw_mesh->mNumVertices, (raw_mesh->mNumFaces)*3); //level 0 is the original model
//	cout << "v num:" << mesh.v_num << endl;
	Vertex* vertices_ptr = mesh.GetVertexPtr();
	GLuint* indices_ptr = mesh.GetIndexPtr();
	for (unsigned int i = 0; i < raw_mesh->mNumVertices; i++) {
		vertices_ptr[i].position_x = raw_mesh->mVertices[i].x;
		vertices_ptr[i].position_y = raw_mesh->mVertices[i].y;
		vertices_ptr[i].position_z = raw_mesh->mVertices[i].z;
		vertices_ptr[i].normal_x = raw_mesh->mNormals[i].x;
		vertices_ptr[i].normal_y = raw_mesh->mNormals[i].y;
		vertices_ptr[i].normal_z = raw_mesh->mNormals[i].z;
		if (raw_mesh->mTextureCoords[0]) {
			vertices_ptr[i].texture_x = raw_mesh->mTextureCoords[0][i].x;
			vertices_ptr[i].texture_y = raw_mesh->mTextureCoords[0][i].y;
		}
		else {
			vertices_ptr[i].texture_x = 0.0f;
			vertices_ptr[i].texture_x = 0.0f;
		}
	}
	// store index for drawing order of vertices
	for (unsigned int i = 0; i < raw_mesh->mNumFaces; i++)
	{
		aiFace face = raw_mesh->mFaces[i];
		memcpy(&(indices_ptr[3*i]), face.mIndices, 3 * sizeof(GLuint));
	}
	// texture
	if (raw_mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[raw_mesh->mMaterialIndex];
		vector<Texture> diffuse_textures = this->LoadMeshMaterial(material, aiTextureType_DIFFUSE, directory);
		mesh.diffuse_texture.insert(mesh.diffuse_texture.end(), diffuse_textures.begin(), diffuse_textures.end());
		vector<Texture> specular_textures = this->LoadMeshMaterial(material, aiTextureType_SPECULAR, directory );
		mesh.specular_texture.insert(mesh.specular_texture.end(), specular_textures.begin(), specular_textures.end());
	}
	mesh.deploy(this->Model::draw_type);
}

void Model::Load(string path, unsigned int level_index) {
	MeshData & current_mesh_set = this->data[level_index];
	string directory = path.substr(0, path.find_last_of('/'));
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw LoadModelError( "ERROR::ASSIMP::"+ string(import.GetErrorString()));
	}
// load mesh
	current_mesh_set.mesh_num = scene->mNumMeshes;
	current_mesh_set.meshes = new Mesh[current_mesh_set.mesh_num];
//	cout << "mesh num:" << this->mesh_num << endl;
	for (unsigned int i = 0; i < current_mesh_set.mesh_num; i++) {
		this->LoadMeshData(current_mesh_set.meshes[i],scene->mMeshes[i], scene, directory);
	}
	import.FreeScene();
}

void StaticModel::Translate(vec3 & position, vec3 pivot, float angle) const{
	glTranslatef(position.x(), position.y(), position.z());
	if (angle != 0.0f)
		glRotatef(angle, pivot[0], pivot[1], pivot[2]);
}

void StaticModel::Render(vec3 & position, unsigned int level_index, vec3 & pivot, float angle, const LightSrc & sun, const DepthMap & depth_buffer) const{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	GLfloat matrix_buf[16];
	this->Translate(position, pivot, angle);
// select shader program
	this->shader.Use();
// move to the position in world coordicate
	glGetFloatv(GL_PROJECTION_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "projection"), 1, GL_FALSE, matrix_buf);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "view"), 1, GL_FALSE, matrix_buf);
// pass in light space matrices
	glPushMatrix();
		glLoadMatrixf(depth_buffer.light_space_view);
		this->Translate(position, pivot, angle);
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
		glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "light_space_view"), 1, GL_FALSE, matrix_buf);
	glPopMatrix();
	glUniformMatrix4fv(glGetUniformLocation(this->shader.ProgramID, "light_space_projection"), 1, GL_FALSE, depth_buffer.light_space_project);
// pass in shadow texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_buffer.depth_textureID);
	glUniform1i(glGetUniformLocation(this->shader.ProgramID, "depth_map"), 0);
// pass in light param
	glUniform3f(glGetUniformLocation(this->shader.ProgramID, "light_direction"), sun.direction[0], sun.direction[1], sun.direction[2]);
	glUniform3f(glGetUniformLocation(this->shader.ProgramID, "light_color"), sun.color[0], sun.color[1], sun.color[2]);
	// draw every mesh
	MeshData & current_mesh_set = this->data[level_index];
	for (unsigned int i = 0; i < current_mesh_set.mesh_num; i++) {
		current_mesh_set.meshes[i].render(this->shader.ProgramID);
	}
	glPopMatrix();
}

void StaticModel::RenderFrame(vec3 & position, unsigned int level_index, vec3 & pivot, float angle,const Shader & frame_shader) const {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	GLfloat matrix_buf[16];
	this->Translate(position, pivot, angle);
	glGetFloatv(GL_PROJECTION_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(frame_shader.ProgramID, "projection"), 1, GL_FALSE, matrix_buf);
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix_buf);
	glUniformMatrix4fv(glGetUniformLocation(frame_shader.ProgramID, "view"), 1, GL_FALSE, matrix_buf);
	// draw every mesh
	MeshData & current_mesh_set = this->data[level_index];
	for (unsigned int i = 0; i < current_mesh_set.mesh_num; i++) {
		current_mesh_set.meshes[i].render(this->shader.ProgramID);
	}
	glPopMatrix();
}