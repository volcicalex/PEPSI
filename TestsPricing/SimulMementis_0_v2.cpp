#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"


TEST(spot_0, SimulMementis4) {

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	double rho = 0;
	int n_samples = 50000;
	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	//PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.0600000);
	
	const char *sigmafile = "../data/vol_mementis.dat";
	PnlVect *sigma = pnl_vect_create_from_file(sigmafile);


	const char *spotfile = "../data/spot_mementis.dat";
	PnlVect *spot = pnl_vect_create_from_file(spotfile);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	/*PnlMat *rho_mat = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_mat, 1, 0);*/
	
	const char *corfile = "../data/cor_mementis.dat";
	PnlMat *rho_mat = pnl_mat_create_from_file(corfile);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_mat, sigma, spot, trend);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price(prix, ic);

	printf("Prix 0: %f \n", prix);
	printf("demi - intervalle de confiance : %f\n", ic);


	//ASSERT_TRUE(false);

	pnl_mat_free(&rho_mat);
	pnl_vect_free(&sigma);
	pnl_vect_free(&spot);
	delete mCarlo;
}