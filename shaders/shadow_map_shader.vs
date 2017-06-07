#version 330

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;

out vec2 TexCoords;

void main()
{
	gl_Position = projection * view * vec4(position, 1.0f);
    TexCoords = texCoords;
}