#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/AsianOption.hpp"


/**
* Programme de test pour le prix en 0 d'une option asiatique
* non parallelise
*/
TEST(spot_0_simple, SimulAsian) {

	int size = 2;
	double strike = 100;	
	double T = 1.5;
	double r = 0.02;
	double correlation = 0.0;
	int timestep = 150;
	int n_samples = 500000;

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.5);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, correlation);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *aOption = new AsianOption(T, timestep, size, payoff_coef, strike);
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, aOption, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;

	clock_t t1 = clock();

	mCarlo->price_simple(prix, ic);

	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);

	//printf("prix %f, ic %f \n", prix, ic);

	//ASSERT_LE(4.67 - ic, prix) << "Error, price at t=0 not in confidence interval, too low";
	//ASSERT_GE(4.67 + ic, prix) << "Error, price at t=0 not in confidence interval, too high";
	ASSERT_TRUE(abs(prix - 4.67) / 4.67 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&payoff_coef);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	delete mCarlo;
}

/**
* Programme de test pour le prix en 0 d'une option asiatique
* parallelise
*/
TEST(spot_0_opm, SimulAsian_opm) {

	int size = 2;
	double strike = 100;
	double T = 1.5;
	double r = 0.02;
	double correlation = 0.0;
	int timestep = 150;
	int n_samples = 500000;

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	PnlVect *spot = pnl_vect_create_from_scalar(size, 100);
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.2);
	PnlVect *payoff_coef = pnl_vect_create_from_scalar(size, 0.5);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, correlation);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *aOption = new AsianOption(T, timestep, size, payoff_coef, strike);
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, aOption, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;

	clock_t t1 = clock();

	mCarlo->price(prix, ic);

	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);

	//printf("prix : %f, ic : %f \n", prix, ic);

	//ASSERT_LE(4.67 - ic, prix) << "Error, price at t=0 not in confidence interval, too low";
	//ASSERT_GE(4.67 + ic, prix) << "Error, price at t=0 not in confidence interval, too high";
	//ASSERT_TRUE(abs((ic / 1.96) - 0.029) / 0.029 <= 0.05); // ecart relatif inf a 5%
	ASSERT_TRUE(abs(prix - 4.67) / 4.67 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&payoff_coef);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	delete mCarlo;
}