#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"


TEST(spot_0, SimulMementis) {

	printf("Vol a 0.2");

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	double rho = 0;
	int n_samples = 50000;
	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.200000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);

	printf("Prix 0: %f \n", prix);
	printf("demi - intervalle de confiance : %f\n", ic);


	//ASSERT_TRUE(false);

	delete mCarlo;
}

TEST(spot_0, SimulMementis2) {

	printf("Vol a 0.1");

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	double rho = 0;
	int n_samples = 50000;
	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.100000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);

	printf("Prix 0: %f \n", prix);
	printf("demi - intervalle de confiance : %f\n", ic);


	//ASSERT_TRUE(false);

	delete mCarlo;
}
