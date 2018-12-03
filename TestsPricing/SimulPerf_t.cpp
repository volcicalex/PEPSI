#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/PerformanceOption.hpp"

TEST(spot_t, SimulPerf) {
	const char *infile = "../data/simul_perf.dat";
	const PnlMat *perfPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 5;
	double r = 0.02;
	double rho = 0.5;
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	double T = 2.0;
	int nbTimeSteps = 96;
	double strike = 100;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	int n_samples = 50000;

	PnlVect *weights = pnl_vect_create_from_scalar(size, 0.2);

	BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
	Option *perf = new PerformanceOption(T, nbTimeSteps, size, weights);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, perf, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.1;

	mCarlo->price(perfPath, t, prix, ic);

	printf("prix t %f\n", prix);
	printf("demi - intervalle de confiance t %f\n", ic);

	delete mCarlo;
}
