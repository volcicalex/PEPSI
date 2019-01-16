#include "pch.h"

#include "../Win32Pricing/src/Couverture.hpp"
#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/PerformanceOption.hpp"

/**
* Programme de test pour la couverture d'une option performance
*/
TEST(PnL, pnlPerf) {

	const char *infile = "../data/market_perf.dat";
	PnlMat *Path = pnl_mat_create_from_file(infile);

	double fdStep = 0.1;

	int size = 4;
	int nbTimeSteps = 26;
	int n_samples = 50000;
	int H = 104;

	double r = 0.3;
	double rho = 0.3;
	double T = 2;
	double pnl = 0;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_scalar(size, 0.1);
	PnlVect *weights = pnl_vect_create(size);
	pnl_vect_set(weights, 0, 0.5);
	pnl_vect_set(weights, 1, 0.125);
	pnl_vect_set(weights, 2, 0.125);
	pnl_vect_set(weights, 3, 0.25);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *call = new PerformanceOption(T, nbTimeSteps, size, weights);

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	mCarlo->pnl(pnl, Path);
	printf("P&L: %f\n", pnl);

	pnl_mat_free(&Path);

	delete mCarlo;
}