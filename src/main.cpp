/**
 * Project: EDEF
 * @file main.cpp
 * @date 10. 4. 2018
 * @author xdocek09
 * @brief Entry point of EDEF (Evolutionary designer of edge filter).
 */
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <set>

#include "Config.h"
#include "Image.h"
#include "CGP.h"

/**
 * Class that manages input arguments and program help.
 */
class Args {
public:
	static const char PROGRAM_NAME[];

	static const std::set<std::string> keyWords;

	enum Action {
		NOPE, //! No action requested.
		TRAIN, //! Find filter on training set.
		REPAIR, //! Repair filter.
		DAMAGE, //! Damage filter.
		TEST,	//! Test filter on testing set.
		STATUS, //! Get filter status.
		USE, //! Use filter on image.
		HELP	//! Show help.
	};

	/**
	 * Constructs Args.
	 *
	 * @param[in] argc	Number of arguments.
	 * @param[in] argv	The arguments.
	 * @throw std::invalid_argument When arguments are invalid.
	 */
	Args(int argc, char** argv){
		//arguments parsing
		for(int i=1; i<argc; i++){
			std::string actArg(argv[i]);
			if(actArg=="-train"){

				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=TRAIN;
			}else if(actArg=="-repair"){
				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=REPAIR;
			}else if(actArg=="-damage"){
				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=DAMAGE;
			}else if(actArg=="-test"){
				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=TEST;
			}else if(actArg=="-status"){
				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=STATUS;
			}else if(actArg=="-status"){
				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=USE;
			}else if(actArg=="-set"){
				if(++i>=argc) throw std::invalid_argument("No value for set.");

				//read paths to images
				while(i<argc && keyWords.find(argv[i])==keyWords.end()){
					set.push_back(argv[i++]);
				}
				if(keyWords.find(argv[i])!=keyWords.end()){
					--i;
				}
			}else if(actArg=="-setOut"){
				if(++i>=argc) throw std::invalid_argument("No value for setOut.");
				//read paths to images
				while (i < argc && keyWords.find(argv[i]) == keyWords.end()) {
					setOut.push_back(argv[i++]);
				}
				if (keyWords.find(argv[i]) != keyWords.end()) {
					--i;
				}
			}else if(actArg=="-chromosome"){
				if(++i>=argc){
					throw std::invalid_argument("Must specify chromosome file that can be open for reading.");
				}
				chromosome.open(argv[i], std::ios::binary);
				if(!chromosome){
					throw std::invalid_argument("Must specify chromosome file that can be open for reading.");
				}

			}else if(actArg=="-on"){
				if(++i>=argc){
					throw std::invalid_argument("Must specify image file that can be open for reading.");
				}
				on.open(argv[i], std::ios::binary);
				if(!on){
					throw std::invalid_argument("Must specify image file that can be open for reading.");
				}

			}else if(actArg=="-out"){
				if(++i>=argc){
					throw std::invalid_argument("Must specify file that can be open for writting.");
				}
				out.open(argv[i], std::ios::binary);
				if(!out){
					throw std::invalid_argument("Must specify file that can be open for writting.");
				}
			}else if(actArg=="-config"){
				if(++i>=argc){
					throw std::invalid_argument("Must specify chromosome file that can be open for reading.");
				}

				std::ifstream f(argv[i]);
				if(!f){
					throw std::invalid_argument("Must specify chromosome file that can be open for reading.");
				}
				config.read(f);

			}else if (actArg=="-h"){
				//show help
				if(action!=Action::NOPE)
					throw std::invalid_argument("Invalid combination of arguments.");
				action=Action::HELP;
			}else{
				throw std::invalid_argument("Invalid argument: "+actArg);
			}
		}

		//actions control

		switch (action) {
			case TRAIN:
				if(set.size()==0 || setOut.size()==0 || config.empty() || !out)
					throw std::invalid_argument("-train needs: -set, -setOut, -config, -out.");

				break;
			case REPAIR:
				if(set.size()==0 || setOut.size()==0 || !chromosome || config.empty() || !out)
					throw std::invalid_argument("-repair needs: -set, -setOut, -chromosome, -config, -out.");
				break;
			case DAMAGE:
				if(!chromosome || !out)
					throw std::invalid_argument("-damage needs: -chromosome, -out.");
				break;
			case TEST:
				if(set.size()==0 || setOut.size()==0 || !chromosome)
					throw std::invalid_argument("-test needs: -set, -setOut, -chromosome.");
				break;
			case STATUS:
				if(!chromosome)
					throw std::invalid_argument("-status needs: -chromosome.");
				break;
			case USE:
				if(!chromosome || !on || !out)
					throw std::invalid_argument("-use needs: -chromosome, -on, -out.");
				break;
			case NOPE:
				throw std::invalid_argument("No action.");
				break;
			default:
				break;
		}
	}

	/**
	 * Prints program help to stdout.
	 */
	static void showHelp(){
		std::cout << "Thank you for using "<< PROGRAM_NAME <<". "
				<< "This program was developed at FIT BUT as project to course: Bio-Inspired Computers. \n\n"
				<< "\t-train" <<"\n\t\tYou want to develop new edge detection filter on given training data set.\n"
				<<"\t\tPROVIDE: -set, -setOut, -config, -out\n"
				<< "\t-repair" <<"\n\t\tIt will try to find new implementation of filter with given resources (damaged before). \n"
				<<"\t\tPROVIDE: -set, -setOut, -chromosome, -config -out\n"
				<< "\t-damage" <<"\n\t\tWill damage the existing filter. Randomly selects block and changes it to block with 0 output.\n"
				<<"\t\tPROVIDE: chromosome -out\n"
				<< "\t-test" <<"\n\t\tYou want to test your developed filter on given testing data set.\n"
				<<"\t\tPROVIDE: -set, -setOut, -chromosome\n"
				<< "\t-status" <<"\n\t\tChecks filter state (how much it is damaged).\n"
				<<"\t\tPROVIDE: -chromosome\n"
				<< "\t-use" <<"\n\t\tUse filter on given image.\n"
				<<"\t\tPROVIDE: -chromosome -on -out\n"


				<< "\t-set" <<"\n\t\tPaths to images for train/test set (filled with jpg images).\n"
				<< "\t-setOut" <<"\n\t\tPaths to images for required filter output. Image must be on same position as coresponding image from -set.\n"

				<< "\t-on" <<"\n\t\tPath to image (jpg).\n"
				<< "\t-out" <<"\n\t\tPath to file for result.\n"

				<< "\t-chromosome" <<"\n\t\tPath to saved chromosome.\n"
				<< "\t-config" <<"\n\t\tPath to configuration file.\n"

				<< "\t-h" <<"\n\t\tWrites help to stdout and exists the program."
				<< std::endl;
	}


	/**
	 * Parsed action from arguments.
	 *
	 * @return	Action user wants to perform.
	 */
	Action getAction() const {
		return action;
	}


	Config& getConfig() {
		return config;
	}

	std::ifstream& getChromosome() {
		return chromosome;
	}

	std::ifstream& getOn() {
		return on;
	}

	std::ofstream& getOut() {
		return out;
	}

	const std::vector<std::string>& getSet() const {
		return set;
	}

	const std::vector<std::string>& getSetOut() const {
		return setOut;
	}

private:

	std::vector<std::string> set; //! Paths to images for data set.
	std::vector<std::string> setOut; //! Paths to images for data set ouput.
	std::ifstream on; //! Image.
	std::ofstream out; //! Result.
	std::ifstream chromosome; //! File containing chromosome.
	Config config; //!Loaded configuration.

	Action action=Action::NOPE; //! Action user wants to perform.

};
const std::set<std::string> Args::keyWords={
			"-train",
			"-repair",
			"-damage",
			"-test",
			"-status",
			"-set",
			"-setOut",
			"-chromosome",
			"-on",
			"-out",
			"-config",
			"-h"
};
const char Args::PROGRAM_NAME[]="EDEF";

int main(int argc, char* argv[]){
	try {
		Args myArgs(argc, argv);

		Image img("data/test.jpg");

		std::cout << img.getWidth() << ", " << img.getHeight() << ", "  << img.getBytesPerPixel() <<std::endl;
		for(unsigned y=0; y<img.getHeight(); ++y){
			for(unsigned x=0; x<img.getWidth(); ++x){
				std::cout << static_cast<unsigned>(img.getPixels()[y*img.getWidth()+x]) << " ";
			}
			std::cout << std::endl;
		}

		switch (myArgs.getAction()) {
		case Args::Action::HELP:
			//User needs our help!
			Args::showHelp();
			break;
		case Args::Action::TRAIN:
			{
				//train new chromosome
				Config& config=myArgs.getConfig();
				for(auto KV : myArgs.getConfig()){
					std::cout << KV.first << " = " << KV.second <<std::endl;
				}


				const std::set<std::string> confKeys={
						"COLS", "ROWS", "L_BACK",
						"POPULATION_SIZE", "MAX_MUTATIONS", "GENERATIONS", "RUNS"};

				for(auto k : confKeys){
					if(config.find(k)==config.end()){
						std::cerr << "Invalid configuration file.\n";
						std::cerr << "\tNo key: "<< k<< std::endl;
						return 3;
					}
				}

				unsigned cols;
				unsigned rows;
				unsigned lBack;
				unsigned populationSize;
				unsigned maxMutations;
				unsigned generations;
				unsigned runs;

				std::stringstream sConv;
				sConv << config["COLS"];
				sConv >> cols;
				sConv.clear();

				sConv << config["ROWS"];
				sConv >> rows;
				sConv.clear();

				sConv << config["L_BACK"];
				sConv >> lBack;
				sConv.clear();

				sConv << config["POPULATION_SIZE"];
				sConv >> populationSize;
				sConv.clear();

				sConv << config["MAX_MUTATIONS"];
				sConv >> maxMutations;
				sConv.clear();

				sConv << config["GENERATIONS"];
				sConv >> generations;
				sConv.clear();

				sConv << config["RUNS"];
				sConv >> runs;
				sConv.clear();

				std::cout << runs <<std::endl;

				CGP cgp(cols, rows, lBack);
				cgp.setPopulationSize(populationSize);
				cgp.setMutationMax(maxMutations);
				cgp.setGenerations(generations);

				//evolve chromosome

				//load images
				std::cout << "Load train set." << std::endl;
				std::vector<Image> train;
				for(auto iPath: myArgs.getSet()){
					train.push_back(std::move(Image(iPath)));
				}
				std::cout << "\tLOADED" << std::endl;

				std::cout << "Load train setOut." << std::endl;
				std::vector<Image> trainOut;
				for (auto iPath : myArgs.getSetOut()) {
					trainOut.push_back(std::move(Image(iPath)));
				}
				std::cout << "\tLOADED" << std::endl;

				std::cout << "Start " << runs << " evolution runs." << std::endl;
				//perform evolution
				Chromosome c=cgp.evolve(runs, train, trainOut);

				std::cout << "Saving chromosome." << std::endl;
				//save chromosome
				for(auto g : c){
					myArgs.getOut()<< g;
				}
				std::cout << "\tSAVED" << std::endl;
			}
			break;
		case Args::Action::REPAIR:

			break;
		case Args::Action::DAMAGE:

			break;
		case Args::Action::TEST:

			break;
		default:
			std::cerr << "Unexpected error: \n";
			std::cerr << "\t Something is rotten in the state of this program!"
					<< std::endl;
			return 2;
			break;
		}

	} catch (const std::invalid_argument& e) {
		std::cerr << "Arguments error: \n";
		std::cerr << "\t" << e.what() << "\n\n";

		Args::showHelp();

		return 1;
	}

}
/*** End of file: main.cpp ***/
