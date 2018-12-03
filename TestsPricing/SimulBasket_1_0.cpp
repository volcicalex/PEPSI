#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/BasketOption.hpp"

/**
* Programme de test pour le prix en 0 d'une option asiatique
*/

TEST(spot_0, SimulBasket_1) {

	int size = 40;
	double strike = 100;
	double T = 3;
	double r = 0.04879;
	double correlation = 0.0;
	int timestep = 1;
	int n_samples = 50000;

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.025);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	BlackScholesModel *bsmodel = new BlackScholesModel(size, r, correlation, sigma, spot, trend);
	Option *bOption = new BasketOption(T, timestep, size, payoff_coef, strike);
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	//
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, bOption, rng, fdStep, n_samples);
	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);
	ASSERT_LE(13.616294 - ic, prix) << "Error, price at t=0 not in confidence interval, too low";
	ASSERT_GE(13.616294 + ic, prix) << "Error, price at t=0 not in confidence interval, too high";
	//printf("ic carr %f \n", (pow(ic,2) * n_samples) / pow(1.96,2));
	//printf("ecar rel %f \n", (((pow(ic,2) * n_samples) / pow(1.96,2))-32.053411)/32.053411);
	ASSERT_TRUE(abs((((pow(ic, 2) * n_samples) / pow(1.96, 2)) - 32.053411) / 32.053411) <= 0.05); // ecart relatif inf a 5%

	delete mCarlo;
}