#version 330
#extension GL_ARB_explicit_uniform_location : require

in vec2 TexCoords;
uniform sampler2D diffuse_texture;

void main()
{
//	if(texture(diffuse_texture, TexCoords).a == 1.0f){
//    	gl_FragDepth = gl_FragCoord.z;
//   }
//    else{
//    	gl_FragDepth = 1.0f;
//    }
}