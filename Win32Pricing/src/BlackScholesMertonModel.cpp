//
// Created by lecalvmy on 9/13/18.
//
#include "BlackScholesMertonModel.hpp"

/**
* Constructeur de la classe
* @param[in] size : nombre d'actifs du modèle
* @param[in]  r : taux d'intérêt
* @param[in]  rho : paramètre de corrélation
* @param[in]  sigma : vecteur de volatilités
* @param[in]  spot : valeurs initiales des sous-jacents
* @param[in]  trend : tendance du modèle
*/
BlackScholesMertonModel::BlackScholesMertonModel(int size, double r, PnlMat *rho, PnlVect *sigma, PnlVect *spot, PnlVect *trend) {

	size_ = size;
	r_ = r;
	rho_ = rho;
	sigma_ = sigma;
	spot_ = spot;
	trend_ = trend;

	L = pnl_mat_create(size_, size_);
	pnl_mat_clone(L, rho_);
	pnl_mat_chol(L);

	G = pnl_mat_new();
	Gi = pnl_vect_new();
	Ld = pnl_vect_new();

	Div = pnl_mat_new();
	DatesDiv = pnl_mat_new();

	Div_d = pnl_vect_new();
	DatesDiv_d = pnl_vect_new();
}

/**
	* Génère une trajectoire du modèle et la stocke dans path
	*
	* @param[out] path contient une trajectoire du modèle.
	* C'est une matrice de taille (nbTimeSteps+1) x d
	* @param[in] T  maturité
	* @param[in] nbTimeSteps nombre de dates de constatation
	*/
void BlackScholesMertonModel::asset(PnlMat *path, double T, int nbTimeSteps, PnlRng *rng) {

	double step = T / nbTimeSteps;
	double sqrt_step = sqrt(step);
	double sigma;
	double expo_t;
	double Wt;
	double st;
int indiceDivCourant;
double dateDivCourant;
	pnl_mat_rng_normal(G, nbTimeSteps, size_, rng);

	for (int d = 0; d < size_; d++) {

		pnl_mat_get_row(Ld, L, d);
		sigma = pnl_vect_get(sigma_, d);
		expo_t = exp((r_ - pow(sigma, 2) / 2) * step);
		Wt = sigma * sqrt_step;

		/////////////////////////////////////////////
		pnl_mat_get_row(Div_d, Div, d);
		pnl_mat_get_row(DatesDiv_d, DatesDiv, d);

		indiceDivCourant = 0 ;
		dateDivCourant = pnl_vect_get(DatesDiv_d, indiceDivCourant);
		for (int i = 1; i < nbTimeSteps + 1; i++) {
			pnl_mat_get_row(Gi, G, i - 1);
			st = MGET(path, i - 1, d) * expo_t * exp(Wt * pnl_vect_scalar_prod(Gi, Ld));

			// ti < Tj <= ti+1
			if ((i-1)*step < dateDivCourant <= i*step ){
				st = st * (1- pnl_vect_get(Div_d, indiceDivCourant));
				indiceDivCourant++;
				dateDivCourant= pnl_vect_get(DatesDiv_d, indiceDivCourant);
			}
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
void BlackScholesMertonModel::asset(PnlMat *path, double t, double T, int nbTimeSteps, PnlRng *rng, const PnlMat *past) {

	double step = T / nbTimeSteps;
	double sqrt_step = sqrt(step);
	int nextIndex = ceil(t / step);
	double shiftStep = nextIndex * step - t;
	double sigma;
	double expo_t;
	double Wt;
	double st;

	pnl_mat_rng_normal(G, nbTimeSteps + 1 - nextIndex, size_, rng);

	//////////////
	int indiceDivCourant = 0;
	double dateDivCourant = pnl_vect_get(DatesDiv_d, indiceDivCourant);
	bool finDividende = false;


	for (int d = 0; d < size_; d++) {



		pnl_mat_get_row(Ld, L, d);
		sigma = pnl_vect_get(sigma_, d);


		/////////////////////////////////////////////
		pnl_mat_get_row(Div_d, Div, d);
		pnl_mat_get_row(DatesDiv_d, DatesDiv, d);
		while (t > dateDivCourant  && (dateDivCourant != -1) && indiceDivCourant < Div_d->size){
			indiceDivCourant++;
			dateDivCourant = pnl_vect_get(DatesDiv_d, indiceDivCourant);

		}

		/*t < dateDivCourant  && indiceDivCourant >= Div_d->m */
		if ((indiceDivCourant >= Div_d->size)||(dateDivCourant = -1)){
			bool finDividende = true;
		}


		/* Si t n'est pas un pas de discrétisation alors on simule le prochain pas */
		if (shiftStep != 0.0) {
			pnl_mat_get_row(Gi, G, 0);
			st = MGET(past, nextIndex, d) * exp((r_ - pow(sigma, 2) / 2) * shiftStep + sigma * sqrt(shiftStep) * pnl_vect_scalar_prod(Ld, Gi));
			// if not outranged and
			if ( t < dateDivCourant <= nextIndex * step && !finDividende ) {
				st = st * (1- pnl_vect_get(Div_d, indiceDivCourant));
				indiceDivCourant++;
				dateDivCourant = pnl_vect_get(DatesDiv_d, indiceDivCourant);
			}
			pnl_mat_set(path, nextIndex, d, st);
		}

		expo_t = exp((r_ - pow(sigma, 2) / 2) * step);
		Wt = sigma * sqrt_step;

		for (int i = nextIndex + 1; i < nbTimeSteps + 1; i++) {

			pnl_mat_get_row(Gi, G, i - nextIndex);
			st = MGET(path, i - 1, d) * expo_t * exp(Wt * pnl_vect_scalar_prod(Gi, Ld));

			if ((i-1)*step < dateDivCourant <= i*step ){
				st = st * (1- pnl_vect_get(Div_d, indiceDivCourant));
				indiceDivCourant++;
			}
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
void BlackScholesMertonModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep) {

	double nbSteps = ceil(t / timestep);

	pnl_mat_clone(shift_path, path);

	for (int i = nbSteps; i < path->m; i++) {
		pnl_mat_set(shift_path, i, d, (1 + h) * MGET(path, i, d));
	}
}

/* Destructeur pour le modele de BlackScholes */
BlackScholesMertonModel::~BlackScholesMertonModel()
{
	pnl_vect_free(&sigma_);
	pnl_vect_free(&spot_);
	pnl_vect_free(&trend_);
	pnl_mat_free(&L);
	pnl_mat_free(&G);
	pnl_vect_free(&Ld);
	pnl_vect_free(&Gi);
	pnl_mat_free(&Div);
	pnl_mat_free(&DatesDiv);
	pnl_vect_free(&Div_d);
	pnl_vect_free(&DatesDiv_d);
}
