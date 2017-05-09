#version 330
#extension GL_ARB_explicit_uniform_location : require
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 LightDirection;
out vec4 FragPosition;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 light_direction;

void main()
{
// vertix position
    FragPosition = view * vec4(position, 1.0f);
    gl_Position = projection * FragPosition;

// translate normal vector
	mat4 vector_view = transpose(inverse(view));
    Normal = normalize(vec3(vector_view * vec4(normal,0.0f)));
    LightDirection = vec3(vector_view * vec4(light_direction,0.0f));

//  texture coordinates
    TexCoords = texCoords; 
}