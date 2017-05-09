#version 330
#extension GL_ARB_explicit_uniform_location : require
in vec2 TexCoords;
in vec3 Normal;
in vec3 LightDirection;
in vec4 FragPosition;
out vec4 color;

uniform sampler2D depth_buffer;

uniform float specular_strength;
uniform sampler2D specular_texture;
uniform sampler2D diffuse_texture;

uniform vec3 light_color;

void main(){   
// Ambient
    vec3 ambient = 0.1f * light_color;

// Diffuse 
    vec3 diffuse = max(dot(Normal, LightDirection), 0.0f) * light_color;

// get color from ambient and diffuse components
	color = vec4((ambient + diffuse),1.0f) * vec4(texture(diffuse_texture, TexCoords));

// Specular
	if(specular_strength != 0.0f){
		vec3 view_dir = -normalize(vec3(FragPosition));
		vec3 reflect_dir = normalize(reflect(-LightDirection, Normal));
		vec3 specular = specular_strength * pow(max(dot(view_dir, reflect_dir), 0.0f), 32) * light_color;
		color += vec4(specular,1.0f) * vec4(texture(specular_texture, TexCoords));
	}

// alpha check to get correct depth
   	if(color.a == 1.0f){
    	gl_FragDepth = gl_FragCoord.z;
    }
    else{
    	gl_FragDepth = 1.0f;
    }
}