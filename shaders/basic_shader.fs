#version 330

uniform vec3 light_color;

uniform float specular_strength;
uniform sampler2D specular_texture;
uniform sampler2D diffuse_texture;

in vec2 TexCoords;
in vec3 Normal;

in vec3 LightDirection;
in vec4 ViewPosition;

out vec4 color;



void main(){

// Ambient
    vec3 ambient = 0.15f * light_color * texture(diffuse_texture, TexCoords);

// Diffuse 
    vec3 diffuse = max(dot(Normal, LightDirection), 0.0f) * light_color * texture(diffuse_texture, TexCoords);

// Specular
    vec3 specular;
    if(specular_strength != 0.0f){
        vec3 view_dir = -normalize(vec3(ViewPosition));
        vec3 reflect_dir = normalize(reflect(-LightDirection, Normal));
        specular = specular_strength * pow(max(dot(view_dir, reflect_dir), 0.0f), 32) * light_color * texture(specular_texture, TexCoords);
    }
    else{
        specular = vec3(0.0f);
    }

// get color which is the combination of ambient, diffuse and specular
    color = vec4(ambient + diffuse + specular ,1.0f);
}