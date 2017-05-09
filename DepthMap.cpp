#include "DepthMap.h"


DepthMap::~DepthMap(){
	glDeleteTextures(1, &(this->depth_textureID));
	glDeleteBuffers(1,&(this->FBO));
}

void DepthMap::init(){
// gen texture ID

    glGenTextures(1, &(this->depth_textureID));
    glBindTexture(GL_TEXTURE_2D, this->depth_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->map_width, this->map_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// gen FBO
	std::cout << glGenFramebuffers;
	glGenFramebuffers(1, &(this->FBO));
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_textureID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}