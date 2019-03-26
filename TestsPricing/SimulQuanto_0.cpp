#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/QuantoOption.hpp"
#include "pnl/pnl_finance.h"

/**
* Programme de test pour le prix en 0 d'une option call
* non parallelise
*/
TEST(spot_0_simple, SimulQuanto) {
	
	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double rd = 0.036;
	double rf = 0.04879;
	double rho = 0.1;
	double T = 3.0;
	int nbTimeSteps = 1;
	double strike = 100;
	int n_samples = 50000;
	
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.20);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_scalar(2, rd);
	pnl_vect_set(trend, 1, rf);
	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size+1, size+1, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	QuantoOption *quanto = new QuantoOption(T, nbTimeSteps, size, rf,weights, strike);

	/*! nombre d'actifs par marché */
	PnlVectInt *nbAssetsPerMarket_ = pnl_vect_int_create(2);
	pnl_vect_int_set(nbAssetsPerMarket_, 0, 0);
	pnl_vect_int_set(nbAssetsPerMarket_, 1, 1);

	PnlVect *sigmaChangeRate = pnl_vect_create_from_scalar(1, 0.20);
	PnlVect *spotChangeRate = pnl_vect_create_from_scalar(1, 1.05);
	PnlVect *spotRiscklessAsset = pnl_vect_create_from_scalar(1, 1);

	// Initialisation du modele de BS qui integre les taux de change
	BlackScholesModel *bsmodel_CR = new BlackScholesModel(1, 2, quanto->size_, nbAssetsPerMarket_, sigmaChangeRate, rd, rho_vect, sigma, spot, spotChangeRate, spotRiscklessAsset, trend);
	
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel_CR, quanto, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;


	clock_t t1 = clock();
	mCarlo->price_simple(prix, ic);
	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);
	printf("prix Monte Carlo : %f, ic %f \n", prix, ic);

	// Initialisation du modele de BS qui integre les taux de change
	Model *bsmodel = new BlackScholesModel(size, rd, rho_vect, sigma, spot, trend);
	double prix3 = quanto->prixExact0(bsmodel, rho);
	//double prix2 = pnl_bs_call(100, strike, T, r, 0, 0.2);
	printf("prix exact : %f \n", prix3);
	printf("rapport des prix : %f \n", prix/prix3);
	//GTEST_ASSERT_LE(abs(prix - prix2), ic);
	//ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	delete mCarlo;
}


