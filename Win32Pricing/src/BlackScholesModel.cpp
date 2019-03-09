//
// Created by lecalvmy on 9/13/18.
//
#include "BlackScholesModel.hpp"

/**
* Constructeur de la classe
* @param[in] size : nombre d'actifs du modèle
* @param[in]  r : taux d'intérêt
* @param[in]  rho : matrice de corrélation
* @param[in]  sigma : vecteur de volatilités
* @param[in]  spot : valeurs initiales des sous-jacents
* @param[in]  trend : tendance du modèle
*/
BlackScholesModel::BlackScholesModel(int size, double r, PnlMat *rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend) {

	size_ = size;
	r_ = r;
	rho_ = rho;
	sigma_ = sigma;
	spot_ = spot;
	trend_ = trend;

	L = pnl_mat_create(size_, size_);
	pnl_mat_clone(L, rho_);
	pnl_mat_chol(L);
	G = pnl_mat_create_from_zero(2,2);
	Gi = pnl_vect_create_from_zero(2);
	Ld = pnl_vect_create_from_zero(2);
}
/**
* Constructeur de la classe dans le cas des taux de change
* @param[in] nbAssets : nombre d'actifs du modèle : domestiques + etrangers
* @param[in] nbMarkets : nombre de marchés differents
* @param[in] size : nombre d'actifs du modèle : nbAssets + nbMarkets
* @param[in]  nbAssetsPerMarket : vecteurs de nombre d'actifs par marché
* @param[in]  r : le taux d'intérêt sans risque domestique
* @param[in]  rho : matrice de corrélation
* @param[in]  sigma : vecteur de volatilités : domestiques + etrangers + taux de change
* @param[in]  spot : valeurs initiales des sous-jacents : domestiques + etrangers
* @param[in]  spotChangeRate : valeurs initiales des taux de change
* @param[in] spotRiscklessAsset : : valeurs initiales des actifs sans risque etrangers
* @param[in]  trend : tendance du modèle
*/
BlackScholesModel::BlackScholesModel(int nbAssets, int nbMarkets, int size, PnlVectInt  *nbAssetsPerMarket, PnlVect *sigmaChangeRate, double r, PnlMat *rho, PnlVect *sigma, PnlVect *spot, PnlVect *spotChangeRate, PnlVect *spotRiscklessAsset, PnlVect *trend) {
	// Construire le nouveau vecteur des vols : sigma domestiques + sigma etrangers adaptés + sigma change rate
	PnlVect *newSigma = pnl_vect_create(nbAssets + nbMarkets);
	// copy the domestic part of sigma
	int myCurrentAssets = pnl_vect_int_get(nbAssetsPerMarket, 0);
	int myForeignAssets;

	for (int i = 0; i < myCurrentAssets; i++) {
		pnl_vect_set(newSigma,i, pnl_vect_get(sigma, i));
	}
	// modify the foreign part of sigma
	for (int i = 1; i < nbMarkets; i++) {
		myForeignAssets = pnl_vect_int_get(nbAssetsPerMarket, i);
		for (int j = 0; j < myForeignAssets; j++) {
			pnl_vect_set(newSigma, myCurrentAssets + j, pnl_vect_get(sigma, myCurrentAssets + j) + pnl_vect_get(sigmaChangeRate, i));
		}
		myCurrentAssets = myCurrentAssets + myForeignAssets;
	}
	//copy all of sigmaChangeRate
	for (int i = nbAssets; i < nbMarkets+ nbAssets; i++) {
		pnl_vect_set(newSigma, i, pnl_vect_get(sigmaChangeRate, i));
	}
		// copy the domestic part of sigma
	myCurrentAssets = pnl_vect_int_get(nbAssetsPerMarket, 0);
	for (int i = 0; i < myCurrentAssets; i++) {
		pnl_vect_set(newSigma,i, pnl_vect_get(sigma, i));
	}
	// modify the foreign part of sigma
	for (int i = 1; i < nbMarkets; i++) {
		myForeignAssets = pnl_vect_int_get(nbAssetsPerMarket, i);
		for (int j = 0; j < myForeignAssets; j++) {
			pnl_vect_set(newSigma, myCurrentAssets + j, pnl_vect_get(sigma, myCurrentAssets + j) + pnl_vect_get(sigmaChangeRate, i));
		}
		myCurrentAssets = myCurrentAssets + myForeignAssets;
	}
	//copy all of sigmaChangeRate
	for (int i = nbAssets; i < nbMarkets+ nbAssets; i++) {
		pnl_vect_set(newSigma, i, pnl_vect_get(sigmaChangeRate, i));
	}




	// Construire le nouveau vecteur des spots correspondents
	PnlVect *newSpot = pnl_vect_create(nbAssets + nbMarkets);
	// copy the domestic part of spot
	myCurrentAssets = pnl_vect_int_get(nbAssetsPerMarket, 0);
	for (int i = 0; i < myCurrentAssets; i++) {
		pnl_vect_set(newSpot, i, pnl_vect_get(spot, i));
	}
	// modify the foreign part of spot
	for (int i = 1; i < nbMarkets; i++) {
		myForeignAssets = pnl_vect_int_get(nbAssetsPerMarket, i);
		for (int j = 0; j < myForeignAssets; j++) {
			pnl_vect_set(newSpot, myCurrentAssets + j, pnl_vect_get(spot, myCurrentAssets + j)*pnl_vect_get(spotChangeRate, i));
		}
		myCurrentAssets = myCurrentAssets + myForeignAssets;
	}
	//copy all of sigmaChangeRate
	for (int i = nbAssets; i < nbMarkets + nbAssets; i++) {
		pnl_vect_set(newSpot, i, pnl_vect_get(spotChangeRate, i)*pnl_vect_get(spotRiscklessAsset, i));
	}
	BlackScholesModel(nbAssets + nbMarkets, r, rho, newSigma, newSpot, trend);
}

/**
	* Génère une trajectoire du modèle et la stocke dans path
	*
	* @param[out] path contient une trajectoire du modèle.
	* C'est une matrice de taille (nbTimeSteps+1) x d
	* @param[in] T  maturité
	* @param[in] nbTimeSteps nombre de dates de constatation
	*/
void BlackScholesModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) {

	double step = T / nbTimeSteps;
	double sqrt_step = sqrt(step);
	double sigma;
	double expo_t;
	double Wt;
	double st;

	pnl_mat_rng_normal(G, nbTimeSteps, size_, rng);


	for (int d = 0; d < size_; d++) {

		pnl_mat_get_row(Ld, L, d);
		sigma = pnl_vect_get(sigma_, d);
		expo_t = exp((r_ - sigma * sigma / 2) * step);
		Wt = sigma * sqrt_step;

		for (int i = 1; i < nbTimeSteps + 1; i++) {

			pnl_mat_get_row(Gi, G, i - 1);
			st = MGET(path, i - 1, d) * expo_t * exp(Wt * pnl_vect_scalar_prod(Gi, Ld));
			pnl_mat_set(path, i, d, st);

		}
	}
}



/**
 * Calcule une trajectoire du sous-jacent connaissant le
 * passé jusqu' à la date t
 *
 * @param[out] path  contient une trajectoire du sous-jacent
 * donnée jusqu'à l'instant t par la matrice past
 * @param[in] t date jusqu'à laquelle on connait la trajectoire.
 * t n'est pas forcément une date de discrétisation
 * @param[in] nbTimeSteps nombre de pas de constatation
 * @param[in] T date jusqu'à laquelle on simule la trajectoire
 * @param[in] past trajectoire réalisée jusqu'a la date t
 */
void BlackScholesModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past) {

	double epsilon = 0.00000001;
	double step = T / nbTimeSteps;
	double sqrt_step = sqrt(step);
	double decimal_step = t / step;
	double floor_step = floor(decimal_step);
	int nextIndex = ((floor_step - epsilon < decimal_step) && (decimal_step < floor_step + epsilon)) ? floor_step : ceil(decimal_step);
	double shiftStep = nextIndex * step - t;

	double sigma;
	double expo_t;
	double Wt;
	double st;

	pnl_mat_rng_normal(G, nbTimeSteps + 1 - nextIndex, size_, rng);

	for (int d = 0; d < size_; d++) {

		pnl_mat_get_row(Ld, L, d);
		sigma = pnl_vect_get(sigma_, d);

		/* Si t n'est pas un pas de discrétisation alors on simule le prochain pas */
		if (shiftStep != 0.0) {
			pnl_mat_get_row(Gi, G, 0);
			st = MGET(past, nextIndex, d) * exp((r_ - sigma * sigma / 2) * shiftStep + sigma * sqrt(shiftStep) * pnl_vect_scalar_prod(Ld, Gi));
			pnl_mat_set(path, nextIndex, d, st);
		}

		expo_t = exp((r_ - sigma * sigma / 2) * step);
		Wt = sigma * sqrt_step;

		for (int i = nextIndex + 1; i < nbTimeSteps + 1; i++) {

			pnl_mat_get_row(Gi, G, i - nextIndex);
			st = MGET(path, i - 1, d) * expo_t * exp(Wt * pnl_vect_scalar_prod(Gi, Ld));
			pnl_mat_set(path, i, d, st);
		}
	}
}

/**
	 * Shift d'une trajectoire du sous-jacent
	 *
	 * @param[in]  path contient en input la trajectoire
	 * du sous-jacent
	 * @param[out] shift_path contient la trajectoire path
	 * dont la composante d a été shiftée par (1+h)
	 * à partir de la date t.
	 * @param[in] t date à partir de laquelle on shift
	 * @param[in] h pas de différences finies
	 * @param[in] d indice du sous-jacent à shifter
	 * @param[in] timestep pas de constatation du sous-jacent
	 */
void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep) {

	double epsilon = 0.00000001;
	double decimal_step = t / timestep;
	double floor_step = floor(decimal_step);
	int nbSteps = ((floor_step - epsilon < decimal_step) && (decimal_step < floor_step + epsilon)) ? floor_step : ceil(decimal_step);

	pnl_mat_clone(shift_path, path);

	for (int i = nbSteps; i < path->m; i++) {
		pnl_mat_set(shift_path, i, d, (1 + h) * MGET(path, i, d));
	}

}

/**
	* Simulation du marché (simulation du modèle sous la probabilité historique)
	* @param[out] simulatedMarket : contient les valeurs simulées du marché
	* @param[in]  T : maturité
	* @param[in]  H : nombre de pas de simulation
	* @param[in] rng : générateur de nombres aléatoires
	*/
void BlackScholesModel::simul_market(PnlMat *simulatedMarket, double T, int H,  PnlRng *rng) {

	double step = T / H;
	double sqrt_step = sqrt(step);
	double sigma;
	double expo_t;
	double Wt;
	double st;
	double trend_d;

	pnl_mat_rng_normal(G, H, size_, rng);

	for (int d = 0; d < size_; d++) {

		pnl_mat_get_row(Ld, L, d);
		sigma = pnl_vect_get(sigma_, d);
		trend_d = pnl_vect_get(trend_, d);
		expo_t = exp((trend_d- sigma * sigma / 2) * step);
		Wt = sigma * sqrt_step;

		for (int i = 1; i < H + 1; i++) {

			pnl_mat_get_row(Gi, G, i - 1);
			st = MGET(simulatedMarket, i - 1, d) * expo_t * exp(Wt * pnl_vect_scalar_prod(Gi, Ld));
			pnl_mat_set(simulatedMarket, i, d, st);

		}
	}
}

/* Destructeur pour le modele de BlackScholes */
BlackScholesModel::~BlackScholesModel()
{
/*	pnl_vect_free(&sigma_);
	pnl_vect_free(&spot_);
	pnl_vect_free(&trend_);
	pnl_mat_free(&rho_);*/
	pnl_mat_free(&L);
	pnl_mat_free(&G);
	pnl_vect_free(&Ld);
	pnl_vect_free(&Gi);
}
