#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

const int CASCADE_NUM = 3;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// light related
uniform vec3 light_direction;

// shadow related
uniform mat4 light_space_projection[CASCADE_NUM];
uniform mat4 light_space_view;

// basic parameters
out vec2 TexCoords;
out vec3 Normal;

// light related
out vec3 LightDirection;
out vec4 ViewPosition;

// shadow related
out vec4 LightSpacePosition[CASCADE_NUM];
out float z_clip_value;


void main()
{
// select projection matrix according to z value
    ViewPosition = view * model * vec4(position, 1.0f);
	gl_Position = projection * ViewPosition;

	z_clip_value = -ViewPosition.z / ViewPosition.w;

// translate normal vector
    Normal = normalize(vec3(transpose(inverse(view * model)) * vec4(normal, 0.0f)));
    LightDirection = normalize(vec3(view * vec4(light_direction, 0.0f)));

//  texture coordinates
    TexCoords = texCoords;

// 	get position in shadow ortho
	for(int i = 0; i < CASCADE_NUM; i++){
		LightSpacePosition[i] = light_space_projection[i] * light_space_view 
								* model * vec4(position,1.0);	
	}
}