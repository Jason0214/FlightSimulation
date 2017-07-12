#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 LightDirection;
out vec4 ViewPosition;
out vec4 LightSpacePosition;
out flat int light_space_level;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float shadow_clip[2];
uniform mat4 light_space_projection[2];
uniform mat4 light_space_view;

uniform vec3 light_direction;

void main()
{
// vertix position
    ViewPosition = view * model * vec4(position, 1.0f);
    gl_Position = projection * ViewPosition;

	if(-ViewPosition.z/ViewPosition.w < shadow_clip[1]){
		light_space_level = 0;
	}
	else{
		light_space_level = 1;
	}

// translate normal vector
	mat4 mat_view = transpose(inverse(view));
    Normal = normalize(vec3(mat_view * vec4(normal,0.0f)));
    LightDirection = vec3(mat_view * vec4(light_direction,0.0f));

//  texture coordinates
    TexCoords = texCoords; 

// light space position, for shadow mapping
	LightSpacePosition = light_space_projection[light_space_level] * light_space_view * vec4(position,1.0);
}