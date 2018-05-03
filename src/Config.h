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
	 * @throw std::runtime_error when config is invalid
	 */
	Config(std::istream& input){
		read(input);
	};

	/**
	 * Reads configuration from input stream.
	 * Old configuration is forgotten.
	 *
	 * @param[in] input	input	With configuration.
	 * @throw std::runtime_error when config is invalid
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
	 * Searches the container for an element with a key equivalent to k and returns an iterator to it if found,
	 * otherwise it returns an iterator to map::end.
	 *
	 * @return	An iterator to the element, if an element with specified key is found, or map::end otherwise.
	 */
	std::map<std::string, std::string>::iterator find(const std::string k) {
		return data.find(k);
	}
	/**
	 * Searches the container for an element with a key equivalent to k and returns an iterator to it if found,
	 * otherwise it returns an iterator to map::end.
	 *
	 * @return	An iterator to the element, if an element with specified key is found, or map::end otherwise.
	 */
	std::map<std::string, std::string>::const_iterator find(const std::string k) const {
		return data.find(k);
	}

	/**
	 * Check if configuration is empty.
	 *
	 * @return	True empty. False otherwise.
	 */
	bool empty(){
		return data.empty();
	}

	uint32_t getCols() const {
		return cols;
	}

	unsigned getGenerations() const {
		return generations;
	}

	unsigned getlBack() const {
		return lBack;
	}

	unsigned getMaxMutations() const {
		return maxMutations;
	}

	unsigned getPopulationSize() const {
		return populationSize;
	}

	uint32_t getRows() const {
		return rows;
	}

	unsigned getRuns() const {
		return runs;
	}

	void setCols(uint32_t cols) {
		this->cols = cols;
	}

	void setGenerations(unsigned generations) {
		this->generations = generations;
	}

	void setlBack(unsigned lBack) {
		this->lBack = lBack;
	}

	void setMaxMutations(unsigned maxMutations) {
		this->maxMutations = maxMutations;
	}

	void setPopulationSize(unsigned populationSize) {
		this->populationSize = populationSize;
	}

	void setRows(uint32_t rows) {
		this->rows = rows;
	}

	void setRuns(unsigned runs) {
		this->runs = runs;
	}

private:
	std::map<std::string, std::string> data; //! Loaded configuration

	uint32_t cols;
	uint32_t rows;
	unsigned lBack;
	unsigned populationSize;
	unsigned maxMutations;
	unsigned generations;
	unsigned runs;
};

#endif /* SRC_CONFIG_H_ */

/*** End of file: Config.h ***/
