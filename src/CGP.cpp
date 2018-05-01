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

const unsigned CHROMOSOME_BLOCK_SIZE=3; //!number of integers representing one block in chromosome


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
				outputs[out]=std::numeric_limits<u_int8_t>::max();
				break;
			case Function::IDENTITY:
				outputs[out]=x;
				break;
			case Function::INVERSION:
				outputs[out]=std::numeric_limits<u_int8_t>::max()-x;
				break;
			case Function::BIT_OR:
				outputs[out]=x|y;
				break;
			case Function::BIT_OR_WITH_INVERSE:
				outputs[out]=(std::numeric_limits<u_int8_t>::max()-x) |y;
				break;
			case Function::BIT_AND:
				outputs[out]=x&y;
				break;
			case Function::INVERSION_OF_BIT_AND:
				outputs[out]=std::numeric_limits<u_int8_t>::max()-(x&y);
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
					if(std::numeric_limits<u_int8_t>::max()>s){
						outputs[out]=s;
					}else{
						outputs[out]=std::numeric_limits<u_int8_t>::max();
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
		return outputs[c[c.size()-1]];
	}

}

inline std::set<unsigned> CGP::usedBlocks(const Chromosome&c){
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

inline u_int64_t CGP::fitness(const Chromosome& c, const std::vector<Image>& train, const std::vector<Image>& trainOut){
	u_int64_t fitness=0;

	//apply filter on Image
	//filter interpretation
	std::set<unsigned> usedB=usedBlocks(c);

	std::vector<uint8_t> inputs(PARAM_IN);
	std::vector<uint8_t> resImage;

	unsigned selectedImage=0;
	for(const Image& img: train){
		std::cout << "Evaluate chromosome on: " << img.getPath() <<std::endl;
		std::cout << "\twidth:" << img.getWidth() <<std::endl;
		std::cout << "\theight:" << img.getHeight() <<std::endl;
		if(img.getHeight()==0 || img.getWidth()==0) continue;
		const std::vector<uint8_t>& px=img.getPixels();
		//for every image
		resImage.resize(img.getHeight()*img.getWidth());

		std::cout << "\tedges"<<std::endl;
		//do the edges
		for(unsigned y=0; y<img.getHeight(); ++y){
			for(unsigned x=0; x<img.getWidth(); ++x){

				unsigned cnt=0;
				for(long tmpY=-1; tmpY<=1; tmpY++){	// -1 0 1 y offsets
					long useY=tmpY+y;

					//fix offset according to actual position
					if(useY<0) useY=0;
					if(useY>=img.getHeight()) useY=img.getHeight()-1;

					for(long tmpX=-1; tmpX<=1; tmpX++){	// -1 0 1 x offsets
						long useX=tmpX;
						//fix offset according to actual position
						if(useX<0) useX=0;
						if(useX>=img.getWidth()) useX=img.getWidth()-1;
						std::cout<< y*img.getWidth()+x << std::endl;
						std::cout << "\t"<< static_cast<unsigned>(img.getPixels()[0]) << std::endl;
						inputs[cnt++]=px[y*img.getWidth()+x];
					}
				}

				//apply filter on given inputs
				resImage[y*img.getWidth()+x]=apply(c, usedB, inputs);

				if(y!=0 && y!=img.getHeight()-1) x=img.getWidth()-1; //jump
			}
		}
		std::cout << "\trest"<<std::endl;
		//the rest
		for(unsigned y=1; y<img.getHeight()-1; ++y){
			for(unsigned x=1; x<img.getWidth()-1; ++x){
				//get inputs
				inputs[0]=px[x-(y-1)*img.getWidth()-1];
				inputs[1]=px[x-(y-1)*img.getWidth()];
				inputs[2]=px[x-(y-1)*img.getWidth()+1];
				inputs[3]=px[x-1];
				inputs[4]=px[x];
				inputs[5]=px[x+1];
				inputs[6]=px[x+(y+1)*img.getWidth()-1];
				inputs[7]=px[x+(y+1)*img.getWidth()];
				inputs[8]=px[x+(y+1)*img.getWidth()+1];

				//apply filter on given inputs
				resImage[y*img.getWidth()+x]=apply(c, usedB, inputs);

			}
		}
		auto pxRight=trainOut[selectedImage].getPixels();
		//evaluate the result
		for(unsigned y=0; y<img.getHeight(); ++y){
			for(unsigned x=0; x<img.getWidth(); ++x){
				unsigned diff=abs(pxRight[y*img.getWidth()+x]-resImage[y*img.getWidth()+x]);
				if(diff>std::numeric_limits<u_int64_t>::max()-fitness){
					//overflow
					return std::numeric_limits<u_int64_t>::max();
				}
				fitness+=diff;
			}
		}

		++selectedImage;
	}

	return fitness;
}

inline void CGP::evaluate(const Population& population, unsigned& bestFitness, unsigned& bestIndex,
		const std::vector<Image>& train, const std::vector<Image>& trainOut){
	bestFitness=0;
	bestIndex=0;

	//evaluate and find the best one
	for(unsigned i=0; i< population.size(); ++i){
		unsigned actF=fitness(population[i], train, trainOut);
		if(actF<bestFitness){	//lower is better
			bestFitness=actF;
			bestIndex=0;
		}
	}
}

inline void CGP::mutate(Chromosome& c){
	std::uniform_int_distribution<std::mt19937::result_type> distMutations(1, mutationMax);
	std::uniform_int_distribution<std::mt19937::result_type> distOutputs(0,rows*cols+PARAM_IN);
	std::uniform_int_distribution<std::mt19937::result_type> distIndex(0,c.size()-1);

	unsigned gens = distMutations(randGen);   //number of gens for mutations
	for (unsigned j = 0; j < gens; ++j) {
		unsigned i = distIndex(randGen); //select chromosome index for mutation
		unsigned col = (i / rows*CHROMOSOME_BLOCK_SIZE);

		std::uniform_int_distribution<std::mt19937::result_type> dist(0,colVal[col].size());

		if (i < rows*cols*CHROMOSOME_BLOCK_SIZE) {
			//block mutation
			if ((i % CHROMOSOME_BLOCK_SIZE) < 2) {
				//block input mutation
				c[i]=colVal[col][dist(randGen)];
			} else { //mutace fce
				if(damaged.find(PARAM_IN+i)!=damaged.end()){
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

	unsigned bestFitness;	//best fitness
	unsigned bestIndex;	//index of best in population
	Chromosome theMVP;	//best chromosome so far


	//distribution for connecting outputs
	std::uniform_int_distribution<std::mt19937::result_type> distOutputs(0,rows*cols+PARAM_IN);
	//run evolution multiple times
	for (unsigned run=0; run < runs; run++) {
		std::cout << "Evolution run: " << run << std::endl;
		//create initial population
		Population population;
		population.resize(populationSize);

		std::cout << "\tGenerate intial population."<< std::endl;
		for (unsigned i = 0; i < populationSize; i++) {
			//for i-th chromosome
			for(unsigned actColumn=0; actColumn< cols; ++actColumn){
				std::uniform_int_distribution<std::mt19937::result_type> dist(0,colVal[actColumn].size());
				for (unsigned r = 0; r < rows; ++r) {
					//one block

					//first input
					population[i].push_back(colVal[actColumn][dist(randGen)]);
					//second input
					population[i].push_back(colVal[actColumn][dist(randGen)]);
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
		std::cout << "\tEvaluate intial population of "<< population.size() << " individuals."<< std::endl;
		//evaluate intial population
		evaluate(population, bestFitness, bestIndex, train, trainOut);

		theMVP=population[bestIndex];

		std::cout << "\tStart evolution."<< std::endl;
		//evolution
		for(unsigned actGen=0; actGen<generations; ++actGen){
			//mutate the best
			for (unsigned i=0; i < populationSize;  i++) {
				population[i]=theMVP;
				mutate(population[i]);
			}

			//evaluate population

			unsigned tmpBestFitness;
			unsigned tmpBestIndex;
			evaluate(population, tmpBestFitness, tmpBestIndex, train, trainOut);
			if(tmpBestFitness>=bestFitness){
				//we searched at least as good individual as actual MVP
				//so change it
				//(Equal is because of diversity)
				theMVP=population[tmpBestIndex];
			}
		}

		std::cout << "\tBest fitness in run: " << bestFitness << std::endl;

	}

	//the end
	return theMVP;

}


/*** End of file: CGP.cpp ***/
