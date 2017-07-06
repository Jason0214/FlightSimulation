#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 LightDirection;
out vec4 ViewPosition;
out vec4 LightSpacePosition;
flat out int level;

uniform vec3 light_direction;

uniform mat4 projection[3];
uniform mat4 view;

uniform mat4 light_space_projection[3];
uniform mat4 light_space_view[3];
uniform float z_clip[4];

void main()
{
// select projection matrix according to z value
    ViewPosition = view * vec4(position, 1.0f);
	if(ViewPosition.z/ViewPosition.w < z_clip[1]){
		gl_Position = projection[0] * ViewPosition;
		level = 0;
	}
	else if(ViewPosition.z/ViewPosition.w < z_clip[2]){
		gl_Position = projection[1] * ViewPosition;
		level = 1;
	}
	else{
		gl_Position = projection[2] * ViewPosition;
		level = 2;
	}

// translate normal vector
	mat4 mat_view = transpose(inverse(view));
    Normal = normalize(vec3(mat_view * vec4(normal,0.0f)));
    LightDirection = vec3(mat_view * vec4(light_direction,0.0f));

//  texture coordinates
    TexCoords = texCoords;

// 	choose shadow level
	LightSpacePosition = light_space_projection[0] * light_space_view[0] * vec4(position,1.0);
}