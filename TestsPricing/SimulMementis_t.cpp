#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"


/* Methode de test du prix en t
 * On verifie si le prix calcule en connaissant le passe jusqu'en t=0 avec une echeance de T (deuxieme methode de asset)
 * est le meme que le prix en 0 avec une echeance de T (premiere methode asset)
 */
TEST(spot_t, SimulMementis1) {

	printf("=== Donnees cas fav === \n");

	const char *infile = "../data/mementis_casFav.dat";
	const PnlMat *mementisPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	int nbTimeSteps = 12;
	int n_samples = 50000;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	double rho = 0;
	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.100000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.0;

	double step_1 = t * nbTimeSteps / mementis->T_;
	double step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past, t, prix, ic);

	printf("t = %f \n", t);
	printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);


	FCPMementis *mementis2 = new FCPMementis(nbTimeSteps);

	PnlVect *spot2 = pnl_vect_create(mementis2->size_);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(mementis2->size_, mementis2->taux_capitalisation_, rho_vect, sigma, spot2, trend);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, mementis2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price(prix2, ic2);

	printf("prix 0 echeance T-t : %f\n", prix2);
	printf("demi - intervalle de confiance 0 echeance T-t : %f\n", ic2);

	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}


/* Methode de test du prix en t
 * On verifie si le prix calcule en connaissant le passe jusqu'en t=0 avec une echeance de T (deuxieme methode de asset)
 * est le meme que le prix en 0 avec une echeance de T (premiere methode asset)
 */
TEST(spot_t, SimulMementis2) {

	printf("=== Donnees cas defav === \n");

	const char *infile = "../data/mementis_casDefav.dat";
	const PnlMat *mementisPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	int nbTimeSteps = 12;
	int n_samples = 50000;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	double rho = 0;
	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.100000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.0;

	double step_1 = t * nbTimeSteps / mementis->T_;
	double step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past, t, prix, ic);

	printf("t = %f \n", t);
	printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);


	FCPMementis *mementis2 = new FCPMementis(nbTimeSteps);

	PnlVect *spot2 = pnl_vect_create(mementis2->size_);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(mementis2->size_, mementis2->taux_capitalisation_, rho_vect, sigma, spot2, trend);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, mementis2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price(prix2, ic2);

	printf("prix 0 echeance T-t : %f\n", prix2);
	printf("demi - intervalle de confiance 0 echeance T-t : %f\n", ic2);

	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}



/* Methode de test du prix en t
 * On verifie si le prix calcule en connaissant le passe jusqu'en t=0 avec une echeance de T (deuxieme methode de asset)
 * est le meme que le prix en 0 avec une echeance de T (premiere methode asset)
 */
TEST(spot_t, SimulMementis3) {

	printf("=== Donnees cas median1 === \n");

	const char *infile = "../data/mementis_casMedian1.dat";
	const PnlMat *mementisPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	int nbTimeSteps = 12;
	int n_samples = 50000;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	double rho = 0;
	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.100000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.0;

	double step_1 = t * nbTimeSteps / mementis->T_;
	double step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past, t, prix, ic);

	printf("t = %f \n", t);
	printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);


	FCPMementis *mementis2 = new FCPMementis(nbTimeSteps);

	PnlVect *spot2 = pnl_vect_create(mementis2->size_);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(mementis2->size_, mementis2->taux_capitalisation_, rho_vect, sigma, spot2, trend);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, mementis2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price(prix2, ic2);

	printf("prix 0 echeance T-t : %f\n", prix2);
	printf("demi - intervalle de confiance 0 echeance T-t : %f\n", ic2);

	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}


/* Methode de test du prix en t
 * On verifie si le prix calcule en connaissant le passe jusqu'en t=0 avec une echeance de T (deuxieme methode de asset)
 * est le meme que le prix en 0 avec une echeance de T (premiere methode asset)
 */
TEST(spot_t, SimulMementis4) {

	printf("=== Donnees cas median2 === \n");

	const char *infile = "../data/mementis_casMedian2.dat";
	const PnlMat *mementisPath = pnl_mat_create_from_file(infile);

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	int nbTimeSteps = 12;
	int n_samples = 50000;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);

	double rho = 0;
	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.100000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);

	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);

	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);

	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);

	double prix = 0.0;
	double ic = 0.0;
	double t = 0.0;

	double step_1 = t * nbTimeSteps / mementis->T_;
	double step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past, t, prix, ic);

	printf("t = %f \n", t);
	printf("prix t echeance T : %f\n", prix);
	printf("demi - intervalle de confiance t echeance T : %f\n", ic);


	FCPMementis *mementis2 = new FCPMementis(nbTimeSteps);

	PnlVect *spot2 = pnl_vect_create(mementis2->size_);
	pnl_mat_get_row(spot2, past, (int)(past->m - 1));

	PnlRng *rng2 = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng2, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng2, time(NULL));

	double prix2 = 0;
	double ic2 = 0;

	Model *bsmodel2 = new BlackScholesModel(mementis2->size_, mementis2->taux_capitalisation_, rho_vect, sigma, spot2, trend);
	MonteCarlo *mCarlo2 = new MonteCarlo(bsmodel2, mementis2, rng2, fdStep, n_samples);

	// Calcul du prix en 0 echeance T-t
	mCarlo2->price(prix2, ic2);

	printf("prix 0 echeance T-t : %f\n", prix2);
	printf("demi - intervalle de confiance 0 echeance T-t : %f\n", ic2);

	ASSERT_TRUE(abs(prix - prix2) / prix2 <= 0.05); // ecart relatif inf a 5%

	pnl_vect_free(&spot);
	pnl_vect_free(&spot2);
	pnl_vect_free(&sigma);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	delete mCarlo;
	delete mCarlo2;
}


