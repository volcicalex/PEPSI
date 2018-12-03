#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/CallOption.hpp"
#include "pnl/pnl_finance.h"


TEST(spot_0, SimulCall) {
	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	double T = 1.0;
	int nbTimeSteps = 365;
	double strike = 100;
	int n_samples = 50000;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);
	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
	Option *call = new CallOption(T, nbTimeSteps, size, weights, strike);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);
	double prix2 = pnl_bs_call(100, strike, T, r, 0, 0.2);

	GTEST_ASSERT_LE(abs(prix - prix2), ic);

	delete mCarlo;
}
