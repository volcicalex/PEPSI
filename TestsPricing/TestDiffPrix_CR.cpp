#include "pch.h"

#include "../Win32Pricing/src/BlackScholesModel.hpp"
#include "../Win32Pricing/src/MonteCarlo.hpp"
#include "../Win32Pricing/src/FCPMementis.hpp"
#include "pnl/pnl_finance.h"

/* Tests les dividendes reçus par l'investisseur dans le "cas defavorable" (cf brochure) */
TEST(Mementis1, EgalitePrix) {

	double fdStep = 1;  //valeur quelconque car non utilisee pour ce test
	double rho = 0.3;
	int n_samples = 50000;
	int nbTimeSteps = 12;

	// produit mementis avec des actions domestiques.
	FCPMementis *mementis = new FCPMementis(nbTimeSteps);
	mementis->nbMarkets_ = 1;
	PnlVect *sigma = pnl_vect_create_from_scalar(mementis->size_, 0.0600000);
	
	// spots des actions qui composent le panier classés : EUR/USD/JPY/GBX
	 
	PnlVect *spot = pnl_vect_create_from_list(25, 10, 10.51, 38.21, 15.68, 10.5, 15.70, 14.41, 2.61, 2.76, 23.71, 84.84, 10.78, 26.19, 10.40, 42.04, 58.95, 192.91, 5.5, 3091.5, 2624.5, 6753.5, 30700.5, 708.5, 6351.5, 346.60);
	
	/*! vecteur des tendances qu'on utilise pour stocker les taux sans risque des marches etrangers */
	
	PnlVect *trend = pnl_vect_create_from_zero(mementis->size_);
	mementis->trend_ = trend;

	PnlMat *rho_vect = pnl_mat_create_from_scalar(mementis->size_, mementis->size_, rho);
	pnl_mat_set_diag(rho_vect, 1, 0);

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_init(rng, PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL));



	// produit mementis avec des actions multidevises
	FCPMementis *mementisCR = new FCPMementis(nbTimeSteps);
	/*! nombre d'actifs par marché */
	PnlVectInt *nbAssetsPerMarket_ = pnl_vect_int_create(4);
	pnl_vect_int_set(nbAssetsPerMarket_, 0, 13);
	pnl_vect_int_set(nbAssetsPerMarket_, 1, 5);
	pnl_vect_int_set(nbAssetsPerMarket_, 2, 5);
	pnl_vect_int_set(nbAssetsPerMarket_, 3, 2);


	mementisCR->trend_ = trend;
	mementisCR->nbAssetsPerMarket_ = nbAssetsPerMarket_;
	int nbAssets = 25;
	int nbMarkets = 4;
	mementisCR->nbMarkets_ = nbMarkets;
	PnlMat *rho_CR = pnl_mat_create_from_scalar(mementis->size_ + nbMarkets - 1, mementis->size_ + nbMarkets - 1, rho);
	pnl_mat_set_diag(rho_CR, 1, 0);


	PnlVect *risklessrates = pnl_vect_create(4);
	double tauxJPY = 0.128;
	double tauxUSD = 0.231;
	double tauxGBX = 0.211;
	pnl_vect_set(risklessrates, 0, mementis->taux_capitalisation_);
	pnl_vect_set(risklessrates, 1, tauxUSD);
	pnl_vect_set(risklessrates, 2, tauxJPY);
	pnl_vect_set(risklessrates, 3, tauxGBX);

	mementisCR->risklessrates_ = risklessrates;
	// vecteur des volatilités des devises etrangères
	PnlVect *sigmaChangeRate = pnl_vect_create_from_scalar(3, 0.1);

	// vecteur des spot des taux de change
	PnlVect *spotChangeRate = pnl_vect_create(3);
	double spotUSD = 0.88;
	double spotJPY = 0.008;
	double spotGBX = 0.01163061;
	pnl_vect_set(spotChangeRate, 0, spotUSD);
	pnl_vect_set(spotChangeRate, 1, spotJPY);
	pnl_vect_set(spotChangeRate, 2, spotGBX);
	
	PnlVect *spotRiscklessAsset = pnl_vect_create_from_scalar(3, 1);




	// Initialisation du modele de BS sans taux de changes
	Model *bsmodel = new BlackScholesModel(mementis->size_, mementis->taux_capitalisation_, rho_vect, sigma, spot, trend);
	MonteCarlo *mCarlo = new MonteCarlo(bsmodel, mementis, rng, fdStep, n_samples);
	double prix = 0.0;
	double ic = 0.0;
	mCarlo->price_simple(prix, ic);
	printf("Prix 0: %f \n", prix);
	printf("demi - intervalle de confiance : %f\n", ic);

	
	// Initialisation du modele de BS qui integre les taux de change
	BlackScholesModel *bsmodel_CR = new BlackScholesModel(nbAssets, nbMarkets, mementisCR->size_, nbAssetsPerMarket_, sigmaChangeRate, mementisCR->taux_capitalisation_, rho_CR, sigma, spot, spotChangeRate, spotRiscklessAsset, trend);
	MonteCarlo *mCarlo_CR = new MonteCarlo(bsmodel_CR, mementisCR, rng, fdStep, n_samples);
	double prix_CR = 0.0;
	double ic_CR = 0.0;
	mCarlo_CR->price_simple(prix_CR, ic_CR);
	printf("Prix_CR 0: %f \n", prix_CR);
	printf("demi - intervalle de confiance_CR : %f\n", ic_CR);


	//const type_info &t1 = typeid(mementis);  // t1 qualifie le type de *pd.
	//printf(&t1.name);

	//ASSERT_LE(prix, prix_CR + 0.001);
	//ASSERT_LE(ic, ic_CR + 0.001);
}