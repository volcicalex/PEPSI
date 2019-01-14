#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/BasketOption.hpp"

/**
* Programme de test pour le prix en 0 d'une option asiatique
*/

TEST(delta_0, DeltaBasket) {

	double fdStep = 0.1;
	double T = 3;
	double r = 0.04879;
	double strike = 100;
	double correlation = 0;
	int size = 40;
	int nbTimeSteps = 1;
	int n_samples = 5000;

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.0);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.025);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	PnlMat *past = pnl_mat_create_from_scalar(1, size, 100);
	PnlVect *delta = pnl_vect_create(size);
	PnlVect *conf_delta = pnl_vect_create(size);

	Model *bsmodel = new BlackScholesModel(size, r, correlation, sigma, spot, trend);
	Option *aOption = new BasketOption(T, nbTimeSteps, size, payoff_coef, strike);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, aOption, rng, fdStep, n_samples);

	mCarlo->delta(past, 0, delta, conf_delta);

	for (int i = 0; i < size; i++) {
		printf("Delta actif %u: %f\n", i + 1, GET(delta, i));
		printf("Standard Deviation actif %u: %f\n", i + 1, GET(conf_delta, i));
	}

	delete mCarlo;
}