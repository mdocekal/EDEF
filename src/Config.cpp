/**
 * Project: EDEF
 * @file Config.cpp
 * @date 11. 4. 2018
 * @author xdocek09
 * @brief Source file of Simple configuration file loader.
 */

#include "Config.h"
#include <sstream>

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
}


/*** End of file: Config.cpp ***/
