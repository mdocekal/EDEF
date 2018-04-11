/**
 * Project: EDEF
 * @file Config.h
 * @date 11. 4. 2018
 * @author xdocek09
 * @brief Header file of Simple configuration file loader.
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include <map>
#include <iostream>

/**
 * Loads and stores configuration from config file.
 * Format of file is key, value pair separated with =.
 * Example:
 * 	NAME = Chosse
 */
class Config  {
public:

	/**
	 * Creates empty Config.
	 */
	Config(){};

	/**
	 * Creates Config from given stream.
	 *
	 * @param[in] input	With configuration.
	 */
	Config(std::istream& input){
		read(input);
	};

	/**
	 * Reads configuration from input stream.
	 * Old configuration is forgotten.
	 *
	 * @param[in] input	input	With configuration.
	 */
	void read(std::istream& input);

	/**
	 * Get value for given key.
	 *
	 * @param[in] key	The key.
	 * @return	Reference to value for given key.
	 */
	std::string& operator[] (const std::string& key) {
	    return data[key];
	}

	/**
	 * Get iterator to beginning.
	 *
	 * @return	Begin iterator
	 */
	std::map<std::string, std::string>::iterator begin(){
		return data.begin();
	}

	/**
	 * Get const iterator to beginning.
	 *
	 * @return	Begin const iterator
	 */
	std::map<std::string, std::string>::const_iterator begin() const {
		return data.cbegin();
	}

	/**
	 * Get const iterator to beginning.
	 *
	 * @return	Begin const iterator
	 */
	std::map<std::string, std::string>::const_iterator cbegin() const {
		return data.cbegin();
	}

	/**
	 * Get iterator to past the end element.
	 *
	 * @return	End iterator
	 */
	std::map<std::string, std::string>::iterator end() {
		return data.end();
	}

	/**
	 * Get const iterator to past the end element.
	 *
	 * @return	End const iterator
	 */
	std::map<std::string, std::string>::const_iterator end() const {
		return data.cend();
	}

	/**
	 * Get const iterator to past the end element.
	 *
	 * @return	End const iterator
	 */
	std::map<std::string, std::string>::const_iterator cend() const {
		return data.cend();
	}

	/**
	 * Check if configuration is empty.
	 *
	 * @return	True empty. False otherwise.
	 */
	bool empty(){
		return data.empty();
	}


private:
	std::map<std::string, std::string> data; //! Loaded configuration
};

#endif /* SRC_CONFIG_H_ */

/*** End of file: Config.h ***/
