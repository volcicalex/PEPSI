#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/AsianOption.hpp"


/* Methode 1 de test du prix en t
 * On calcule le prix calcule en connaissant le passe jusqu'en t avec une echeance de T (deuxieme methode de asset)
 */
/* pas de sens pour asiatique : path dependant 
TEST(spot_t, SimulAsian) {

	printf("=== Methode 1 === \n");

	const char *infile = "../data/simul_asian.dat";
	const PnlMat *asianPath = pnl_mat_create_from_file(infile);

	double fdStep = 1; // valeur quelconque car non utilisee pour ce test

	int size = 2;
	double r = 0.02;
	double rho = 0;
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	double T = 1.500000;
	int nbTimeSteps = 450;
	double strike = 90;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	int n_samples = 50000;

	PnlVect *weights = pnl_vect_create_from_scalar(size, 0.5);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *asian = new AsianOption(T, nbTimeSteps, size, weights, strike);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, asian, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.5;

	double step_1 = t * nbTimeSteps / T;
	double step = floor(t * nbTimeSteps / T);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past = pnl_mat_create(step, size);
	pnl_mat_extract_subblock(past, asianPath, 0, step, 0, size);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price_simple(asianPath, t, prix, ic);

	printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	delete mCarlo;
}*/

/* Methode 2 de test du prix en t
 * On verifie si le prix calcule en connaissant le passe jusqu'en t=0 avec une echeance de T (deuxieme methode de asset)
 * est le meme que le prix en 0 avec une echeance de T (premiere methode asset)
 * non parallelise
 */
TEST(spot_t_simple, SimulAsian2) {

	//printf("=== Methode 2 === \n");

	const char *infile = "../data/simul_asian.dat";
	const PnlMat *asianPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	double T = 1.0;
	int nbTimeSteps = 365;
	double strike = 100;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	int n_samples = 50000;

	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *asian = new AsianOption(T, nbTimeSteps, size, weights, strike);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, asian, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0;

	double step = floor(t * nbTimeSteps / T) + 1;

	PnlMat *past = pnl_mat_create_from_scalar(1, size, 100.0000);
	pnl_mat_extract_subblock(past, asianPath, 0, step, 0, size);

	// Calcul du prix connaissant le passe jusqu'en t
	
	clock_t t1 = clock();

	mCarlo->price_simple(past, t, prix, ic);

	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);

	/*printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);*/

	PnlVect *spot2 = pnl_vect_create(size);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(size, r, rho_vect, sigma, spot2, trend);
	Option *asian2 = new AsianOption(T - t, nbTimeSteps - step, size, weights, strike);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, asian2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price_simple(prix2, ic2);

	/*printf("prix 0 echeance T-t : %f\n", prix2);
	printf("demi - intervalle de confiance 0 echeance T-t : %f\n", ic2);*/

	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}

/* Methode 2 de test du prix en t avec Open MP
 * On verifie si le prix calcule en connaissant le passe jusqu'en t=0 avec une echeance de T (deuxieme methode de asset)
 * est le meme que le prix en 0 avec une echeance de T (premiere methode asset)
 * open mp
 */
TEST(spot_t_opm, SimulAsian2_opm) {

	//printf("=== Methode 2 === \n");

	const char *infile = "../data/simul_asian.dat";
	const PnlMat *asianPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test

	int size = 1;
	double r = 0.02;
	double rho = 0;
	PnlVect *sigma = pnl_vect_create_from_scalar(size, 0.200000);
	PnlVect *spot = pnl_vect_create_from_scalar(size, 100.000000);
	PnlVect *trend = pnl_vect_create_from_zero(size);

	double T = 1.0;
	int nbTimeSteps = 365;
	double strike = 100;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	int n_samples = 50000;

	PnlVect *weights = pnl_vect_create_from_scalar(size, 1.0);

	PnlMat *rho_vect = pnl_mat_create_from_scalar(size, size, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	Model *bsmodel = new BlackScholesModel(size, r, rho_vect, sigma, spot, trend);
	Option *asian = new AsianOption(T, nbTimeSteps, size, weights, strike);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, asian, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0;

	double step = floor(t * nbTimeSteps / T) + 1;

	PnlMat *past = pnl_mat_create_from_scalar(1, size, 100.0000);
	pnl_mat_extract_subblock(past, asianPath, 0, step, 0, size);

	// Calcul du prix connaissant le passe jusqu'en t
	clock_t t1 = clock();

	mCarlo->price(past, t, prix, ic);

	clock_t t2 = clock();
	float temps = (float)(t2 - t1) / CLOCKS_PER_SEC;
	printf("temps = %f\n", temps);

	/*printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);*/

	PnlVect *spot2 = pnl_vect_create(size);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(size, r, rho_vect, sigma, spot2, trend);
	Option *asian2 = new AsianOption(T - t, nbTimeSteps - step, size, weights, strike);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, asian2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price(prix2, ic2);

	/*printf("prix 0 echeance T-t : %f\n", prix2);
	printf("demi - intervalle de confiance 0 echeance T-t : %f\n", ic2);*/

	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&weights);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}

