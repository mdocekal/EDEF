/**
 * Project: EDEF
 * @file CGP.h
 * @date 30. 4. 2018
 * @author (just minor adjustments) xdocek09
 * @brief Source file for CGP (cartesian genetic programming) class.
 * Inspired from implementation provided in lecture BIN at FIT BUT.
 * https://www.fit.vutbr.cz/study/courses/BIN
 */

#ifndef SRC_CGP_H_
#define SRC_CGP_H_

#include <vector>
#include <set>
#include <random>
#include <iostream>
#include <cstdint>
#include "Image.h"



/**
 * Representation of chromosome.
 */
typedef std::vector<uint32_t> Chromosome;

/**
 * Representation of whole population.
 */
typedef std::vector<Chromosome> Population;

/**
 * Class for cartesian genetic programming.
 * Suitable for generating image filters.
 */
class CGP {
public:
	/**
	 * Functions for blocks.
	 */
	enum class Function {
		MAX_VAL,					//! Maximum value for given range. (255)
		IDENTITY,					//! x
		INVERSION,					//! MAX_VAL-x
		BIT_OR,						//! x | y
		BIT_OR_WITH_INVERSE,		//! (MAX_VAL-x) | y
		BIT_AND,					//! x & y
		INVERSION_OF_BIT_AND,		//! MAX_VAL-(x&y)
		XOR,						//! x^y
		DIVIDING_BY_TWO,			//! x>>1
		DIVIDING_BY_FOUR,			//! x>>2
		MULTIPLE_DIVIDE_AND_OR,		//! (x << 4) | (y >> 4)
		SUM,						//! x+y
		SUM_WITH_SATURATION,		//! x+y with saturation (Example for 4 bit x: 13+5=15)
		INT_AVG,					//! Integer average (x+y)>>1
		MAX,						//! max(x,y)
		MIN,						//! min(x,y)
		DAMAGED						//! always 0	MUST STAY LAST IN ENUM
	};

	/**
	 * CGP initialization.
	 *
	 * @param[in] c
	 * 	Number of columns.
	 * @param[in] r
	 * 	Number of rows.
	 */
	CGP(const unsigned c, const unsigned r);

	/**
	 * CGP initialization.
	 *
	 * @param[in] c
	 * 	Number of columns.
	 * @param[in] r
	 * 	Number of rows.
	 * @param[in] lBack
	 * 	CGP lBack parameter.
	 */
	CGP(const unsigned c, const unsigned r, const unsigned lBack);

	/**
	 * Evolve chromosome.
	 *
	 * @param[in] runs
	 * 	Number of evolution runs.
	 * @param[in] train
	 * 	Train images that will be used as input for filter.
	 * @param[in] trainOut
	 * 	Train images that will be used for filter evaluation. (Desired result)
	 * @return Evolved chromosome of a filter.
	 */
	Chromosome evolve(const unsigned runs,
				const std::vector<Image> & train, const std::vector<Image> & trainOut);

	/**
	 * Use filter on image.
	 *
	 * @param[in] c
	 * 	Chromosome representation of filter.
	 * @param[in] img
	 * 	The image for processing.

	 * @return
	 * 	Result of filter.
	 */
	Image useFilter(Chromosome c, const Image& img);

	/**
	 * Use filter on image.
	 *
	 * @param[in] c
	 * 	Chromosome representation of filter.
	 * @param[in] img
	 * 	The image for processing.
	 * @param[out] resImage
	 *  Result of filter.
	 * @param[in] usedB
	 *  Used blocks in chromosome
	 */
	void useFilter(Chromosome c, const Image& img, std::vector<uint8_t>& resImage, const std::set<unsigned>& usedB);


	/**
	 * Get used blocks in chromosome.
	 *
	 * @param[in] c
	 * 	Chromosome.
	 * @return List of used block indexes. (first block index is PARAM_IN)
	 */
	static std::set<unsigned> usedBlocks(const Chromosome&c);

	/**
	 * Damages block on given index.
	 *
	 * @param[in|out] c
	 * 	Chromosome you want to damage.
	 * @param[in] idx
	 *  Index of block you want to damage.
	 */
	static void damageBlock(Chromosome&c, unsigned idx);

	unsigned getCols() const {
		return cols;
	}

	void setCols(unsigned cols) {
		this->cols = cols;
		calcColVals();
	}

	unsigned getBack() const {
		return lBack;
	}

	void setBack(unsigned back = 1) {
		lBack = back;
		calcColVals();
	}

	unsigned getMutationMax() const {
		return mutationMax;
	}

	void setMutationMax(unsigned mutationMax = 3) {
		this->mutationMax = mutationMax;
	}


	unsigned getRows() const {
		return rows;
	}

	void setRows(unsigned rows) {
		this->rows = rows;
		calcColVals();
	}

	const std::set<unsigned>& getDamaged() const {
		return damaged;
	}

	void setDamaged(const std::set<unsigned>& damaged) {
		this->damaged = damaged;
	}

	unsigned getPopulationSize() const {
		return populationSize;
	}

	void setPopulationSize(unsigned populationSize = 5) {
		this->populationSize = populationSize;
	}

	unsigned getGenerations() const {
		return generations;
	}

	void setGenerations(unsigned generations = 50000) {
		this->generations = generations;
	}

	static const unsigned PARAM_IN=9;	//! Number of inputs. (9-> 3x3 kernel)
	static const unsigned PARAM_OUT=1;	//! Number of outpus.
	static const unsigned CHROMOSOME_BLOCK_SIZE=3; //!number of integers representing one block in chromosome
private:
	unsigned cols;	//! cols in CGP matrix
	unsigned rows;	//! rows in CGP matrix
	unsigned populationSize=5;	//! size of population
	unsigned generations=50000; //! number of generations
	unsigned mutationMax=3; //! maximum number of mutations for one mutation
	unsigned lBack=1; //! CGP lBack parameter



	std::mt19937 randGen;	//! Random number generator.

	std::uniform_int_distribution<std::mt19937::result_type> distFunctions; //! Distribution for generatting random functions

	std::vector<std::vector<int>> colVal; //! Pre calculated posible values of inputs for columns

	std::vector<uint8_t> outputs; //! tmp cache for block outputs when filter is applied.

	std::set<unsigned> damaged; //! Contains indexes of damaged blocks. (first block index is PARAM_IN)


	/**
	 * Prepare posible input values for each column.
	 */
	void calcColVals();


	/**
	 * Evaluates given population.
	 *
	 * @param[in] population
	 * 	Population for evaluation.
	 * @param[out] bestFitness
	 * 	Fitness of best individual.
	 * @param[out] bestIndex
	 * 	Index of best individual in population.
	 * @param[in] train
	 * 	Train images that will be used as input for filter.
	 * @param[in] trainOut
	 * 	Train images that will be used for filter evaluation. (Desired result)
	 */
	void evaluate(const Population& population, uint64_t& bestFitness, unsigned& bestIndex,
			const std::vector<Image>& train, const std::vector<Image>& trainOut);

	/**
	 * Calculates fitness for given chromosome.
	 *
	 * @param[in] c
	 * 	Chromosome for fitness checking.
	 * @param[in] train
	 * 	Train images that will be used as input for filter.
	 * @param[in] trainOut
	 * 	Train images that will be used for filter evaluation. (Desired result)
	 * @return Chromosome fitness on training data.
	 */
	uint64_t fitness(const Chromosome& c, const std::vector<Image>& train, const std::vector<Image>& trainOut);

	/**
	 * Apply filter.
	 *
	 * @param[in] c
	 * 	Filter representation by chromosome.
	 * @param[in] usedBlocks
	 * 	Indexes of used blocks.
	 * @param[in] input
	 * 	Filter inputs.
	 * @return Result of filter
	 */
	uint8_t apply(const Chromosome&c, const std::set<unsigned>& usedBlocks, const std::vector<uint8_t>& inputs);



	/**
	 * Mutatates given chromosome
	 *
	 * @param[in|out] c
	 * 	Chromosome that will be mutated.
	 */
	void mutate(Chromosome& c);


};

#endif /* SRC_CGP_H_ */
