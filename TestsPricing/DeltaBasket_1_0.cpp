#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/BasketOption.hpp"

/**
* Programme de test pour le prix en 0 d'une option asiatique
*/

TEST(delta_0, DeltaBasket_2) {

	double fdStep = 0.1;
	double T = 1.0;
	double r = 0.04879;
	double strike = 100.0;
	double correlation = 0.7;
	int size = 40;
	int nbTimeSteps = 1;
	int n_samples = 50000;

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.0);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.025);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlMat *rho = pnl_mat_create_from_scalar(size, size, correlation);
	pnl_mat_set_diag(rho, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	PnlMat *past = pnl_mat_create_from_scalar(1, size, 100);
	PnlVect *delta = pnl_vect_create(size);
	PnlVect *conf_delta = pnl_vect_create(size);

	Model *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
	Option *aOption = new BasketOption(T, nbTimeSteps, size, payoff_coef, strike);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, aOption, rng, fdStep, n_samples);

	mCarlo->delta(past, 0, delta, conf_delta);

	double delta_attendu[40] = { 0.0162094, 0.01621, 0.01622, 0.0162203, 0.0162145, 0.0162044, 0.016215, 0.0162163, 
		0.0161995, 0.0161993, 0.0162085, 0.0161999, 0.0161848, 0.0161824, 0.0162018, 0.0161994, 0.0161971, 0.0162066, 
		0.0162119, 0.0162086, 0.0162088, 0.0162014, 0.0162149, 0.0162236, 0.0162311, 0.0162138, 0.0162064, 0.0162129, 
		0.0162166, 0.0162183, 0.0161925, 0.0162276, 0.016189, 0.0161958, 0.0162017, 0.0162142, 0.0162028, 0.0162092, 
		0.0162104, 0.0162025 };
	int delta_reussi = 0;

	for (int i = 0; i < size; i++) {
		printf("Delta actif %u: %f --- Delta attendu %u: %f\n", i + 1, GET(delta, i), i + 1, delta_attendu[i]);
		printf("Standard Deviation actif %u: %f\n", i + 1, GET(conf_delta, i));
		if ((delta_attendu[i] - GET(conf_delta, i) < GET(delta, i)) &&
			(delta_attendu[i] + GET(conf_delta, i) > GET(delta, i))) {
			delta_reussi++;
			printf("Delta reussi\n\n");
		}
		else printf("Delta echoue\n\n");
	}

	printf("Nombre de deltas reussis: %i\n\n", delta_reussi);
	ASSERT_GE(delta_reussi, 0.5 * size) << "Error, delta at t=0 not in confidence interval";

	delete mCarlo;
}