/**
 * Project: EDEF
 * @file CGP.cpp
 * @date 30. 4. 2018
 * @author xdocek09
 * @brief Source file for CGP (cartesian genetic programming) class.
 * Inspired from implementation provided in lecture BIN at FIT BUT.
 * https://www.fit.vutbr.cz/study/courses/BIN
 */

#include "CGP.h"

CGP::CGP(const unsigned c, const unsigned r): cols(c), rows(r){
	//-----------------------------------------------------------------------
	//Priprava pole moznych hodnot vstupu pro sloupec podle l-back a ostatnich parametru
	//-----------------------------------------------------------------------
	/*
	 * int param_in = PARAM_IN;          //pocet vstupu komb. obvodu
int param_out = PARAM_OUT;        //pocet vystupu komb. obvodu
	 */
	sloupce_val = new sl_rndval *[cols];
	for (int i = 0; i < cols; i++) {
		sloupce_val[i] = new sl_rndval;

		int minidx = rows * (i - l_back) + PARAM_IN;
		if (minidx < PARAM_IN)
			minidx = PARAM_IN; //vystupy bloku zacinaji od param_in do param_in+m*n
		int maxidx = i * rows + PARAM_IN;

		sloupce_val[i]->pocet = PARAM_IN + maxidx - minidx;
		sloupce_val[i]->hodnoty = new int[sloupce_val[i]->pocet];

		int j = 0;
		for (int k = 0; k < PARAM_IN; k++, j++) //vlozeni indexu vstupu komb. obvodu
			sloupce_val[i]->hodnoty[j] = k;
		for (int k = minidx; k < maxidx; k++, j++) //vlozeni indexu moznych vstupu ze sousednich bloku vlevo
			sloupce_val[i]->hodnoty[j] = k;
	}
};

Chromosome CGP::evolve(const unsigned generations, const unsigned runs,
			const std::vector<Image>& train, const std::vector<Image>& trainOut){




	//run evolution multiple times
	for (unsigned run=0; run < runs; run++) {

		//create initial population
		std::vector<Chromosome> population;
		population.resize(populationMax);

		int sloupec;
		/**
		 * int param_m = PARAM_M;            //pocet sloupcu
int param_n = PARAM_N;            //pocet radku
int outputidx   = param_m*sizesloupec; //index v poli chromozomu, kde zacinaji vystupy
int param_out = PARAM_OUT;        //pocet vystupu komb. obvodu
		 */
		for (unsigned i = 0; i < populationMax; i++) {
			Chromosome& actChrom = population[i];
			for (unsigned j = 0; j < param_m * param_n; j++) {
				sloupec = (int) (j / param_n);
				// vstup 1
				*p_chrom++ = sloupce_val[sloupec]->hodnoty[(rand()
						% (sloupce_val[sloupec]->pocet))];
				// vstup 2
				*p_chrom++ = sloupce_val[sloupec]->hodnoty[(rand()
						% (sloupce_val[sloupec]->pocet))];
				// funkce
				rnd = rand() % FUNCTIONS;
				*p_chrom++ = rnd;
			}
			for (int j = outputidx; j < outputidx + param_out; j++) //napojeni vystupu
				*p_chrom++ = rand() % maxidx_out;
		}
	}

}


/*** End of file: CGP.cpp ***/
