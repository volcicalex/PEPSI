#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/AsianOption.hpp"

/**
* Programme de test pour le prix en 0 d'une option asiatique
*/

TEST(delta_0, DeltaAsian) {

	double fdStep = 0.1;
	double T = 1.5;
	double r = 0.02;
	double strike = 100;
	double correlation = 0;
	int size = 2;
	int nbTimeSteps = 150;
	int n_samples = 50000;

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.0);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.5);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlMat *rho = pnl_mat_create_from_scalar(size, size, correlation);
	pnl_mat_set_diag(rho, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	PnlMat *past = pnl_mat_create_from_scalar(1, 2, 100);
	PnlVect *delta = pnl_vect_create(2);
	PnlVect *conf_delta = pnl_vect_create(2);

	Model *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
	Option *aOption = new AsianOption(T, nbTimeSteps, size, payoff_coef, strike);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, aOption, rng, fdStep, n_samples);

	mCarlo->delta(past, 0, delta, conf_delta);

	printf("Delta actif %u: %f\n", 1, GET(delta, 0));
	printf("Standard Deviation actif %u: %f\n", 1, GET(conf_delta, 0));

	ASSERT_LE(0.281640 - GET(conf_delta, 0), GET(delta, 0)) << "Error, delta of first option not in confidence interval, too low";
	ASSERT_GE(0.281640 + GET(conf_delta, 0), GET(delta, 0)) << "Error, delta of first option not in confidence interval, too high";

	printf("Delta actif %u: %f\n", 2, GET(delta, 1));
	printf("Standard Deviation actif %u: %f\n", 2, GET(conf_delta, 1));

	ASSERT_LE(0.281951 - GET(conf_delta, 1), GET(delta, 1)) << "Error, delta of second option not in confidence interval, too low";
	ASSERT_GE(0.281951 + GET(conf_delta, 1), GET(delta, 1)) << "Error, delta of second option not in confidence interval, too high";

	delete mCarlo;
}