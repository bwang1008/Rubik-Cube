#ifndef TEXTURE_TO_RENDER_H
#define TEXTURE_TO_RENDER_H
#include <string>
#include <fstream>

class TextureToRender {
public:
	TextureToRender();
	TextureToRender(std::ifstream &in);
	~TextureToRender();
	void create(int width, int height);
	void create2(int width, int height);
	void bind();
	void unbind();
	std::string to_string();
	int getTexture() const { return tex_; }
private:
	int w_, h_;
	unsigned int fb_ = -1;
	unsigned int tex_ = -1;
	unsigned int dep_ = -1;
};

#endif
