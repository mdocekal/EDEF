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
#include <vector>

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
	 * Creates image from data.
	 * Only one byte pet pixel
	 *
	 * @param[in] w
	 * 	Width
	 * @param[in] h
	 * 	Height
	 * @param[in] p
	 * 	Image pixels.
	 */
	Image(unsigned w, unsigned h, const std::vector<uint8_t>& p);

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

	/**
	 * Saves image into file.
	 *
	 * @param[in] path
	 * 	Path to file.
	 * @throw std::runtime_error	When image could not be saved.
	 */
	void save(const std::string& path);

	/**
	 * Fills image with data.
	 * Only one byte pet pixel
	 *
	 * @param[in] w
	 * 	Width
	 * @param[in] h
	 * 	Height
	 * @param[in] p
	 * 	Image pixels.
	 */
	void setData(unsigned w, unsigned h, const std::vector<uint8_t>& p);


	int getBytesPerPixel() const {
		return bytesPerPixel;
	}

	unsigned getHeight() const {
		return height;
	}

	const std::vector<uint8_t>& getPixels() const {
		return pixels;
	}

	unsigned getWidth() const {
		return width;
	}

	const std::string& getPath() const {
		return path;
	}

private:
	int width=0;
	int height=0;
	int bytesPerPixel=0;
	std::vector<uint8_t> pixels;
	std::string path;
	/**
	 * Free pixels.
	 */
	void free();
};

#endif /* SRC_IMAGE_H_ */

/*** End of file: Image.h ***/
