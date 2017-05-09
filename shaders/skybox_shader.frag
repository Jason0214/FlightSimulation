#version 330
#extension GL_ARB_explicit_uniform_location : require
in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{
    color = texture(skybox, TexCoords);
}