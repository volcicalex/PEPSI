#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"


/* Test du prix en t pour le fonds Mementis
 */
TEST(spot_t, SimulMementis5) {

	PnlVect *prixMementis = pnl_vect_create_from_zero(13);

	//printf("=== Donnees cas fav === \n");

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

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 1.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past1 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past1, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past1, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 2.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past2 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past2, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past2, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);


	t = 3.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past3 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past3, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past3, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);


	t = 4.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past4 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past4, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past4, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 5.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past5 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past5, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past5, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 6.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past6 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past6, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past6, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 7.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past7 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past7, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past7, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 8.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past8 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past8, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past8, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 9.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past9 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past9, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past9, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 10.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past10 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past10, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past10, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 11.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past11 = pnl_mat_create(step, mementis->size_);

	pnl_mat_extract_subblock(past11, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past11, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	t = 12.0;

	step_1 = t * nbTimeSteps / mementis->T_;
	step = floor(t * nbTimeSteps / mementis->T_);
	step = (step == step_1) ? step += 1 : step += 2;
	PnlMat *past12 = pnl_mat_create(step, mementis->size_);

	pnl_vect_set(prixMementis, (int)t, prix);

	pnl_mat_extract_subblock(past12, mementisPath, 0, step, 0, mementis->size_);

	// Calcul du prix connaissant le passe jusqu'en t
	mCarlo->price(past12, t, prix, ic);

	pnl_vect_set(prixMementis, (int)t, prix);

	//printf("t = %f \n", t);
	printf("t = %f, prix t echeance T = %f +- %f \n", t, prix, ic);
	//printf("demi - intervalle de confiance t echeance T : %f\n", ic);

	pnl_vect_print(prixMementis);

	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&trend);
	pnl_mat_free(&rho_vect);
	pnl_mat_free(&past);
	pnl_mat_free(&past1);
	pnl_mat_free(&past2);
	pnl_mat_free(&past3);
	pnl_mat_free(&past4);
	pnl_mat_free(&past5);
	pnl_mat_free(&past6);
	pnl_mat_free(&past7);
	pnl_mat_free(&past8);
	pnl_mat_free(&past9);
	pnl_mat_free(&past10);
	pnl_mat_free(&past11);
	pnl_mat_free(&past12);
	delete mCarlo;
}