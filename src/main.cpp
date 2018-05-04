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
			}else if(actArg=="-use"){
				if(action!=NOPE) throw std::invalid_argument("You can not select multiple actions.");
				action=USE;
			}else if(actArg=="-set"){
				if(++i>=argc) throw std::invalid_argument("No value for set.");

				//read paths to images
				while(i<argc && keyWords.find(argv[i])==keyWords.end()){
					set.push_back(argv[i++]);
				}
				if(i < argc && keyWords.find(argv[i])!=keyWords.end()){
					--i;
				}
			}else if(actArg=="-setOut"){
				if(++i>=argc) throw std::invalid_argument("No value for setOut.");
				//read paths to images

				while (i < argc && keyWords.find(argv[i]) == keyWords.end()) {
					setOut.push_back(argv[i++]);
				}
				if (i < argc && keyWords.find(argv[i]) != keyWords.end()) {
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
				on=argv[i];

			}else if(actArg=="-out"){
				if(++i>=argc){
					throw std::invalid_argument("Must specify file that can be open for writting.");
				}
				out=argv[i];
			}else if(actArg=="-config"){
				if(++i>=argc){
					throw std::invalid_argument("Must specify config file that can be open for reading.");
				}

				std::ifstream f(argv[i]);
				if(!f){
					throw std::invalid_argument("Must specify config file that can be open for reading.");
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
				if(set.size()==0 || setOut.size()==0 || config.empty() || out.size()==0)
					throw std::invalid_argument("-train needs: -set, -setOut, -config, -out.");

				break;
			case REPAIR:
				if(set.size()==0 || setOut.size()==0 || !chromosome || config.empty() || out.size()==0)
					throw std::invalid_argument("-repair needs: -set, -setOut, -chromosome, -config, -out.");
				break;
			case DAMAGE:
				if(!chromosome || out.size()==0)
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
				if(!chromosome || on.size()==0 || out.size()==0)
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
				<<"\t\tPROVIDE: -chromosome -out\n"
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

	std::string& getOn() {
		return on;
	}

	std::string& getOut() {
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
	std::string on; //! Image path.
	std::string out; //! Result.
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

/**
 * Loads chromosome from file.
 *
 * @param[in] f
 * 	File with chromosome.
 * @param[out] c
 * 	Loaded chromosome.
 * @param[out] cols
 *  Loaded number of cols.
 * @param[out] rows
 *	Loaded number of rows.
 */
inline void loadChromosome(std::ifstream& f, Chromosome& c, uint32_t& cols, uint32_t& rows){
	f.read(reinterpret_cast<char *>(&cols), sizeof(cols));
	f.read(reinterpret_cast<char *>(&rows), sizeof(rows));
	c.resize(CGP::CHROMOSOME_BLOCK_SIZE*cols*rows+1);
	f.read(reinterpret_cast<char *>(&c[0]), sizeof(uint32_t)*(CGP::CHROMOSOME_BLOCK_SIZE*cols*rows+1));

}

/**
 * Save chromosome to file.
 *
 * @param[in] path
 * 	Path to file for saving.
 * @param[in] c
 * 	Chromosome for saving.
 * @param[in] cols
 *  Cols for saving.
 * @param[in] rows
 * 	Rows for saving.
 */
inline void saveChromosome(std::string& path, Chromosome& c, uint32_t cols, uint32_t rows){
	std::ofstream f(path, std::ios::binary);
	if (!f) {
		throw std::invalid_argument(
				"Must specify chromosome file that can be open for writting.");
	}

	uint32_t val = cols;
	f.write(reinterpret_cast<const char *>(&val), sizeof(val));
	val = rows;
	f.write(reinterpret_cast<const char *>(&val), sizeof(val));

	for (auto g : c) {
		f.write(reinterpret_cast<const char *>(&g), sizeof(g));
	}

}

/**
 * Get indexes of damaged blocks in given chromosome.
 *
 * @param[in] c
 * 	The chromosome.
 * @return
 * 	Indexes of damaged blocks.
 */
inline std::set<uint32_t> getDamagedBlocks(const Chromosome& c){
	std::set<uint32_t> damagedBlocks;
	for (unsigned i = 2; i < c.size(); i += CGP::CHROMOSOME_BLOCK_SIZE) { //third is function
		if (c[i] == static_cast<unsigned>(CGP::Function::DAMAGED)) {
			damagedBlocks.insert(
					i / CGP::CHROMOSOME_BLOCK_SIZE + CGP::PARAM_IN);
		}
	}
	return damagedBlocks;
}

int main(int argc, char* argv[]){
	try {
		Args myArgs(argc, argv);
		Config& config=myArgs.getConfig();
		//read config
		switch (myArgs.getAction()) {
		case Args::Action::HELP:
			//User needs our help!
			Args::showHelp();
			break;
		case Args::Action::TRAIN:
			{
				//train new chromosome
				std::cout << "Used configuration: ";
				for(auto KV : config){
					std::cout << KV.first << " = " << KV.second <<", ";
				}

				std::cout << std::endl;

				CGP cgp(config.getCols(), config.getRows(), config.getlBack());
				cgp.setPopulationSize(config.getPopulationSize());
				cgp.setMutationMax(config.getMaxMutations());
				cgp.setGenerations(config.getGenerations());

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

				std::cout << "Start " << config.getRuns() << " evolution runs." << std::endl;


				//perform evolution
				Chromosome c=cgp.evolve(config.getRuns(), train, trainOut);


				std::cout << "Saving chromosome." << std::endl;
				//save chromosome
				saveChromosome(myArgs.getOut(), c, config.getCols(), config.getRows());
				std::cout << "\tSAVED" << std::endl;
			}
			break;
		case Args::Action::USE:
			//use given filter on given image
			{
				//load image
				std::cout << "Load image." << std::endl;
				Image img(myArgs.getOn());
				std::cout << "\tLOADED" << std::endl;

				//load chromosome
				std::cout << "Load chromosome." << std::endl;
				uint32_t cols;
				uint32_t rows;
				Chromosome c;
				loadChromosome(myArgs.getChromosome(), c, cols, rows);
				std::cout << "\tLOADED" << std::endl;

				CGP cgp(cols, rows);
				std::cout << "Apply filter." << std::endl;
				Image res(cgp.useFilter(c, img));

				std::cout << "Save." << std::endl;
				res.save(myArgs.getOut());

			}
			break;
		case Args::Action::REPAIR:
			{
				//chromosome repair
				//load chromosome
				std::cout << "Load chromosome." << std::endl;
				uint32_t cols;
				uint32_t rows;
				Chromosome c;
				loadChromosome(myArgs.getChromosome(), c, cols, rows);
				config.setCols(cols);
				config.setRows(rows);

				std::cout << "\tLOADED" << std::endl;

				std::cout << "Used configuration: ";
				for (auto KV : config) {
					std::cout << KV.first << " = " << KV.second << ", ";
				}
				std::cout<< std::endl;
				//load images
				std::cout << "Load train set." << std::endl;
				std::vector<Image> train;
				for (auto iPath : myArgs.getSet()) {
					train.push_back(std::move(Image(iPath)));
				}
				std::cout << "\tLOADED" << std::endl;

				std::cout << "Load train setOut." << std::endl;
				std::vector<Image> trainOut;
				for (auto iPath : myArgs.getSetOut()) {
					trainOut.push_back(std::move(Image(iPath)));
				}
				std::cout << "\tLOADED" << std::endl;

				std::cout << "Start repair." << std::endl;


				CGP cgp(config.getCols(), config.getRows(), config.getlBack());
				cgp.setPopulationSize(config.getPopulationSize());
				cgp.setMutationMax(config.getMaxMutations());
				cgp.setGenerations(config.getGenerations());
				//set damged blocks
				cgp.setDamaged(getDamagedBlocks(c));

				//perform evolution
				Chromosome repC=cgp.evolve(config.getRuns(), train, trainOut);

				std::cout << "Saving chromosome." << std::endl;
				//save chromosome
				saveChromosome(myArgs.getOut(), repC, config.getCols(), config.getRows());
				std::cout << "\tSAVED" << std::endl;


			}
			break;
		case Args::Action::DAMAGE:
			//damage given chromosome
			{
				//load chromosome
				std::cout << "Load chromosome." << std::endl;
				uint32_t cols;
				uint32_t rows;
				Chromosome c;
				loadChromosome(myArgs.getChromosome(), c, cols, rows);
				std::cout << "\tLOADED" << std::endl;

				std::vector<unsigned> blocks;
				std::set<uint32_t> damagedBlocks(getDamagedBlocks(c));

				//filter allready damaged blocks
				for(unsigned idx=CGP::PARAM_IN; idx<CGP::PARAM_IN+cols*rows; ++idx){
					if(damagedBlocks.find(idx)==damagedBlocks.end()){
						blocks.push_back(idx);
					}
				}


				std::mt19937 randGen;
				randGen.seed(std::random_device()());

				//randomly damage block
				if(blocks.size()>0){
					std::uniform_int_distribution<std::mt19937::result_type> dist(0, blocks.size()-1);
					unsigned damage=blocks[dist(randGen)];

					CGP::damageBlock(c, damage);
					std::cout << "Block "<< damage << " is damaged." << std::endl;
				}else{
					std::cout << "All blocks are damaged."<< std::endl;
				}

				std::cout << "Saving chromosome." << std::endl;
				//save chromosome
				saveChromosome(myArgs.getOut(), c, cols, rows);
				std::cout << "\tSAVED" << std::endl;
			}

			break;

		case Args::Action::STATUS:
			//check filter status
			{
				//load chromosome
				uint32_t cols;
				uint32_t rows;
				Chromosome c;
				loadChromosome(myArgs.getChromosome(), c, cols, rows);

				std::cout << "Chromosome ("<< c.size() <<"): ";
				unsigned cnt=0;
				for(auto g :c){
					if(cnt==c.size()-1){
						std::cout << "Out: ";
					}else if(cnt++%CGP::CHROMOSOME_BLOCK_SIZE ==0)std::cout <<CGP::PARAM_IN+(cnt/CGP::CHROMOSOME_BLOCK_SIZE) <<": ";
					std::cout << g << " ";
					if(cnt%CGP::CHROMOSOME_BLOCK_SIZE ==0)std::cout <<"| ";
				}
				std::cout <<std::endl;

				std::set<unsigned> usedB(CGP::usedBlocks(c));

				std::cout << "Blocks: "<< cols*rows <<"\n";
				std::cout << "Used blocks ("<< usedB.size() <<"): ";

				std::vector<uint32_t> damagedUsedBlocks;

				for(auto uB:usedB){
					std::cout << uB << " ";
					if(c[(uB-CGP::PARAM_IN)*CGP::CHROMOSOME_BLOCK_SIZE+2] == static_cast<unsigned>(CGP::Function::DAMAGED)){
						//damaged
						damagedUsedBlocks.push_back(uB);
					}
				}
				std::cout << "\nDamaged used blocks ("<< damagedUsedBlocks.size() <<"): ";
				for(auto duB:damagedUsedBlocks){
					std::cout << duB << " ";
				}

				std::set<uint32_t> damagedBlocks(getDamagedBlocks(c));
				//damaged blocks
				std::cout << "\nDamaged blocks ("<< damagedBlocks.size() <<"): ";
				for(auto dB:damagedBlocks){
					std::cout << dB << " ";
				}
				std::cout << std::endl;

			}

			break;
		case Args::Action::TEST:
			//test filter on given data
			{
				//load images
				std::cout << "Load test set." << std::endl;
				std::vector<Image> test;
				for (auto iPath : myArgs.getSet()) {
					test.push_back(std::move(Image(iPath)));
				}
				std::cout << "\tLOADED" << std::endl;

				std::cout << "Load test setOut." << std::endl;
				std::vector<Image> testOut;
				for (auto iPath : myArgs.getSetOut()) {
					testOut.push_back(std::move(Image(iPath)));
				}
				std::cout << "\tLOADED" << std::endl;

				//load chromosome
				std::cout << "Load chromosome." << std::endl;
				uint32_t cols;
				uint32_t rows;
				Chromosome c;
				loadChromosome(myArgs.getChromosome(), c, cols, rows);
				std::cout << "\tLOADED" << std::endl;

				CGP cgp(cols, rows);
				std::cout << "Fitness: " << std::flush;
				std::cout << cgp.fitness(c, test, testOut) << std::endl;
			}
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
	} catch (const std::runtime_error& e) {
		std::cerr << "Runtime error: \n";
		std::cerr << e.what() << "\n\n";

		Args::showHelp();

		return 1;
	}

}
/*** End of file: main.cpp ***/
