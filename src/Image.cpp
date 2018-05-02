/**
 * Project: EDEF
 * @file Image.cpp
 * @date 11. 4. 2018
 * @author xdocek09
 * @brief 
 */

/*** End of file: Image.cpp ***/
#include "Image.h"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_NO_GIF
#include "stb_image.h"
#include "stb_image_write.h"

Image::Image(const std::string& path) :path(path) {
	load(path);
}

Image::Image(unsigned w, unsigned h, const std::vector<uint8_t>& p){
	setData(w,h, p);
}

void Image::setData(unsigned w, unsigned h, const std::vector<uint8_t>& p){
	width=w;
	height=h;
	bytesPerPixel=1;
	pixels.insert(pixels.end(), &p[0], &p[width*height]);
}

void Image::save(const std::string& path){
	if(stbi_write_jpg(&path[0], width, height, bytesPerPixel, &pixels[0], 100)==0){
		std::string msg("Could not save image: ");
		msg+=path;
		throw std::runtime_error(msg);
	}
}

void Image::load(const std::string& path){
	free();
	uint8_t* tmpPixels=stbi_load(path.c_str(), &width, &height, &bytesPerPixel, 0);

	std::string msg("Invalid image: ");
	msg+=path;
	if(tmpPixels == NULL) throw std::runtime_error(msg);

	pixels.insert(pixels.end(), &tmpPixels[0], &tmpPixels[width*height]);
	stbi_image_free(tmpPixels);
}

void Image::free(){
	pixels.clear();
}
