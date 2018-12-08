#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"


TEST(spot_0, SimulMementis) {
	double fdStep = 1;  //valeur quelconque car non utilisee pour ce tes

	double r = 0.02;
	double rho = 0;
	int n_samples = 50000;

	Option *mementis = new FCPMementis(n_samples);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(mementis->size_, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	Model *bsmodel = new BlackScholesModel(mementis->size_, r, rho, sigma, spot, trend);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);

	printf("Prix 0: %f", prix);

	ASSERT_TRUE(false);

	delete mCarlo;
}
