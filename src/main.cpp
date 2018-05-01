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
#include <vector>

#include "Config.h"
#include "Image.h"

/**
 * Class that manages input arguments and program help.
 */
class Args {
public:
	static const char PROGRAM_NAME[];

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
				set=argv[i];
			}else if(actArg=="-setOut"){
				if(++i>=argc) throw std::invalid_argument("No value for setOut.");
				setOut=argv[i];
			}else if(actArg=="-chromosome"){
				if(++i>=argc || ! (chromosome= fopen(argv[i], "rb"))){
					throw std::invalid_argument("Must specify chromosome file that can be open for reading.");
				}
				shouldClose.push_back(chromosome);
			}else if(actArg=="-on"){
				if(++i>=argc || ! (on= fopen(argv[i], "rb"))){
					throw std::invalid_argument("Must specify image file that can be open for reading.");
				}
				shouldClose.push_back(on);
			}else if(actArg=="-config"){
				if(++i>=argc || ! (chromosome= fopen(argv[i], "rb"))){
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
				if(set.length()==0 || setOut.length()==0 || config.empty())
					throw std::invalid_argument("-train needs: -set, -setOut, -config.");

				break;
			case REPAIR:
				if(set.length()==0 || setOut.length()==0 || chromosome==nullptr || config.empty())
					throw std::invalid_argument("-repair needs: -set, -setOut, -chromosome, -config.");
				break;
			case DAMAGE:
				if(chromosome==nullptr)
					throw std::invalid_argument("-damage needs: -chromosome.");
				break;
			case TEST:
				if(set.length()==0 || setOut.length()==0 || chromosome==nullptr)
					throw std::invalid_argument("-test needs: -set, -setOut, -chromosome.");
				break;
			case STATUS:
				if(chromosome==nullptr)
					throw std::invalid_argument("-status needs: -chromosome.");
				break;
			case USE:
				if(chromosome==nullptr || on==nullptr)
					throw std::invalid_argument("-use needs: -chromosome, -on.");
				break;
			case NOPE:
				throw std::invalid_argument("No action.");
				break;
			default:
				break;
		}
	}

	~Args(){
		for(FILE* f : shouldClose){
			fclose(f);
		}
	}

	/**
	 * Prints program help to stdout.
	 */
	static void showHelp(){
		std::cout << "Thank you for using "<< PROGRAM_NAME <<". "
				<< "This program was developed at FIT BUT as project to course: Bio-Inspired Computers. \n\n"
				<< "\t-train" <<"\n\t\tYou want to develop new edge detection filter on given training data set. Developed filter chromosome will be printed to stdout.\n"
				<<"\t\tPROVIDE: -set, -setOut, -config\n"
				<< "\t-repair" <<"\n\t\tIt will try to find new implementation of filter with given resources (damaged before). Developed filter chromosome will be printed to stdout.\n"
				<<"\t\tPROVIDE: -set, -setOut, -chromosome, -config\n"
				<< "\t-damage" <<"\n\t\tWill damage the existing filter. Randomly selects block and changes it to block with 0 output. Damaged filter chromosome will be printed to stdout.\n"
				<<"\t\tPROVIDE: chromosome\n"
				<< "\t-test" <<"\n\t\tYou want to test your developed filter on given testing data set.\n"
				<<"\t\tPROVIDE: -set, -setOut, -chromosome\n"
				<< "\t-status" <<"\n\t\tChecks filter state (how much it is damaged).\n"
				<<"\t\tPROVIDE: -chromosome\n"
				<< "\t-use" <<"\n\t\tUse filter on given image.\n"
				<<"\t\tPROVIDE: -chromosome -on\n"


				<< "\t-set" <<"\n\t\tPath to folder with train/test set (filled with jpg images).\n"
				<< "\t-setOut" <<"\n\t\tPath to folder with required filter output. Name of image must corespond with name of input image from -set.\n"

				<< "\t-on" <<"\n\t\tPath to image (jpg).\n"

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

	FILE* getChromosome() const {
		return chromosome;
	}

	const std::string& getSet() const {
		return set;
	}

	const std::string& getSetOut() const {
		return setOut;
	}

	const Config& getConfig() const {
		return config;
	}

private:

	std::string set; //! Path to folder with data set.
	std::string setOut; //! Path to folder with data set ouput.
	FILE* on=nullptr; //! Image.
	FILE* chromosome=nullptr; //! File containing chromosome.
	Config config; //!Loaded configuration.

	Action action=Action::NOPE; //! Action user wants to perform.

	std::vector<FILE*> shouldClose; //! Files that we need to close in destructor.

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

			for(auto KV : myArgs.getConfig()){
				std::cout << KV.first << " = " << KV.second <<std::endl;
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
