#version 330
#extension GL_ARB_explicit_uniform_location : require
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vec4 pos = projection * view * vec4(position, 1.0);
    gl_Position = pos.xyww;
    TexCoords = position;
}