#version 330


in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;

void main()
{
    float depth_value = texture(screenTexture, TexCoords).r;
    color = vec4(vec3(depth_value),1.0);
}