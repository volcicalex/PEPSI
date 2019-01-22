#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/BasketOption.hpp"

/**
* Programme de test pour le prix en 0 d'une option asiatique
*/

TEST(delta_0, DeltaBasket) {

	double fdStep = 0.1;
	double T = 3.0;
	double r = 0.04879;
	double strike = 100.0;
	double correlation = 0.0;
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

	double delta_attendu[40] = { 0.0248956, 0.0249324, 0.0249503, 0.0250191, 0.0249342, 0.024961, 0.0249808, 0.0248986,
		0.0249032, 0.0249421, 0.0249554, 0.0249416, 0.0249236, 0.0249541, 0.0249515, 0.0250078, 0.0248439, 0.0248783,
		0.0249421, 0.0249178, 0.0249168, 0.0249511, 0.0249393, 0.0250007, 0.0249344, 0.0248973, 0.0249136, 0.0249047,
		0.024927, 0.0248953, 0.0249739, 0.024962, 0.0249467, 0.0249322, 0.0249571, 0.0249858, 0.0248853, 0.024914,
		0.024861, 0.0250143 };
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