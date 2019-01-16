#include "pch.h"

#include "../Win32Pricing/src/Couverture.hpp"
#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/AsianOption.hpp"

TEST(PnL, pnlAsian) {

	const char *infile = "../data/market_asian.dat";
	PnlMat *Path = pnl_mat_create_from_file(infile);

	double fdStep = 0.1;

	int size = 1;
	int nbTimeSteps = 26;
	int n_samples = 100000;
	int H = 104;

	double r = 0.03;
	double rho = 0.3;
	double T = 2;
	double strike = 100;
	double pnl = 0;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_scalar(size, 0.1);
	PnlVect *weights = pnl_vect_create_from_scalar(1, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *option = new AsianOption(T, nbTimeSteps, size, weights, strike);

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, option, rng, fdStep, n_samples);

	mCarlo->pnl(pnl, Path);
	printf("P&L: %f\n", pnl);

	pnl_mat_free(&Path);

	delete mCarlo;
}