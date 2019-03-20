#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/BasketOption.hpp"
#include <fstream>
#include <iostream>

using namespace std;


TEST(spot_t_graph, SimulBasket) {
	const char *infile = "../data/simul_basket.dat";
	const PnlMat *basketPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	double T = 1.0;
	int nbTimeSteps = 365;
	double strike = 100;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	int n_samples = 50000;

	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *basket = new BasketOption(T, nbTimeSteps, size, weights, strike);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, basket, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0;

	double step = floor(t * nbTimeSteps / T) + 1;

	PnlMat *past = pnl_mat_create_from_scalar(1, size, 100.0000);
	pnl_mat_extract_subblock(past, basketPath, 0, step, 0, size);

	PnlVect *spot2 = pnl_vect_create(size);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(size, r, rho_vect, sigma, spot2, trend);
	Option *basket2 = new BasketOption(T - t, nbTimeSteps - step, size, weights, strike);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, basket2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price(prix2, ic2);

	string const nomFichier("../data/baskett.txt");

	ofstream monFlux(nomFichier.c_str());

	if (!monFlux) {
		cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
	}


	printf("prix theorique : %f \n", prix2);

	for (size_t i = 0; i < 1001; i++)
	{
		mCarlo->price(past, t, prix, ic);
		monFlux << prix - prix2 << endl;
		printf("%i \n", i);
	}

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}