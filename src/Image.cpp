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
#define STBI_NO_GIF
#include "stb_image.h"

Image::Image(const std::string& path) :path(path) {
	load(path);
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
