/**
 * Project: EDEF
 * @file Image.h
 * @date 11. 4. 2018
 * @author xdocek09
 * @brief Header file for class that represents Image.
 */

#ifndef SRC_IMAGE_H_
#define SRC_IMAGE_H_

#include <stdint.h>
#include <string>

/**
 * Representation of image.
 * JPG image loading from file.
 * Image saving to file.
 */
class Image {
public:
	/**
	 * Creates image.
	 */
	Image(){};

	/**
	 * Creates image from file.
	 *
	 * @param[in] path	to file containing jpg image.
	 * @throw std::runtime_error	When image is invalid.
	 */
	Image(const std::string& path);

	/**
	 * Loads image from file.
	 * Old is forgotten.
	 *
	 * @param[in] path	to file containing jpg image.
	 * @throw std::runtime_error	When image is invalid.
	 */
	void load(const std::string& path);


	~Image();

	int getBytesPerPixel() const {
		return bytesPerPixel;
	}

	unsigned getHeight() const {
		return height;
	}

	uint8_t* getPixels() const {
		return pixels;
	}

	unsigned getWidth() const {
		return width;
	}

private:
	int width=0;
	int height=0;
	int bytesPerPixel=0;
	uint8_t* pixels=nullptr;

	/**
	 * Free pixels.
	 */
	void free();
};

#endif /* SRC_IMAGE_H_ */

/*** End of file: Image.h ***/
