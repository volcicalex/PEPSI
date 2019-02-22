#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/PerformanceOption.hpp"
#include "pnl/pnl_finance.h"
#include <fstream>
#include <iostream>

using namespace std;


TEST(spot_0_graph, SimulPerf0) {
	
	int size = 5;
	double T = 2.0;
	double r = 0.03;
	double correlation = 0.5;
	int timestep = 12;
	int n_samples = 50000;

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, correlation);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *pOption = new PerformanceOption(T, timestep, size, payoff_coef);
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, pOption, rng, fdStep, n_samples);

	string const nomFichier("../data/perf0.txt");

	ofstream monFlux(nomFichier.c_str());

	if (!monFlux) {
		cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
	}

	double prix = 0.0;
	double ic = 0.0;
	printf("prix theorique : 1.257353 \n");

	for (size_t i = 0; i < 1001; i++)
	{
		mCarlo->price(prix, ic);
		monFlux << prix - 1.257353 << endl;
		printf("%i \n", i);
	}

	delete mCarlo;
}