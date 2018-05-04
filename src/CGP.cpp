/**
 * Project: EDEF
 * @file CGP.cpp
 * @date 30. 4. 2018
 * @author (just minor adjustments) xdocek09
 * @brief Source file for CGP (cartesian genetic programming) class.
 * Inspired from implementation provided in lecture BIN at FIT BUT.
 * https://www.fit.vutbr.cz/study/courses/BIN
 */

#include "CGP.h"
#include <queue>
#include <random>




CGP::CGP(const unsigned c, const unsigned r): cols(c), rows(r){
	randGen.seed(std::random_device()());
	calcColVals();
	outputs.resize(cols*rows);
}

CGP::CGP(const unsigned c, const unsigned r, const unsigned lBack): cols(c), rows(r), lBack(lBack),
		distFunctions(0,static_cast<unsigned>(Function::DAMAGED)-1){//DAMAGED is last and we do not want to generate it.
	randGen.seed(std::random_device()());
	calcColVals();
	outputs.resize(cols*rows);
}

inline void CGP::calcColVals(){
	colVal.clear();
	colVal.resize(cols);

	for (unsigned i = 0; i < cols; i++) {
		int minidx = rows * (i - lBack) + PARAM_IN;	//index of first reachable block output
		if (minidx < static_cast<long>(PARAM_IN))
			minidx = PARAM_IN; //block outputs are from PARAM_IN to PARAM_IN+cols*rows
		unsigned maxidx = i * rows + PARAM_IN; //index of first not reachable block output

		colVal[i].resize(PARAM_IN + maxidx - minidx);

		unsigned j = 0;
		for (unsigned k = 0; k < PARAM_IN; k++, j++) //inserting indexes of inputs
			colVal[i][j] = k;

		for (unsigned k = minidx; k < maxidx; k++, j++) //inserting indexes of posible block outputs
			colVal[i][j] = k;

	}
}

inline uint8_t CGP::apply(const Chromosome&c, const std::set<unsigned>& usedBlocks, const std::vector<uint8_t>& inputs){


	for(auto blockIdx : usedBlocks){

		//its a set so indexes are sorted so it should be ok
		//and we will have calculated all outputs in needed order


		unsigned out=blockIdx-PARAM_IN;	//block output

		//get inputs
		unsigned in1=c[(blockIdx-PARAM_IN)*CHROMOSOME_BLOCK_SIZE];
		unsigned in2=c[(blockIdx-PARAM_IN)*CHROMOSOME_BLOCK_SIZE+1];
		Function f=static_cast<Function>(c[(blockIdx-PARAM_IN)*CHROMOSOME_BLOCK_SIZE+2]);

		unsigned x,y;

		if(in1<PARAM_IN){
			//connected to input
			x=inputs[in1];
		}else{
			x=outputs[in1-PARAM_IN];
		}

		if(in2<PARAM_IN){
			//connected to input
			y=inputs[in2];
		}else{
			y=outputs[in2-PARAM_IN];
		}

		//evaluate block
		switch (f) {
			case Function::MAX_VAL:
				outputs[out]=std::numeric_limits<uint8_t>::max();
				break;
			case Function::IDENTITY:
				outputs[out]=x;
				break;
			case Function::INVERSION:
				outputs[out]=std::numeric_limits<uint8_t>::max()-x;
				break;
			case Function::BIT_OR:
				outputs[out]=x|y;
				break;
			case Function::BIT_OR_WITH_INVERSE:
				outputs[out]=(std::numeric_limits<uint8_t>::max()-x) |y;
				break;
			case Function::BIT_AND:
				outputs[out]=x&y;
				break;
			case Function::INVERSION_OF_BIT_AND:
				outputs[out]=std::numeric_limits<uint8_t>::max()-(x&y);
				break;
			case Function::XOR:
				outputs[out]=x^y;
				break;
			case Function::DIVIDING_BY_TWO:
				outputs[out]=x>>1;
				break;
			case Function::DIVIDING_BY_FOUR:
				outputs[out]=x>>2;
				break;
			case Function::MULTIPLE_DIVIDE_AND_OR:
				outputs[out]=(x << 4) | (y >> 4);
				break;
			case Function::SUM:
				outputs[out]=x+y;
				break;
			case Function::SUM_WITH_SATURATION:
				{
					unsigned s=x+y;
					if(std::numeric_limits<uint8_t>::max()>s){
						outputs[out]=s;
					}else{
						outputs[out]=std::numeric_limits<uint8_t>::max();
					}
				}
				break;
			case Function::INT_AVG:
				outputs[out]=(x+y)>>1;
				break;
			case Function::MAX:
				outputs[out]=(x>y)?x:y;
				break;
			case Function::MIN:
				outputs[out]=(x<y)?x:y;
				break;
			default:
				//DAMAGED
				outputs[out]=0;
		}
	}

	if(c[c.size()-1]<PARAM_IN){
		//get it straight from inputs
		return inputs[c[c.size()-1]];
	}else{
		//get result from calculated output of a block
		return outputs[c[c.size()-1]-PARAM_IN];
	}

}

void CGP::damageBlock(Chromosome&c, unsigned idx){
	c[(idx-PARAM_IN)*CHROMOSOME_BLOCK_SIZE+2]=static_cast<uint32_t>(Function::DAMAGED);
}

std::set<unsigned> CGP::usedBlocks(const Chromosome&c){
	std::set<unsigned> used;
	std::queue<unsigned> process;

	//start from output
	process.push(c[c.size()-1]);

	while(!process.empty()){
		if(process.front()>=PARAM_IN){
			//its block, not just input
			used.insert(process.front());
			process.push(c[(process.front()-PARAM_IN)*CHROMOSOME_BLOCK_SIZE]);		//first input
			process.push(c[(process.front()-PARAM_IN)*CHROMOSOME_BLOCK_SIZE+1]);	//second input
		}
		process.pop();
	}

	return used;
}

Image CGP::useFilter(Chromosome c, const Image& img){
	static std::vector<uint8_t> resImage;	//the cache
	std::set<unsigned> usedB=usedBlocks(c);
	useFilter(c, img, resImage, usedB);
	return Image(img.getWidth(), img.getHeight(), resImage);
}

inline void CGP::useFilter(Chromosome c, const Image& img, std::vector<uint8_t>& resImage, const std::set<unsigned>& usedB){
	std::vector<uint8_t> inputs(PARAM_IN);

	const std::vector<uint8_t>& px = img.getPixels();
	resImage.resize(img.getHeight() * img.getWidth());

	//do the edges
	//we are extending borders
	for (unsigned y = 0; y < img.getHeight(); ++y) {
		for (unsigned x = 0; x < img.getWidth(); ++x) {

			unsigned cnt = 0;
			for (long tmpY = -1; tmpY <= 1; tmpY++) {	// -1 0 1 y offsets
				long useY = tmpY + y;

				//fix offset according to actual position
				if (useY < 0)
					useY = 0;
				if (useY >= img.getHeight())
					useY = img.getHeight() - 1;

				for (long tmpX = -1; tmpX <= 1; tmpX++) {	// -1 0 1 x offsets
					long useX = tmpX+x;
					//fix offset according to actual position
					if (useX < 0)
						useX = 0;
					if (useX >= img.getWidth())
						useX = img.getWidth() - 1;
					inputs[cnt++] = px[useY * img.getWidth() + useX];
				}
			}

			//apply filter on given inputs
			resImage[y * img.getWidth() + x] = apply(c, usedB, inputs);

			if (y != 0 && y != img.getHeight() - 1 && x!=img.getWidth()-1){
				if(img.getWidth()>1){
					x = img.getWidth() - 2; //jump
				}
			}

		}
	}

	//the rest
	for (unsigned y = 1; y < img.getHeight() - 1; ++y) {
		for (unsigned x = 1; x < img.getWidth() - 1; ++x) {
			//get inputs
			inputs[0] = px[x + (y - 1) * img.getWidth() - 1];
			inputs[1] = px[x + (y - 1) * img.getWidth()];
			inputs[2] = px[x + (y - 1) * img.getWidth() + 1];
			inputs[3] = px[x + y*img.getWidth() - 1];
			inputs[4] = px[x + y*img.getWidth()];
			inputs[5] = px[x + y*img.getWidth() + 1];
			inputs[6] = px[x + (y + 1) * img.getWidth() - 1];
			inputs[7] = px[x + (y + 1) * img.getWidth()];
			inputs[8] = px[x + (y + 1) * img.getWidth() + 1];

			//apply filter on given inputs
			resImage[y * img.getWidth() + x] = apply(c, usedB, inputs);

		}
	}
}

uint64_t CGP::fitness(const Chromosome& c, const std::vector<Image>& train, const std::vector<Image>& trainOut){
	uint64_t fitness=0;

	//apply filter on Image
	//filter interpretation
	std::set<unsigned> usedB=usedBlocks(c);
	static std::vector<uint8_t> resImage;	//the cache
	std::vector<uint8_t> inputs(PARAM_IN);


	unsigned selectedImage=0;
	for(const Image& img: train){
		if (img.getHeight() == 0 || img.getWidth() == 0) continue;
		useFilter(c, img, resImage, usedB);
		auto pxRight=trainOut[selectedImage].getPixels();
		//evaluate the result
		for(unsigned y=0; y<img.getHeight(); ++y){
			for(unsigned x=0; x<img.getWidth(); ++x){
				unsigned diff=abs(pxRight[y*img.getWidth()+x]-resImage[y*img.getWidth()+x]);
				if(diff>std::numeric_limits<uint64_t>::max()-fitness){
					//overflow
					return std::numeric_limits<uint64_t>::max();
				}
				fitness+=diff;
			}
		}

		++selectedImage;
	}

	return fitness;
}

inline void CGP::evaluate(const Population& population, uint64_t& bestFitness, unsigned& bestIndex,
		const std::vector<Image>& train, const std::vector<Image>& trainOut){
	bestFitness=std::numeric_limits<uint64_t>::max();
	bestIndex=0;

	//evaluate and find the best one
	for(unsigned i=0; i< population.size(); ++i){
		unsigned actF=fitness(population[i], train, trainOut);
		if(actF<bestFitness){	//lower is better
			bestFitness=actF;
			bestIndex=i;
		}
	}
}

inline void CGP::mutate(Chromosome& c){
	std::uniform_int_distribution<std::mt19937::result_type> distMutations(1, mutationMax);
	std::uniform_int_distribution<std::mt19937::result_type> distOutputs(0,rows*cols+PARAM_IN-1);
	std::uniform_int_distribution<std::mt19937::result_type> distIndex(0,c.size()-1);

	unsigned gens = distMutations(randGen);   //number of gens for mutations
	for (unsigned j = 0; j < gens; ++j) {
		unsigned i = distIndex(randGen); //select chromosome index for mutation

		if (i < rows*cols*CHROMOSOME_BLOCK_SIZE) {

			//block mutation
			unsigned col = (i / (rows*CHROMOSOME_BLOCK_SIZE));
			std::uniform_int_distribution<std::mt19937::result_type> dist(0,colVal[col].size()-1);
			if ((i % CHROMOSOME_BLOCK_SIZE) < 2) {
				//block input mutation
				c[i]=colVal[col][dist(randGen)];
			} else { //mutace fce
				if(damaged.find(PARAM_IN+i/CHROMOSOME_BLOCK_SIZE)!=damaged.end()){
					//this block is damaged
					continue;
				}
				c[i]=distFunctions(randGen);
			}
		} else {
			//output mutation
			c[i]=distOutputs(randGen);
		}
	}
}

Chromosome CGP::evolve(const unsigned runs,
			const std::vector<Image>& train, const std::vector<Image>& trainOut){

	uint64_t bestFitness=std::numeric_limits<uint64_t>::max();	//best fitness
	Chromosome theMVP;	//best chromosome so far


	//distribution for connecting outputs
	std::uniform_int_distribution<std::mt19937::result_type> distOutputs(0,rows*cols+PARAM_IN-1);
	//run evolution multiple times
	for (unsigned run=0; run < runs; run++) {
		uint64_t bestFitnessRun=std::numeric_limits<uint64_t>::max();	//best fitness
		Chromosome bestInRun;	//best chromosome so far
		std::cout << "Evolution run: " << run << std::endl;
		//create initial population
		Population population(populationSize);

		for (unsigned i = 0; i < populationSize; i++) {
			//for i-th chromosome
			for(unsigned actColumn=0; actColumn< cols; ++actColumn){
				std::uniform_int_distribution<std::mt19937::result_type> dist(0,colVal[actColumn].size()-1);
				for (unsigned r = 0; r < rows; ++r) {
					//one block

					//first input
					unsigned ra=dist(randGen);
					population[i].push_back(colVal[actColumn][ra]);
					//second input
					ra=dist(randGen);

					population[i].push_back(colVal[actColumn][ra]);
					//function

					if(damaged.find(PARAM_IN+population[i].size()/CHROMOSOME_BLOCK_SIZE)==damaged.end()){
						population[i].push_back(distFunctions(randGen));
					}else{
						//block is damaged
						population[i].push_back(static_cast<int>(Function::DAMAGED));
					}

				}
			}

			//do not forget to connect the outputs
			for (unsigned j = 0; j < PARAM_OUT; ++j)
				population[i].push_back(distOutputs(randGen));
		}

		//evaluate intial population

		uint64_t tmpBestFitness;
		unsigned tmpBestIndex;

		evaluate(population, tmpBestFitness, tmpBestIndex, train, trainOut);
		if(tmpBestFitness<=bestFitnessRun){
			bestFitnessRun=tmpBestFitness;
			bestInRun=population[tmpBestIndex];
		}

		//evolution
		for(unsigned actGen=0; actGen<generations; ++actGen){
			//mutate the best
			for (unsigned i=0; i < populationSize;  i++) {
				population[i]=bestInRun;
				mutate(population[i]);
			}

			//evaluate population

			evaluate(population, tmpBestFitness, tmpBestIndex, train, trainOut);
			if(tmpBestFitness<=bestFitnessRun){
				//we searched at least as good individual as actual MVP
				//so change it
				//(Equal is because of diversity)
				bestInRun=population[tmpBestIndex];
				bestFitnessRun=tmpBestFitness;

			}
		}

		std::cout << "\tBest fitness in run: " << bestFitnessRun << std::endl;
		if(bestFitnessRun<=bestFitness){
			theMVP=bestInRun;
			bestFitness=bestFitnessRun;
		}

		std::cout << "\tBest fitness so far in all runs: " << bestFitness << std::endl;
	}

	//the end
	return theMVP;

}


/*** End of file: CGP.cpp ***/
