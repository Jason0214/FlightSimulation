#version 330
#extension GL_ARB_explicit_uniform_location : require
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 LightDirection;
out vec4 FragPosition;
out vec4 LightSpaceFragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 light_space_projection;
uniform mat4 light_space_view;

uniform vec3 light_direction;

void main()
{
// vertix position
    FragPosition = view * vec4(position, 1.0f);
    gl_Position = projection * FragPosition;

// translate normal vector
	mat4 mat_view = transpose(inverse(view));
    Normal = normalize(vec3(mat_view * vec4(normal,0.0f)));
    LightDirection = vec3(mat_view * vec4(light_direction,0.0f));

//  texture coordinates
    TexCoords = texCoords; 
    
// light space position, for shadow mapping
	LightSpaceFragPos = light_space_projection * light_space_view * vec4(position,1.0);
}