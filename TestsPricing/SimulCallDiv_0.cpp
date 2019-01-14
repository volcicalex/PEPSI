#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/CallOption.hpp"
#include "pnl/pnl_finance.h"


TEST(spot_0, SimulCallDiv) {
	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	double T = 12.0;
	int nbTimeSteps = 12;
	double strike = 100;
	int n_samples = 50000;

	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);
	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	//Model *bsmodel = new BlackScholesModel(size, r, rho, sigma, spot, trend);
	Option *call = new CallOption(T, nbTimeSteps, size, weights, strike);
	//MonteCarlo *mCarlo = new MonteCarlo(bsmodel, call, rng, fdStep, n_samples);

	//PnlVect *dividendes = pnl_vect_create_from_scalar(12, 0.01);
	double prix = 0.0;
	double ic = 0.0;

	//mCarlo->price(dividendes, prix, ic);

	//printf("prix asset %f \n", prix);

	/* Prix call versant dividende discret fixe */

	// Formule fermee du prix du call versant des dividendes en temps discret
	double S_0 = GET(spot, 0);
	double sigma_ = GET(sigma, 0);
	double delta = 0.01;
	double F = S_0 * pow((1 - delta), 12) * exp(r * T);

	// Parametres pour utiliser la methode de calcul de la fonction de repartition de la loi normale
	int which = 1;
	double p = 0;
	double q = 0;
	double d1 = (1/(sigma_*sqrt(T))) * (log(S_0/strike) + (r + 0.5 * pow(sigma_, 2))*T);
	double d2 = d1 - sigma_ * sqrt(T);
	double mean = 0;
	double sd = 1;
	int status = 0;
	double bound = 0;

	// Calcul de N(d1)
	pnl_cdf_nor(&which, &p, &q, &d1, &mean, &sd, &status, &bound);
	double N_d1 = p;

	// Calcul de N(d2)
	pnl_cdf_nor(&which, &p, &q, &d2, &mean, &sd, &status, &bound);
	double N_d2 = p;

	//Verification de N(d1) et N(d2) par le calcul du prix d'un call classique
	double verif = S_0 * N_d1 - strike * exp(-r * T) * N_d2;
	double verif2 = pnl_bs_call(100, strike, T, r, 0, 0.2);
	GTEST_ASSERT_LE(abs(verif-verif2), 0.005);

	// Formule fermee pour call versant dividende discret fixe
	double prix2 = exp(-r * T)*(F*N_d1 - strike * N_d2);
	printf("prix call avec div %f \n", prix2);
	//delete mCarlo;
}
