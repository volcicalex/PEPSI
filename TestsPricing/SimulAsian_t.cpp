#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/AsianOption.hpp"

TEST(spot_t, SimulAsian) {

	const char *infile = "../data/simul_asian.dat";
	const PnlMat *asianPath = pnl_mat_create_from_file(infile);

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	int size = 2;
	double r = 0.02;
	double rho = 0;
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	double T = 1.500000;
	int nbTimeSteps = 450;
	double strike = 90;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	int n_samples = 50000;

	PnlVect *weights = pnl_vect_create_from_scalar(size, 0.5);

	Model *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
	Option *asian = new AsianOption(T, nbTimeSteps, size, weights, strike);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, asian, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.5;

	mCarlo->price(asianPath, t, prix, ic);

	printf("prix t %f\n", prix);
	printf("demi - intervalle de confiance t %f\n", ic);

	delete mCarlo;
}
