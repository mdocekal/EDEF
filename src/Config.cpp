/**
 * Project: EDEF
 * @file Config.cpp
 * @date 11. 4. 2018
 * @author xdocek09
 * @brief Source file of Simple configuration file loader.
 */

#include "Config.h"
#include <sstream>
#include <set>

void Config::read(std::istream& input){

	std::string line;
	while( std::getline(input, line) ){
	  std::istringstream sLine(line);
	  std::string key;
	  if( std::getline(sLine, key, '=') ){
	    std::string value;
	    if(std::getline(sLine, value)){
	    	data[key]=value;
	    }

	  }
	}

	static const std::set<std::string> confKeys={
							"COLS", "ROWS", "L_BACK",
							"POPULATION_SIZE", "MAX_MUTATIONS", "GENERATIONS", "RUNS"};

	for(auto k : confKeys){
		if(this->find(k)==this->end()){
			std::string msg("Invalid configuration file.\n\tNo key: ");
			msg+=k;
			throw std::runtime_error(msg);
		}
	}

	std::stringstream sConv;
	sConv << data["COLS"];
	sConv >> cols;
	sConv.clear();

	sConv << data["ROWS"];
	sConv >> rows;
	sConv.clear();

	sConv << data["L_BACK"];
	sConv >> lBack;
	sConv.clear();

	sConv << data["POPULATION_SIZE"];
	sConv >> populationSize;
	sConv.clear();

	sConv << data["MAX_MUTATIONS"];
	sConv >> maxMutations;
	sConv.clear();

	sConv << data["GENERATIONS"];
	sConv >> generations;
	sConv.clear();

	sConv << data["RUNS"];
	sConv >> runs;
	sConv.clear();
}


/*** End of file: Config.cpp ***/
