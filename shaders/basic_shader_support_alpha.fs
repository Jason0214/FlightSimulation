#version 330

in vec2 TexCoords;
in vec3 Normal;
in vec3 LightDirection;
in vec4 ViewPosition;
in vec4 LightSpacePosition;
out vec4 color;
flat in int level;

uniform vec3 light_color;

uniform float specular_strength;
uniform sampler2D specular_texture;
uniform sampler2D diffuse_texture;

uniform sampler2D shadow_map[3];


float CalculateShadow(vec4 LightSpacePosition){
    // change from range (-1,1) to (0,1)
    float shadow_value;
    vec3 depth_texture_coords = (LightSpacePosition.xyz / LightSpacePosition.w) * 0.5 + 0.5;
    if( 0.0f < depth_texture_coords.x && depth_texture_coords.x < 1.0f 
    && 0.0f < depth_texture_coords.y && depth_texture_coords.y < 1.0f
    && 0.0f < depth_texture_coords.z &&  depth_texture_coords.z < 1.0f){
        float closest_depth = texture(shadow_map[0], depth_texture_coords.xy).r;
        float current_depth = depth_texture_coords.z;
        float bias = max(0.05 * (1.0 - dot(Normal, LightDirection)), 0.005f); 
        shadow_value = current_depth - bias > closest_depth ? 1.0f : 0.0f;    
    }
    else{
        shadow_value = 0.0f;
    }
    return shadow_value;
}


void main(){   
// alpha
    float alpha = texture(diffuse_texture, TexCoords).a;

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

// Shadow mapping  
    float shadow = CalculateShadow(LightSpacePosition);


// get color which is the combination of ambient, diffuse and specular
    color = vec4(ambient + (1.0f - shadow)*(diffuse + specular), alpha);

// alpha check to get correct depth
    if(color.a > 0.5f){
        gl_FragDepth = gl_FragCoord.z;
    }
    else{
        gl_FragDepth = 1.0f;
    }

    if(level != 0){
        gl_FragDepth = 1.0f;
    }
    else{
        gl_FragDepth = gl_FragCoord.z;
    }
}