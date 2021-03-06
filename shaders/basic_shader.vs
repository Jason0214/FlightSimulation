#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// light related
uniform vec3 light_direction;

// basic parameters
out vec2 TexCoords;
out vec3 Normal;

// light related
out vec3 LightDirection;
out vec4 ViewPosition;


void main()
{
// select projection matrix according to z value
    ViewPosition = view * model * vec4(position, 1.0f);
	gl_Position = projection * ViewPosition;

// translate normal vector
    Normal = normalize(vec3(transpose(inverse(view * model)) * vec4(normal, 0.0f)));
    LightDirection = normalize(vec3(view * vec4(light_direction, 0.0f)));

//  texture coordinates
    TexCoords = texCoords;
}