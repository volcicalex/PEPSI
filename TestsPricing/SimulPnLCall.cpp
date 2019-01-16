#include "pch.h"

#include "../Win32Pricing/src/Couverture.hpp"
#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/CallOption.hpp"

/**
* Programme de test pour la couverture d'un Call
*/
TEST(SimulPnL, PnLCall) {

	double fdStep = 0.1;

	int size = 1;
	int nbTimeSteps = 25;
	int n_samples = 50000;
	int H = 150;

	double r = 0.05;
	double rho = 0;
	double T = 0.5;
	double strike = 100;
	double pnl = 0;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_scalar(size, 0.05);
	PnlVect *weights = pnl_vect_create_from_scalar(1, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	BlackScholesModel *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *call = new CallOption(T, nbTimeSteps, size, weights, strike);

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	PnlMat *Path = pnl_mat_create(H + 1, size);
	pnl_mat_set_row(Path, spot, 0);
	bsmodel->simul_market(Path, T, H, rng);

	mCarlo->pnl(pnl, Path);
	printf("P&L: %f\n", pnl);

	pnl_mat_free(&Path);

	delete mCarlo;
}