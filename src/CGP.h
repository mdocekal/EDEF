/**
 * Project: EDEF
 * @file CGP.h
 * @date 30. 4. 2018
 * @author xdocek09
 * @brief Source file for CGP (cartesian genetic programming) class.
 * Inspired from implementation provided in lecture BIN at FIT BUT.
 * https://www.fit.vutbr.cz/study/courses/BIN
 */

#ifndef SRC_CGP_H_
#define SRC_CGP_H_

#include <random>


/**
 * Representation of chromosome.
 */
typedef std::vector<int> Chromosome;

/**
 * Class for cartesian genetic programming.
 * Suitable for generating image filters.
 */
class CGP {
public:


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
	 * Evolve chromosome.
	 *
	 * @param[in] generations
	 * 	Number of generations.
	 * @param[in] runs
	 * 	Number of evolution runs.
	 * @param[in] train
	 * 	Train images that will be used as input for filter.
	 * @param[in] trainOut
	 * 	Train images that will be used for filter evaluation. (Desired result)
	 * @return Evolved chromosome of a filter.
	 */
	Chromosome evolve(const unsigned generations, const unsigned runs,
			const std::vector<Image>& train, const std::vector<Image>& trainOut);



	unsigned getCols() const {
		return cols;
	}

	void setCols(unsigned cols) {
		this->cols = cols;
	}

	unsigned getBack() const {
		return lBack;
	}

	void setBack(unsigned back = 1) {
		lBack = back;
	}

	unsigned getMutationMax() const {
		return mutationMax;
	}

	void setMutationMax(unsigned mutationMax = 3) {
		this->mutationMax = mutationMax;
	}

	unsigned getPopulationMax() const {
		return populationMax;
	}

	void setPopulationMax(unsigned populationMax = 5) {
		this->populationMax = populationMax;
	}

	unsigned getRows() const {
		return rows;
	}

	void setRows(unsigned rows) {
		this->rows = rows;
	}

private:
	unsigned cols;	//! cols in CGP matrix
	unsigned rows;	//! rows in CGP matrix
	unsigned populationMax=5;	//! maximum size of population
	unsigned mutationMax=3; //! maximum number of mutations for one mutation
	unsigned lBack=1; //! CGP lBack parameter

	static const unsigned PARAM_IN=9;	//! Number of inputs. (9-> 3x3 kernel)
	static const unsigned PARAM_OUT=1;	//! Number of outpus.

	std::mt19937 randGen;	//! Random number generator.

};

#endif /* SRC_CGP_H_ */
