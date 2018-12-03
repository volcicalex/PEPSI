//
// Created by lecalvmy on 9/18/18.
//

#include "AsianOption.hpp"

/**
* Constructeur de la classe
* @param[in]  T : maturité
* @param[in]  nbTimeSteps : nombre de pas de temps de discrétisation
* @param[in]  size : dimension du modèle
* @param[in] weights : poids des actifs
* @param[in]  strike : prix d'exercice de l'option
*/
AsianOption::AsianOption(double T, int nbTimeSteps, int size, PnlVect* weights, double strike) {
	T_ = T;
	nbTimeSteps_ = nbTimeSteps;
	size_ = size;
	weights_ = weights;
	strike_ = strike;
}

/**
 * Calcule la valeur du payoff sur la trajectoire
 *
 * @param[in] path est une matrice de taille (N+1) x d
 * contenant une trajectoire du modèle telle que créée
 * par la fonction asset.
 * @return phi(trajectoire)
 */
double AsianOption::payoff(const PnlMat *path) {
	double sum = 0;
	PnlVect *meanPrice = pnl_vect_create(size_);
	pnl_mat_sum_vect(meanPrice, path, 'r');
	pnl_vect_mult_scalar(meanPrice, 1.0 / path->m);
	sum = pnl_vect_scalar_prod(meanPrice, weights_);
	pnl_vect_free(&meanPrice);
	return fmax(sum - strike_, 0);
}
