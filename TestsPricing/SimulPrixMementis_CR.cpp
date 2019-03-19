#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"

/* Tests les dividendes reçus par l'investisseur dans le "cas defavorable" (cf brochure) */
TEST(Mementis, EgalitePrix) {

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	double rho = 0;
	int n_samples = 10;
	int nbTimeSteps = 12;

	FCPMementis *mementis = new FCPMementis(nbTimeSteps);
	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.0600000);

	// Donnees recuperees de l'exemple Cas favorable dans la brochure
	PnlVect *spot = pnl_vect_create_from_list(25, 1215.38, 965.75, 4870.73, 21760.2, 1680.67, 2688.74, 674.78, 668.82, 225.17, 5827.98, 108.02,
		154.13, 657.3, 103.18, 4307.29, 3862.34, 281.54, 6769.75, 24295.89, 4431.66, 1021.74, 448.29, 2224.68, 1545.99, 1272.41);


	
	/*! nombre d'actifs par marché */
	PnlVectInt *nbAssetsPerMarket_ = pnl_vect_int_create(3);
	pnl_vect_int_set(nbAssetsPerMarket_, 0, 15);
	pnl_vect_int_set(nbAssetsPerMarket_, 1, 5);
	pnl_vect_int_set(nbAssetsPerMarket_, 2, 5);

	mementis->nbAssetsPerMarket_ = nbAssetsPerMarket_;

	/*! vecteur des tendances qu'on utilise pour stocker les taux sans risque des marches etrangers */
	PnlVect *trend_ = pnl_vect_create(3);

	pnl_vect_set(trend_, 0, mementis->taux_capitalisation_);
	pnl_vect_set(trend_, 1, 0.01);
	pnl_vect_set(trend_, 2, 0.01);

	mementis->trend_ = trend_;

	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));

	int nbAssets = 25;
	int nbMarkets = 3;

	PnlMat *rho_CR = pnl_mat_create_from_scalar(mementis->size_ + nbMarkets, mementis->size_+ nbMarkets, rho);
	pnl_mat_set_diag(rho_CR, 1, 0);

	PnlVect *sigmaChangeRate = pnl_vect_create_from_scalar(2, 0.06);
	PnlVect *spotChangeRate = pnl_vect_create_from_scalar(2,1.2);
	PnlVect *spotRiscklessAsset = pnl_vect_create_from_scalar(2, 1.2);
	

	// Initialisation du modele de BS sans taux de changes
	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend_);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);
	double prix = 0.0;
	double ic = 0.0;
	//mCarlo->price(prix, ic);
	printf("Prix 0: %f \n", prix);
	printf("demi - intervalle de confiance : %f\n", ic);


	// Initialisation du modele de BS qui integre les taux de change
	BlackScholesModel *bsmodel_CR = new BlackScholesModel(nbAssets, nbMarkets, mementis->size_, nbAssetsPerMarket_, sigmaChangeRate, mementis->taux_capitalisation_, rho_CR, sigma, spot, spotChangeRate, spotRiscklessAsset, trend_);
	MonteCarlo *mCarlo_CR = new MonteCarlo(bsmodel_CR, mementis, rng, fdStep, n_samples);
	double prix_CR = 0.0;
	double ic_CR = 0.0;
	mCarlo_CR->price(prix_CR, ic_CR);
	printf("Prix_CR 0: %f \n", prix_CR);
	printf("demi - intervalle de confiance_CR : %f\n", ic_CR);

	

	
	//ASSERT_LE(prix, prix_CR + 0.001);
	//ASSERT_LE(ic, ic_CR + 0.001);
}