#include <GL/glew.h>
#include <debuggl.h>
#include <iostream>
#include <sstream>
#include "texture_to_render.h"

TextureToRender::TextureToRender()
{
	glGenFramebuffers(1,&fb_);
	glGenTextures(1,&tex_);
	glGenRenderbuffers(1,&dep_);
}

TextureToRender::~TextureToRender()
{
	if (fb_ < 0)
		return ;
	unbind();
	glDeleteFramebuffers(1, &fb_);
	glDeleteTextures(1, &tex_);
	glDeleteRenderbuffers(1, &dep_);
}

TextureToRender::TextureToRender(std::ifstream &in){
	glGenFramebuffers(1,&fb_);
	glGenTextures(1,&tex_);
	glGenRenderbuffers(1,&dep_);
	int w,h;
	in >> w >> h;
	in >> std::noskipws;
	char c;
	in >> c;
	char *s = new char[4*w*h];
	for(int i = 0;i < 4*w*h; ++i)in >> s[i];
	create(w,h);
	bind();
	CHECK_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	//CHECK_GL_ERROR(glDrawPixels(w,h,GL_RGB,GL_BYTE, s));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, tex_));
	CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s));
	unbind();
	in >> std::skipws;
	delete s;
}

std::string TextureToRender::to_string(){
	std::ostringstream res;	
	int w = w_, h = h_;
	res << w << ' ' << h << '\n';
	char *s = new char[4*w*h];
	bind();
	CHECK_GL_ERROR(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA,GL_UNSIGNED_BYTE,s));
	std::string t(s,4*w*h);
	res << t << '\n';
	unbind();
	delete s; // TODO fix leak
	return res.str();	
}

void TextureToRender::create(int width, int height)
{
	w_ = width;
	h_ = height;

	//bind();
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fb_));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, tex_));
	CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	//CHECK_GL_ERROR(glTexImage2DMultisample(GL_TEXTURE_2D, 4, GL_RGBA, width, height, GL_TRUE));

	// dunno what this does
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	//CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// depth
	CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, dep_));
	CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
	CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dep_));

	// config framebuffer
	
	CHECK_GL_ERROR(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_, 0));
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	CHECK_GL_ERROR(glDrawBuffers(1, DrawBuffers));

	// FIXME: Create the framebuffer object backed by a texture

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Failed to create framebuffer object as render target" << std::endl;
	} else {
		std::cerr << "Framebuffer ready" << std::endl;
	}
	unbind();
}

void TextureToRender::create2(int width, int height)
{
	w_ = width;
	h_ = height;

	//bind();
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fb_));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, tex_));
	CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
	//CHECK_GL_ERROR(glTexImage2DMultisample(GL_TEXTURE_2D, 4, GL_RGBA, width, height, GL_TRUE));

	// dunno what this does
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	//CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	//CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// depth
	CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, dep_));
	CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
	CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dep_));

	// config framebuffer
	
	CHECK_GL_ERROR(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_, 0));
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	CHECK_GL_ERROR(glDrawBuffers(1, DrawBuffers));

	// FIXME: Create the framebuffer object backed by a texture

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Failed to create framebuffer object as render target" << std::endl;
	} else {
		std::cerr << "Framebuffer ready" << std::endl;
	}
	unbind();
}

void TextureToRender::bind()
{
	// FIXME: Unbind the framebuffer object to GL_FRAMEBUFFER
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fb_));
	CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, dep_));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, tex_));
	//glClear(GL_DEPTH_BUFFER_BIT);
	CHECK_GL_ERROR(glViewport(0,0,w_,h_));
}

void TextureToRender::unbind()
{
	// FIXME: Unbind current framebuffer object from the render target
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, 0));
}
