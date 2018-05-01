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
#include <set>

CGP::CGP(const unsigned c, const unsigned r, const unsigned lBack): cols(c), rows(r), lBack(lBack){
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
		int maxidx = i * rows + PARAM_IN; //index of first not reachable block output

		colVal[i].resize(PARAM_IN + maxidx - minidx);

		int j = 0;
		for (int k = 0; k < PARAM_IN; k++, j++) //inserting indexes of inputs
			colVal[i][j] = k;
		for (int k = minidx; k < maxidx; k++, j++) //inserting indexes of posible block outputs
			colVal[i][j] = k;
	}
}

inline uint8_t CGP::apply(const Chromosome&c, const std::set<unsigned>& usedBlocks, const std::vector<uint8_t>& inputs){
	for(auto blockIdx : usedBlocks){
		//its a set so indexes are sorted so it should be ok
		//and we will have calculated all outputs in needed order


		unsigned out=blockIdx-PARAM_IN;	//block output

		//get inputs
		unsigned in1=c[(blockIdx-PARAM_IN)*3];
		unsigned in2=c[(blockIdx-PARAM_IN)*3+1];
		Function f=c[(blockIdx-PARAM_IN)*3+2];

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
				unsigned s=x+y;
				if(std::numeric_limits<u_int8_t>::max()>s){
					outputs[out]=s;
				}else{
					outputs[out]=std::numeric_limits<u_int8_t>::max();
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

inline static std::set<unsigned> CGP::usedBlocks(const Chromosome&c){
	std::set<unsigned> used;
	std::queue<unsigned> process;

	//start from output
	process.push(c[c.size()-1]);

	while(!process.empty()){
		if(process.front()>=PARAM_IN){
			//its block, not just input
			used.insert(process.front());
			process.push(c[(process.front()-PARAM_IN)*3]);		//first input
			process.push(c[(process.front()-PARAM_IN)*3+1]);	//second input
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
	for(const Image& img: train){
		if(img.getHeight()==0 || img.getWidth()==0) continue;
		auto px=img.getPixels();
		//for every image
		std::vector<uint8_t> resImage(img.getHeight()*img.getWidth());

		//do the edges
		for(unsigned y=0; y<img.getHeight(); ++y){
			int yRep=(y==0)? 1: -1;
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

						inputs[cnt++]=px[y*img.getWidth()+x];
					}
				}

				//apply filter on given inputs
				resImage[y*img.getWidth()+x]=apply(c, usedB, inputs);

				if(y!=0 && y!=img.getHeight()-1) x=img.getWidth()-1; //jump
			}
		}

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

		//evaluate the result
		for(unsigned y=0; y<img.getHeight(); ++y){
			for(unsigned x=0; x<img.getWidth(); ++x){
				int diff=abs(px[y*img.getWidth()+x]-resImage[y*img.getWidth()+x]);
				if(diff>std::numeric_limits<u_int64_t>::max()-fitness){
					//overflow
					return std::numeric_limits<u_int64_t>::max();
				}
				fitness+=diff;
			}
		}
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

Chromosome CGP::evolve(const unsigned generations, const unsigned runs,
			const std::vector<Image>& train, const std::vector<Image>& trainOut){

	int bestFit;	//best fitness
	long bestFitIdx;	//index of best in population

	//distribution for generatting random functions
	//DAMAGED is last and we do not want to generate it.
	std::uniform_int_distribution<std::mt19937::result_type> distFunctions(0,Function::DAMAGED-1);
	//distribution for connecting outputs
	std::uniform_int_distribution<std::mt19937::result_type> distOutputs(0,rows*cols+PARAM_IN);
	//run evolution multiple times
	for (unsigned run=0; run < runs; run++) {

		//create initial population
		Population population;
		population.resize(populationMax);

		for (unsigned i = 0; i < populationMax; i++) {
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
					population[i].push_back(distFunctions(randGen));
				}
			}

			//do not forget to connect the outputs
			for (unsigned j = 0; j < PARAM_OUT; ++j)
				population[i].push_back(distOutputs(randGen));
		}

		//evaluate intial population
		bestFit=0;
		bestFitIdx=-1;


		//-----------------------------------------------------------------------
		        //Ohodnoceni pocatecni populace
		        //-----------------------------------------------------------------------
		        bestfit = 0; bestfit_idx = -1;
		        ohodnoceni(data /*vektor ocekavanych dat*/, 0, param_populace, -1);
		        for (int i=0; i < param_populace; i++) { //nalezeni nejlepsiho jedince
		            if (fitt[i] > bestfit) {
		               bestfit = fitt[i];
		               bestfit_idx = i;
		            }
		        }

		        //bestfit_idx ukazuje na nejlepsi reseni v ramci pole jedincu "populace"
		        //bestfit obsahuje fitness hodnotu prvku s indexem bestfit_idx

		        if (bestfit_idx == -1)
		           return 0;


	}



}


/*** End of file: CGP.cpp ***/
