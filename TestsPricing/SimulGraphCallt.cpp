#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/CallOption.hpp"
#include "pnl/pnl_finance.h"
#include <fstream>
#include <iostream>

using namespace std;


TEST(spot_t_graph, SimulCall) {
	const char *infile = "../data/simul_call.dat";
	const PnlMat *callPath = pnl_mat_create_from_file(infile);

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
	Option *call = new CallOption(T, nbTimeSteps, size, weights, strike);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0;

	double step = floor(t * nbTimeSteps / T) + 1;

	PnlMat *past = pnl_mat_create_from_scalar(1, size, 100.0000);
	pnl_mat_extract_subblock(past, callPath, 0, step, 0, size);

	string const nomFichier("../data/callt.txt");

	ofstream monFlux(nomFichier.c_str());

	if (!monFlux) {
		cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
	}

	double prix2 = pnl_bs_call(100, strike, T, r, 0, 0.2);
	printf("prix theorique : %f \n", prix2);

	for (size_t i = 0; i < 1001; i++)
	{
		mCarlo->price(past, t, prix, ic);
		monFlux << prix - prix2 << endl;
		printf("%i \n", i);
	}

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
}