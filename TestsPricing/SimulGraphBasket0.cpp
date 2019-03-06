#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/BasketOption.hpp"
#include "pnl/pnl_finance.h"
#include <fstream>
#include <iostream>

using namespace std;


TEST(spot_graph_0, SimulBasket0) {
	int size = 40;
	double strike = 100;
	double T = 3;
	double r = 0.04879;
	double correlation = 0.0;
	int timestep = 1;
	int n_samples = 50000;

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.025);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, correlation);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *bOption = new BasketOption(T, timestep, size, payoff_coef, strike);
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	//
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, bOption, rng, fdStep, n_samples);

	string const nomFichier("../data/basket0.txt");

	ofstream monFlux(nomFichier.c_str());

	if (!monFlux) {
		cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
	}

	double prix = 0.0;
	double ic = 0.0;
	printf("prix theorique : 13.627 \n");

	for (size_t i = 0; i < 1001; i++)
	{
		mCarlo->price(prix, ic);
		monFlux << prix - 13.627 << endl;
		printf("%i \n", i);
	}

	delete mCarlo;
}