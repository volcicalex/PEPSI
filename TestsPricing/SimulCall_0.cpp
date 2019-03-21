#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/CallOption.hpp"
#include "pnl/pnl_finance.h"

/**
* Programme de test pour le prix en 0 d'une option call
* non parallelise
*/
TEST(spot_0_simple, SimulCall) {
	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	double T = 1.0;
	int nbTimeSteps = 365;
	double strike = 100;
	int n_samples = 500000;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);
	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *call = new CallOption(T, nbTimeSteps, size, weights, strike);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;


	clock_t t1 = clock();

	mCarlo->price_simple(prix, ic);

	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);

	double prix2 = pnl_bs_call(100, strike, T, r, 0, 0.2);
	//printf("prix %f, ic %f \n", prix, ic);
	//printf("prix2 %f \n", prix2);
	//GTEST_ASSERT_LE(abs(prix - prix2), ic);
	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	delete mCarlo;
}

/**
* Programme de test pour le prix en 0 d'une option call
* parallelise
*/
TEST(spot_0_opm, SimulCall_opm) {
	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	double T = 1.0;
	int nbTimeSteps = 365;
	double strike = 100;
	int n_samples = 500000;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);
	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *call = new CallOption(T, nbTimeSteps, size, weights, strike);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	/*
	double prix3 = 0.0;
	double ic3 = 0.0;
	*/
	double prix2 = pnl_bs_call(100, strike, T, r, 0, 0.2);
	/*
	for (int i = 0; i < 5; i++)
	{
		printf("prix th %f\n", prix2);
		mCarlo->price(prix, ic);
		printf("prix opm %f, ic %f, abs %f \n", prix, ic, abs(prix - prix2));
		mCarlo->price(prix3, ic3);
		printf("prix %f, ic %f, abs %f \n", prix3, ic3, abs(prix3 - prix2));
	}*/

	clock_t t1 = clock();

	mCarlo->price(prix, ic);

	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);

	//printf("prix %f, ic %f \n", prix, ic);
	//printf("prix %f \n", prix2);
	//GTEST_ASSERT_LE(abs(prix - prix2), ic);
	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	delete mCarlo;
}
