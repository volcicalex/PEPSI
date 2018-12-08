#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/PerformanceOption.hpp"

/**
* Programme de test pour le prix en 0 d'une option asiatique
*/

TEST(spot_0, SimulPerformance) {

	int size = 5;
	double T = 2.0;
	double r = 0.03;
	double correlation = 0.5;
	int timestep = 12;
	int n_samples = 50000;

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	Model *bsmodel = new BlackScholesModel(size, r, correlation, sigma, spot, trend);
	Option *pOption = new PerformanceOption(T, timestep, size, payoff_coef);
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, pOption, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);

	ASSERT_LE(1.257353 - ic, prix) << "Error, price at t=0 not in confidence interval, too low";
	ASSERT_GE(1.257353 + ic, prix) << "Error, price at t=0 not in confidence interval, too high";
	ASSERT_TRUE(abs(ic / 1.96 - 0.000587) / 0.000587 <= 0.05); // erreur relative inf a 5%

	delete mCarlo;
}